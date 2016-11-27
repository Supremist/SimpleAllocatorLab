#include "classifiedallocator.h"
#include <algorithm>

ClassifiedAllocator::ClassifiedAllocator(size_t pageCount, size_t pageSize, const set<size_t> &pageClassesSizes) :
	AbstractAllocator(),
	m_pageSize(pageSize),
	m_memoryBlock(pageSize * pageCount),
	m_manager(new MemoryManager<BlockMemoryRange>(pageSize * pageCount)),
	m_pageClassesSizes(pageClassesSizes),
	m_pagesInfo(vector<std::shared_ptr<PageInfo>>(pageCount))
{
}

ClassifiedAllocator::~ClassifiedAllocator()
{
	m_pagesInfo.clear();
	for (ClassifiedPageMap::iterator it = m_classifiedPageMap.begin();
		 it != m_classifiedPageMap.end(); ++it) {
		delete it->second;
	}
	delete m_manager;
}

void ClassifiedAllocator::clearUnusedClasses()
{
	for (set<size_t>::iterator it = m_pageClassesSizes.begin(); it != m_pageClassesSizes.end(); ) {
		if (m_classifiedPageMap.count(*it) == 0) {
			it = m_pageClassesSizes.erase(it);
		} else {
			++it;
		}
	}
}

ClassifiedAllocator::PointerInfo ClassifiedAllocator::getPointerInfo(void *ptr)
{
	PointerInfo result;
	if (!ptr) {
		result.isValid = false;
		return result;
	}

	result.position = m_memoryBlock.pointerToPosition(ptr);
	size_t pageIndex = result.position / m_pageSize;
	result.pageInfo = getPageInfo(pageIndex);
	if (!result.pageInfo) {
		result.isValid = false;
		return result;
	}
	ClassifiedManager::MemoryIter page = result.pageInfo->managedIter();

	result.offset = result.position - pageIndex * m_pageSize;
	result.blockIndex = result.offset / page->blockSize();
	result.isValid = result.offset % page->blockSize() == 0
			&& result.position == page->start();
	return result;
}

set<size_t> ClassifiedAllocator::pageClassesSizes() const
{
	return m_pageClassesSizes;
}

void ClassifiedAllocator::addPageClassesSizes(const set<size_t> &pageClassesSizes)
{
	m_pageClassesSizes.insert(pageClassesSizes.begin(), pageClassesSizes.end());
}

void *ClassifiedAllocator::mem_alloc(size_t size)
{
	if (size <= 0) {
		return nullptr;
	}

	ClassifiedManager::MemoryIter range;
	set<size_t>::iterator size_iter = m_pageClassesSizes.lower_bound(size);
	if (size_iter != m_pageClassesSizes.end() && *size_iter < m_pageSize) {
		size_t alignedSize = *size_iter;
		PagesList *pages = getEqualySizedPages(alignedSize);
		PagesList::iterator iter;
		if (pages && (iter = findFreePage(pages)) != pages->end()) {
			// Classified page with free block found
			range = *iter;
		} else {
			// Allocate new classified page
			range = m_manager->allocate(m_pageSize);
			if (range == m_manager->end()) {
				return nullptr;
			}
			range->free();
			range->resizeBlocks(alignedSize);
			if (pages) {
				pages->push_front(range);
			} else {
				pages = new list<ClassifiedManager::MemoryIter>({range});
				m_classifiedPageMap.insert(std::make_pair(alignedSize, pages));
			}
			auto info = PageInfo::createClassifiedPage(pages, pages->begin());
			auto debug = *info->managedIter();
			updatePagesInfo(std::shared_ptr<PageInfo>(info));
		}

		BlockMemoryRange::Block block = range->allocateBlock();
		if (block.isValid) {
			size_t offset = range->start() + block.index * range->blockSize();
			return m_memoryBlock.positionToPointer(offset);
		} else {
			//TODO Fail
			return nullptr;
		}
	} else {
		// Allocate large page
		size_t normalPageCount = BlockAlignUtils::alignUpper(size, m_pageSize);
		range = m_manager->allocate(normalPageCount * m_pageSize);
		if (range == m_manager->end()) {
			return nullptr;
		}
		updatePagesInfo(std::shared_ptr<PageInfo>(PageInfo::createLargePage(range)));
		return m_memoryBlock.positionToPointer(range->start());
	}
}

void ClassifiedAllocator::mem_free(void *addr)
{
	mem_free(getPointerInfo(addr));
}


void ClassifiedAllocator::mem_dump(std::ostream &output, void *start, size_t dumpSize)
{
	size_t startPosition;
	if (start) {
		startPosition = m_memoryBlock.pointerToPosition(start);
	} else {
		startPosition = 0;
	}

	size_t startIndex = BlockAlignUtils::alignUpper(startPosition, m_pageSize);
	size_t endIndex;
	if (dumpSize >= m_manager->size() || dumpSize <= 0) {
		endIndex = m_manager->size() / m_pageSize;
	} else {
		endIndex = BlockAlignUtils::alignUpper(startPosition + dumpSize, m_pageSize);
	}

	for (size_t index = startIndex; index < endIndex; ++index) {
		output << index * m_pageSize << " - " << (index + 1) * m_pageSize << " ";
		PageInfo::Ptr pageInfo = getPageInfo(index);
		if (pageInfo) {
			if (pageInfo->isLarge()) {
				output << "Large allocated page";
			} else {
				size_t blockSize = pageInfo->managedIter()->blockSize();
				output << "Page clasified with block size ";
				output << blockSize << ":\n";
				auto debug = *pageInfo->managedIter();
				auto blocks = pageInfo->managedIter()->blocks();
				for (size_t blockIndex = 0; blockIndex < blocks.size(); ++blockIndex) {
					output << " " << blockIndex * blockSize << " - ";
					output << (blockIndex + 1) * blockSize << " ";
					if (blocks[blockIndex]) {
						output << "Free block";
					} else {
						output << "Allocated block";
					}
					output << "\n";
				}
			}
		} else {
			output << "Free page";
		}
		output << "\n";
	}
	output << "\n";
}

void *ClassifiedAllocator::mem_realloc(void *addr, size_t size)
{
	PointerInfo info = getPointerInfo(addr);
	if (!info.isValid) {
		return nullptr;
	}
	auto page = info.pageInfo->managedIter();
	BlockMemoryRange backup = *page;
	ClassifiedManager::MemoryIter previous;
	if (page == m_manager->begin()) {
		previous = m_manager->end();
	} else {
		previous = --page;
	}
	mem_free(info);
	void *result = mem_alloc(size);
	if (result) {
		return result;
	}
	auto newIter = m_manager->restoreBlock(backup, previous);
	info.pageInfo->restore(newIter);
	return nullptr;
}

size_t ClassifiedAllocator::largestFreeBlockSize() const
{
	size_t result = m_manager->largestFreeRange()->size();
	if (result == 0) {
		size_t max = 0;
		for (ClassifiedPageMap::const_iterator iter = ++m_classifiedPageMap.cbegin();
			 iter != m_classifiedPageMap.cend(); ++iter) {
			PagesList::iterator page = findFreePage(iter->second);
			if (page != iter->second->end() && max < iter->first) {
				max = iter->first;
			}
		}
		return max;
	}
	return result;
}

size_t ClassifiedAllocator::freeSpaceSize() const
{
	size_t sum = 0;
	for (const auto &page : m_pagesInfo) {
		if (!page || page->isFree() || page->isLarge()) {
			sum += m_pageSize;
		} else {
			const BlockMemoryRange &range = *page->managedIter();
			for (const auto &block : range.blocks()) {
				if (block) {
					sum += range.blockSize();
				}
			}
		}
	}
	return sum;
}

size_t ClassifiedAllocator::managedSize() const
{
	return m_manager->size();
}

void ClassifiedAllocator::updatePagesInfo(const PageInfo::Ptr &newPage)
{
	size_t lastIndex = newPage->managedIter()->end() / m_pageSize;
	for(size_t index = newPage->managedIter()->start() / m_pageSize; index < lastIndex; ++index) {
		m_pagesInfo[index] = newPage;
	}
}

PageInfo::Ptr ClassifiedAllocator::getPageInfo(size_t pageIndex)
{
	if (pageIndex < 0 || pageIndex >= m_pagesInfo.size()) {
		return nullptr;
	}
	auto pageInfo = m_pagesInfo[pageIndex];
	if (pageInfo && pageInfo->isFree()) {
		return nullptr;
	}
	return pageInfo;
}

PagesList *ClassifiedAllocator::getEqualySizedPages(size_t blockSize)
{
	size_t cnt = m_classifiedPageMap.count(blockSize);
	if (cnt == 1) {
		PagesList *pages = m_classifiedPageMap.at(blockSize);
		if (pages && pages->size() > 0) {
			return pages;
		}
		delete pages;
		m_classifiedPageMap.erase(blockSize);
	} else if (cnt > 1) {
		//TODO error
	}
	return nullptr;
}

void ClassifiedAllocator::mem_free(const ClassifiedAllocator::PointerInfo &info)
{
	if (!info.isValid) {
		return;
	}
	auto page = info.pageInfo->managedIter();
	page->freeBlock(info.blockIndex);
	if (page->isFree()) {
		m_manager->free(page);
		info.pageInfo->free();
	}
}

PagesList::iterator ClassifiedAllocator::findFreePage(PagesList *pages) const
{
	auto iter = pages->begin();
	while (iter != pages->end() && (*iter)->fullyAllocated()) { ++iter; }
	return iter;
}
