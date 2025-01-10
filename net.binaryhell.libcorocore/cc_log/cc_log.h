#ifndef CC_LOG_H
#define CC_LOG_H

#include "cc_config.h"

typedef void (* cc_log_write__fptr)( const char* line, size_t size );

typedef enum {
    CC_LOG_FORMAT_NONE = 0,
    CC_LOG_FORMAT_TIMESTAMP = 1,
    CC_LOG_FORMAT_THREAD_ID = 2,
    CC_LOG_FORMAT_COROUTINE_ID = 4,
    CC_LOG_FORMAT_SEVERITY = 8,
} cc_log_format_params_t;

typedef enum {
    CC_LOG_SEVERITY_DEBUG = 0,
    CC_LOG_SEVERITY_VERBOSE = 1,
    CC_LOG_SEVERITY_INFO = 2,
    CC_LOG_SEVERITY_WARNING = 3,
    CC_LOG_SEVERITY_ERROR = 4,
} cc_log_severity_t;

typedef struct {
    cc_log_format_params_t log_format;
    cc_log_write__fptr log_write;
} cc_log_module_init_params_t;

void cc_log_module_init( const cc_log_module_init_params_t* params );

void cc_log_set_severity( cc_log_severity_t severity );

void cc_log_debug( const char* format, ... ) cc__attribute_format__( 1, 2 );
void cc_log_verbose( const char* format, ... ) cc__attribute_format__( 1, 2 );
void cc_log_info( const char* format, ... ) cc__attribute_format__( 1, 2 );
void cc_log_warning( const char* format, ... ) cc__attribute_format__( 1, 2 );
void cc_log_error( const char* format, ... ) cc__attribute_format__( 1, 2 );

#endif
