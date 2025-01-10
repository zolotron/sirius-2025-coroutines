#ifndef CC_STACK_H
#define CC_STACK_H

#include "cc_config.h"

#if defined(PLATFORM_WINDOWS)

typedef struct {
    void* handle;
    void* ptr;
    size_t total_size;
    size_t guard_size;
} cc_stack_t;

#elif defined(PLATFORM_LINUX)

typedef struct {
    void* ptr;
    size_t total_size;
    size_t guard_size;
} cc_stack_t;

#endif

void cc_stack_module_init( void );

void cc_stack_clear( cc_stack_t* self );
bool cc_stack_init( cc_stack_t* self, size_t size );
void cc_stack_deinit( cc_stack_t* self );

size_t cc_stack_get_minimal_size( void );

#endif
