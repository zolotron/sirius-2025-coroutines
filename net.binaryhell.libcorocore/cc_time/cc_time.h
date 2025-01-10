#ifndef CC_TIME_H
#define CC_TIME_H

#include "cc_config.h"

typedef struct {
    int32 year;
    int32 month;
    int32 day;
    int32 hour;
    int32 minute;
    int32 second;
    int32 msecond;
    int32 usecond;
    int32 nsecond;

    int32 day_of_week;
    int32 day_of_year;
    int32 week_of_year;
} cc_time_info_t;

uint64 cc_time( void );
uint64 cc_time_monotonic( void );

bool cc_time_pack( const cc_time_info_t* info, uint64* time );
void cc_time_unpack( cc_time_info_t* info, uint64 time );

size_t cc_time_info_format( char* buffer, size_t buffer_size, const char* format, const cc_time_info_t* info );
size_t cc_time_format( char* buffer, size_t buffer_size, const char* format, uint64 time );

#endif
