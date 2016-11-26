#include <iostream>

#include "classifiedallocator.h"

using std::cout;


int main(int argc, char *argv[])
{
	ClassifiedAllocator alloc(10, 100, {3, 7, 18, 40});
	cout << "hello";
    return 0;
}
