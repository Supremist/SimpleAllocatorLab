#include "abstractallocator.h"

AbstractAllocator::AbstractAllocator(size_t managedSize) :
	m_managedSize(managedSize)
{
	m_start = malloc(m_managedSize);
}

AbstractAllocator::~AbstractAllocator()
{
	free(m_start);
}

size_t AbstractAllocator::allocatedSize() const
{
	return managedSize() - freeSpaceSize();
}

size_t AbstractAllocator::managedSize() const
{
	return m_managedSize;
}

void *AbstractAllocator::startAddress() const
{
	return m_start;
}

void AbstractAllocator::resize(size_t newSize)
{
	m_managedSize = newSize;
}

size_t AbstractAllocator::pointerToPosition(void *pointer)
{
	return reinterpret_cast<size_t> (pointer) - reinterpret_cast<size_t> (m_start);
}

void *AbstractAllocator::positionToPointer(size_t position)
{
	return reinterpret_cast<void *> (position + reinterpret_cast<size_t> (m_start));
}
