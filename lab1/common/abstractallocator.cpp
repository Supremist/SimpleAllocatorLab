#include "abstractallocator.h"

AbstractAllocator::AbstractAllocator()
{
}

size_t AbstractAllocator::allocatedSize() const
{
	return managedSize() - freeSpaceSize();
}
