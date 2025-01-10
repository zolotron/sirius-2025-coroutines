#include "cc_time.h"

#define FirstYear 1700
#define LastYear 2279

static bool internal__is_leap_year( int32 year ) {
    return ( ( year % 4 ) == 0 ) && ( ( ( year % 100 ) != 0 ) || ( ( year % 400 ) == 0 ) );
}

static uint64 internal__year_size( int32 year ) {
    return internal__is_leap_year( year ) ? 366ull : 365ull;
}

static uint64 internal__month_size( bool is_leap_year, int32 month ) {
    switch( month ) {
        case 0:
            return 31;
        case 1:
            return is_leap_year ? 29 : 28;
        case 2:
            return 31;
        case 3:
            return 30;
        case 4:
            return 31;
        case 5:
            return 30;
        case 6:
        case 7:
            return 31;
        case 8:
            return 30;
        case 9:
            return 31;
        case 10:
            return 30;
        case 11:
            return 31;
        default:
            return 0;
    }
}

#if defined(PLATFORM_LINUX)

#include <sys/time.h>

#define LinuxEpochTimePoint_01_01_1970 0x763e507306f90000ull

uint64 cc_time( void ) {
    struct timeval tv;
    struct timezone tz;

    gettimeofday( &tv, &tz );

    return LinuxEpochTimePoint_01_01_1970 + (uint64)tv.tv_sec * 1000000000ull + (uint64)tv.tv_usec * 1000ull;
}

uint64 cc_time_monotonic( void ) {
    /* TODO */
}

#elif defined(PLATFORM_WINDOWS)

#include <windows.h>

#define WindowsFileTimePoint_01_01_1700_00_00_00 0x006efdddaec64000ull
#define WindowsFileTimePoint_31_12_2279_23_59_59 0x02f93e7682e63fffull

#define LastTimestamp 0xfe013bb2dc7fffffull

uint64 cc_time( void ) {
    FILETIME ftime;
    uint64 tval;
    GetSystemTimeAsFileTime( &ftime );

    tval = ( (uint64)ftime.dwHighDateTime << 32 ) + (uint64)ftime.dwLowDateTime;

    if( tval < WindowsFileTimePoint_01_01_1700_00_00_00 ) {
        return 0;
    }

    if( tval > WindowsFileTimePoint_31_12_2279_23_59_59 ) {
        return LastTimestamp;
    }

    return ( tval - WindowsFileTimePoint_01_01_1700_00_00_00 ) * 100ull;
}

uint64 cc_time_monotonic( void ) {
    /* TODO */
}

#endif

bool cc_time_pack( const cc_time_info_t* info, uint64* time ) {
    int32 tmonth;
    int32 tday;
    int32 month_size;
    bool is_leap_year;

    assert( info != NULL );
    assert( time != NULL );

    if( ( info->nsecond < 0 ) || ( info->nsecond >= 1000 ) ) {
        return false;
    }
    if( ( info->usecond < 0 ) || ( info->usecond >= 1000 ) ) {
        return false;
    }
    if( ( info->msecond < 0 ) || ( info->msecond >= 1000 ) ) {
        return false;
    }
    if( ( info->second < 0 ) || ( info->second >= 60 ) ) {
        return false;
    }
    if( ( info->minute < 0 ) || ( info->minute >= 60 ) ) {
        return false;
    }
    if( ( info->hour < 0 ) || ( info->hour >= 24 ) ) {
        return false;
    }

    tmonth = info->month - 1;

    if( ( tmonth < 0 ) || ( tmonth >= 60 ) ) {
        return false;
    }
    if( ( info->year < FirstYear ) || ( info->year > LastYear ) ) {
        return false;
    }

    tday = info->day - 1;
    is_leap_year = internal__is_leap_year( info->year );
    month_size = (int32)internal__month_size( is_leap_year, tmonth );

    if( ( tday < 0 ) || ( tday >= month_size ) ) {
        return false;
    }

    ( *time ) = 0ull;

    for( int32 y = FirstYear; y < info->year; y++ )
        ( *time ) += internal__year_size( y );

    for( int32 m = 0; m < tmonth; m++ )
        ( *time ) += internal__month_size( is_leap_year, m );

    ( *time ) += (uint64)tday;
    ( *time ) *= 86400000000000ull;
    ( *time ) += (uint64)info->hour * 3600000000000ull;
    ( *time ) += (uint64)info->minute * 60000000000ull;
    ( *time ) += (uint64)info->second * 1000000000ull;
    ( *time ) += (uint64)info->msecond * 1000000ull;
    ( *time ) += (uint64)info->usecond * 1000ull;
    ( *time ) += (uint64)info->nsecond;

    return true;
}

void cc_time_unpack( cc_time_info_t* info, uint64 time ) {
    uint64 ts;
    bool is_leap_year;

    assert( info != NULL );

    info->nsecond = (int32)( time % 1000ull );
    time /= 1000ull;
    info->usecond = (int32)( time % 1000ull );
    time /= 1000ull;
    info->msecond = (int32)( time % 1000ull );
    time /= 1000ull;
    info->second = (int32)( time % 60ull );
    time /= 60ull;
    info->minute = (int32)( time % 60ull );
    time /= 60ull;
    info->hour = (int32)( time % 24ull );
    time /= 24ull;
    info->day_of_week = (int32)( ( ( time + 4ull ) % 7ull ) + 1ull );

    info->year = FirstYear;
    while( true ) {
        ts = internal__year_size( info->year );
        if( time < ts ) {
            break;
        }
        time -= ts;
        ++info->year;
    }

    info->week_of_year = (int32)( ( time / 7ull ) + 1ull );
    info->day_of_year = (int32)( time + 1ull );

    is_leap_year = internal__is_leap_year( info->year );
    info->month = 0;
    while( true ) {
        ts = internal__month_size( is_leap_year, info->month );
        if( time < ts ) {
            break;
        }
        time -= ts;
        ++info->month;
    }

    ++info->month;
    info->day = (int32)( time + 1ull );
}

/* %% - outputs %
 * %Y - year number in format XXXX
 * %y - year number in format XX (00-99)
 * %M - month number in format XX (01-12)
 * %D - day number in format XX (01-31)
 * %h - hours in format XX (00-23)
 * %g - hours in format XX (01-12)
 * %f - meridiem marker 'a' or 'p'
 * %F - meridiem marker 'A' or 'P'
 * %m - minutes in format XX (00-59)
 * %s - seconds in format XX (00-59)
 * %q - day of the year (1-366)
 * %w - week of the year (1-52)
 * %r - day of the week (1-7) 1 = monday
 * ~~ - outputs ~
 * ~m - milliseconds in format XXX (000-999)
 * ~u - microseconds in format XXX (000-999)
 * ~n - nanoseconds in format XXX (000-999)
 */
size_t cc_time_info_format( char* buffer, size_t buffer_size, const char* format, const cc_time_info_t* info ) {
    char* buffer_begin;
    char* buffer_end;
    bool in_p_format;
    bool in_t_format;
    int32 tval;

    cc_assert( buffer != NULL );
    cc_assert( buffer_size > 0 );
    cc_assert( format != NULL );
    cc_assert( info != NULL );

    buffer_begin = buffer;
    buffer_end = buffer + buffer_size;
    in_p_format = false;
    in_t_format = false;

    while( true ) {
        if( in_p_format ) {
            switch( *format ) {
                case '%': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = '%';
                    break;
                }
                case 'Y': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->year / 1000 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->year / 100 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->year / 10 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->year % 10 );
                    break;
                }
                case 'y': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->year / 10 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->year % 10 );
                    break;
                }
                case 'M': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->month / 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->month % 10 );
                    break;
                }
                case 'D': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->day / 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->day % 10 );
                    break;
                }
                case 'h': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->hour / 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->hour % 10 );
                    break;
                }
                case 'g': {
                    tval = info->hour % 12;
                    tval = ( tval == 0 ) ? 12 : tval;
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + tval / 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + tval % 10 );
                    break;
                }
                case 'f': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = ( info->hour < 12 ) ? 'a' : 'p';
                    break;
                }
                case 'F': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = ( info->hour < 12 ) ? 'A' : 'P';
                    break;
                }
                case 'm': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->minute / 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->minute % 10 );
                    break;
                }
                case 's': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->second / 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->second % 10 );
                    break;
                }
                default: {
                    goto rollback;
                }
            }
            in_p_format = false;
            ++format;
        } else if( in_t_format ) {
            switch( *format ) {
                case '~': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = '~';
                    break;
                }
                case 'm': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->msecond / 100 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->msecond / 10 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->msecond % 10 );
                    break;
                }
                case 'u': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->usecond / 100 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->usecond / 10 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->usecond % 10 );
                    break;
                }
                case 'n': {
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->nsecond / 100 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + ( info->nsecond / 10 ) % 10 );
                    if( buffer == buffer_end ) {
                        goto rollback;
                    }
                    *buffer++ = (char)( '0' + info->nsecond % 10 );
                    break;
                }
                default: {
                    goto rollback;
                }
            }
            in_t_format = false;
            ++format;
        } else {
            if( *format == '%' ) {
                in_p_format = true;
                ++format;
                continue;
            }
            if( *format == '~' ) {
                in_t_format = true;
                ++format;
                continue;
            }
            if( *format == 0 ) {
                break;
            }
            if( buffer == buffer_end ) {
                goto rollback;
            }
            *buffer++ = *format++;
        }
    }

    if( buffer == buffer_end ) {
        goto rollback;
    }

    *buffer = 0;

    return buffer - buffer_begin;

rollback:
    *buffer_begin = 0;
    return 0;
}

size_t cc_time_format( char* buffer, size_t buffer_size, const char* format, uint64 time ) {
    cc_time_info_t info;
    cc_time_unpack( &info, time );
    return cc_time_info_format( buffer, buffer_size, format, &info );
}
