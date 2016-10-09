#include "memorypage.h"


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
MemoryPage<Range>::MemoryPage(size_t pageSize)
{
	m_ranges.push_back(Range(0, pageSize));
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::findFree(size_t requestedSize)
{
	for (MemoryIter iter = begin(); iter != end(); ++iter) {
		if (iter->isFree() && iter->size() >= requestedSize) {
			return iter;
		}
	}
	return end();
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::findRange(size_t blockIndex, MemoryIter searchFrom)
{
	for (MemoryIter iter = searchFrom; iter != end(); ++iter) {
		if (blockIndex >= iter->start() && blockIndex < iter->end()) {
			return iter;
		}
	}
	return end();
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::findRange(size_t blockIndex)
{
	return findRange(blockIndex, begin());
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::restoreBlock(const Range &range, MemoryPage::MemoryIter searchFrom)
{
	MemoryIter old = findRange(range.start(), searchFrom);
	if (old == end() || !old->isFree())
		return end();

	if(range.start() > old->start())
		old = splitRange(old, range.start() - old->start());
	if(range.end() < old->end())
		splitRange(old, range.size());
	*old = range;
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter MemoryPage<Range>::allocate(size_t requestedSize)
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
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::reallocate(MemoryPage::MemoryIter range, size_t newSize)
{
	Range backup = *range;
	free(range);
	MemoryIter result = allocate(newSize);
	if (result == end())
		restoreBlock(backup, begin());
	return result;
}

template <typename Range>
void MemoryPage<Range>::free(MemoryPage::MemoryIter range)
{
	if(range == end())
		return;

	range->free();
	for (int shift : {1, -1}) {
		MemoryPage<Range>::MemoryIter other = range + shift;
		if(other != end() && other->isFree())
			mergeRange(range, other);
	}
}

template <typename Range>
typename MemoryPage<Range>::ConstMemoryIter
MemoryPage<Range>::begin() const
{
	return m_ranges.begin();
}

template <typename Range>
typename MemoryPage<Range>::ConstMemoryIter
MemoryPage<Range>::end() const
{
	return m_ranges.end();
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::begin()
{
	return m_ranges.begin();
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::end()
{
	return m_ranges.end();
}

template <typename Range>
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::splitRange(MemoryPage::MemoryIter range, size_t newSize)
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
typename MemoryPage<Range>::MemoryIter
MemoryPage<Range>::mergeRange(MemoryIter range, MemoryIter otherRange)
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
typename MemoryPage<Range>::ConstMemoryIter
MemoryPage<Range>::largestRange() const
{
	ConstMemoryIter max = begin();
	for (ConstMemoryIter iter = begin(); iter != end(); ++iter) {
		if (iter->size() > max->size()) {
			max = iter;
		}
	}
	return max;
}

template <typename Range>
size_t MemoryPage<Range>::freeSpaceSize()
{
	size_t result = 0;
	for (MemoryIter iter = begin(); iter != end(); ++iter) {
		if (iter->isFree()) {
			result += iter->size();
		}
	}
	return result;
}
