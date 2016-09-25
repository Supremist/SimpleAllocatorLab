#pragma once

#include <cstdlib>
#include <vector>
#include <list>


class ClassifiedMemoryPage
{
public:
	using std::vector;
	using std::list;

	ClassifiedMemoryPage();
	ClassifiedMemoryPage(size_t pageSize, size_t blockSize);

	struct Block {
		size_t index;
		bool isValid;
	};

	using BlockType = bool;

	bool isFree() const;
	size_t maxBlockCount() const;
	size_t pageSize() const;
	size_t blockSize() const;

	bool resizeBlocks(size_t newSize);
	bool resizePage  (size_t newSize);

	Block allocateBlock();
	void free(size_t blockIndex);

private:
	void initBlocks(bool isBlockFree);
	inline void setBlockFree(BlockType * block, bool isFree);

	size_t m_pageSize;
	size_t m_blockSize;
	bool is_free = true;
	vector<BlockType> m_blocks;
	list<size_t> m_freeBlocksIndexes;

};
