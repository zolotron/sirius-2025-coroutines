#include "cc_string.h"

#include <stdarg.h>
#include <stdio.h>

#include "cc_memory/cc_memory.h"

char* cc_sprintf( const char* format, ... ) {
    char* result;
    va_list args;
    int res;
    size_t length;

    result = NULL;
    length = 128;

    while( true ) {
        result = (char*)cc_realloc( result, length );
        if( result == NULL ) {
            /* pm_set_errno(ENOMEM); */
            return NULL;
        }

        va_start( args, format );
        res = vsnprintf( result, length - 1, format, args );
        va_end( args );

        if( res < 0 ) {
            /* pm_set_errno(errno); */
            return NULL;
        }

        if( (size_t)res < length ) {
            return result;
        }

        length = (size_t)res;
    }
}
