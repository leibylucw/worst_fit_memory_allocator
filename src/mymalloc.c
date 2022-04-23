#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION 16
#define SIZE_MULTIPLE 8
#define MINIMUM_SPLIT_SIZE MINIMUM_ALLOCATION + sizeof(Header)

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

void construct_new_block(Header* block_header, unsigned int size, bool used) {
	block_header->data_size = size;
	block_header->used = used;
	block_header->previous = NULL;
	block_header->next = NULL;
}

Header* get_block_header(void* ptr) {
	return PTR_ADD_BYTES(ptr, sizeof(Header)*-1);
}

Header* get_block_data(void* ptr) {
	return PTR_ADD_BYTES(ptr, sizeof(Header));
}

Header* get_worst_fit_block(unsigned int size) {
	Header* largest_block = NULL;
	Header* cur;

	for (cur = head; cur != NULL; cur = cur->next) {
		if (cur->data_size > size && cur->used == false) {
			largest_block = cur;
		}
	}

	return largest_block;
}

bool can_coalesce(Header* block_header) {
	if (block_header != NULL && block_header->used == false ) {
		return true;
	} else {
		return false;
	}
}

void make_neighbors(Header* block_header_1, Header* block_header_2) {
	block_header_1->next = block_header_2;
	block_header_2->previous = block_header_1;
}

void coalesce(Header* cur) {
	Header* neighbor1 = cur->next;
	Header* neighbor2 = neighbor1->next;

	if (neighbor2 != NULL) {
	cur->next = neighbor2;
	neighbor2->previous = cur;
	unsigned int extra_size = neighbor1->data_size + sizeof(Header);
	cur->data_size += extra_size;
	} else {
	cur->next = NULL;
	}

	if (is_tail(neighbor1)) {
		tail = cur;
	}
}

void split(Header* old_header, unsigned int size) {
	if (old_header->data_size - (size + sizeof(Header)) >= MINIMUM_SPLIT_SIZE) {
		Header* new_header = PTR_ADD_BYTES(old_header, sizeof(Header) + old_header->data_size);
		construct_new_block(new_header, size, false);

		Header* right_most_neighbor = old_header->next;
		make_neighbors(old_header, new_header);
		make_neighbors(new_header, right_most_neighbor);
	}
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

	Header* largest_block = get_worst_fit_block(size);

if (largest_block == NULL) {
	Header* block_header = sbrk(sizeof(Header) + size);
	construct_new_block(block_header, size, true);
	insert_header(block_header);
	return  get_block_data(block_header);
}

	else {
		largest_block->used = true;
		split(largest_block, size);
		return  get_block_data(largest_block);
	}
}

void my_free(void* ptr) {
	if(ptr == NULL)
		return;

	Header* cur = get_block_header(ptr);
	Header* left_neighbor = cur->previous;
	Header* right_neighbor = cur->next;

	cur->used = false;

	if (can_coalesce(right_neighbor)) {
		coalesce(cur);
	}

	if (can_coalesce(left_neighbor)) {
		coalesce(left_neighbor);
	}

	if (tail->used == false) {
		tail = cur->previous;
		brk(cur);
	}
}
