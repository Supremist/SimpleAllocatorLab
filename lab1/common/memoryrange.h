#pragma once

#include <cstdlib>

class MemoryRange
{

public:
	MemoryRange(size_t rangeSize);
	MemoryRange(size_t rangeStart, size_t rangeSize, bool free = true);

	virtual void resize(size_t newSize);
	virtual MemoryRange *split(size_t newSize);
	void merge(const MemoryRange &other);
	size_t end() const;
	size_t start() const;
	size_t size() const;
	bool isFree() const;
	void allocate();
	void free();

	MemoryRange & operator = (const MemoryRange & other) = default;

private:
	size_t m_start;
	size_t m_size;
	bool is_free;
};
