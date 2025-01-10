#ifndef CC_CONTEXT_H
#define CC_CONTEXT_H

#include "cc_config.h"

#define CC_CONTEXT_INCLUDE

#if defined(PLATFORM_WINDOWS)
#  if defined(ARCH_X86_64)
#     define ARCH__DEFINED
#     include "cc_context_x86_64_win.h"
#  endif
#elif defined(PLATFORM_LINUX)
#  if defined(ARCH_X86_64)
#     define ARCH__DEFINED
#     include "cc_context_x86_64_lnx.h"
#  endif
#else
#  error "Target platform not specified, available platforms: PLATFORM_WINDOWS, PLATFORM_LINUX"
#endif

#if !defined(ARCH__DEFINED)
#  error "Target architecture not specified, available architectures: ARCH_X86_64"
#endif

#undef CC_CONTEXT_INCLUDE

typedef enum {
    CC_CONTEXT_CREATE_STACK_PARAM_NONE,
    CC_CONTEXT_CREATE_STACK_PARAM_COROUTINE,
} cc_context_stack_create_mode_t;

typedef struct {
    cc_context_stack_create_mode_t stack_create_mode;
    size_t stack_size;
    cc_functor__fptr main;
    const void* args;
} cc_context_create_params_t;

void cc_context_create_params_init( cc_context_create_params_t* params );

cc_context_t* cc_context_create( const cc_context_create_params_t* params );
void cc_context_destroy( cc_context_t* context );

void cc_context_switch( cc_context_t* context, cc_context_t* target );
void cc_context_entry_point( void );

#endif
