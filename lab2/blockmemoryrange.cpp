#include "blockmemoryrange.h"

BlockMemoryRange::BlockMemoryRange(size_t size): BlockMemoryRange(0, 1, size)
{
}

BlockMemoryRange::BlockMemoryRange(size_t start, size_t blockCount, size_t blockSize) :
	MemoryRange(start, blockCount * blockSize),
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
	if (isFree()) {
		m_blockSize = newSize;
	} // TODO throw an exception in else block
}

void BlockMemoryRange::resize(size_t newSize)
{
	if (!canResize(newSize)) {
		return; //TODO throw an excaption
	}

	_resize(newSize);
}

void BlockMemoryRange::free()
{
	MemoryRange::free();
	resizeBlocks(size());
	updateBlocks();
}

bool BlockMemoryRange::canResize(size_t newSize)
{
	if (isFree() || newSize >= size()) {
		return true;
	}

	for (size_t index = BlockAlignUtils::alignUpper(newSize, m_blockSize); index < maxBlockCount(); ++index) {
		if (!m_blocks[index]) {
			return false;
		}
	}
	return true;
}

BlockMemoryRange *BlockMemoryRange::split(size_t newSize)
{
	if (newSize >= size())
		return nullptr;

	vector<bool> otherBlocks;
	if (!isFree() && !isLarge()) {
		size_t newBlocksSize = BlockAlignUtils::alignUpper(newSize, m_blockSize);
		otherBlocks.insert(otherBlocks.begin(), m_blocks.begin() + newBlocksSize, m_blocks.end());
		m_blocks.resize(newBlocksSize);
	}

	MemoryRange *base = MemoryRange::split(newSize);
	if (!base) {
		return nullptr;
	}

	BlockMemoryRange *result = new BlockMemoryRange(*base);
	delete base;

	if(!isFree() && !isLarge()) {
		result->m_blocks = otherBlocks;
		result->initFreeBlocksIndexes();
	}
	return result;
}

bool BlockMemoryRange::fullyAllocated()
{
	return !isFree() && m_freeBlocksIndexes.empty();
}

BlockMemoryRange::Block BlockMemoryRange::allocateBlock()
{
	if (isFree()) {
		updateBlocks();
		allocate();
	} else if (m_freeBlocksIndexes.empty()) {
		return {0, false};
	}

	size_t index = m_freeBlocksIndexes.front();
	m_freeBlocksIndexes.pop_front();
	setBlockFree(m_blocks[index], false);
	return {index, true};
}

void BlockMemoryRange::freeBlock(size_t blockIndex)
{
	setBlockFree(m_blocks[blockIndex], true);
	m_freeBlocksIndexes.push_front(blockIndex);

	if (m_freeBlocksIndexes.size() == maxBlockCount()) {
		free();
	}
}

bool BlockMemoryRange::isLarge() const
{
	return m_blockSize == size();
}

vector<BlockMemoryRange::BlockType> BlockMemoryRange::blocks() const
{
	return m_blocks;
}

BlockMemoryRange::BlockMemoryRange() : MemoryRange(0,0)
{
}

BlockMemoryRange::BlockMemoryRange(const MemoryRange &other) :
	MemoryRange(other),
	m_blockSize(other.size())
{
}

void BlockMemoryRange::_resize(size_t newSize)
{
	if (isLarge()) {
		MemoryRange::resize(newSize);
		resizeBlocks(newSize);
	} else {
		size_t newBlockCount = BlockAlignUtils::alignUpper(newSize, m_blockSize);
		MemoryRange::resize(newBlockCount * blockSize());
	}
	if (!isFree()) {
		updateBlocks();
	}
}

void BlockMemoryRange::updateBlocks()
{
	size_t newCount = maxBlockCount();
	if (newCount > m_blocks.size()) {
		for (size_t index = m_blocks.size(); index < newCount; ++index) {
			m_freeBlocksIndexes.push_back(index);
		}
	} else {
		initFreeBlocksIndexes();
	}
	m_blocks.resize(newCount, true);
}

void BlockMemoryRange::initFreeBlocksIndexes()
{
	m_freeBlocksIndexes.clear();
	for (size_t index = 0; index < m_blocks.size(); ++index) {
		if (m_blocks[index]) {
			m_freeBlocksIndexes.push_back(index);
		}
	}
}

void BlockMemoryRange::setBlockFree(vector<BlockType>::reference block, bool isFree)
{
	block = isFree;
}

