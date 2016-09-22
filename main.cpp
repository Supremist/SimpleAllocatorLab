#include <iostream>
#include "simpleallocator.h"


int main(int argc, char *argv[])
{
	SimpleAllocator a(1000);
	std::cout << a.mem_alloc(10) << "\n";
	std::cout << a.mem_alloc(10) << "\n";
	//cout << "Hello World!" << endl;
    return 0;
}
