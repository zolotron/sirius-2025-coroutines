#include "cc_context.h"

#include "cc_coroutine/cc_coroutine.h"

static void internal__default_coroutine_main( void* args ) {
    cc_unused( args );
}

void cc_context_create_params_init( cc_context_create_params_t* params ) {
    assert( params != NULL );
    params->stack_create_mode = CC_CONTEXT_CREATE_STACK_PARAM_COROUTINE;
    params->stack_size = cc_stack_get_minimal_size();
    params->main = internal__default_coroutine_main;
    params->args = NULL;
}
