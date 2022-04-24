#include <stdio.h>
#include <unistd.h>

#include "mymalloc.h"

void test_worst_fit() {
	printf("Testing worst fit algorithm...\n");

	void* a = my_malloc(20);
	void* b = my_malloc(30);
	void* c = my_malloc(40);

	my_free(b);

	void* d = my_malloc(30);

	if (d != b) {
		printf("d should point to the same block as b\n");
	} else {
		printf("All good!\n");
	}

	my_free(a);
	my_free(c);
}

void test_coalesce() {
	printf("Testing coalescing algorithm...\n");

	void* a = my_malloc(16);
	void* b = my_malloc(16);
	void* c = my_malloc(50);

	my_free(a);
	my_free(b);

	void* d = my_malloc(20);

	if (a != d) {
		printf("a should point to the same block as d\n");
	} else {
		printf("All good!\n");
	}

	my_free(c);
}

void test_split() {
	printf("Testing splitting algorithm...\n");

	void* a = my_malloc(16);
	void* b = my_malloc(64);
	void* c = my_malloc(16);

	my_free(b);

	void* d = my_malloc(24);
	void* e = my_malloc(16);

	if (d == e) {
		printf("d should not reference the same block as e\n");
	} else {
		printf("All good!\n");
	}

	my_free(a);
	my_free(c);
	my_free(d);
	my_free(e);
}

int main() {
	// You can use sbrk(0) to get the current position of the break.
	// This is nice for testing cause you can see if the heap is the same size
	// before and after your tests, like here.
	void* heap_at_start = sbrk(0);

	void* block = my_malloc(100);
	my_free(block);

	void* heap_at_end = sbrk(0);
	unsigned int heap_size_diff = (unsigned int)(heap_at_end - heap_at_start);

	if(heap_size_diff)
		printf("Hmm, the heap got bigger by %u (0x%X) bytes...\n", heap_size_diff, heap_size_diff);

	// ADD MORE TESTS HERE.

	test_worst_fit();
	test_coalesce();
	test_split();

	return 0;
}
