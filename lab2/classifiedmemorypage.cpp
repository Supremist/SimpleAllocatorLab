#include "classifiedmemorypage.h"

ClassifiedMemoryPage::ClassifiedMemoryPage(): ClassifiedMemoryPage(0,0)
{
}

ClassifiedMemoryPage::ClassifiedMemoryPage(size_t pageSize, size_t blockSize) :
	m_pageSize(pageSize),
	m_blockSize(blockSize)
{
}

bool ClassifiedMemoryPage::isFree() const
{
	return is_free;
}

size_t ClassifiedMemoryPage::maxBlockCount() const
{
	return m_pageSize / m_blockSize;
}

size_t ClassifiedMemoryPage::pageSize() const
{
	return m_pageSize;
}

size_t ClassifiedMemoryPage::blockSize() const
{
	return m_blockSize;
}

bool ClassifiedMemoryPage::resizeBlocks(size_t newSize)
{
	if (is_free) {
		m_blockSize = newSize;
	}
}

bool ClassifiedMemoryPage::resizePage(size_t newSize)
{
	if (is_free) {
		m_pageSize = newSize;
	}
}

ClassifiedMemoryPage::Block ClassifiedMemoryPage::allocateBlock()
{
	if (is_free) {
		initBlocks(true);
		is_free = false;
	} else if (m_freeBlocks.empty()) {
		return {0, false};
	}

	size_t index = m_freeBlocksIndexes.front();
	m_freeBlocksIndexes.pop_front();
	setBlockFree(&m_blocks[index], false);
	return {index, true};
}

void ClassifiedMemoryPage::free(size_t blockIndex)
{
	setBlockFree(&m_blocks[blockIndex], true);
	m_freeBlocksIndexes.push_back(blockIndex);

	if (m_freeBlocksIndexes.size() == maxBlockCount()) {
		is_free = true;
	}
}

void ClassifiedMemoryPage::initBlocks(bool isBlockFree)
{
	size_t count = maxBlockCount();
	m_blocks = vector<BlockType>(count, isBlockFree);
	m_freeBlocksIndexes.clear();
	if (isBlockFree) {
		for (size_t index = 0; index < count; ++index) {
			m_freeBlocksIndexes.push_back(index);
		}
	}
}

void ClassifiedMemoryPage::setBlockFree(ClassifiedMemoryPage::BlockType *block, bool isFree)
{
	*block = isFree;
}

