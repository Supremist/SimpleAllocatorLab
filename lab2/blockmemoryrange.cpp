#include "blockmemoryrange.h"

BlockMemoryRange::BlockMemoryRange(): BlockMemoryRange(0,0)
{
}

BlockMemoryRange::BlockMemoryRange(size_t pageStart, size_t pageSize, size_t blockSize) :
	MemoryRange(pageStart, (pageSize / blockSize) * blockSize),
	m_blockSize(blockSize)
{
}

size_t BlockMemoryRange::maxBlockCount() const
{
	return size() / m_blockSize;
}

size_t BlockMemoryRange::blockSize() const
{
	return m_blockSize;
}

void BlockMemoryRange::resizeBlocks(size_t newSize)
{
	if (is_free) {
		m_blockSize = newSize;
	} // TODO throw an exception in else block
}

void BlockMemoryRange::resize(size_t newSize)
{
	if (!is_free && newSize <= size()) {
		return; // TODO throw an exception
	}
	size_t newBlockCount = newSize / blockSize;
	MemoryRange::resize(newBlockCount * blockSize);
	if (!is_free) {
		updateBlocks();
	}
}

BlockMemoryRange::Block BlockMemoryRange::allocateBlock()
{
	if (is_free) {
		updateBlocks();
		is_free = false;
	} else if (m_freeBlocks.empty()) {
		return {0, false};
	}

	size_t index = m_freeBlocksIndexes.front();
	m_freeBlocksIndexes.pop_front();
	setBlockFree(&m_blocks[index], false);
	return {index, true};
}

void BlockMemoryRange::freeBlock(size_t blockIndex)
{
	setBlockFree(&m_blocks[blockIndex], true);
	m_freeBlocksIndexes.push_back(blockIndex);

	if (m_freeBlocksIndexes.size() == maxBlockCount()) {
		is_free = true;
	}
}

void BlockMemoryRange::updateBlocks()
{
	size_t newCount = maxBlockCount();
	for (size_t index = m_blocks.size(); index < newCount; ++index) {
		m_freeBlocksIndexes.push_back(index);
	}
	m_blocks.resize(newCount, true);
}

void BlockMemoryRange::setBlockFree(BlockMemoryRange::BlockType *block, bool isFree)
{
	*block = isFree;
}

size_t BlockMemoryRange::align(size_t pageSize, size_t blockSize)
{
	return (pageSize + blockSize - 1) / blockSize;
}

