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

	MemoryIter findFree(size_t requestedSize);
	MemoryIter findRange(size_t blockIndex, MemoryIter searchFrom);
	MemoryIter findRange(size_t blockIndex);
	MemoryIter restoreBlock(const MemoryRange &range, MemoryIter searchFrom);

	MemoryIter allocate(size_t requestedSize);
	MemoryIter reallocate(MemoryIter range, size_t newSize);
	void free(MemoryIter range);

	MemoryIter begin() const;
	MemoryIter end() const;

	MemoryIter splitRange(MemoryIter range, size_t newSize);
	MemoryIter mergeRange(MemoryIter range, MemoryIter otherRange);


private:

	list<MemoryRange> m_ranges;

};

MemoryPage::MemoryIter operator +(const MemoryPage::MemoryIter &iter, int i);
MemoryPage::MemoryIter operator -(const MemoryPage::MemoryIter &iter, int i);
