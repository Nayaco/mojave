#ifndef HASH_TYPES
#define HASH_TYPES 1

#include <stdint.h>

#define H_nullptr             ((void*)0)
#define false               0
#define true                1

typedef void                *Gptr;

typedef long long           Int64;
typedef unsigned long long  Uint64;
typedef long                Int32;
typedef unsigned long       Uint32;
typedef uint8_t             boolean;
typedef boolean (*cmp)(Gptr a, Gptr b);

#endif