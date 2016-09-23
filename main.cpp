#include <iostream>
#include "simpleallocator.h"

using std::cout;

void mem_dump(const MemoryPage * page, const size_t block_size) {
	cout << "Page dump:\n";
	for (MemoryPage::MemoryIter iter = page->begin(); iter != page->end(); ++iter) {
		cout << iter->start() * block_size << " - " << iter->end() * block_size<< " ";
		if (iter->isFree()) {
			cout << "Free";
		} else {
			cout << "Allocated";
		}
		cout << "\n";
	}

}


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

//	mem_dump(alloc.page(), alloc.blockSize());
//	alloc.mem_free(second);
//	mem_dump(alloc.page(), alloc.blockSize());

//	cout << (second = alloc.mem_alloc(30)) << "\n";
//	alloc.mem_free(first);
//	mem_dump(alloc.page(), alloc.blockSize());

//	mem_dump(alloc.page(), alloc.blockSize());
//	alloc.mem_free(third);
//	alloc.mem_free(first);
//	mem_dump(alloc.page(), alloc.blockSize());
//	alloc.mem_free(second);
//	mem_dump(alloc.page(), alloc.blockSize());

	mem_dump(alloc.page(), alloc.blockSize());
	alloc.mem_realloc(second, 3000);
	mem_dump(alloc.page(), alloc.blockSize());
    return 0;
}
