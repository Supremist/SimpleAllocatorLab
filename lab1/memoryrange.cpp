#include "memoryrange.h"

MemoryRange::MemoryRange(size_t rangeStart, size_t rangeSize, bool free):
	m_start(rangeStart),
	m_size(rangeSize),
	is_free(free)
{
}

void MemoryRange::resize(size_t newSize)
{
	m_size = newSize;
}

MemoryRange MemoryRange::split(size_t newSize)
{
	if (newSize > m_size)
		return MemoryRange(0, 0);

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

size_t MemoryRange::end() const
{
	return m_start + m_size;
}

size_t MemoryRange::start() const
{
	return m_start;
}

size_t MemoryRange::size() const
{
	return m_size;
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
