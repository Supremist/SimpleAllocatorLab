#pragma once

#include "classifiedmemorypage.h"
#include <vector>
#include <map>
#include <set>

class ClassifiedAllocator
{
public:
	using std::vector;
	using std::map;
	using std::set;

	ClassifiedAllocator(size_t size, size_t pageSize, const set<size_t> &blockSizes = vector());



	set<size_t> blockSizes() const;
	void setBlockSizes(const set<size_t> &blockSizes);

private:
	void initBlockSizes(size_t smalestSize, size_t biggestSize, size_t factor = 2);

	vector<ClassifiedMemoryPage *> m_pages;
	set<size_t> m_blockSizes;
	map<size_t, ClassifiedMemoryPage *> m_freePageMap;

};
