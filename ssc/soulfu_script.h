#ifndef _H_SOULFU_SCRIPT
#define _H_SOULFU_SCRIPT

#include "buffer.h"

enum SSError
{
	SSE_NONE,
	SSE_EXTERNAL,
	SSE_HEADERIZE,
	SSE_COMPILERIZE,
	SSE_END
};

// path needs to be allocated during each API call
void set_working_dir(char *path);
enum SSError src_headerize(struct Buffer *script, struct Buffer *run);
// filename without extension
enum SSError src_compilerize(struct Buffer *script, struct Buffer *run, char *filename);
signed char src_define_setup(char *dirpath);
void sfs_init(void);

#endif
