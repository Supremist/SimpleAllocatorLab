#include "blockalignutils.h"

namespace BlockAlignUtils {

size_t alignUpper(size_t pageSize, size_t blockSize)
{
	return (pageSize + blockSize - 1) / blockSize;
}

size_t alignLower(size_t pageSize, size_t blockSize)
{
	return pageSize / blockSize;
}

std::set<size_t> generateBlockSizes(size_t smalestSize, size_t biggestSize, size_t factor)
{
	std::set<size_t> result;
	for(size_t item = smalestSize; item <= biggestSize; item *= factor) {
		result.insert(item);
	}
	return result;
}


}
