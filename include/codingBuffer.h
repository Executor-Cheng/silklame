#include <stdlib.h>
#include <memory.h>

typedef struct coding_buffer_s
{
	void* ptr;

	size_t length;

	size_t capacity;
} coding_buffer;

int coding_buffer_initialize(coding_buffer* buffer, size_t capacity);

void coding_buffer_ensuredappend(coding_buffer* buffer, void* ptr, size_t length);

int coding_buffer_append(coding_buffer* buffer, void* ptr, size_t length);

int coding_buffer_ensure_capacity(coding_buffer* buffer, size_t capacity, size_t growMultiplier);

int coding_buffer_set_capacity(coding_buffer* buffer, size_t length);

void coding_buffer_free(coding_buffer* buffer);