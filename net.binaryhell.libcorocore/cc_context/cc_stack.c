#include "cc_stack.h"
#include "cc_log/cc_log.h"

static size_t internal__page_size = 0;

#if defined(PLATFORM_LINUX)

#include <sys/mman.h>
#include <unistd.h>

void cc_stack_module_init( void ) {
    internal__page_size = (size_t)sysconf( _SC_PAGESIZE );
}

void cc_stack_clear( cc_stack_t* self ) {
    assert( self != NULL );
    self->ptr = NULL;
    self->total_size = 0;
    self->guard_size = 0;
}

bool cc_stack_init( cc_stack_t* self, size_t size ) {
    char* ptr;

    assert( self != NULL );

    if( size < internal__page_size ) {
        size = internal__page_size;
    }

    /* Align size to page size and add extra page to detect stack overflow condition */
    size = ( ( ( size - 1 ) / internal__page_size ) + 2 ) * internal__page_size;

    ptr = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );

    if( ptr == MAP_FAILED ) {
        goto rollback0;
    }

    if( mprotect( ptr, internal__page_size, PROT_NONE ) == -1 ) {
        goto rollback1;
    }

    self->ptr = ptr;
    self->total_size = size;
    self->guard_size = internal__page_size;
    return true;

rollback1:
    munmap( ptr, size );
rollback0:
    self->ptr = NULL;
    self->total_size = 0;
    self->guard_size = 0;
    return false;
}

void cc_stack_deinit( cc_stack_t* self ) {
    assert( self != NULL );

    if( self->ptr != NULL ) {
        munmap( self->ptr, self->total_size );
    }

    self->ptr = NULL;
    self->total_size = 0;
    self->guard_size = 0;
}

#elif defined(PLATFORM_WINDOWS)

#include <windows.h>

void cc_stack_module_init( void ) {
    SYSTEM_INFO info;
    GetSystemInfo( &info );
    internal__page_size = (size_t)info.dwPageSize;
}

void cc_stack_clear( cc_stack_t* self ) {
    assert( self != NULL );
    self->handle = NULL;
    self->ptr = NULL;
    self->total_size = 0;
    self->guard_size = 0;
}

bool cc_stack_init( cc_stack_t* self, size_t size ) {
    HANDLE handle;
    char* ptr;
    BOOL bres;
    DWORD old_protect;

    assert( self != NULL );

    if( size < internal__page_size ) {
        size = internal__page_size;
    }

    /* Align size to page size and add extra page to detect stack overflow condition */
    size = ( ( ( size - 1 ) / internal__page_size ) + 2 ) * internal__page_size;

    handle =
        CreateFileMappingW(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            (DWORD)( size >> 32 ),
            (DWORD)size,
            NULL );

    if( handle == NULL ) {
        goto rollback0;
    }

    ptr =
        MapViewOfFile(
            handle,
            FILE_MAP_READ | FILE_MAP_WRITE,
            0,
            0,
            size );

    if( ptr == NULL ) {
        goto rollback1;
    }

    bres = VirtualProtect( ptr, internal__page_size, PAGE_NOACCESS, &old_protect );

    if( !bres ) {
        goto rollback2;
    }

    self->handle = handle;
    self->ptr = ptr;
    self->total_size = size;
    self->guard_size = internal__page_size;

    return true;

rollback2:
    UnmapViewOfFile( ptr );
rollback1:
    CloseHandle( handle );
rollback0:
    self->handle = NULL;
    self->ptr = NULL;
    self->total_size = 0;
    self->guard_size = 0;
    return false;
}

void cc_stack_deinit( cc_stack_t* self ) {
    assert( self != NULL );

    if( self->ptr != NULL ) {
        UnmapViewOfFile( self->ptr );
    }

    if( self->handle != NULL ) {
        CloseHandle( self->handle );
    }

    self->handle = NULL;
    self->ptr = NULL;
    self->total_size = 0;
    self->guard_size = 0;
}

#endif

size_t cc_stack_get_minimal_size( void ) {
    if(internal__page_size <= 65536) {
        return 65536;
    }
    return internal__page_size;
}
