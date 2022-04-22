#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8

typedef struct Header {
	unsigned int data_size;
	bool used;
	struct Header* previous;
	struct Header* next;
} Header;

Header* head;
Header* tail;

bool is_head(Header* ptr) {
	return ptr == head;
}

bool is_tail(Header* ptr) {
	return ptr == tail;
}

void insert_header(Header* ptr) {
	if (ptr == NULL) {
		return;
	}

	else if (head == NULL) {
		head = ptr;
		tail = ptr;
	}

	else {
		tail->next = ptr;
		ptr->previous = tail;
		tail = ptr;
	}
}

Header* construct_new_header(unsigned int size) {
	Header* h = sbrk(sizeof(Header) + size);
	h->data_size = size;
	h->used = true;
	h->previous = NULL;
	h->next = NULL;

	return h;
}

Header* get_chunk_header(void* ptr) {
	return PTR_ADD_BYTES(ptr, sizeof(Header)*-1);
}

Header* get_worst_fit(unsigned int size) {
	Header* largest = NULL;
	Header* cur;

	for (cur = head; cur != NULL; cur = cur->next) {
		if (cur->data_size > size && cur->used == false) {
			largest = cur;
		}
	}

	return largest;
}

void print_headers() {
	Header* cur;
	for (cur = head; cur != NULL; cur = cur->next) {
		printf("%d, %d\n", cur->data_size, cur->used);
	}
}

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

	Header* largest = get_worst_fit(size);

if (largest == NULL) {
	Header* h = construct_new_header(size);
	insert_header(h);
	return  PTR_ADD_BYTES(tail, sizeof(Header));
}

	else {
		largest->used = true;
		return  PTR_ADD_BYTES(largest, sizeof(Header));
	}
}

void my_free(void* ptr) {
	if(ptr == NULL)
		return;

	Header* chunk_header = get_chunk_header(ptr);

	if (is_tail(chunk_header)) {
		tail = chunk_header->previous;
		brk(chunk_header);
	}

	else {
		chunk_header->used = false;
	}
}
