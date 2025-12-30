#ifndef _H_BUFFER
#define _H_BUFFER

struct Buffer
{
	unsigned char *mem;
	unsigned int used;
	unsigned int max;
};

int alloc_buffer(struct Buffer *buff, unsigned int size);
void free_buffer(struct Buffer *buff);
int read_file_to_buffer(char *filename, struct Buffer *buff);
int write_buffer_to_file(char *filename, const struct Buffer *buff);

#endif
