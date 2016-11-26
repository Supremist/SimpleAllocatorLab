#pragma once

#include <list>
#include <cstdlib>

using std::list;


template <typename Range>
class MemoryManager
{
public:

	using MemoryIter = typename list<Range>::iterator;
	using ConstMemoryIter = typename list<Range>::const_iterator;

	MemoryManager(size_t pageSize);

	MemoryIter findFree(size_t requestedSize);
	MemoryIter findRange(size_t blockIndex, MemoryIter searchFrom);
	MemoryIter findRange(size_t blockIndex);
	MemoryIter restoreBlock(const Range &range, MemoryIter searchFrom);

	MemoryIter allocate(size_t requestedSize);
	MemoryIter reallocate(MemoryIter range, size_t newSize);
	void free(MemoryIter range);

	ConstMemoryIter cbegin() const;
	ConstMemoryIter cend() const;
	MemoryIter begin();
	MemoryIter end();

	MemoryIter splitRange(MemoryIter range, size_t newSize);
	MemoryIter mergeRange(MemoryIter range, MemoryIter otherRange);

	ConstMemoryIter largestFreeRange() const;
	size_t freeSpaceSize() const;
	size_t size() const;

protected:
	size_t m_size;
	list<Range> m_ranges;

};

template <typename Range>
std::_List_iterator<Range> operator+(const std::_List_iterator<Range> &iter, int i);
template <typename Range>
std::_List_iterator<Range> operator-(const std::_List_iterator<Range> &iter, int i);

#include "memorymanager.hpp"
