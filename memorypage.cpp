#include "memorypage.h"

MemoryPage::MemoryPage(size_t pageSize)
{
	m_ranges.push_back(MemoryRange(0, pageSize));
}

MemoryPage::MemoryIter * MemoryPage::findFree(size_t requestedSize)
{
	for (MemoryIter iter = m_ranges.begin(); iter != m_ranges.end(); ++iter) {
		if (iter->isFree() && iter->size() >= requestedSize) {
			return iter;
		}
	}
	return nullptr;
}

MemoryPage::MemoryIter * MemoryPage::findRange(size_t blockIndex)
{
	for (MemoryIter iter = m_ranges.begin(); iter != m_ranges.end(); ++iter) {
		if (blockIndex >= iter->start() && blockIndex < iter->end()) {
			return iter;
		}
	}
}

MemoryPage::MemoryIter *MemoryPage::allocate(size_t requestedSize)
{

}
