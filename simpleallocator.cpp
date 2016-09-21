#include "simpleallocator.h"

SimpleAllocator::SimpleAllocator(size_t pageSize, size_t blockSize):
	m_blockSize(blockSize)
{
	m_size = getBlockCount(pageSize, m_blockSize);
	m_page = new MemoryRange (0, m_size);
	m_start = malloc(m_size * m_blockSize);
}

SimpleAllocator::~SimpleAllocator()
{
	delete m_page;
	free(m_start);
}

void *SimpleAllocator::mem_alloc(size_t size)
{
	auto blockCount = getBlockCount(size, m_blockSize);
	auto range = m_page->findFree(blockCount);
	if (!range)
		return nullptr;

	auto allocated = range->allocRange(blockCount);
	if (!allocated)
		return nullptr;

	return getBlockPointer(allocated->start);
}

void SimpleAllocator::mem_free(void *addr)
{
	auto range = m_page->findRange(getBlockIndex(addr));

}

size_t SimpleAllocator::getBlockCount(size_t pageSize, size_t blockSize)
{
	return (pageSize + 1) / blockSize;
}

size_t SimpleAllocator::getBlockIndex(void *block)
{
	return getBlockCount(block - m_start, m_blockSize);
}

void *SimpleAllocator::getBlockPointer(size_t blockIndex)
{
	return m_start + blockIndex * m_blockSize;
}
