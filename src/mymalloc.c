#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8

unsigned int round_up_size(unsigned int data_size) {
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

void* my_malloc(unsigned int size) {
	if(size == 0)
		return NULL;

	size = round_up_size(size);

	// ------- Don't remove anything above this line. -------
	// Here's where you'll put your code!

	// AT LEAST SOME OF IT

	// USE FUNCTIONS

	// PLEASE

	void* newBreak = sbrk(size);

	return newBreak;
}

void my_free(void* ptr) {
	if(ptr == NULL)
		return;

	// and here's where you free stuff.

	brk(ptr);
}
