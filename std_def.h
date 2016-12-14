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
*   File Name : std_def.h                 
*   Version : 1.0
*   Description : Standard Macro Defines
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-06-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _STD_DEF_H
#define _STD_DEF_H

#include "std_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Logic Value Constant */
#define FALSE       0
#define TRUE        1
#define FAILURE    -1

/* Null Pointer Constant */
#ifndef NULL
#define NULL        ((void_t *)0)
#endif

/***************************************************************************************
*   The Size of CPU's Address Pointer Register, UNITs in Bytes.
*   Valid Value is 2 or 4 for 8-Bits and 32-Bits CPU Separably.
***************************************************************************************/
#define SIZE_OF_PTR     sizeof(void_t *)

/***************************************************************************************
*   Expands to An Integral Constant Expression that Has Type size_t, The Value of which
*   is The Offset in Bytes from the Beginning of a Structure Designated by Type of the 
*   Member Designated by The Identifier (If the Specified Member is a Bit-Field, The 
*   Behaviour is Undefined).
***************************************************************************************/
#define OFFSET_OF(type, member)     ((size_t) &((type *)0)->member)

/***************************************************************************************
*   Cast a Member of a Structure Out to The Containing Structure
*       ptr   : The Pointer to the Member.
*       type  : The Type of the Container Struct which the Member is Embedded In.
*       member: The Name of the Member within the Struct.
***************************************************************************************/
#define CONTAINER_OF(ptr, type, member)     \
    ((type *)((uint8_t *)(ptr) - OFFSET_OF(type, member)))

#define FIELD_SIZEOF(type, field)  (sizeof(((type *)0) -> field))

#define ALIGN(type, x, a)     __ALIGN_MASK(x, (type)(a) - 1)
#define __ALIGN_MASK(x, mask)   (((x) + (mask)) & ~(mask))

/***************************************************************************************
*   Macro to Calculate Static Dimension of a 1-dim Array
***************************************************************************************/
#define ARRAY_SIZEOF(array_)    (sizeof(array_) / sizeof((array_)[0]))

#define DIV_ROUND_UP(x, y)  (((x) + ((y) - 1)) / (y))
#define ROUND_UP(n, d)      ((((n) + ((d) - 1)) / (d)) * (d))

#define  ABS(x)     ({int32_t __x = (x); (__x < 0) ? -__x : __x;})
#define LABS(x)     ({int64_t __x = (x); (__x < 0) ? -__x : __x;})

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

/*  min()/max() Macros that Also do Strict Type-Checking */
#define MIN_T(type, x, y) ({type __x = (x); type __y = (y); __x < __y ? __x : __y; })
#define MAX_T(type, x, y) ({type __x = (x); type __y = (y); __x > __y ? __x : __y; })

/*  Check at Compile Time that 'variable' is a Certain Type. */
#define TYPE_CHECK_VAR(type, x)     ({(type *) __ptr; __ptr = &x; (void_t)__ptr;})

/*  Check at Compile Time that 'function' is a Certain Type, or is a Pointer to That Type */
#define TYPE_CHECK_FN(type, f)      ({(type) __tmp = f; (void_t)__tmp;})

/*  Force a Compilation Error if Condition is True */
#define BUILD_BUG_ON(condition) ((void_t)sizeof(int8_t[1 - 2*!!(condition)]))

/***************************************************************************************
*   Force a Compilation Error if Condition is True, But Also Produce a Result 
*   (of Value 0 and Type size_t), so The Expression can be Used e.g. in a Structure
*   Initializer (or where-ever else Comma Expressions aren't Permitted).
***************************************************************************************/
#define BUILD_BUG_ON_ZERO(e)    (sizeof(int8_t[1 - 2 * !!(e)]) - 1)

#ifdef __cplusplus
}
#endif

#endif  /* End of _STD_DEF_H */
