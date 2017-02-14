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
*   Group : Interrupt Lock 
*   File Name : lock.c
*   Version : 1.0
*   Description : Base for Interrupt Lock
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-08-2005      Created Initial Version 1.0
*       JinHui Han      04-08-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h" 
#include "std_def.h"
#include "config.h"

#define  EXTERN_GLOBALS
#include "lock.h"

#ifdef CORTEX_M

/***************************************************************************************
*   interrupt_enable() Implementation.
***************************************************************************************/
cpu_sr_t interrupt_enable(void_t)
{
    cpu_sr_t status; 

    /* Get the Status Register of CPU */
    status = ; 
    /* Enable the Interrupt */

    /* Return the Status Register of CPU */
    return status; 
}

/***************************************************************************************
*   interrupt_disable() Implementation.
***************************************************************************************/
cpu_sr_t interrupt_disable(void_t)
{
    cpu_sr_t status; 

    /* Get the Status Register of CPU */
    status = ; 
    /* Disbale the Interrupt */

    /* Return the Status Register of CPU */
    return status; 
}

/***************************************************************************************
*   interrupt_restore() Implementation.
***************************************************************************************/
void_t   interrupt_restore(cpu_sr_t cpu_sr)
{
    /* Restore the Status Register of CPU */
    
}

#endif  /* End of CORTEX_M */
