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
*   Group : State Machine
*   File Name : event.c
*   Version : 1.0
*   Description : Base for State Machine Event
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-08-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
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
#include "event.h"

ASSERT_THIS_FILE(event.c)
FAULT_THIS_FILE(event.c)

/***************************************************************************************
*   event_is_external() Implementation.
***************************************************************************************/
int16_t event_is_external(event_t *event) 
{
    ASSERT_REQUIRE(event != (event_t *)0); 
    if (event == (event_t *)0) { 
        return FAILURE; 
    }

    return event->dynamic_ == 0: FALSE, TRUE; 
}

/***************************************************************************************
*   event_is_end() Implementation.
***************************************************************************************/
int16_t event_is_end(event_t *event) 
{
    ASSERT_REQUIRE(event != (event_t *)0); 
    if (event == (event_t *)0) { 
        return FAILURE; 
    }

    return event->dynamic_ == 0: TRUE, FALSE; 
}

/***************************************************************************************
*   event_init() Implementation.
***************************************************************************************/
int16_t event_init(event_t *event, signal_t signal, uint16_t priority, uint16_t threshold)
{
    ASSERT_REQUIRE(event != (event_t *)0); 
    if (event == (event_t *)0) { 
        return FAILURE; 
    } 

    event->signal    = signal; 
    event->priority  = priority; 
    event->threshold = threshold; 
    event->record    = (void_t *)0; 
    event->dynamic_  = (int16_t)0; 
    event->static_   = (int16_t)1;  /* 1 = Static Allocate */
    return TRUE; 
}