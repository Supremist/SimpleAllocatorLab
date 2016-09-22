#include "memoryrange.h"

MemoryRange::MemoryRange(size_t rangeStart, size_t rangeSize, bool free):
	m_start(rangeStart),
	m_size(rangeSize),
	is_free(free)
{
}

MemoryRange MemoryRange::split(size_t newSize)
{
	size_t old_size = m_size;
	resize(newSize);
	return MemoryRange(end(), old_size - newSize, is_free);
}

void MemoryRange::merge(const MemoryRange &other)
{
	size_t old_end = end();
	if (m_start > other.start()) {
		m_start = other.start();
	}
	if (old_end < other.end()){
		resize(other.end() - m_start);
	}
}

size_t MemoryRange::end()
{
	return m_start + m_size;
}

size_t MemoryRange::size()
{
	m_size;
}

bool MemoryRange::isFree()
{
	return is_free;
}

void MemoryRange::allocate()
{
	is_free = false;
}

void MemoryRange::free()
{
	is_free = true;
}
