#pragma once

#include <cstdlib>
#include "memorypage.h"
#include "blockalignutils.h"
#include "abstractallocator.h"

class SimpleAllocator : public AbstractAllocator
{
public:
	explicit SimpleAllocator(size_t pageSize, size_t blockSize = 4);
	~SimpleAllocator();

	void * mem_alloc(size_t size) override;
	void * mem_realloc(void *addr, size_t size) override;
	void mem_free(void *addr) override;
	void mem_dump(ostream &output, void *start = nullptr, size_t dumpSize = 0) override;

	size_t freeSpaceSize() const override;
	size_t largestFreeBlockSize() const override;

	const MemoryPage * page() const;
	size_t blockSize() const;

private:
	size_t getBlockIndex(void *block);
	void * getBlockPointer(size_t blockIndex);

	MemoryPage * m_page;
	size_t m_blockSize;
};
