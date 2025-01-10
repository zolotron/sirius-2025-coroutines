#ifndef CC_INIT_H
#define CC_INIT_H

#include "cc_config.h"
#include "cc_log/cc_log.h"
#include "cc_memory/cc_memory.h"

typedef struct {
    cc_memory_module_init_params_t memory_params;
    cc_log_module_init_params_t log_params;
} cc_init_params_t;

void cc_init( const cc_init_params_t* params );

#endif
