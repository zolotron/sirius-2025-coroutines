#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cc_init.h"
#include "cc_coroutine/cc_coroutine.h"
#include "cc_thread/cc_thread.h"

/* -- Initialization code ------------------------------------------------- */

void* test_malloc( size_t size )
{
    return malloc( size );
}

void* test_calloc( size_t size )
{
    return calloc( size, 1 );
}

void* test_realloc( void* ptr, size_t size )
{
    return realloc( ptr, size );
}

void test_free( void* ptr )
{
    free( ptr );
}

void* test_memset( void* ptr, int value, size_t length )
{
    return memset( ptr, value, length );
}

void* test_memcpy( void* restrict dst, const void* restrict src, size_t length )
{
    return memcpy( dst, src, length );
}

void* test_memdup( const void* ptr, size_t length )
{
    void* result;
    result = malloc( length );
    if( result == NULL ) return NULL;
    memcpy( result, ptr, length );
    return result;
}

char* test_strdup( const char* s )
{
    return strdup( s );
}

void test_log_write( const char* line, size_t size )
{
    fwrite( line, size, 1, stdout );
}

void initialize()
{
    cc_init_params_t iparams;
    cc_thread_create_params_t tparams;

    printf( "main : enter (initialize libcorocore)\n" );

    iparams.memory_params.malloc = test_malloc;
    iparams.memory_params.calloc = test_calloc;
    iparams.memory_params.realloc = test_realloc;
    iparams.memory_params.free = test_free;
    iparams.memory_params.memset = test_memset;
    iparams.memory_params.memcpy = test_memcpy;
    iparams.memory_params.memdup = test_memdup;
    iparams.memory_params.strdup = test_strdup;
    iparams.log_params.log_format = CC_LOG_FORMAT_NONE;
    iparams.log_params.log_write = test_log_write;

    cc_init( &iparams );

    printf( "main : enter (attach main thread)\n" );

    cc_thread_create_params_init( &tparams );
    tparams.mode = CC_THREAD_CREATE_MODE_ATTACH_THREAD;
    tparams.name = "main";

    cc_thread_create( &tparams );
}

/* -- Example starts here ------------------------------------------------- */

void coro1( void* args )
{
    printf( "coro1: enter (args = %p)\n", args );
    cc_coroutine_yield();
    printf( "coro1: leave\n" );
}

void coro2( void* args )
{
    printf( "coro2: enter (args = %p)\n", args );
    cc_coroutine_yield();
    printf( "coro2: step 1\n" );
    cc_coroutine_yield();
    printf( "coro2: leave\n" );
}

void coro3( void* args )
{
    printf( "coro3: enter (args = %p)\n", args );
    cc_coroutine_yield();
    printf( "coro3: step 1\n" );
    cc_coroutine_yield();
    printf( "coro3: step 2\n" );
    cc_coroutine_yield();
    printf( "coro3: step 3\n" );
    cc_coroutine_yield();
    printf( "coro3: leave\n" );
}

cc_coroutine_t* create_coro( const char* name, cc_functor__fptr main )
{
    cc_coroutine_create_params_t cparams;
    cc_coroutine_create_params_init( &cparams );
    cparams.name = name;
    cparams.main = main;
    return cc_coroutine_create( &cparams );
}

int main()
{
    int i;

    cc_coroutine_t* c1;
    cc_coroutine_t* c2;
    cc_coroutine_t* c3;

    initialize();

    printf( "main : enter (create coroutines)\n" );

    c1 = create_coro( "coro1", coro1 );
    c2 = create_coro( "coro2", coro2 );
    c3 = create_coro( "coro3", coro3 );

    printf( "main : execute active coroutines\n" );
    i = 0;

    while( !cc_list_empty( &thread->scheduler->active_queue ) )
    {
        printf( "main : iteration %i\n", i++ );
        cc_scheduler_call_active( thread->scheduler );
    }

    printf( "main : leave\n" );

    cc_thread_destroy( thread );

    return 0;
}
