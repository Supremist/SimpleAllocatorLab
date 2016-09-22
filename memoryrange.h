#pragma once

#include <cstdlib>

class MemoryRange {

public:
	MemoryRange(size_t rangeStart, size_t rangeSize, bool free = true);

	void resize(size_t newSize);
	MemoryRange split(size_t newSize);
	void merge(const MemoryRange &other);
	size_t end() const;
	size_t start() const;
	size_t size() const;
	bool isFree();
	void allocate();
	void free();

private:
	size_t m_start;
	size_t m_size;
	bool is_free;
};
