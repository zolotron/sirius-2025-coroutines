#ifndef CC_MEMORY_H
#define CC_MEMORY_H

#include "cc_config.h"

typedef void* (* cc_malloc__fptr)( size_t size );
typedef void* (* cc_calloc__fptr)( size_t size );
typedef void* (* cc_realloc__fptr)( void* ptr, size_t size );
typedef void (* cc_free__fptr)( void* ptr );

typedef void* (* cc_memset__fptr)( void* ptr, int value, size_t length );
typedef void* (* cc_memcpy__fptr)( void* restrict dst, const void* restrict src, size_t length );
typedef void* (* cc_memdup__fptr)( const void* ptr, size_t length );

typedef char* (* cc_strdup__fptr)( const char* s );

typedef struct {
    cc_malloc__fptr malloc;
    cc_calloc__fptr calloc;
    cc_realloc__fptr realloc;
    cc_free__fptr free;

    cc_memset__fptr memset;
    cc_memcpy__fptr memcpy;
    cc_memdup__fptr memdup;

    cc_strdup__fptr strdup;
} cc_memory_module_init_params_t;

void cc_memory_module_init( const cc_memory_module_init_params_t* params );

extern cc_malloc__fptr cc_malloc;
extern cc_calloc__fptr cc_calloc;
extern cc_realloc__fptr cc_realloc;
extern cc_free__fptr cc_free;

extern cc_memset__fptr cc_memset;
extern cc_memcpy__fptr cc_memcpy;
extern cc_memdup__fptr cc_memdup;

extern cc_strdup__fptr cc_strdup;

#endif
