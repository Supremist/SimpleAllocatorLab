#include "memorypage.h"

MemoryPage::MemoryPage(size_t pageSize)
{
	m_ranges.push_back(MemoryRange(0, pageSize));
}

MemoryPage::MemoryIter MemoryPage::findFree(size_t requestedSize)
{
	for (MemoryIter iter = m_ranges.begin(); iter != m_ranges.end(); ++iter) {
		if (iter->isFree() && iter->size() >= requestedSize) {
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
			m_ranges.insert(firstFree, 1, tail);
		}
		firstFree->allocate();
		return firstFree;
	}
	return end();
}

void MemoryPage::free(MemoryPage::MemoryIter range)
{
	range->free();
	MemoryIter previous = range;
	previous--;
	MemoryIter next = range;
	next++;

	if(next->isFree())
		range->merge(*next);

	if (previous->isFree())
		previous->merge(*range);

}

MemoryPage::MemoryIter MemoryPage::begin()
{
	m_ranges.begin();
}

MemoryPage::MemoryIter MemoryPage::end()
{
	m_ranges.end();
}
