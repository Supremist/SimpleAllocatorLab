#include "simpleallocator.h"

SimpleAllocator::SimpleAllocator(size_t pageSize, size_t blockSize):
	AbstractAllocator(),
	m_blockSize(blockSize),
	m_memoryBlock(BlockAlignUtils::alignUpper(pageSize, blockSize) * blockSize)
{
	m_page = new MemoryManager<MemoryRange>(managedSize() / m_blockSize);
}

SimpleAllocator::~SimpleAllocator()
{
	delete m_page;
}

void *SimpleAllocator::mem_alloc(size_t size)
{
	auto blockCount = BlockAlignUtils::alignUpper(size, m_blockSize);
	MemoryManager<MemoryRange>::MemoryIter allocated = m_page->allocate(blockCount);

	if (allocated == m_page->end())
		return nullptr;

	return getBlockPointer(allocated->start());
}

void *SimpleAllocator::mem_realloc(void *addr, size_t size)
{
	auto blockCount = BlockAlignUtils::alignUpper(size, m_blockSize);
	MemoryManager<MemoryRange>::MemoryIter range = m_page->findRange(getBlockIndex(addr));
	range = m_page->reallocate(range, blockCount);

	if (range == m_page->end())
		return nullptr;

	return getBlockPointer(range->start());
}

void SimpleAllocator::mem_free(void *addr)
{
	MemoryManager<MemoryRange>::MemoryIter range = m_page->findRange(getBlockIndex(addr));
	m_page->free(range);
}

void SimpleAllocator::mem_dump(ostream &output, void *start, size_t dumpSize)
{
	size_t startBlockIndex = getBlockIndex(start);
	MemoryManager<MemoryRange>::MemoryIter startIter;
	if (!start) {
		startIter = m_page->begin();
	} else {
		startIter = m_page->findRange(startBlockIndex);
	}

	MemoryManager<MemoryRange>::MemoryIter endIter;
	if (dumpSize >= managedSize() || dumpSize == 0) {
		endIter = m_page->end();
	} else {
		endIter = m_page->findRange(startBlockIndex + BlockAlignUtils::alignUpper(dumpSize, m_blockSize), startIter);
	}

	output << "Page dump:\n";
	for (MemoryManager<MemoryRange>::MemoryIter iter = startIter; iter != endIter; ++iter) {
		output << iter->start() * m_blockSize << " - " << iter->end() * m_blockSize<< " ";
		if (iter->isFree()) {
			output << "Free";
		} else {
			output << "Allocated";
		}
		output << "\n";
	}
}

size_t SimpleAllocator::managedSize() const
{
	return m_memoryBlock.size();
}

size_t SimpleAllocator::freeSpaceSize() const
{
	return m_page->freeSpaceSize();
}

size_t SimpleAllocator::largestFreeBlockSize() const
{
	return m_page->largestFreeRange()->size();
}

const MemoryManager<MemoryRange> *SimpleAllocator::page() const
{
	return m_page;
}

size_t SimpleAllocator::blockSize() const
{
	return m_blockSize;
}

size_t SimpleAllocator::getBlockIndex(void *block)
{
	return BlockAlignUtils::alignUpper(m_memoryBlock.pointerToPosition(block), m_blockSize);
}

void *SimpleAllocator::getBlockPointer(size_t blockIndex)
{
	return m_memoryBlock.positionToPointer(blockIndex * m_blockSize);
}
