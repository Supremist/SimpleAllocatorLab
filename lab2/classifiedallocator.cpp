#include "classifiedallocator.h"

ClassifiedAllocator::ClassifiedAllocator(size_t size)
{

}

set<size_t> ClassifiedAllocator::blockSizes() const
{
	return m_blockSizes;
}

void ClassifiedAllocator::setBlockSizes(const set<size_t> &sizes)
{
	m_blockSizes = sizes;
}
