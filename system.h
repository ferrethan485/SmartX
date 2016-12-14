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
*   Group : System Initializer
*   File Name : system.h                 
*   Version : 1.0
*   Description : Base for System Initializer
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-05-2005      Created Initial Version 1.0
*       JinHui Han      04-05-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _SYSTEM_H
#define _SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define SYSTEM_EXT
#else
    #define SYSTEM_EXT     extern
#endif

/***************************************************************************************
*   Initialize the System Hardware
*
*   Description: Initialize the System Hardware. 
*   Argument: none. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*   
*   NOTE: 
***************************************************************************************/
SYSTEM_EXT int16_t system_init(void_t); 

#ifdef __cplusplus
}
#endif

#endif      /* End of _SYSTEM_H */
