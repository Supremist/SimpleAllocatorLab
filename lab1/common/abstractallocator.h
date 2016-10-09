#pragma once

#include <cstdlib>
#include <ostream>

using std::ostream;

class AbstractAllocator
{
public:

	AbstractAllocator(size_t managedSize);
	virtual ~AbstractAllocator();

	virtual void * mem_alloc(size_t size) = 0;
	virtual void * mem_realloc(void *addr, size_t size) = 0;
	virtual void mem_free(void *addr) = 0;
	virtual void mem_dump(ostream &output, void *start = nullptr, size_t dumpSize = 0) = 0;

	virtual size_t largestFreeBlockSize() const = 0;
	virtual size_t freeSpaceSize() const = 0;
	size_t allocatedSize() const;
	size_t managedSize() const;
	void * startAddress() const;

protected:
	size_t pointerToPosition(void *pointer);
	void * positionToPointer(size_t position);


private:
	virtual void resize(size_t newSize); //TODO move to public, reimplement for derived

	size_t m_managedSize;
	void * m_start;
};
