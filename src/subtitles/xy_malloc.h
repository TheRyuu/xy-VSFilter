/************************************************************************/
/* author: xy                                                           */
/* date: 20110514                                                       */
/************************************************************************/
#ifndef SUBTITLES_XY_MALLOC_H_
#define SUBTITLES_XY_MALLOC_H_

#include "../../../PartitionAlloc/config.h"
#include "../../../PartitionAlloc/PartitionAlloc.h"

extern PartitionAllocatorGeneric xy_generic_allocator;

void* xy_malloc(size_t size);
void* xy_malloc(size_t size, size_t offset);
void xy_free(void* p);

// xy_realloc functions currently not used anywhere
#if 0
void* xy_realloc(void* p, size_t size);
void* xy_realloc(void* p, size_t size, size_t align_shift);
#endif
#endif // SUBTITLES_XY_MALLOC_H_
