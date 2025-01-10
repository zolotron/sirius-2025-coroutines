#ifndef CC_CONTEXT_INCLUDE
#   error "Do not include 'cc_context_x86_64_lnx.h' directly, include 'cc_context.h' instead"
#endif

#ifndef CC_CONTEXT_X86_64_LNX_H
#define CC_CONTEXT_X86_64_LNX_H

#include "cc_config.h"
#include "cc_stack.h"

typedef struct cc_context_t {
    void* context;
    cc_stack_t stack;
} cc_context_t;

#endif
