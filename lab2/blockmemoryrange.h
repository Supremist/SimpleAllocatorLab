#pragma once

#include <memoryrange.h>
#include <vector>
#include <list>
#include "blockalignutils.h"

using std::vector;
using std::list;

class BlockMemoryRange: public MemoryRange
{
public:
	struct Block {
		size_t index;
		bool isValid;
	};

	BlockMemoryRange(size_t size);
	BlockMemoryRange(size_t start, size_t blockCount, size_t blockSize);


	using BlockType = bool;

	size_t maxBlockCount() const;
	size_t blockSize() const;

	void resizeBlocks(size_t newSize);
	void resize (size_t newSize) override;
	bool canResize(size_t newSize);
	BlockMemoryRange *split(size_t newSize) override;

	bool fullyAllocated();

	Block allocateBlock();
	void freeBlock(size_t blockIndex);

	vector<BlockType> blocks() const;

private:
	BlockMemoryRange();

	void _resize(size_t newSize);
	void updateBlocks();
	void initFreeBlocksIndexes();
	inline void setBlockFree(vector<BlockType>::reference block, bool isFree);

	size_t m_blockSize;
	vector<BlockType> m_blocks;
	list<size_t> m_freeBlocksIndexes;

};
