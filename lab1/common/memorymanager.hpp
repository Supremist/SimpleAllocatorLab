#include "memorymanager.h"

template <typename Range>
std::_List_iterator<Range> operator+(const std::_List_iterator<Range> &iter, int i)
{
	std::_List_iterator<Range> result = iter;
	if (i > 0) {
		while (i > 0) {
			result++;
			i--;
		}
	} else {
		return operator -(iter, -i);
	}
	return result;
}

template <typename Range>
std::_List_iterator<Range> operator-(const std::_List_iterator<Range> &iter, int i)
{
	std::_List_iterator<Range> result = iter;
	if (i > 0) {
		while (i > 0) {
			result--;
			i--;
		}
	} else {
		return operator +(iter, -i);
	}
	return result;
}



template <typename Range>
MemoryManager<Range>::MemoryManager(size_t pageSize) :
	m_size(pageSize)
{
	m_ranges.push_back(Range(pageSize));
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::findFree(size_t requestedSize)
{
	for (MemoryIter iter = begin(); iter != end(); ++iter) {
		if (iter->isFree() && iter->size() >= requestedSize) {
			return iter;
		}
	}
	return end();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::findRange(size_t blockIndex, typename MemoryManager<Range>::MemoryIter searchFrom)
{
	if (searchFrom == end()) {
		searchFrom = begin();
	}
	for (MemoryIter iter = searchFrom; iter != end(); ++iter) {
		if (blockIndex >= iter->start() && blockIndex < iter->end()) {
			return iter;
		}
	}
	return end();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::findRange(size_t blockIndex)
{
	return findRange(blockIndex, begin());
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::restoreBlock(const Range &range, typename MemoryManager<Range>::MemoryIter searchFrom)
{
	MemoryIter old = findRange(range.start(), searchFrom);
	if (old == end() || !old->isFree())
		return end();

	if(range.start() > old->start())
		old = splitRange(old, range.start() - old->start());
	if(range.end() < old->end())
		splitRange(old, range.size());
	*old = range;
	return old;
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter MemoryManager<Range>::allocate(size_t requestedSize)
{
	MemoryIter firstFree = findFree(requestedSize);
	if (firstFree != end() && requestedSize <= firstFree->size()) {
		splitRange(firstFree, requestedSize);
		firstFree->allocate();
		return firstFree;
	}
	return end();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::reallocate(typename MemoryManager<Range>::MemoryIter range, size_t newSize)
{
	Range backup = *range;
	free(range);
	MemoryIter result = allocate(newSize);
	if (result == end())
		restoreBlock(backup, begin());
	return result;
}

template <typename Range>
void MemoryManager<Range>::free(typename MemoryManager<Range>::MemoryIter range)
{
	if(range == end())
		return;

	range->free();
	for (int shift : {1, -1}) {
		MemoryManager<Range>::MemoryIter other;
		if (range == begin() && shift == -1) {
			other = end();
		} else if (range == end() && shift == 1) {
			other = begin();
		} else {
			other = range + shift;
		}
		if(other != end() && other->isFree())
			mergeRange(range, other);
	}
}

template <typename Range>
typename MemoryManager<Range>::ConstMemoryIter
MemoryManager<Range>::cbegin() const
{
	return m_ranges.cbegin();
}

template <typename Range>
typename MemoryManager<Range>::ConstMemoryIter
MemoryManager<Range>::cend() const
{
	return m_ranges.cend();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::begin()
{
	return m_ranges.begin();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::end()
{
	return m_ranges.end();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::splitRange(typename MemoryManager<Range>::MemoryIter range, size_t newSize)
{
	if(range == end())
		return end();

	Range *tail = range->split(newSize);
	if (tail) {
		auto insertPosition = range;
		m_ranges.insert(++insertPosition, 1, *tail);
		delete tail;
		return insertPosition;
	}
	return end();
}

template <typename Range>
typename MemoryManager<Range>::MemoryIter
MemoryManager<Range>::mergeRange(typename MemoryManager<Range>::MemoryIter range,
								 typename MemoryManager<Range>::MemoryIter otherRange)
{
	if (range != end() && otherRange != end()
			&& range != otherRange
			&& range->isFree() == otherRange->isFree()) {
		range->merge(*otherRange);
		m_ranges.erase(otherRange);
		return range;
	}
	return end();
}

template <typename Range>
typename MemoryManager<Range>::ConstMemoryIter
MemoryManager<Range>::largestFreeRange() const
{
	ConstMemoryIter max = cbegin();
	for (ConstMemoryIter iter = cbegin(); iter != cend(); ++iter) {
		if (iter->isFree() && iter->size() > max->size()) {
			max = iter;
		}
	}
	return max;
}

template <typename Range>
size_t MemoryManager<Range>::freeSpaceSize() const
{
	size_t result = 0;
	for (ConstMemoryIter iter = cbegin(); iter != cend(); ++iter) {
		if (iter->isFree()) {
			result += iter->size();
		}
	}
	return result;
}

template <typename Range>
size_t MemoryManager<Range>::size() const
{
	return m_size;
}

