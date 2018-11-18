#pragma once
#include <stddef.h>

// TODO: not only x86...
typedef unsigned long long	uint64_t;
typedef long long			s64;
typedef unsigned int		uint32_t;
typedef int					s32;
typedef unsigned short		uint16_t;
typedef short				s16;
typedef unsigned char		uint8_t;
typedef char				s8;

typedef uint64_t	uint64_t;
typedef s64	int64_t;
typedef uint32_t	uint32_t;
typedef s32	int32_t;
typedef uint16_t	uint16_t;
typedef s16	int16_t;
typedef uint8_t	uint8_t;
typedef s8	int8_t;

typedef int64_t		intmax_t;
typedef uint64_t	uintmax_t;

typedef int8_t		int_least8_t;
typedef uint8_t		uint_least8_t;
typedef int16_t		int_least16_t;
typedef uint16_t	uint_least16_t;
typedef int32_t		int_least32_t;
typedef uint32_t	uint_least32_t;
typedef int64_t		int_least64_t;
typedef uint64_t	uint_least64_t;

typedef int8_t		int_fast8_t;
typedef uint8_t		uint_fast8_t;
typedef int16_t		int_fast16_t;
typedef uint16_t	uint_fast16_t;
typedef int32_t		int_fast32_t;
typedef uint32_t	uint_fast32_t;
typedef int64_t		int_fast64_t;
typedef uint64_t	uint_fast64_t;

/* Pointer size in an x86 cpu is 32 bits */
typedef s32 intptr_t;
typedef uint32_t uintptr_t;