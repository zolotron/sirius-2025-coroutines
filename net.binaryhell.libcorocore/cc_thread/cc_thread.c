#include "cc_thread.h"

#include "cc_memory/cc_memory.h"

__thread cc_thread_t* thread = NULL;

static void internal__thread_main( void* args );

void cc_thread_create_params_init( cc_thread_create_params_t* params ) {
    cc_assert( params != NULL );

    params->mode = CC_THREAD_CREATE_MODE_NEW_THREAD;
    params->name = NULL;
    params->name_value_type = CC_VALUE_TYPE_CONST;
    cc_coroutine_create_params_init( &params->coroutine_params );
}

void cc_thread_create_params_deinit( cc_thread_create_params_t* params ) {
    cc_assert( params != NULL );

    params->mode = CC_THREAD_CREATE_MODE_NEW_THREAD;

    if( ( params->name != NULL ) && ( params->name_value_type == CC_VALUE_TYPE_HEAP ) ) {
        cc_free( (void*)params->name );
    }
    params->name = NULL;
    params->name_value_type = CC_VALUE_TYPE_CONST;

    cc_coroutine_create_params_deinit( &params->coroutine_params );
}

bool cc_thread_create_params_copy( cc_thread_create_params_t* d_params, const cc_thread_create_params_t* s_params ) {
    cc_assert( d_params != NULL );
    cc_assert( s_params != NULL );

    cc_thread_create_params_deinit( d_params );

    d_params->mode = s_params->mode;

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

    if( !cc_coroutine_create_params_copy( &d_params->coroutine_params, &s_params->coroutine_params ) ) {
        cc_thread_create_params_deinit( d_params );
        return false;
    }

    return true;
}

#if defined(PLATFORM_LINUX)

static void* internal__thread_main_trampoline( void* param ) {
    internal__thread_main( param );
    return NULL;
}

cc_thread_t* cc_thread_create( const cc_thread_create_params_t* params ) {
    cc_thread_t* result;
    int ires;

    cc_assert( params != NULL );

    /* -- Allocate thread structure -------------------------------------- */

    result = cc_calloc( sizeof( cc_thread_t ) );
    if( result == NULL ) {
        goto rollback0;
    }

    /* -- Initialize thread parameters ----------------------------------- */

    cc_thread_create_params_init( &result->params );

    if( !cc_thread_create_params_copy( &result->params, params ) ) {
        goto rollback1;
    }

    /* -- Create scheduler ----------------------------------------------- */

    result->scheduler = cc_scheduler_create();
    if( result->scheduler == NULL ) {
        goto rollback2;
    }

    /* -- Create and start thread ---------------------------------------- */

    if( params->mode == CC_THREAD_CREATE_MODE_NEW_THREAD ) {
        ires = pthread_create( &result->handle, NULL, internal__thread_main_trampoline, result );
        if( ires != 0 ) {
            goto rollback3;
        }
    } else {
        thread = result;
    }

    return result;

rollback3:
    cc_scheduler_destroy( result->scheduler );
rollback2:
    cc_thread_create_params_deinit( &result->params );
rollback1:
    cc_free( result );
rollback0:
    return NULL;
}

#elif defined(PLATFORM_WINDOWS)

static DWORD internal__thread_main_trampoline( LPVOID param ) {
    internal__thread_main( param );
    return 0;
}

cc_thread_t* cc_thread_create( const cc_thread_create_params_t* params ) {
    cc_thread_t* result;

    cc_assert( params != NULL );

    /* -- Allocate thread structure -------------------------------------- */

    result = cc_calloc( sizeof( cc_thread_t ) );
    if( result == NULL ) {
        goto rollback0;
    }

    /* -- Initialize thread parameters ----------------------------------- */

    cc_thread_create_params_init( &result->params );

    if( !cc_thread_create_params_copy( &result->params, params ) ) {
        goto rollback1;
    }

    /* -- Create scheduler ----------------------------------------------- */

    result->scheduler = cc_scheduler_create();
    if( result->scheduler == NULL ) {
        goto rollback2;
    }

    /* -- Create and start thread ---------------------------------------- */

    if( params->mode == CC_THREAD_CREATE_MODE_NEW_THREAD ) {
        result->handle = CreateThread( NULL, 0, internal__thread_main_trampoline, result, 0, NULL );
        if( result->handle == NULL ) {
            goto rollback3;
        }
    } else {
        thread = result;
    }

    return result;

rollback3:
    cc_scheduler_destroy( result->scheduler );
rollback2:
    cc_thread_create_params_deinit( &result->params );
rollback1:
    cc_free( result );
rollback0:
    return NULL;
}

#endif

void cc_thread_destroy( cc_thread_t* self ) {

}

void cc_thread_yield( cc_thread_t* self ) {

}

bool cc_thread_join( cc_thread_t* self, uint64 msec ) {

}

void cc_thread_sleep( uint64 secs ) {

}

void cc_thread_msleep( uint64 msecs ) {

}

void cc_thread_usleep( uint64 usecs ) {

}

static void internal__thread_main( void* args ) {
    thread = (cc_thread_t*)args;

    /* -- Create coroutine scheduler ------------------------------------- */



}
