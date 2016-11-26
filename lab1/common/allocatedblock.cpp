#include "allocatedblock.h"


AllocatedBlock::AllocatedBlock(size_t size) :
	m_size(size)
{
	m_start = malloc(size);
}

AllocatedBlock::~AllocatedBlock()
{
	free(m_start);
}

void *AllocatedBlock::startAddress() const
{
	return m_start;
}

size_t AllocatedBlock::pointerToPosition(void *pointer)
{
	return reinterpret_cast<size_t> (pointer) - reinterpret_cast<size_t> (m_start);
}

void *AllocatedBlock::positionToPointer(size_t position)
{
	return reinterpret_cast<void *> (position + reinterpret_cast<size_t> (m_start));
}

size_t AllocatedBlock::size() const
{
	return m_size;
}
