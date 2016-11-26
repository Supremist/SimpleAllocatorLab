#pragma once

#include <vector>
#include <unordered_map>
#include <set>
#include <memory>
#include <abstractallocator.h>
#include "blockmemoryrange.h"
#include "blockalignutils.h"
#include "memorymanager.h"
#include "allocatedblock.h"
#include "pageinfo.h"

using std::vector;
using std::list;
using std::unordered_map;
using std::set;

using ClassifiedPageMap = unordered_map<size_t, PagesList*>;

class ClassifiedAllocator: AbstractAllocator
{
public:

	explicit ClassifiedAllocator(size_t pageCount, size_t pageSize, const set<size_t> &pageClassesSizes = {});
	~ClassifiedAllocator();

	set<size_t> pageClassesSizes() const;
	void addPageClassesSizes(const set<size_t> &pageClassesSizes);
	void clearUnusedClasses();

	size_t pageSize();
	void setPageSize(size_t size);

	void * mem_alloc(size_t size) override;
	void mem_free(void *addr) override;
	void mem_dump(ostream &output, void *start = nullptr, size_t dumpSize = 0) override;
	void * mem_realloc(void *addr, size_t size) override;

	size_t largestFreeBlockSize() const override;
	size_t freeSpaceSize() const override;
	size_t managedSize() const override;

private:
	void updatePagesInfo(const PageInfo::Ptr &newPage);
	PageInfo::Ptr getPageInfo(size_t pageIndex);
	PagesList * getEqualySizedPages(size_t blockSize);

	PagesList::iterator findFreePage(PagesList *pages) const;

	size_t m_pageSize;
	AllocatedBlock m_memoryBlock;
	set<size_t> m_pageClassesSizes;
	ClassifiedManager * m_manager;
	ClassifiedPageMap m_classifiedPageMap;
	vector<PageInfo::Ptr> m_pagesInfo;
};
