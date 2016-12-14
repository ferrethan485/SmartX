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
*   Component : Active Object Application FrameWork
*   Group : Manager
*   File Name : lookup.h
*   Version : 1.0
*   Description : Base for Lookup Table
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-18-2005      Created Initial Version 1.0
*       JinHui Han      02-18-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _LOOKUP_H
#define _LOOKUP_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define LOOKUP_EXT
#else
    #define LOOKUP_EXT      extern
#endif

/***************************************************************************************
*   Lookup Table for (n/8), where n is the Index into the Table. 
*   Index Range n = 0..63.
***************************************************************************************/
LOOKUP_EXT uint8_t const ROM_KEYWORDS lookup_div8[64];

/***************************************************************************************
*   Lookup Table for  (1 << (n % 8)), where n is the Index into the Table.
*   Index Range n = 0..63. 
***************************************************************************************/
LOOKUP_EXT uint8_t const ROM_KEYWORDS lookup_bit_or[64];

/***************************************************************************************
*   Lookup Table for ~(1 << (n % 8)), where n is the Index into the Table. 
*   Index Range n = 0..63.
***************************************************************************************/
LOOKUP_EXT uint8_t const ROM_KEYWORDS lookup_bit_and[64];

/***************************************************************************************
*   Lookup Table for Priority Resolution, where n is the Index into the Table.
*   Index Range n = 0..255.
*
*   Note: (1) Index into Table is Bit Pattern to Resolve Highest Priority, Indexed 
*             Value Corresponds to Highest Priority Bit Position (i.e. 0..7).
*         (2) This Lookup Delivers the 1-Based Number of the Least Significant 1-Bit 
*             of a Byte.
***************************************************************************************/
LOOKUP_EXT uint8_t const ROM_KEYWORDS lookup_unmap[256];

#ifdef __cplusplus
}
#endif

#endif      /* End of _LOOKUP_H */
