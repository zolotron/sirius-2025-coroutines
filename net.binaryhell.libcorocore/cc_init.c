#include "cc_init.h"

#include "cc_context/cc_stack.h"

void cc_init( const cc_init_params_t* params ) {
    cc_assert( params != NULL );

    cc_stack_module_init();
    cc_memory_module_init( &params->memory_params );
    cc_log_module_init( &params->log_params );
}
