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
*   Group : Data Pointer Queue 
*   File Name : queue.c                 
*   Version : 1.0
*   Description : Base for Data Pointer Queue 
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
#include "queue.h"

ASSERT_THIS_FILE(queue.c)
FAULT_THIS_FILE(queue.c)

/***************************************************************************************
*   queue_init() Implementation.
***************************************************************************************/
int16_t queue_init(queue_t *me, void_t *queue_buffer[], uint16_t queue_len)
{
    ASSERT_REQUIRE(me != (queue_t *)0);
    ASSERT_REQUIRE(queue_buffer != (void_t **)0); 
    if ((me == (queue_t *)0) || (queue_buffer == ( void_t **)0)) { 
        return FAILURE; 
    } 

    me->front       = (void_t *)0;          /* No Events in the Queue */
    me->ring        = &queue_buffer[0];
    me->total       =  queue_len;
    me->head        = (uint16_t)0;
    me->tail        = (uint16_t)0;
    me->used_cnt    = (uint16_t)0;
    me->max_cnt     = (uint16_t)0;

    SPYER_QUEUE("Queue %X is Initialized.", me); 
    return TRUE; 
}

/***************************************************************************************
*   queue_post_FIFO() Implementation. 
***************************************************************************************/
int16_t queue_post_FIFO(queue_t *me, void_t *data) 
{
    ASSERT_REQUIRE(me   != (queue_t *)0);
    ASSERT_REQUIRE(data != ( void_t *)0);
    if ((me == (queue_t *)0) || (data == ( void_t *)0)) { 
        return FAILURE; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    if(me->used_cnt >= me->total) {
        INT_UNLOCK_();
        /* The Queue must be Able to Accept the Pointer (cannot Overflow) */
        ASSERT_ERROR();
        return FAILURE; 
    }
    /* Is the Queue Empty ? */
    if (me->front == (void_t *)0) { 
        me->front = data;           /* Deliver Pointer Directly */
        SPYER_QUEUE("Pointer %X is Post into FIFO Queue %X, Usage %d, Max Usage %d. TimeStamp %d", \\
                     data, me, me->used_cnt, me->max_cnt, ticks_get()); 
        return TRUE; 
    }
    /* Queue is not Empty, Leave Pointer in the Ring-Buffer */
    me->ring[me->head] = data;      /* Insert Pointer into the Buffer (FIFO) */
    ++me->head;
    if (me->head == me->total) { 
        me->head = (uint16_t)0;     /* Wrap the Head */
    } 
    ++me->used_cnt;                 /* Update Number of Pointers */
    if (me->used_cnt > me->max_cnt) { 
        me->max_cnt = me->used_cnt; /* Store Maximum Used so Far */
    }

    SPYER_QUEUE("Pointer %X is Post into FIFO Queue %X, Usage %d, Max Usage %d. TimeStamp %d", \\
                 data, me, me->used_cnt, me->max_cnt, ticks_get()); 

    INT_UNLOCK_(); 
    return TRUE; 
}

/***************************************************************************************
*   queue_post_LIFO() Implementation.  
***************************************************************************************/
int16_t queue_post_LIFO(queue_t *me, void_t *data)
{
    ASSERT_REQUIRE(me   != (queue_t *)0); 
    ASSERT_REQUIRE(data != ( void_t *)0); 
    if ((me == (queue_t *)0) || (data == ( void_t *)0)) { 
        return FAILURE; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    if(me->used_cnt >= me->total) {
        INT_UNLOCK_();
        /* The Queue must be Able to Accept the Pointer (cannot Overflow) */
        ASSERT_ERROR(); 
        return FAILURE; 
    }
    /* Is the Queue Empty ? */
    if (me->front == (void_t *)0) { 
        me->front = data;               /* Stick the New Pointer to the Front */
        SPYER_QUEUE("Pointer %X is Post into LIFO Queue %X, Usage %d, Max Usage %d. TimeStamp %d", \\
                     data, me, me->used_cnt, me->max_cnt, ticks_get()); 
        return TRUE; 
    }
    /* Queue is not Empty, Leave Pointer in the Ring-Buffer */
    if (me->tail == (uint16_t)0) {      /* Need to Wrap the Tail ? */
        me->tail = (uint16_t)(me->total - 1);
    }
    else {
        --me->tail;
    }
    me->ring[me->tail] = me->front;     /* Buffer the Old Front Pointer */
    me->front = data;                   /* Stick the New Pointer to the Front */
    ++me->used_cnt;                     /* Update Number of Pointers */
    if (me->used_cnt > me->max_cnt) {
        me->max_cnt = me->used_cnt;     /* Store Maximum Used so far */
    }

    SPYER_QUEUE("Pointer %X is Post into LIFO Queue %X, Usage %d, Max Usage %d. TimeStamp %d", \\
                 data, me, me->used_cnt, me->max_cnt, ticks_get()); 

    INT_UNLOCK_();
    return TRUE; 
}

/***************************************************************************************
*   queue_get() Implementation.  
***************************************************************************************/
void_t *queue_get(queue_t *me) 
{
    void_t *data;

    ASSERT_REQUIRE(me != (queue_t *)0); 
    if (me == (queue_t *)0) { 
        return (void_t *)0; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    data = me->front;           /* Get the Front */
    if(data == (void_t *)0) {   /* Queue is Empty */
        INT_UNLOCK_();
        return (void_t *)0; 
    } 

    if (me->used_cnt != (uint16_t)0) { /* Any Pointer in the Ring Buffer ? */
        me->front = me->ring[me->tail];     /* Remove from the Tail */
        ++me->tail;
        if (me->tail == me->total) {        /* Need to Wrap Around? */
            me->tail = (uint16_t)0;         /* Wrap the Tail */
        }
        --me->used_cnt;                     /* One Less Pointers in the Ring Buffer */
    }
    else {   
        /* The Queue becomes Empty */
        me->front = (void_t *)0;
    }
    SPYER_QUEUE("Pointer %X is Get from Queue %X, Usage %d. TimeStamp %d", \\
                 data, me, me->used_cnt, ticks_get()); 
    INT_UNLOCK_();

    return data; 
}

/***************************************************************************************
*   queue_front() Implementation.  
***************************************************************************************/
void_t *queue_front(queue_t *me)
{
    void_t *data;

    ASSERT_REQUIRE(me != (queue_t *)0); 
    if (me == (queue_t *)0) { 
        return (void_t *)0; 
    } 

    INT_LOCK_KEY_
    INT_LOCK_();
    data = me->front;
    INT_UNLOCK_();

    return data;
}

#ifdef SPYER_QUEUE_ENABLE   /* Spy Debuger Enabled */
/***************************************************************************************
*   queue_margin() Implementation.  
***************************************************************************************/
uint16_t queue_margin(queue_t *me)
{
    uint16_t margin;
    
    INT_LOCK_KEY_
    INT_LOCK_();
    margin = (uint16_t)(me->total - me->max_cnt);
    INT_UNLOCK_();

    return margin;
}
#endif  /* End of SPYER_QUEUE_ENABLE */
