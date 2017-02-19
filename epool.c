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
*   Group : Event Pool 
*   File Name : epool.c                 
*   Version : 1.0
*   Description : Base for Event Pool 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-20-2005      Created Initial Version 1.0
*       JinHui Han      02-20-2005      Verified Version 1.0
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
#include "mpool.h"
#include "event.h"

#define  EXTERN_GLOBALS
#include "epool.h"

ASSERT_THIS_FILE(epool.c)
FAULT_THIS_FILE(epool.c)

/***************************************************************************************
*   epool_init() Implementation.
***************************************************************************************/
int16_t epool_init(mpool_t *me, void_t *begin, uint32_t size, uint16_t event_size)
{
    int16_t ret; 

    /* Perfom the Platform-Dependent Initialization of the Pool */
    ret = mpool_init(me, begin, size, event_size);

    SPYER_EMPOOL("Event Pool %X is Initialized, Start %X, Size %d, Event Size %d.", \\
                 me, begin, size, event_size); 
    return ret; 
}

/***************************************************************************************
*   epool_new() Implementation.
***************************************************************************************/
event_t *epool_new(mpool_t *me, signal_t signal, uint16_t priority, uint16_t threshold)
{
    event_t *event;

    /* Get Event -- Platform-Dependent */
    event = (event_t *)mpool_get(me);
    ASSERT_ENSURE(event != (event_t *)0);   /* Pool must not Run out of Events */
    if (event == (event_t *)0) { 
        return NULL; 
    } 

    event->signal    = signal;      /* Set Signal   for This Event */
    event->priority  = priority;    /* Set Priority for This Event */
    event->threshold = threshold;    /* Set Highest Priority Threshold for This Event */
    event->record    = (void_t *)0; /* Set Data Record for This Event */
    /***********************************************************************************
    *   Store the Dynamic Attributes of the Event: The Reference Counter = 0
    ***********************************************************************************/
    event->dynamic_ = (int16_t)0; 
    event->static_  = (int16_t)0; 

    SPYER_EMPOOL("New Event %X from Pool %X, Signal %d, Priority %d. TimeStamp %d", \\ 
                  event, me, signal, priority, ticks_get());
    return event;
}

/***************************************************************************************
*   epool_release() Implementation.
***************************************************************************************/
int16_t epool_release(event_t *event)
{
    int16_t ret;

    ASSERT_REQUIRE(event != (event_t *)0); 
    if (event == (event_t *)0) { 
        return FAILURE; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    --event->dynamic_;
    if (event->dynamic_ > 0) {  /* Isn't This the Last Reference ? */
        /* Decrement the Reference Counter */
        INT_UNLOCK_();
        /* Cast the 'const' Away, which is Legitimate Because it's a Pool Event */
        SPYER_EMPOOL("Attempt Recycle Event %X to Pool %X, Signal %d, Dynamic %d. TimeStamp %d", \\ 
                      event, event->mpool_, event->signal, event->dynamic, ticks_get());
    }
    else {  /* This is the Last Reference to This Event, Recycle It */
        INT_UNLOCK_();
        if (event->static_ == (int16_t)1) {
            return ret; 
        } 
        /* Cast 'const' Away, which is Legitimate, Because it's a Pool Event */
        ret = mpool_put((void_t *)event);
        if (ret != TRUE) { 
            return FAILURE;
        } 
        SPYER_EMPOOL("Recycle Event %X to Pool %X, Signal %d, Dynamic %d. TimeStamp %d", \\ 
                      event, event->mpool_, event->signal, event->dynamic, ticks_get());
    } 
    return  event->dynamic_; 
}

/***************************************************************************************
*   epool_all_recycled() Implementation.
***************************************************************************************/
int16_t epool_all_recycled(mpool_t *me)
{
    return mpool_all_recycled(me); 
}

#ifdef SPYER_EPOOL_ENABLE   /* Spy Debuger Enabled */
/***************************************************************************************
*   epool_margin() Implementation.
***************************************************************************************/
uint16_t epool_margin(mpool_t *me)
{
    return mpool_margin(me);
}
#endif  /* End of SPYER_EPOOL_ENABLE */

