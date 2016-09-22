#pragma once

#include <list>
#include <cstdlib>
#include "memoryrange.h"

using std::list;

class MemoryPage
{
public:
	using MemoryIter = list<MemoryRange>::iterator;

	MemoryPage(size_t pageSize);

	MemoryIter * findFree(size_t requestedSize);
	MemoryIter * findRange(size_t blockIndex);
	MemoryIter * allocate(size_t requestedSize);

private:
	list<MemoryRange> m_ranges;

};

