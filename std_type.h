/***************************************************************************************
*   The MIT License (MIT)
*   Copyright (C) 2005 JinHui Han, <Email: ferrethan485@sohu.com, QQ: 1901589440>
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in
*   all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*   THE SOFTWARE.
***************************************************************************************/

/***************************************************************************************
*   Component : Base Header File
*   Group : 
*   File Name : std_type.h                 
*   Version : 1.0
*   Description : Standard Type Defines
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-06-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _STD_TYPE_H
#define _STD_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************
*   Integer Type Definitions:
*   (1) 'signed' is Redundant Below, Except for 'signed char' and If the typedef is 
*       Used to Declare a Bit-Field.
*   (2) '__int64' is Used Instead of 'long long' so that This Header can be Used in
*       '-strict' Mode.
*   NOTE: Doesn't Pollute the POSIX Name-Space.
***************************************************************************************/

/* Exact-Width Signed Integer Types, WG14/N843 C99 Standard */
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
//typedef signed long long    int64_t;

/* This is a Special 64 bit Data Type that is 8-Byte Aligned */
#define aligned_int64_t     (signed long long __attribute__((aligned(8))));

/* Minimum Values of Exact-Width signed integer Types */
#define INT8_MIN                    -128
#define INT16_MIN                 -32768
#define INT32_MIN           (~0x7FFFFFFF)   /* -2147483648 is unsigned */
#define INT64_MIN   (~0x7FFFFFFFFFFFFFFFLL) /* -9223372036854775808LL is unsigned */

/* Maximum Values of Exact-Width signed integer Types */
#define INT8_MAX                    127
#define INT16_MAX                 32767
#define INT32_MAX            2147483647
#define INT64_MAX   9223372036854775807LL

/* Exact-Width Unsigned Integer Types, WG14/N843 C99 Standard */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
//typedef unsigned long long  uint64_t;

/* This is a Special 64 bit Data Type that is 8-Byte Aligned */
#define aligned_uint64_t    (unsigned long long __attribute__((aligned(8))));

/* Maximum Values of Exact-Width unsigned integer Types */
#define UINT8_MAX                    255
#define UINT16_MAX                 65535
#define UINT32_MAX            4294967295U
#define UINT64_MAX  18446744073709551615ULL

/* void Type and (void *) Types */
typedef void    void_t;
typedef void *  ptr_t;

/* Maximum Value of void Type Pointer */
#define PTR_MAX         4294967295U

/* The Unsigned Integral Type of The Result of The sizeof Operator. */
typedef unsigned int size_t;   /* Others (e.g. <stdio.h>) Also Define */

/* Limits of size_t */
#define SIZE_MAX        4294967295U

/***************************************************************************************
*   An Integral Type whose Range of Values can Represent Distinct Codes for All Members
*   of the Largest Extended Character Set Specified among the Supported Locales; 
*   The null Character shall Have the Code Value Zero and Each Member of the Basic 
*   Character Set shall have a Code Value when Used as the Long Character in an Integer 
*   Character Constant.
***************************************************************************************/
typedef unsigned short wchar_t; /* Also in <stdlib.h> and <inttypes.h> */

/*  Limits of wchar_t */
#define WCHAR_MIN       0
#define WCHAR_MAX       65535

/***************************************************************************************
*   Type which is the Integral Type of an Object that can be Modified as an Atomic 
*   Entity, Even in the Presence of Asynchronous Interrupts.
***************************************************************************************/
/*
typedef signed char  sig_atomic_t;   // For  8 Bits Memory Data Bus
#define SIG_ATOMIC_MIN  (~0x7F)      // Minimum Limits of sig_atomic_t 
#define SIG_ATOMIC_MAX    127        // Maximum Limits of sig_atomic_t 
*/
typedef signed short sig_atomic_t;   // For 16 Bits Memory Data Bus
#define SIG_ATOMIC_MIN  (~0x7FFF)    // Minimum Limits of sig_atomic_t 
#define SIG_ATOMIC_MAX    32767      // Maximum Limits of sig_atomic_t 
/*
typedef signed int   sig_atomic_t;      // For 32 Bits Memory Data Bus
#define SIG_ATOMIC_MIN  (~0x7FFFFFFF)   // Minimum Limits of sig_atomic_t 
#define SIG_ATOMIC_MAX    2147483647    // Maximum Limits of sig_atomic_t 
*/

/* Boolean Types */
typedef unsigned short bool_t;

/* Float Type, WG14/N843 C99 Standard */
typedef float       float32_t;
typedef double      float64_t;
typedef long double float128_t;

/* This is a Special  64 bit Float Type that is  8-byte Aligned */
#define aligned_float64_t   (     double __attribute__((aligned(8))));
/* This is a Special 128 bit Float Type that is 16-byte Aligned */
#define aligned_float128_t  (long double __attribute__((aligned(16))));

/* Grab the 16-Bits Float Internal Binary Representation */
typedef union float16_bin_tag
{
    float16_t data;
    uint16_t  bin;
} float16_bin_t;

/* Grab the 32-Bits Float Internal Binary Representation */
typedef union float32_bin_tag
{
    float32_t data;
    uint32_t  bin;
} float32_bin_t;

/* Grab the 64-Bits Float Internal Binary Representation */
typedef union float64_bin_tag
{
    float64_t data;
    struct uint
    {
        uint32_t uint1;
        uint32_t uint2;
    } bin;
} float64_bin_t;

#ifdef __cplusplus
}
#endif

#endif  /* End of _STD_TYPE_H */
