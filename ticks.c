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
*   Group : System Ticks
*   File Name : ticks.c
*   Version : 1.0
*   Description : Base for System Ticks
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-09-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h"
#include "std_def.h"
#include "config.h" 
#include "lock.h"
#include "debug.h"
#include "spyer.h"

#define  EXTERN_GLOBALS
#include "ticks.h"

/***************************************************************************************
*   ticks_init() Implementation.
***************************************************************************************/
void_t ticks_init(void_t)
{
    /* Initialize the System Ticks */
    sys_ticks = (uint32_t)0; 

    /* Spyer */
    SPYER_TICKS("System Ticks Initialized, Ticks %d", sys_ticks); 
}

/***************************************************************************************
*   ticks_increase() Implementation.
***************************************************************************************/
void_t ticks_increase(void_t) 
{
    /* Increase the System Ticks */
    ++sys_ticks; 

    /* Spyer */
    SPYER_TICKS("System Ticks Increased, Ticks %d", sys_ticks); 
}

/***************************************************************************************
*   ticks_get() Implementation.
***************************************************************************************/
uint32_t ticks_get(void_t)
{
    uint32_t ret; 

    INT_LOCK_KEY_

    INT_LOCK_();
    ret = sys_ticks; 
    INT_UNLOCK_(); 

    return ret; 
}
