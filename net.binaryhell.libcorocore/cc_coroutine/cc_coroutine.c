#include "cc_coroutine.h"

#include "cc_context/cc_context.h"
#include "cc_memory/cc_memory.h"
#include "cc_thread/cc_thread.h"
#include "cc_time/cc_time.h"
#include "cc_log/cc_log.h"

static void internal__dummy_coroutine( void* context ) {
    cc_unused( context );
}

void cc_coroutine_create_params_init( cc_coroutine_create_params_t* params ) {
    cc_assert( params != NULL );

    params->name = NULL;
    params->name_value_type = CC_VALUE_TYPE_CONST;
    params->args = NULL;
    params->args_size = 0;
    params->args_value_type = CC_VALUE_TYPE_CONST;
    params->main = internal__dummy_coroutine;
    params->create_detached = false;
}

void cc_coroutine_create_params_deinit( cc_coroutine_create_params_t* params ) {
    cc_assert( params != NULL );

    if( ( params->name != NULL ) && ( params->name_value_type == CC_VALUE_TYPE_HEAP ) ) {
        cc_free( (void*)params->name );
    }
    params->name = NULL;
    params->name_value_type = CC_VALUE_TYPE_CONST;

    if( ( params->args != NULL ) && ( params->args_value_type == CC_VALUE_TYPE_HEAP ) ) {
        cc_free( params->args );
    }
    params->args = NULL;
    params->args_size = 0;
    params->args_value_type = CC_VALUE_TYPE_CONST;

    params->main = internal__dummy_coroutine;
}

bool cc_coroutine_create_params_copy( cc_coroutine_create_params_t* d_params, const cc_coroutine_create_params_t* s_params ) {
    cc_assert( d_params != NULL );
    cc_assert( s_params != NULL );

    cc_coroutine_create_params_deinit( d_params );

    if( s_params->name_value_type == CC_VALUE_TYPE_CONST ) {
        d_params->name = s_params->name;
        d_params->name_value_type = CC_VALUE_TYPE_CONST;
    } else {
        d_params->name = cc_strdup( s_params->name );
        if( d_params->name == NULL ) {
            return false;
        }
        d_params->name_value_type = CC_VALUE_TYPE_HEAP;
    }

    if( s_params->args_value_type == CC_VALUE_TYPE_CONST ) {
        d_params->args = s_params->args;
        d_params->args_value_type = CC_VALUE_TYPE_CONST;
    } else {
        d_params->args = cc_malloc( s_params->args_size );
        if( d_params->args == NULL ) {
            cc_coroutine_create_params_deinit( d_params );
            return false;
        }
        cc_memcpy( d_params->args, s_params->args, s_params->args_size );
        d_params->args_value_type = CC_VALUE_TYPE_HEAP;
    }
    d_params->args_size = s_params->args_size;

    d_params->main = s_params->main;
    d_params->create_detached = s_params->create_detached;

    return true;
}

cc_coroutine_t* cc_coroutine_create( const cc_coroutine_create_params_t* params ) {
    cc_coroutine_t* result;
    cc_context_create_params_t context_create_params;

    cc_assert( params != NULL );
    cc_assert( params->main != NULL );

    result = cc_calloc( sizeof( cc_coroutine_t ) );
    if( result == NULL ) {
        goto rollback0;
    }

    /* -- Initialize parameters ------------------------------------------ */

    cc_coroutine_create_params_init( &result->params );

    if( !cc_coroutine_create_params_copy( &result->params, params ) ) {
        goto rollback1;
    }

    /* -- Create context ------------------------------------------------- */

    cc_context_create_params_init( &context_create_params );
    context_create_params.main = params->main;
    context_create_params.args = params->args;
    result->context = cc_context_create( &context_create_params );
    if( result->context == NULL ) {
        goto rollback2;
    }

    if( params->create_detached ) {
        result->state = CC_COROUTINE_STATE_DETACHED;
    } else {
        cc_assert( thread != NULL );
        result->state = CC_COROUTINE_STATE_ACTIVE;
        cc_list_push_back( &thread->scheduler->active_queue, &result->queue_link );
    }

    return result;

rollback2:
    cc_coroutine_create_params_deinit( &result->params );
rollback1:
    cc_free( result );
rollback0:
    return NULL;
}

void cc_coroutine_destroy( cc_coroutine_t* self ) {
    if( self == NULL ) {
        return;
    }
    cc_context_destroy( self->context );
    cc_coroutine_create_params_deinit( &self->params );
    cc_free( self );
}

void cc_coroutine_entry_point( void ) {
    //cc_log_info("CORO-1");
    thread->coroutine->params.main( thread->coroutine->params.args );
    //cc_log_info("CORO-2");
    thread->coroutine->state = CC_COROUTINE_STATE_ACTIVE_RETURN;
    cc_context_switch( thread->coroutine->context, &thread->scheduler->context );
}

void cc_coroutine_yield( void ) {
    cc_context_switch( thread->coroutine->context, &thread->scheduler->context );
}

void cc_coroutine_call( cc_coroutine_t* coroutine ) {
    cc_context_switch( &thread->scheduler->context, coroutine->context );
}

void cc_coroutine_invoke( cc_thread_t* target_thread, cc_functor__fptr method, void* args ) {
    cc_thread_t* source_thread;
    cc_coroutine_t* coroutine;

    cc_assert( target_thread != NULL );
    cc_assert( method != NULL );

    source_thread = thread;
    coroutine = thread->coroutine;

    /* Transfer current coroutine to target thread */
    coroutine->state = CC_COROUTINE_STATE_ACTIVE_TRANSFER;
    coroutine->transfer_target = target_thread;
    cc_context_switch( coroutine->context, &thread->scheduler->context );

    /* Now we are on the target side, execute our logic */
    method( args );

    /* And finally transfer coroutine to source thread */
    coroutine->state = CC_COROUTINE_STATE_ACTIVE_TRANSFER;
    coroutine->transfer_target = source_thread;
    cc_context_switch( coroutine->context, &thread->scheduler->context );
}

void cc_coroutine_transfer( cc_thread_t* target_thread ) {
    cc_coroutine_t* coroutine;

    cc_assert( target_thread != NULL );

    coroutine = thread->coroutine;

    /* Transfer current coroutine to target thread */
    coroutine->state = CC_COROUTINE_STATE_ACTIVE_TRANSFER;
    coroutine->transfer_target = target_thread;
    cc_context_switch( coroutine->context, &thread->scheduler->context );
}

void cc_coroutine_sleep( uint64 secs ) {
    cc_coroutine_t* coroutine;
    coroutine = thread->coroutine;
    coroutine->wakeup_at = cc_time_monotonic() + secs * 1000000000ull;
    coroutine->state = CC_COROUTINE_STATE_ACTIVE_SLEEP;
    cc_context_switch( coroutine->context, &thread->scheduler->context );
    coroutine->wakeup_at = 0ull;
}

void cc_coroutine_msleep( uint64 msecs ) {
    cc_coroutine_t* coroutine;
    coroutine = thread->coroutine;
    coroutine->wakeup_at = cc_time_monotonic() + msecs * 1000000ull;
    coroutine->state = CC_COROUTINE_STATE_ACTIVE_SLEEP;
    cc_context_switch( coroutine->context, &thread->scheduler->context );
    coroutine->wakeup_at = 0ull;
}

void cc_coroutine_usleep( uint64 usecs ) {
    cc_coroutine_t* coroutine;
    coroutine = thread->coroutine;
    coroutine->wakeup_at = cc_time_monotonic() + usecs * 1000;
    coroutine->state = CC_COROUTINE_STATE_ACTIVE_SLEEP;
    cc_context_switch( coroutine->context, &thread->scheduler->context );
    coroutine->wakeup_at = 0ull;
}
