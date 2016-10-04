#include "memorypage.h"

MemoryPage::MemoryPage(size_t pageSize)
{
	m_ranges.push_back(MemoryRange(0, pageSize));
}

MemoryPage::MemoryIter MemoryPage::findFree(size_t requestedSize)
{
	for (MemoryIter iter = m_ranges.begin(); iter != m_ranges.end(); ++iter) {
		if (iter->isFree() && iter->size() >= requestedSize) {
			auto a = iter->size();
			return iter;
		}
	}
	return m_ranges.end();
}

MemoryPage::MemoryIter MemoryPage::findRange(size_t blockIndex, MemoryIter searchFrom)
{
	for (MemoryIter iter = searchFrom; iter != end(); ++iter) {
		if (blockIndex >= iter->start() && blockIndex < iter->end()) {
			return iter;
		}
	}
	return end();
}

MemoryPage::MemoryIter MemoryPage::findRange(size_t blockIndex)
{
	return findRange(blockIndex, begin());
}

MemoryPage::MemoryIter MemoryPage::restoreBlock(const MemoryRange &range, MemoryPage::MemoryIter searchFrom)
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

MemoryPage::MemoryIter MemoryPage::allocate(size_t requestedSize)
{
	MemoryIter firstFree = findFree(requestedSize);
	if (firstFree != end() && requestedSize <= firstFree->size()) {
		splitRange(firstFree, requestedSize);
		firstFree->allocate();
		return firstFree;
	}
	return end();
}

MemoryPage::MemoryIter MemoryPage::reallocate(MemoryPage::MemoryIter range, size_t newSize)
{
	MemoryRange backup = *range;
	free(range);
	MemoryIter result = allocate(newSize);
	if (result == end())
		restoreBlock(backup, begin());
	return result;
}

void MemoryPage::free(MemoryPage::MemoryIter range)
{
	if(range == end())
		return;

	range->free();
	for (int shift : {1, -1}) {
		MemoryIter other = range + shift;
		if(other != end() && other->isFree())
			mergeRange(range, other);
	}
}

MemoryPage::ConstMemoryIter MemoryPage::begin() const
{
	return m_ranges.begin();
}

MemoryPage::ConstMemoryIter MemoryPage::end() const
{
	return m_ranges.end();
}

MemoryPage::MemoryIter MemoryPage::begin()
{
	return m_ranges.begin();
}

MemoryPage::MemoryIter MemoryPage::end()
{
	return m_ranges.end();
}

MemoryPage::MemoryIter MemoryPage::splitRange(MemoryPage::MemoryIter range, size_t newSize)
{
	if(range == end())
		return end();

	MemoryRange tail = range->split(newSize);
	if (tail.size() > 0) {
		auto insertPosition = range;
		m_ranges.insert(++insertPosition, 1, tail);
		return insertPosition;
	}
	return end();
}

MemoryPage::MemoryIter MemoryPage::mergeRange(MemoryIter range, MemoryIter otherRange)
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

MemoryPage::ConstMemoryIter MemoryPage::largestRange() const
{
	ConstMemoryIter max = begin();
	for (ConstMemoryIter iter = begin(); iter != end(); ++iter) {
		if (iter->size() > max->size()) {
			max = iter;
		}
	}
	return max;
}

size_t MemoryPage::freeSpaceSize()
{
	size_t result = 0;
	for (MemoryIter iter = begin(); iter != end(); ++iter) {
		if (iter->isFree()) {
			result += iter->size();
		}
	}
	return result;
}

MemoryPage::MemoryIter operator +(const MemoryPage::MemoryIter &iter, int i)
{
	MemoryPage::MemoryIter result = iter;
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

MemoryPage::MemoryIter operator -(const MemoryPage::MemoryIter &iter, int i)
{
	MemoryPage::MemoryIter result = iter;
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
