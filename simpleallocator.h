#pragma once

#include <cstdlib>
#include "memoryrange.h"

class SimpleAllocator
{
public:
	explicit SimpleAllocator(size_t pageSize, size_t blockSize = 4);
	~SimpleAllocator();

	void * mem_alloc(size_t size);
	//void * mem_realloc(void *addr, size_t size);
	void mem_free(void *addr);

private:
	inline size_t getBlockCount(size_t pageSize, size_t blockSize);
	size_t getBlockIndex(void * block);
	void * getBlockPointer(size_t blockIndex);

	size_t m_size;
	MemoryRange *m_page;
	size_t m_blockSize;
	void *m_start;
};
