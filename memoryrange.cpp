#include "memoryrange.h"

MemoryRange::MemoryRange(size_t rangeStart, size_t rangeSize, bool free):
	start(rangeStart),
	size(rangeSize),
	isFree(free),
	next(nullptr)
{
}

MemoryRange::~MemoryRange()
{
	MemoryRange *iter = next;
	while (iter != nullptr) {
		MemoryRange *tmp = iter;
		iter = iter->next;
		delete tmp;
	}
}

MemoryRange *MemoryRange::findFree(size_t requestedSize)
{
	for (MemoryRange * iter = this; iter != nullptr; iter = iter->next) {
		if (iter->isFree && iter->size >= requestedSize) {
			return iter;
		}
	}
	return nullptr;
}

MemoryRange *MemoryRange::findRange(size_t blockIndex)
{
	for (MemoryRange * iter = this; iter != nullptr; iter = iter->next) {
		if (blockIndex >= iter->start
				&& blockIndex <= iter->start + iter->size) {
			return iter;
		}
	}
	return nullptr;
}

MemoryRange *MemoryRange::allocRange(size_t requestedSize)
{
	if (requestedSize > size)
		return nullptr;

	insertAfter(new MemoryRange(start + requestedSize + 1, size - requestedSize));
	size = requestedSize;
	isFree = false;
}

void MemoryRange::freeRange()
{

}

void MemoryRange::insertAfter(MemoryRange *newRange)
{
	if(!newRange)
		return;

	MemoryRange *tail = next;
	next = newRange;
	MemoryRange *iter = newRange;
	while(iter->next) iter = iter->next;
	iter->next = tail;
}

