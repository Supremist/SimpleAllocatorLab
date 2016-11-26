#pragma once

#include <cstdlib>

class AllocatedBlock
{
public:
	AllocatedBlock(size_t size);
	virtual ~AllocatedBlock();

	void *startAddress() const;
	size_t pointerToPosition(void *pointer);
	void *positionToPointer(size_t position);

	size_t size() const;

private:
	void *m_start;
	size_t m_size;

};
