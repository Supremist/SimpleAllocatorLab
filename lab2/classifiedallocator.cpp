#include "classifiedallocator.h"
#include <algorithm>

ClassifiedAllocator::ClassifiedAllocator(size_t pageCount, size_t pageSize, const set<size_t> &pageClassesSizes) :
	ClassifiedAllocator(vector<size_t>(pageCount, pageSize), pageClassesSizes)
{
}

ClassifiedAllocator::ClassifiedAllocator(const vector<size_t> &pageSizes, const set<size_t> &pageClassesSizes) :
	AbstractAllocator(std::accumulate(pageSizes.cbegin(), pageSizes.cend(), 0)),
	m_pageClassesSizes(pageClassesSizes)
{
	m_pages.reserve(pageSizes.size() + 1);
	BlockMemoryRange *current = new BlockMemoryRange(10);
	m_pages.push_back(current);
	for (size_t pageSize : pageSizes) {
		current = current->split(pageSize);
		if (current) {
			m_pages.push_back(current);
		} else {
			break;
		}
	}
}

ClassifiedAllocator::~ClassifiedAllocator()
{
	for (BlockMemoryRange *page : m_pages) {
		delete page;
	}
}

void ClassifiedAllocator::clearUnusedClasses()
{
	for (set<size_t>::iterator it = m_pageClassesSizes.begin(); it != m_pageClassesSizes.end(); ) {
		if (m_classifiedPageMap.count(*it) == 0) {
			it = m_pageClassesSizes.erase(it);
		} else {
			++it;
		}
	}
}

set<size_t> ClassifiedAllocator::pageClassesSizes() const
{
	return m_pageClassesSizes;
}

void ClassifiedAllocator::addPageClassesSizes(const set<size_t> &pageClassesSizes)
{
	m_pageClassesSizes.insert(pageClassesSizes.begin(), pageClassesSizes.end());
}

void *ClassifiedAllocator::mem_alloc(size_t size)
{

}
