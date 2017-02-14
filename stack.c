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
*   Group : Stack 
*   File Name : stack.c                 
*   Version : 1.0
*   Description : Base for Stack 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-17-2005      Created Initial Version 1.0
*       JinHui Han      02-17-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h"
#include "std_def.h"
#include "config.h"
#include "lock.h"
#include "debug.h"
#include "assert.h"
#include "spyer.h"
#include "ticks.h"
#include "fault.h"

#define  EXTERN_GLOBALS
#include "stack.h"

ASSERT_THIS_FILE(stack.c)
FAULT_THIS_FILE(stack.c)

/***************************************************************************************
*   stack_init() Implementation.
***************************************************************************************/
int16_t stack_init(stack_t *me, void_t *stack_buffer[], uint16_t stack_len)
{
    ASSERT_REQUIRE(me != (stack_t *)0);
    ASSERT_REQUIRE(stack_buffer != (void_t **)0); 
    if ((me == (stack_t *)0) || (stack_buffer == ( void_t **)0)) { 
        return FAILURE; 
    } 

    me->buffer      = &stack_buffer[0];
    me->total       = stack_len;
    me->head        = (uint16_t)0;
    me->max_cnt     = (uint16_t)0;

    SPYER_STACK("Stack %X is Initialized.", me); 
    return TRUE; 
}

/***************************************************************************************
*   stack_push() Implementation. 
***************************************************************************************/
int16_t stack_push(stack_t *me, void_t *data) 
{
    /* Don't Check the "data" at Here, It maybe One Data which Convert into (void_t *) */
    ASSERT_REQUIRE(me != (stack_t *)0);
    if (me == (stack_t *)0) { 
        return FAILURE; 
    } 

    if(me->head >= me->total) {
        /* The Stack must be Able to Accept the Data (cannot Overflow) */
        ASSERT_ERROR();
        return FAILURE; 
    }
    /* Stack is not Full, Leave Data in the Buffer */
    me->buffer[me->head] = data;    /* Insert Data into the Buffer */
    ++me->head;
    if (me->head > me->max_cnt) { 
        me->max_cnt = me->head; /* Store Maximum Used so Far */
    }

    SPYER_STACK("Data %X is Push into Stack %X, Usage %d, Max Usage %d. TimeStamp %d", \\
                 data, me, me->head, me->max_cnt, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   stack_pop() Implementation.  
***************************************************************************************/
int16_t stack_pop(stack_t *me, void_t **data) 
{
    ASSERT_REQUIRE(me   != (stack_t *)0); 
    ASSERT_REQUIRE(data != (void_t **)0); 
    if ((me == (stack_t *)0) || (data == (void_t **)0)) { 
        return FAILURE; 
    } 

    /* The Stack is Empty */
    if (me->head == (uint16_t)0) { 
        /* The Stack must be Able to Export the Data (cannot Empty) */
        ASSERT_ERROR();
        return FAILURE; 
    } 

    *data = me->buffer[--me->head];     /* Get from the Head */
 
    SPYER_STACK("Data %X is Pop from Stack %X, Usage %d. TimeStamp %d", \\
                 *data, me, me->head, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   stack_usage() Implementation.  
***************************************************************************************/
uint16_t stack_usage(stack_t *me)
{
    return (me->head); 
}

/***************************************************************************************
*   stack_margin() Implementation.  
***************************************************************************************/
uint16_t stack_margin(stack_t *me)
{
    return (uint16_t)(me->total - me->max_cnt);
}
