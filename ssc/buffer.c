#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>

int alloc_buffer(struct Buffer *buff, unsigned int size)
{
	buff->mem = malloc(size);
	if (!buff->mem)
		return -1;
	buff->max = size;
	buff->used = 0;
	return 0;
}

void free_buffer(struct Buffer *buff)
{
	if (buff->mem)
	{
		free(buff->mem);
		buff->mem = NULL;
		buff->used = 0;
		buff->max = 0;
	}
}

int read_file_to_buffer(char *filename, struct Buffer *buff)
{
	FILE *input = fopen(filename, "rb");
	if (NULL == input)
	{
		return -1;
	}
	fseek(input, 0, SEEK_END);
	buff->used = ftell(input);
	fseek(input, 0, SEEK_SET);
	fread(buff->mem, buff->used, 1, input);
	fclose(input);
	return 0;
}

int write_buffer_to_file(char *filename, const struct Buffer *buff)
{
	FILE *output = fopen(filename, "wb");
	fwrite(buff->mem, buff->used, 1, output);
	fclose(output);
}
