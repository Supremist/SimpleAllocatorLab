#pragma once

#include <cstdlib>
#include "memorypage.h"

class SimpleAllocator
{
public:
	explicit SimpleAllocator(size_t pageSize, size_t blockSize = 4);
	~SimpleAllocator();

	void * mem_alloc(size_t size);
	void * mem_realloc(void *addr, size_t size);
	void mem_free(void *addr);

	const MemoryPage * page() const;
	size_t blockSize() const;

private:
	inline size_t align(size_t pageSize, size_t blockSize);
	size_t getBlockIndex(void * block);
	void * getBlockPointer(size_t blockIndex);

	size_t m_size;
	MemoryPage * m_page;
	size_t m_blockSize;
	void *m_start;
};
