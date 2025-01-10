#ifndef CC_SCHEDULER_H
#define CC_SCHEDULER_H

#include "cc_context/cc_context.h"
#include "cc_container/cc_list.h"

typedef struct {
    cc_context_t context;
    cc_list_t active_queue;
    cc_list_t inactive_queue;
    cc_list_t waiting_io_queue;
} cc_scheduler_t;

cc_scheduler_t* cc_scheduler_create( void );
void cc_scheduler_destroy( cc_scheduler_t* scheduler );

void cc_scheduler_call_active( cc_scheduler_t* scheduler );

#endif
