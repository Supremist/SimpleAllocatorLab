#pragma once

#include <cstdlib>
#include <set>

namespace BlockAlignUtils {

size_t alignUpper(size_t pageSize, size_t blockSize);
size_t alignLower(size_t pageSize, size_t blockSize);

std::set<size_t> generateBlockSizes(size_t smalestSize, size_t biggestSize, size_t factor = 2);

}
