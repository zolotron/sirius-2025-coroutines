#ifndef CC_CONFIG_H
#define CC_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define cc_unused( V ) ((void)(V))

#define cc_container_of( NODE, TYPE, FIELD ) ( (TYPE*)( (char*)NODE - __builtin_offsetof( TYPE, FIELD ) ) )

#define cc_infinity 0xffffffffffffffffull

#define cc__attribute_format__( A, P ) __attribute__ ((format (printf, A, P)))

typedef enum {
    CC_VALUE_TYPE_CONST = 0,
    CC_VALUE_TYPE_STACK = 1,
    CC_VALUE_TYPE_HEAP = 2,
} cc_value_type_t;

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef float float32;
typedef double float64;

typedef void (* cc_functor__fptr)( void* context );

#define cc_assert( CONDITION ) assert( CONDITION )

#endif
