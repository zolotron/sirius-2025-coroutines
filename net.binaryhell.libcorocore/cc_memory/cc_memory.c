#include "cc_memory.h"

cc_malloc__fptr cc_malloc;
cc_calloc__fptr cc_calloc;
cc_realloc__fptr cc_realloc;
cc_free__fptr cc_free;

cc_memset__fptr cc_memset;
cc_memcpy__fptr cc_memcpy;
cc_memdup__fptr cc_memdup;

cc_strdup__fptr cc_strdup;

void cc_memory_module_init( const cc_memory_module_init_params_t* params ) {
    cc_assert( params != NULL );
    cc_assert( params->malloc != NULL );
    cc_assert( params->calloc != NULL );
    cc_assert( params->realloc != NULL );
    cc_assert( params->free != NULL );
    cc_assert( params->memset != NULL );
    cc_assert( params->memcpy != NULL );
    cc_assert( params->memdup != NULL );
    cc_assert( params->strdup != NULL );

    cc_malloc = params->malloc;
    cc_calloc = params->calloc;
    cc_realloc = params->realloc;
    cc_free = params->free;

    cc_memset = params->memset;
    cc_memcpy = params->memcpy;
    cc_memdup = params->memdup;

    cc_strdup = params->strdup;
}
