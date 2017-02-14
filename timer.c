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
*   Group : Timer 
*   File Name : timer.c
*   Version : 1.0
*   Description : Base for Timer 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-13-2005      Created Initial Version 1.0
*       JinHui Han      03-13-2005      Verified Version 1.0
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
#include "epool.h"
#include "cpool.h"
#include "echain.h"
#include "rchain.h"
#include "fsm.h"
#include "hsm.h"
#include "active.h"
#include "achain.h"
#include "callback.h"
#include "manager.h"

#define  EXTERN_GLOBALS
#include "timer.h"

ASSERT_THIS_FILE(timer.c)
FAULT_THIS_FILE(timer.c)

/***************************************************************************************
*   timer_init() Implementation.
***************************************************************************************/
int16_t timer_init(void_t)
{
    timer_list_ = (timer_t *)0; 
    return TRUE; 
}

/***************************************************************************************
*   timer_arm() Implementation.
***************************************************************************************/
int16_t timer_arm(timer_t *me, int32_t count, int32_t interval, event_t *event)
{
    ASSERT_REQUIRE(me != (timer_t *)0);
    ASSERT_REQUIRE(me->prev == (timer_t *)0);         /* Timer Event must NOT be Used */
    ASSERT_REQUIRE(count > (int32_t)0);               /* Cannot Arm a Timer with 0 Ticks */
    ASSERT_REQUIRE(interval >= (int32_t)0); 
    if((me == (timer_t *)0) || (me->prev != (timer_t *)0)) { 
        return FALSE; 
    } 
    if ((count <= (int32_t)0) || (interval < (int32_t)0)) { 
        return FALSE; 
    } 

    me->counter  = count;       /* Set the Counter  */
    me->interval = interval;    /* Set the Interval */
    me->event    = event;       /* Post Event when Expired */
    me->prev     = me;          /* Mark the Timer in Used, Possess It First */

    INT_LOCK_KEY_
    INT_LOCK_();
    /* Append This Timer into the List Head of Timer List */
    me->next = timer_list_;
    if (timer_list_ != (timer_t *)0) {
        timer_list_->prev = me;
    }
    timer_list_ = me;
    INT_UNLOCK_();

    SPYER_TIMER("Timer %X is Armed, Counter %d, Interval %d. TimeStamp %d", \\
                 me, me->counter, me->interval, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   timer_rearm() Implementation.
***************************************************************************************/
int16_t timer_rearm(timer_t *me, int32_t count, int32_t interval)
{
    int16_t ret;
    
    ASSERT_REQUIRE(me != (timer_t *)0);
    ASSERT_REQUIRE(count > (int32_t)0); 
    ASSERT_REQUIRE(interval >= (int32_t)0); 
    if(me == (timer_t *)0) { 
        return FALSE; 
    } 
    if ((count <= (int32_t)0) || (interval < (int32_t)0)) { 
        return FALSE; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    me->counter  = count; 
    me->interval = interval; 
    if (me->prev == (timer_t *)0) {  /* Is this Timer Disarmed ? */
        ret = FALSE;    /* The Timer is Disarmed */
    }
    else { 
        ret = TRUE;     /* The Timer is Armed */
    }
    INT_UNLOCK_();

    SPYER_TIMER("Timer %X is Rearmed, Counter %d, Interval %d, Status %d. TimeStamp %d", \\
                 me, me->counter, me->interval, ret, ticks_get()); 
    return ret;
}

/***************************************************************************************
*   timer_disarm() Implementation.
*
*   NOTE: Disarm a Timer (no Harm in Disarming an Already Disarmed Timer)
***************************************************************************************/
int16_t timer_disarm(timer_t *me)
{
    int16_t ret;

    ASSERT_REQUIRE(me != (timer_t *)0);
    if(me == (timer_t *)0) { 
        return FALSE; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    if (me->prev != (timer_t *)0) { /* Is the Timer Actually Armed ? */
        /* The Timer was Armed */
        ret = TRUE;
        if (me == timer_list_) {
            timer_list_ = me->next;
            timer_list_->prev = timer_list_;
        }
        else {
            if (me->next != (timer_t *)0) {   
                /* Not the Last in the List */
                me->next->prev = me->prev;
            }
            me->prev->next = me->next;
        }
        me->prev = (timer_t *)0;    /* Mark the Timer as Disarmed */
    }
    else {
        /* The Timer was not Armed */
        ret = FALSE;
    }
    INT_UNLOCK_();

    SPYER_TIMER("Timer %X is Disarmed, Counter %d, Interval %d, Status %d. TimeStamp %d", \\
                 me, me->counter, me->interval, ret, ticks_get()); 
    return ret;
}

/***************************************************************************************
*   task_timer() Implementation.
***************************************************************************************/
int16_t task_timer(void_t)
{
    int16_t  ret; 
    timer_t *timer;

    SPY_BEGIN_(SPY_TIMER_TASK_T, spy_timer_object_, (void_t *)&task_timer);
        SPY_TIME_();                    /* Timestamp */
        SPY_FUNCTION_(&task_timer);     /* This Timer Task */
    SPY_END_();

    timer = timer_list_; 
    while (timer != (timer_t *)0) {
        if (--timer->counter == (int32_t)0) {   /* Is Timer about to Expire ? */
            if (timer->interval != (int32_t)0) {    /* Is it Periodic Timer ? */
                timer->counter = timer->interval;   /* Rearm the Timer */

                SPYER_TIMER("Periodic Timer %X is Reset, Interval %d. TimeStamp %d", \\
                            timer, timer->interval, ticks_get()); 
            }
            else {   
                /* One-Shot Timeout, Disarm by Removing it from the List */
                if (timer == timer_list_) {
                    timer_list_ = timer->next;
                    timer_list_->prev = timer_list_;
                }
                else {
                    if (timer->next != (timer_t *)0) {   /* Not the Last Event ? */
                        timer->next->prev = timer->prev;
                    }
                    timer->prev->next = timer->next;
                }
                timer->prev = (timer_t *)0;         /* Mark the Event Disarmed */

                SPYER_TIMER("One-Shot Timer %X is Disarmed. TimeStamp %d", \\
                            timer, ticks_get()); 
            }

            ASSERT_REQUIRE(timer->event != (event_t *)0); 
            if (timer->event != (event_t *)0) { 
                ret = man_post(timer->event); 
                ASSERT_ENSURE(ret == TRUE); 
                if (ret != TRUE) { 
                    /* Deal With Fault at Here */

                } 
                SPYER_TIMER("Timer Event %X is Post, Fault %d. TimeStamp %d", \\
                            timer->event, ret, ticks_get()); 
            }
        }
        timer = timer->next; 
    }
}
