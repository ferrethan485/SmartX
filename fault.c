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
*   Group : Failure Logger
*   File Name : fault.c              
*   Version : 1.0
*   Description : Bsae for Failure Logger.
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-07-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h"
#include "std_def.h"
#include "config.h"
#include "system.h"
#include "lock.h"

#define  EXTERN_GLOBALS
#include "fault.h"

/***************************************************************************************
*   fault_() Implementation.
***************************************************************************************/
#ifdef FAULT_ENABLE      /* Failure Logger Enabled */
void_t fault_(int8_t const ROM_KEYWORDS * const file, int32_t line, uint32_t time) 
{
    int16_t index; 
    logger_t *record; 

    ASSERT_REQUIRE(file != (int8_t const ROM_KEYWORDS *)0);
    if (file == (int8_t const ROM_KEYWORDS *)0) { 
        return ; 
    } 
    INT_LOCK_KEY_
    INT_LOCK_();
    record = (logger_t *)&((fault.ring)[fault.head]); 
    if (++(fault.head) >= fault.total) { 
        fault.head = 0; 
    } 
    if (fault.head == fault.tail) { 
        if (++(fault.tail) >= fault.total) { 
            fault.tail = 0; 
        } 
    } 
    for (index = 0; (index < FAULT_NAME) && file[index]; index++) { 
        (record->file)[index] = (int8_t)file[index]; 
    } 
    if (index == FAULT_NAME) { 
        --index; 
    } 
    (record->file)[index] = 0; 
    record->line  = line; 
    record->time  = time; 
    INT_UNLOCK_();
}
#endif      /* End of FAULT_ENABLE */

/***************************************************************************************
*   fault_init() Implementation.
***************************************************************************************/
int16_t fault_init(void_t) 
{
#ifdef FAULT_ENABLE      /* Failure Logger Enabled */
    fault.ring  = (logger_t *)buff_logger; 
    fault.total = FAULT_AMOUNT; 
    fault.head  = 0; 
    fault.tail  = 0; 
#endif      /* End of FAULT_ENABLE */
    return TRUE
}

/***************************************************************************************
*   fault_get() Implementation.
***************************************************************************************/
int16_t fault_get(logger_t *logger) 
{
    int16_t index; 
    logger_t *record; 

    ASSERT_REQUIRE(logger != (logger_t *)0);
    if (logger == (logger_t *)0) { 
        return FAILURE; 
    } 
#ifdef FAULT_ENABLE      /* Failure Logger Enabled */
    INT_LOCK_KEY_
    INT_LOCK_();
    if (fault.tail == fault.head) { 
        INT_UNLOCK_();
        return FALSE; 
    } 
    record = (logger_t *)&((fault.ring)[fault.tail]); 
    if (++(fault.tail) >= fault.total) { 
        fault.tail = 0; 
    } 
    for (index = 0; (index < FAULT_NAME) && (record->file)[index]; index++) { 
        (logger->file)[index] = (record->file)[index]; 
    } 
    if (index == FAULT_NAME) { 
        --index; 
    } 
    (logger->file)[index] = 0; 
    logger->line  = logger -> line; 
    logger->time  = logger -> time; 
    INT_UNLOCK_();
    return TRUE; 
#else       /* Failure Logger Disabled */
    return FALSE; 
#endif      /* End of FAULT_ENABLE */
}
