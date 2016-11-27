#include <iostream>

#include "classifiedallocator.h"

using std::cout;


int main(int argc, char *argv[])
{
	ClassifiedAllocator alloc(4, 100, {3, 7, 18, 40});
	auto first = alloc.mem_alloc(20);
	alloc.mem_dump(cout);
	auto second = alloc.mem_alloc(150);
	alloc.mem_dump(cout);

	alloc.mem_realloc(first, 200);

//	alloc.mem_free(first);
//	alloc.mem_dump(cout);
//	alloc.mem_free(second);
	alloc.mem_dump(cout);
    return 0;
}
