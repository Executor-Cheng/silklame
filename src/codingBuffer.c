#include <codingBuffer.h>

int coding_buffer_initialize(coding_buffer* buffer, size_t capacity)
{
	void* ptr = malloc(capacity);
	buffer->ptr = ptr;
	if (ptr == NULL)
	{
		return 1;
	}
	buffer->length = 0;
	buffer->capacity = capacity;
	return 0;
}

void coding_buffer_ensuredappend(coding_buffer* buffer, void* ptr, size_t length)
{
	memcpy((char*)buffer->ptr + buffer->length, ptr, length);
	buffer->length += length;
}

int coding_buffer_append(coding_buffer* buffer, void* ptr, size_t length)
{
	size_t estiminatedSize = buffer->length + length;
	if (estiminatedSize > buffer->capacity)
	{
		if (coding_buffer_set_capacity(buffer, estiminatedSize * 2) != 0)
		{
			return 1;
		}
	}
	coding_buffer_ensuredappend(buffer, ptr, length);
	return 0;
}

int coding_buffer_ensure_capacity(coding_buffer* buffer, size_t capacity, size_t growMultiplier)
{
	if (buffer->capacity >= capacity)
	{
		return 0;
	}
	return coding_buffer_set_capacity(buffer, capacity * growMultiplier);
}

int coding_buffer_set_capacity(coding_buffer* buffer, size_t capacity)
{
	void* ptr;
	ptr = realloc(buffer->ptr, capacity);
	if (ptr == NULL)
	{
		return 1;
	}
	buffer->ptr = ptr;
	buffer->capacity = capacity;
	return 0;
}

void coding_buffer_free(coding_buffer* buffer)
{
	if (buffer->ptr != NULL)
	{
		free(buffer->ptr);
	}
	buffer->ptr = NULL;
	buffer->length = 0;
	buffer->capacity = 0;
}
