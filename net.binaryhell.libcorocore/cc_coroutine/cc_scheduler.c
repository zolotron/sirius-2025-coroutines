#include "cc_scheduler.h"

#include "cc_memory/cc_memory.h"
#include "cc_thread/cc_thread.h"
#include "cc_coroutine.h"

cc_scheduler_t* cc_scheduler_create( void ) {
    cc_scheduler_t* result;

    result = cc_calloc( sizeof( cc_scheduler_t ) );
    if( result == NULL ) {
        return NULL;
    }

    cc_list_init( &result->active_queue );
    cc_list_init( &result->inactive_queue );
    cc_list_init( &result->waiting_io_queue );

    return result;
}

void cc_scheduler_destroy( cc_scheduler_t* scheduler ) {
    cc_assert( scheduler != NULL );
    cc_free( scheduler );
}

void cc_scheduler_call_active( cc_scheduler_t* scheduler ) {
    cc_coroutine_t* coroutine;

    cc_assert( scheduler != NULL );

    cc_list_foreach( &scheduler->active_queue, iter ) {
        coroutine = cc_container_of( iter, cc_coroutine_t, queue_link );
        thread->coroutine = coroutine;
        cc_context_switch( &scheduler->context, coroutine->context );
        thread->coroutine = NULL;

        if( coroutine->state == CC_COROUTINE_STATE_ACTIVE_RETURN ) {
            cc_list_pop( iter );
            cc_coroutine_destroy( coroutine );
        }
    }
}
