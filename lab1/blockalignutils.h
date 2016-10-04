#pragma once

#include <cstdlib>

namespace BlockAlignUtils {

size_t alignUpper(size_t pageSize, size_t blockSize);
size_t alignLower(size_t pageSize, size_t blockSize);

}
