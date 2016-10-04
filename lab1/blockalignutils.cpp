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


}
