#pragma once

#include <vector>
#include <map>
#include <set>
#include <abstractallocator.h>
#include "blockmemoryrange.h"
#include "blockalignutils.h"
#include "memorypage.h"

using std::vector;
using std::map;
using std::set;

class ClassifiedAllocator: AbstractAllocator
{
public:
	explicit ClassifiedAllocator(size_t pageCount, size_t pageSize, const set<size_t> &pageClassesSizes = {});
	explicit ClassifiedAllocator(const vector<size_t> &pageSizes = {}, const set<size_t> &pageClassesSizes = {});
	~ClassifiedAllocator();

	set<size_t> pageClassesSizes() const;
	void addPageClassesSizes(const set<size_t> &pageClassesSizes);
	void clearUnusedClasses();

	size_t pageSize();
	void setPageSize(size_t size);

	void * mem_alloc(size_t size) override;

private:
	size_t m_pageSize;
	set<size_t> m_pageClassesSizes;
	vector<BlockMemoryRange *> m_pages;
	map<size_t, BlockMemoryRange *> m_classifiedPageMap;

};
