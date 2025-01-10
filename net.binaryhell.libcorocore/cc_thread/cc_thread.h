#ifndef CC_THREAD_H
#define CC_THREAD_H

#if defined(PLATFORM_LINUX)
#  include <pthread.h>
#elif defined(PLATFORM_WINDOWS)
#  include <windows.h>
#endif

#include "cc_coroutine/cc_scheduler.h"
#include "cc_coroutine/cc_coroutine.h"

typedef enum {
    CC_THREAD_CREATE_MODE_NEW_THREAD = 0,
    CC_THREAD_CREATE_MODE_ATTACH_THREAD = 1
} cc_thread_create_mode_t;

typedef struct {
    cc_thread_create_mode_t mode;
    const char* name;
    cc_value_type_t name_value_type;
    cc_coroutine_create_params_t coroutine_params;
} cc_thread_create_params_t;

typedef struct cc_thread_t {
    /* parameters and constant context */
    cc_thread_create_params_t params;
    cc_scheduler_t* scheduler;
    cc_coroutine_t* coroutine;
    /* platform dependent context  */
#   if defined(PLATFORM_LINUX)
    pthread_t handle;
#   elif defined(PLATFORM_WINDOWS)
    HANDLE handle;
#   endif
} cc_thread_t;

extern __thread cc_thread_t* thread;

void cc_thread_create_params_init( cc_thread_create_params_t* params );
void cc_thread_create_params_deinit( cc_thread_create_params_t* params );
bool cc_thread_create_params_copy( cc_thread_create_params_t* d_params, const cc_thread_create_params_t* s_params );

cc_thread_t* cc_thread_create( const cc_thread_create_params_t* params );
void cc_thread_destroy( cc_thread_t* self );

void cc_thread_yield( cc_thread_t* self );
bool cc_thread_join( cc_thread_t* self, uint64 msec );

void cc_thread_sleep( uint64 secs );
void cc_thread_msleep( uint64 msecs );
void cc_thread_usleep( uint64 usecs );

#endif
