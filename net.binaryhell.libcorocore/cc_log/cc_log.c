#include "cc_log.h"

#include "cc_thread/cc_thread.h"
#include "cc_time/cc_time.h"

#include <stdarg.h>
#include <stdio.h>

static cc_log_format_params_t internal__log_format = CC_LOG_FORMAT_NONE;
static cc_log_write__fptr internal__log_write = NULL;

static cc_log_severity_t internal__log_severity = CC_LOG_SEVERITY_INFO;

static size_t internal__snprintf( char* buffer, size_t buffer_size, const char* format, ... ) {
    int res;
    va_list args;
    va_start( args, format );
    res = vsnprintf( buffer, buffer_size, format, args );
    va_end( args );
    if( ( res < 0 ) || ( (uint64)res >= buffer_size ) ) {
        return 0;
    }
    return (size_t)res;
}

static size_t internal__vsnprintf( char* buffer, size_t buffer_size, const char* format, va_list args ) {
    int res;
    res = vsnprintf( buffer, buffer_size, format, args );
    if( ( res < 0 ) || ( (uint64)res >= buffer_size ) ) {
        return 0;
    }
    return (size_t)res;
}

static void internal__log_message( const char* severity, const char* format, va_list args ) {
    char log_line[1024];
    size_t offset;

    log_line[0] = 0;
    offset = 0;

    if( internal__log_format & CC_LOG_FORMAT_TIMESTAMP ) {
        offset += cc_time_format( &log_line[offset], sizeof( log_line ) - offset, "%D-%M-%Y %h:%m:%s.~m ", cc_time() );
    }

    /* todo: add thread and coroutine ids formatting */

    if( internal__log_format & CC_LOG_FORMAT_SEVERITY ) {
        offset += internal__snprintf( &log_line[offset], sizeof( log_line ) - offset, "%s ", severity );
    }

    offset += internal__vsnprintf( &log_line[offset], sizeof( log_line ) - offset, format, args );

    if( offset < sizeof( log_line ) ) {
        log_line[offset] = '\n';
        ++offset;
    } else {
        log_line[offset - 1] = '\n';
    }

    if( internal__log_write != NULL ) {
        internal__log_write( &log_line[0], offset );
    }
}

void cc_log_module_init( const cc_log_module_init_params_t* params ) {
    cc_assert( params != NULL );
    cc_assert( params->log_write != NULL );
    internal__log_format = params->log_format;
    internal__log_write = params->log_write;
}

void cc_log_set_severity( cc_log_severity_t severity ) {
    internal__log_severity = severity;
}

void cc_log_debug( const char* format, ... ) {
    va_list args;
    if( internal__log_severity <= CC_LOG_SEVERITY_DEBUG ) {
        va_start( args, format );
        internal__log_message( ">DBG<", format, args );
        va_end( args );
    }
}

void cc_log_verbose( const char* format, ... ) {
    va_list args;
    if( internal__log_severity <= CC_LOG_SEVERITY_VERBOSE ) {
        va_start( args, format );
        internal__log_message( "info:", format, args );
        va_end( args );
    }
}

void cc_log_info( const char* format, ... ) {
    va_list args;
    if( internal__log_severity <= CC_LOG_SEVERITY_INFO ) {
        va_start( args, format );
        internal__log_message( "info:", format, args );
        va_end( args );
    }
}

void cc_log_warning( const char* format, ... ) {
    va_list args;
    if( internal__log_severity <= CC_LOG_SEVERITY_WARNING ) {
        va_start( args, format );
        internal__log_message( "warn:", format, args );
        va_end( args );
    }
}

void cc_log_error( const char* format, ... ) {
    va_list args;
    if( internal__log_severity <= CC_LOG_SEVERITY_ERROR ) {
        va_start( args, format );
        internal__log_message( "-err-", format, args );
        va_end( args );
    }
}
