#ifndef CC_COROUTINE_H
#define CC_COROUTINE_H

#include "cc_config.h"

#include "cc_container/cc_list.h"

typedef struct cc_context_t cc_context_t;
typedef struct cc_thread_t cc_thread_t;

typedef enum {
    CC_COROUTINE_STATE_DETACHED,
    CC_COROUTINE_STATE_ACTIVE,
    CC_COROUTINE_STATE_ACTIVE_RETURN,
    CC_COROUTINE_STATE_ACTIVE_SLEEP,
    CC_COROUTINE_STATE_ACTIVE_TRANSFER,
    CC_COROUTINE_STATE_ACTIVE_YIELD_CALLBACK,
    CC_COROUTINE_STATE_INACTIVE,
    CC_COROUTINE_STATE_WAITING_IO,
} cc_coroutine_state_t;

typedef struct {
    const char* name;
    cc_value_type_t name_value_type;
    void* args;
    size_t args_size;
    cc_value_type_t args_value_type;
    cc_functor__fptr main;
    bool create_detached;
} cc_coroutine_create_params_t;

typedef struct cc_coroutine_t {
    cc_context_t* context;
    cc_coroutine_create_params_t params;
    cc_coroutine_state_t state;
    uint64 id;
    uint64 wakeup_at;
    cc_thread_t* transfer_target;
    cc_list_t queue_link;
} cc_coroutine_t;

void cc_coroutine_create_params_init( cc_coroutine_create_params_t* params );
void cc_coroutine_create_params_deinit( cc_coroutine_create_params_t* params );
bool cc_coroutine_create_params_copy( cc_coroutine_create_params_t* d_params, const cc_coroutine_create_params_t* s_params );

cc_coroutine_t* cc_coroutine_create( const cc_coroutine_create_params_t* params );
void cc_coroutine_destroy( cc_coroutine_t* self );

void cc_coroutine_entry_point( void );
void cc_coroutine_yield( void );
void cc_coroutine_call( cc_coroutine_t* coroutine );
void cc_coroutine_invoke( cc_thread_t* target_thread, cc_functor__fptr method, void* args );
void cc_coroutine_transfer( cc_thread_t* target_thread );
void cc_coroutine_sleep( uint64 secs );
void cc_coroutine_msleep( uint64 msecs );
void cc_coroutine_usleep( uint64 usecs );
void cc_coroutine_change_state();

#endif
