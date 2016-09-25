#include "simpleallocator.h"

SimpleAllocator::SimpleAllocator(size_t pageSize, size_t blockSize):
	m_blockSize(blockSize)
{
	m_size = align(pageSize, m_blockSize);
	m_page = new MemoryPage(m_size);
	m_start = malloc(m_size * m_blockSize);
}

SimpleAllocator::~SimpleAllocator()
{
	delete m_page;
	free(m_start);
}

void *SimpleAllocator::mem_alloc(size_t size)
{
	auto blockCount = align(size, m_blockSize);
	MemoryPage::MemoryIter allocated = m_page->allocate(blockCount);

	if (allocated == m_page->end())
		return nullptr;

	return getBlockPointer(allocated->start());
}

void *SimpleAllocator::mem_realloc(void *addr, size_t size)
{
	auto blockCount = align(size, m_blockSize);
	MemoryPage::MemoryIter range = m_page->findRange(getBlockIndex(addr));
	range = m_page->reallocate(range, blockCount);

	if (range == m_page->end())
		return nullptr;

	return getBlockPointer(range->start());
}

void SimpleAllocator::mem_free(void *addr)
{
	MemoryPage::MemoryIter range = m_page->findRange(getBlockIndex(addr));
	m_page->free(range);
}

const MemoryPage *SimpleAllocator::page() const
{
	return m_page;
}

size_t SimpleAllocator::blockSize() const
{
	return m_blockSize;
}

size_t SimpleAllocator::align(size_t pageSize, size_t blockSize)
{
	return (pageSize + blockSize - 1) / blockSize;
}

size_t SimpleAllocator::getBlockIndex(void *block)
{
	return align(reinterpret_cast<size_t> (block) - reinterpret_cast<size_t> (m_start), m_blockSize);
}

void *SimpleAllocator::getBlockPointer(size_t blockIndex)
{
	return reinterpret_cast<void *> (reinterpret_cast<size_t>(m_start) + blockIndex * m_blockSize);
}
