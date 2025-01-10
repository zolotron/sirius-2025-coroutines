#include "cc_context.h"

#if defined(ARCH_X86_64) && defined(PLATFORM_LINUX)

#include "cc_coroutine/cc_coroutine.h"
#include "cc_memory/cc_memory.h"

cc_context_t* cc_context_create( const cc_context_create_params_t* params ) {
    cc_context_t* result;
    char* sp;

    assert( params != NULL );

    result = cc_calloc( sizeof( cc_context_t ) );
    if( result == NULL ) {
        goto rollback0;
    }

    if( params->stack_create_mode == CC_CONTEXT_CREATE_STACK_PARAM_COROUTINE ) {
        if( !cc_stack_init( &result->stack, params->stack_size ) ) {
            goto rollback1;
        }

        sp = (char*)result->stack.ptr + result->stack.total_size;

        /* Stack termination for GDB */
        sp -= 8;
        ( *(uint64*)sp ) = 0;
        sp -= 8;
        ( *(uint64*)sp ) = 0;

        /* Push coroutine entry point function on stack */
        sp -= 8;
        ( *(void**)sp ) = cc_context_entry_point;

        /* Alignment (RBP) */
        sp -= 8;
        ( *(uint64*)sp ) = 0;

        /* Push register context on stack */
        sp -= 0x30;
        cc_memset( sp, 0, 0x30 );

        /* Set main function 'args' value */
        result->context = sp;
    } else {
        cc_stack_clear( &result->stack );
    }

    return result;

rollback1:
    cc_free( result );
rollback0:
    return NULL;
}

void cc_context_destroy( cc_context_t* context ) {
    assert( context != NULL );
    cc_stack_deinit( &context->stack );
    cc_free( context );
}

/* ----------------------------------------------------------------------- */
/* Data which must be saved into context:                                  */
/*   CPU registers: rbx, rbp, r12-r15                                      */
/* ----------------------------------------------------------------------- */
/* void cc_context_switch(cc_context_t* context, cc_context_t* target); */
asm
    (
    "   .text                               \n"
    "   .global     cc_context_switch       \n"
    "cc_context_switch:                     \n"
    /*  Function frame enter                 */
    "   pushq       %rbp                    \n"
    "   movq        %rsp, %rbp              \n"
    /*  Reserve memory for context in stack  */
    "   subq        $0x030, %rsp            \n"
    /*  Store callee safe registers          */
    "   movq        %rbx,   0x000(%rsp)     \n"
    "   movq        %r12,   0x008(%rsp)     \n"
    "   movq        %r13,   0x010(%rsp)     \n"
    "   movq        %r14,   0x018(%rsp)     \n"
    "   movq        %r15,   0x020(%rsp)     \n"
    /*  Switch stack of current coroutine    */
    "   movq        %rsp, (%rdi)            \n"
    "   movq        (%rsi), %rsp            \n"
    /*  Restore callee safe registers        */
    "   movq        0x000(%rsp), %rbx       \n"
    "   movq        0x008(%rsp), %r12       \n"
    "   movq        0x010(%rsp), %r13       \n"
    "   movq        0x018(%rsp), %r14       \n"
    "   movq        0x020(%rsp), %r15       \n"
    /*  Free reserved stack memory           */
    "   addq        $0x030, %rsp            \n"
    /*  Function frame enter                 */
    "   popq        %rbp                    \n"
    /*  Jump to restore point                */
    "   popq        %rcx                    \n"
    "   jmpq        *%rcx                   \n"
    );

/* void cc_context_entry_point(void); */
asm
    (
    "   .text                               \n"
    "   .global     cc_context_entry_point  \n"
    "cc_context_entry_point:                \n"
    "   call        cc_coroutine_entry_point\n"
    );

#endif
