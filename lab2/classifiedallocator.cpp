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
	delete m_manager;
	for (ClassifiedPageMap::iterator it = m_classifiedPageMap.begin();
		 it != m_classifiedPageMap.end(); ++it) {
		delete it->second;
	}
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
			range->free();
			range->resizeBlocks(alignedSize);
			if (pages) {
				pages->push_front(range);
			} else {
				pages = new list<ClassifiedManager::MemoryIter>({range});
				m_classifiedPageMap.insert(std::make_pair(alignedSize, pages));
			}
			auto info = PageInfo::createClassifiedPage(pages, pages->begin());
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
		updatePagesInfo(std::shared_ptr<PageInfo>(PageInfo::createLargePage(range)));
		return m_memoryBlock.positionToPointer(range->start());
	}
}

void ClassifiedAllocator::mem_free(void *addr)
{
	if (!addr) {
		return;
	}
	size_t position = m_memoryBlock.pointerToPosition(addr);
	size_t pageIndex = position / m_pageSize;
	auto pageInfo = getPageInfo(pageIndex);
	if (!pageInfo) {
		return;
	}
	ClassifiedManager::MemoryIter page = pageInfo->managedIter();
	if (page->maxBlockCount() > 1) {
		size_t offset = position - pageIndex * m_pageSize;
		page->freeBlock(offset / page->blockSize());
		if (!page->isFree()) {
			return;
		}
	}
	m_manager->free(page);
	pageInfo->free();
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

	for (size_t index = startIndex; index <= endIndex; ++index) {
		output << index * m_pageSize << " - " << (index + 1) * m_pageSize << " ";
		PageInfo::Ptr pageInfo = getPageInfo(index);
		if (pageInfo) {
			if (pageInfo->isLarge()) {
				output << "Large allocated page";
			} else {
				size_t blockSize = pageInfo->managedIter()->blockSize();
				output << "Page clasified with block size ";
				output << blockSize << ":\n";

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
	auto pageInfo = m_pagesInfo[pageIndex];
	if (pageInfo->isFree()) {
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

PagesList::iterator ClassifiedAllocator::findFreePage(PagesList *pages)
{
	auto iter = pages->begin();
	while (iter != pages->end() && (*iter)->fullyAllocated()) { ++iter; }
	return iter;
}
