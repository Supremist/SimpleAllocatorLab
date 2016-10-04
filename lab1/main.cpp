#include <iostream>
#include "simpleallocator.h"

using std::cout;

int main(int argc, char *argv[])
{
	SimpleAllocator alloc(1000);
	void * first  = alloc.mem_alloc(7);
	void * second = alloc.mem_alloc(3);
	void * third  = alloc.mem_alloc(20);
	void * fourth  = alloc.mem_alloc(10);

//	cout << first  << "\n";
//	cout << second << "\n";
//	cout << third  << "\n";

//	alloc.mem_dump(cout);
//	alloc.mem_free(second);
//	alloc.mem_dump(cout);

//	cout << (second = alloc.mem_alloc(30)) << "\n";
//	alloc.mem_free(first);
//	alloc.mem_dump(cout);

//	alloc.mem_dump(cout);
//	alloc.mem_free(third);
//	alloc.mem_free(first);
//	alloc.mem_dump(cout);
//	alloc.mem_free(second);
//	alloc.mem_dump(cout);

	alloc.mem_dump(cout);
	alloc.mem_realloc(second, 3000);
	alloc.mem_dump(cout);
    return 0;
}
