#include "cc_context.h"

#if defined(ARCH_X86_64) && defined(PLATFORM_WINDOWS)

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

        /* Allocate and clear context */
        sp -= 0xe0;
        cc_memset( sp, 0, 0xe0 );

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
/*   SSE registers: xmm6-xmm15                                             */
/*   CPU registers: rbx, rsi, rdi, rbp, r12-r15                            */
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
    "   subq        $0x0e0, %rsp            \n"
    /*  Store callee safe registers          */
    "   movaps      %xmm6,  0x000(%rsp)     \n"
    "   movaps      %xmm7,  0x010(%rsp)     \n"
    "   movaps      %xmm8,  0x020(%rsp)     \n"
    "   movaps      %xmm9,  0x030(%rsp)     \n"
    "   movaps      %xmm10, 0x040(%rsp)     \n"
    "   movaps      %xmm11, 0x050(%rsp)     \n"
    "   movaps      %xmm12, 0x060(%rsp)     \n"
    "   movaps      %xmm13, 0x070(%rsp)     \n"
    "   movaps      %xmm14, 0x080(%rsp)     \n"
    "   movaps      %xmm15, 0x090(%rsp)     \n"
    "   movq        %rbx,   0x0a0(%rsp)     \n"
    "   movq        %rsi,   0x0a8(%rsp)     \n"
    "   movq        %rdi,   0x0b0(%rsp)     \n"
    "   movq        %r12,   0x0b8(%rsp)     \n"
    "   movq        %r13,   0x0c0(%rsp)     \n"
    "   movq        %r14,   0x0c8(%rsp)     \n"
    "   movq        %r15,   0x0d0(%rsp)     \n"
    /*  Switch stack of current coroutine    */
    "   movq        %rsp, (%rcx)            \n"
    "   movq        (%rdx), %rsp            \n"
    /*  Restore callee safe registers        */
    "   movaps      0x000(%rsp), %xmm6      \n"
    "   movaps      0x010(%rsp), %xmm7      \n"
    "   movaps      0x020(%rsp), %xmm8      \n"
    "   movaps      0x030(%rsp), %xmm9      \n"
    "   movaps      0x040(%rsp), %xmm10     \n"
    "   movaps      0x050(%rsp), %xmm11     \n"
    "   movaps      0x060(%rsp), %xmm12     \n"
    "   movaps      0x070(%rsp), %xmm13     \n"
    "   movaps      0x080(%rsp), %xmm14     \n"
    "   movaps      0x090(%rsp), %xmm15     \n"
    "   movq        0x0a0(%rsp), %rbx       \n"
    "   movq        0x0a8(%rsp), %rsi       \n"
    "   movq        0x0b0(%rsp), %rdi       \n"
    "   movq        0x0b8(%rsp), %r12       \n"
    "   movq        0x0c0(%rsp), %r13       \n"
    "   movq        0x0c8(%rsp), %r14       \n"
    "   movq        0x0d0(%rsp), %r15       \n"
    /*  Free reserved stack memory           */
    "   addq        $0x0e0, %rsp            \n"
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
