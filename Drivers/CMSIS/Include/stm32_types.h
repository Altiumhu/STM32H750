
#ifndef STM32_TYPES_H
#define STM32_TYPES_H

/* Includes ------------------------------------------------------------------- */
#include <stdint.h>

/**
 * @brief Boolean Type definition
 */
//typedef enum {FALSE = 0, TRUE = !FALSE} Bool;

/* NULL pointer */
#ifndef NULL
#define NULL ((void*) 0)
#endif

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/** SMA type for character type */
typedef char CHAR;

/** SMA type for 8 bit unsigned value */
typedef uint8_t UNS_8;

/** SMA type for 8 bit signed value */
typedef int8_t INT_8;

/** SMA type for 16 bit unsigned value */
typedef	uint16_t UNS_16;

/** SMA type for 16 bit signed value */
typedef	int16_t INT_16;

/** SMA type for 32 bit unsigned value */
typedef	uint32_t UNS_32;

/** SMA type for 32 bit signed value */
typedef	int32_t INT_32;

/** SMA type for 64 bit signed value */
typedef int64_t INT_64;

/** SMA type for 64 bit unsigned value */
typedef uint64_t UNS_64;

/** 32 bit boolean type */
//typedef Bool BOOL_32;

///** 16 bit boolean type */
//typedef Bool BOOL_16;

///** 8 bit boolean type */
//typedef Bool BOOL_8;

/**
 * @}
 */
#ifndef __FALSE
#define __FALSE   (0)
#endif

#ifndef __TRUE
#define __TRUE    (1)
#endif

typedef signed char        s8;
typedef unsigned char      u8;
typedef short              s16;
typedef unsigned short     u16;
typedef int                s32;
typedef unsigned int       u32;
typedef long long          s64;
typedef unsigned long long u64;

#endif /* STM32_TYPES_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
