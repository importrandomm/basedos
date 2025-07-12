#ifndef _STDINT_H
#define _STDINT_H

// Exact-width integer types
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// Minimum values of exact-width signed integer types
#define INT8_MIN   (-128)
#define INT16_MIN  (-32768)
#define INT32_MIN  (-2147483647 - 1)
#define INT64_MIN  (-9223372036854775807LL - 1)

// Maximum values of exact-width signed integer types
#define INT8_MAX   127
#define INT16_MAX  32767
#define INT32_MAX  2147483647
#define INT64_MAX  9223372036854775807LL

// Maximum values of exact-width unsigned integer types
#define UINT8_MAX  255
#define UINT16_MAX 65535
#define UINT32_MAX 4294967295U
#define UINT64_MAX 18446744073709551615ULL

// Limits of other integer types
#define UINTPTR_MAX UINT32_MAX
#define INTPTR_MAX  INT32_MAX
#define PTRDIFF_MAX INT32_MAX
#define SIZE_MAX    UINT32_MAX

// Integer constant macros
#define INT8_C(x)  x
#define UINT8_C(x) x##U
#define INT16_C(x) x
#define UINT16_C(x) x##U
#define INT32_C(x) x
#define UINT32_C(x) x##U
#define INT64_C(x) x##LL
#define UINT64_C(x) x##ULL

// Fastest minimum-width integer types
typedef int8_t  int_fast8_t;
typedef uint8_t uint_fast8_t;
typedef int16_t int_fast16_t;
typedef uint16_t uint_fast16_t;
typedef int32_t int_fast32_t;
typedef uint32_t uint_fast32_t;
typedef int64_t int_fast64_t;
typedef uint64_t uint_fast64_t;

// Minimum-width integer types
typedef int8_t  int_least8_t;
typedef uint8_t uint_least8_t;
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;

// Greatest-width integer types
typedef int64_t  intmax_t;
typedef uint64_t uintmax_t;

// Macros for minimum-width integer constants
#define INT8_C(x)  x
#define UINT8_C(x) x##U
#define INT16_C(x) x
#define UINT16_C(x) x##U
#define INT32_C(x) x
#define UINT32_C(x) x##U
#define INT64_C(x) x##LL
#define UINT64_C(x) x##ULL

// Macros for greatest-width integer constants
#define INTMAX_C(x)  x##LL
#define UINTMAX_C(x) x##ULL

// Limits of other integer types
#define INTPTR_MIN  INT32_MIN
#define INTPTR_MAX  INT32_MAX
#define UINTPTR_MAX UINT32_MAX

// Limits of pointer difference type
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX

// Limits of size_t
#define SIZE_MAX    UINT32_MAX

// Limits of wchar_t
#ifndef WCHAR_MIN
#define WCHAR_MIN INT32_MIN
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX INT32_MAX
#endif

// Limits of wint_t
#ifndef WINT_MIN
#define WINT_MIN 0
#endif

#ifndef WINT_MAX
#define WINT_MAX 65535
#endif

// NULL pointer constant
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

// Offset of member in structure
#define offsetof(type, member) ((size_t)&((type *)0)->member)

#endif // _STDINT_H
