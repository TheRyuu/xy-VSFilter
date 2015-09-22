/************************************************************************/
/* author: xy                                                           */
/* date: 20110514                                                       */
/************************************************************************/
#include "xy_malloc.h"
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#define SAVED_PTR(x) ((void *)((uintptr_t) ((char *) x - sizeof (void *)) & ~(sizeof (void *) - 1)))
#define ALIGN_PTR(ptr, alignment, offset) \
    ((void *) ((((uintptr_t) ((char *) ptr + alignment + sizeof (void *) + offset)) & ~(alignment - 1)) - offset))

static const int kAlign = 16;

PartitionAllocatorGeneric xy_generic_allocator;

void* xy_malloc(size_t size)
{
    return xy_malloc(size, 0);
}

void* xy_malloc(size_t size, size_t offset)
{
    void *ret, *tmp;

    if ((tmp = partitionAllocGeneric(xy_generic_allocator.root(), size + kAlign + sizeof(void *))) == nullptr)
        return nullptr;

    ret = ALIGN_PTR(tmp, kAlign, offset);
    *((void **)SAVED_PTR(ret)) = tmp;
    return ret;
}

void xy_free(void* p)
{
    if (p) {
        void **saved = (void**)SAVED_PTR(p);
        partitionFreeGeneric(xy_generic_allocator.root(), *saved);
    }
}

// xy_realloc functions currently not used anywhere
#if 0
void* xy_realloc(void* p, size_t size)
{
    return _aligned_realloc(p, size, kAlign);
}

void* xy_realloc(void* p, size_t size, size_t align_shift)
{
    return _aligned_offset_realloc(p, size, kAlign, align_shift);
}
#endif
