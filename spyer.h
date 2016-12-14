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
*   Group : Spyer Debuger
*   File Name : spyer.h                 
*   Version : 1.0
*   Description : Base for Spyer Debuger
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-16-2005      Created Initial Version 1.0
*       JinHui Han      02-16-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _SPYER_H
#define _SPYER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define SPYER_EXT
#else
    #define SPYER_EXT      extern
#endif

#ifndef SPYER_BUFFER_SIZE
    /***********************************************************************************
    *   This Constant Defines the Size of the Spyer Buffer in Bytes. 
    *   The Minimum Size Requirement of Buffer Used by sprintf() Output is 509.
    ***********************************************************************************/
    #define SPYER_BUFFER_SIZE     1024      /* Default is 1KB */
#endif      /* End of SPYER_BUFFER_SIZE */

/***************************************************************************************
*   The Preprocessor Switch SPYER_ENABLE Enables Spyer Debuger.
***************************************************************************************/

#ifdef SPYER_ENABLE      /* SPYER_ENABLE Defined -- Spy Debuger Enabled */

    /***********************************************************************************
    *   Description: First, Format the Arguments under the Control of the String 
    *                Pointed to by "format". 
    *                Second, Output the Converted Strings into Debuger Serial Port. 
    *   Argument: format -- The Format Strings which Specifies How Subsequent Arguments 
    *                       are Converted. 
    *             ... -- The Arguments List which be Converted. 
    *   Return: none
    *   NOTE:
    *   (1) If there are Insufficient Arguments for the "format", The Behaviour is 
    *       Undefined. 
    *   (2) If the "format" is Exhausted while Arguments Remain, the Excess Arguments 
    *       are Evaluated but Otherwise Ignored. 
    *   (3) The "format" is Composed of Zero or More Characters and Directives: 
    *       (A) Ordinary Multibyte Characters (not %) are Copied Unchanged to the 
    *           Output Stream; 
    *       (B) Conversion Specifiers (%), Each of which Results in Fetching Zero or 
    *           More Subsequent Arguments. 
    ***********************************************************************************/
    SPYER_EXT void_t spyer_(const int8_t * format, ...); 
 
    /***********************************************************************************
    *   SPYER() Macros. 
    ***********************************************************************************/
    #define SPYER(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)

#else                       /* Spyer Debuger Disabled */

    #define SPYER(format, ...)   ((void_t)0)

#endif      /* End of SPYER_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _SPYER_H */
