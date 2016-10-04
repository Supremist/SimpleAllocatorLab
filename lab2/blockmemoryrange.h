#pragma once

#include <memoryrange.h>
#include <vector>
#include <list>

using std::vector;
using std::list;

class BlockMemoryRange: public MemoryRange
{
public:

	BlockMemoryRange();
	BlockMemoryRange(size_t pageStart, size_t pageSize, size_t blockSize);

	struct Block {
		size_t index;
		bool isValid;
	};

	using BlockType = bool;

	size_t maxBlockCount() const;
	size_t blockSize() const;

	void resizeBlocks(size_t newSize);
	void resize (size_t newSize) override;

	Block allocateBlock();
	void freeBlock(size_t blockIndex);

private:
	void updateBlocks();
	inline void setBlockFree(BlockType * block, bool isFree);
	inline size_t align(size_t pageSize, size_t blockSize);

	size_t m_blockSize;
	vector<BlockType> m_blocks;
	list<size_t> m_freeBlocksIndexes;

};
