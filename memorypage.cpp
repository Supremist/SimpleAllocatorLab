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

MemoryPage::MemoryIter MemoryPage::findRange(size_t blockIndex)
{
	for (MemoryIter iter = begin(); iter != end(); ++iter) {
		if (blockIndex >= iter->start() && blockIndex < iter->end()) {
			return iter;
		}
	}
	return end();
}

MemoryPage::MemoryIter MemoryPage::allocate(size_t requestedSize)
{
	MemoryIter firstFree = findFree(requestedSize);
	if (firstFree != end() && requestedSize <= firstFree->size()) {
		MemoryRange tail = firstFree->split(requestedSize);
		if (tail.size() > 0) {
			auto insertPosition = firstFree;
			m_ranges.insert(++insertPosition, 1, tail);
		}
		firstFree->allocate();
		return firstFree;
	}
	return end();
}

void MemoryPage::free(MemoryPage::MemoryIter range)
{
	if(range == end())
		return;

	range->free();
	MemoryIter next = range;
	next++;
	if(next != end() && next->isFree()) {
		range->merge(*next);
		m_ranges.erase(next);
	}

	if (range == begin())
		return;

	MemoryIter previous = range;
	previous--;
	if (previous->isFree()) {
		previous->merge(*range);
		m_ranges.erase(previous);
	}

}

MemoryPage::MemoryIter MemoryPage::begin() const
{
	m_ranges.begin();
}

MemoryPage::MemoryIter MemoryPage::end() const
{
	m_ranges.end();
}
