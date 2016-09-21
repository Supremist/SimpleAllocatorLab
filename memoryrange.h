#pragma once

#include <cstdlib>

class MemoryRange {

public:
	MemoryRange(size_t rangeStart, size_t rangeSize, bool free = true);
	~MemoryRange();

	MemoryRange * findFree(size_t requestedSize);
	MemoryRange * findRange(size_t blockIndex);
	MemoryRange * allocRange(size_t requestedSize);
	void freeRange();
	void insertAfter(MemoryRange *newRange);

	size_t start;
	size_t size;
	bool isFree;
	MemoryRange *next;
};
