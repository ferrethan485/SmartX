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
*   Group : Active Object Manager
*   File Name : manager.c
*   Version : 1.0
*   Description : Based for Active Object Manager
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-31-2005      Created Initial Version 1.0
*       JinHui Han      03-31-2005      Verified Version 1.0
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

#define  EXTERN_GLOBALS
#include "manager.h"

ASSERT_THIS_FILE(manager.c)
FAULT_THIS_FILE(manager.c)

/***************************************************************************************
*   man_version() Implementation.
***************************************************************************************/
int8_t const ROM_KEYWORDS * man_version(void_t)
{
    static int8_t const ROM_KEYWORDS object_version[] = "MANAGER 1.0.00";
    return object_version;
}

/***************************************************************************************
*   man_init() Implementation.
***************************************************************************************/
int16_t man_init(void_t)
{
    int16_t ret; 
    manager_t *manager; 

    /* Get the Manager */
    manager = (manager_t *)&man; 

    manager -> state  = 0; 
    manager -> enrool = (chain_t *)0; 
    /***********************************************************************************
    *   Priority Based Event Scheduler.
    ***********************************************************************************/
    manager -> event    = (chain_t *)0; 
    manager -> priority = (uint16_t)65535;  /* Lowest Priority */
    manager -> head     = (chain_t *)0; 
    manager -> current  = (chain_t *)0; 
    manager -> stack    = (stack_t *)&man_stack; 
    /***********************************************************************************
    *   Priority Based Scan Scheduler.
    ***********************************************************************************/
    manager -> scan_event   = (chain_t *)0; 
    manager -> scan_head    = (chain_t *)0; 
    manager -> scan_current = (chain_t *)0; 
    manager -> idle = (uint16_t)0; 
    /***********************************************************************************
    *   Failure Logging Event. 
    ***********************************************************************************/
    event_init((event_t*)&((manager -> logger_event).super), 
               (signal_t)SIG_FAULT, PRIO_EVT_FAULT, PRIO_EVT_FAULT); 
    ((manager -> logger_event).logger).file[0] = (int8_t)0; 
    ((manager -> logger_event).logger).line  = ( int16_t)0; 
    ((manager -> logger_event).logger).time  = (uint32_t)0; 

    /***********************************************************************************
    *   Initialze the Stack and Failure Logging. 
    ***********************************************************************************/
    ret = stack_init(manager -> stack, (void_t **)&man_buffer[0], (uint16_t)LEN_BUFF_MSTACK); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_MANAGER("The Active Object Manager %X is Initialized.", manager); 

    /* Callback for Initialize the Active Objects Defined by Application */
    on_init (); 
    return TRUE; 
}

/***************************************************************************************
*   man_subscribe() Implementation.
***************************************************************************************/
int16_t man_subscribe(event_t *event, active_t *active)
{
    int16_t   ret; 
    chain_t * chain; 
    chain_t **head; 
    chain_t **enroll; 
    manager_t *manager; 

    ASSERT_REQUIRE(event  != ( event_t *)0);
    ASSERT_REQUIRE(active != (active_t *)0);
    if ((event == (event_t *)0) || (active == (active_t *)0)) { 
        return FAILURE; 
    } 

    /* Get the Manager */
    manager = (manager_t *)&man; 
    /* Get the Enroll Book */
    enroll = (chain_t **)&(manager -> enrool);
    /* Has the Event with Same "signal" in the Subscribes List ? */
    chain = rchain_find(enroll, event->signal)
    if (chain != (chain_t *)0) { 
        /* Release the Event */
        ret = epool_release(event); 
        ASSERT_ENSURE(ret == TRUE); 
        if (ret != TRUE) { 
            return FAILURE; 
        } 
        /* Get List Head of Active Object Chain */
        event = (event_t *)(chain->builtin); 
        head  = (chain_t **)&(event->record); 
        ASSERT_REQUIRE(*head != (chain_t *)0); 
    } 
    else { 
        /* Bind the Event into Register Chain */
        ret = rchain_bind(enroll, event); 
        ASSERT_ENSURE(ret == TRUE); 
        if (ret != TRUE) { 
            return FAILURE; 
        } 
        /* Get List Head of Active Object Chain */
        head = (chain_t **)&(event->record); 
        /* Initialize the List Head of Active Object Chain */
        ret = achain_init(head); 
        ASSERT_ENSURE(ret == TRUE); 
    } 
    /* Append the Active Object to the Subscribes List */
    ret = achain_bind(head, active); 
    ASSERT_ENSURE(ret == TRUE); 
    if (ret != TRUE) { 
        return FAILURE; 
    } 

    SPYER_MANAGER("The Event (Signal %d) is Subscribed by the Active Object %X. TimeStamp %d", \\
                  event->signal, active, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   man_unsubscribe() Implementation.
***************************************************************************************/
int16_t man_unsubscribe(event_t *event, active_t *active)
{
    int16_t   ret; 
    signal_t  signal;
    chain_t * chain; 
    chain_t **head; 
    chain_t **enroll; 
    manager_t *manager; 

    ASSERT_REQUIRE(event  != ( event_t *)0);
    ASSERT_REQUIRE(active != (active_t *)0);
    if ((event == (event_t *)0) || (active == (active_t *)0)) { 
        return FAILURE; 
    } 

    /* Get the Manager */
    manager = (manager_t *)&man; 
    /* Get the Enroll Book */
    enroll = (chain_t **)&(manager -> enrool);
    /* Get the Signal of Event */
    signal = event->signal; 
    /* Release the Event */
    ret = epool_release(event); 
    ASSERT_ENSURE(ret == TRUE); 
    if (ret != TRUE) { 
        return FAILURE; 
    } 
    /* Has the Event with Same "signal" in the Subscribes List ? */
    chain = rchain_find(enroll, signal)
    ASSERT_REQUIRE(chain != (chain_t *)0); 
    if (chain == (chain_t *)0) { 
        return FAILURE; 
    } 
    /* Get List Head of Active Object Chain */
    event = (event_t *)(chain->builtin); 
    head  = (chain_t **)&(event->record); 
    ASSERT_REQUIRE(*head != (chain_t *)0); 
    if (*head == (chain_t *)0) { 
        return FAILURE; 
    } 
    /* Remove the Active Object from the Subscribes List */
    ret = achain_unbind(head, active); 
    ASSERT_ENSURE(ret == TRUE); 
    if (ret != TRUE) { 
        return FAILURE; 
    } 

    /* The Subscribes List of Active Object become Empty ? */
    if (*head == (chain_t *)0) { 
        /* Unbind the Event from Register Chain */
        ret = rchain_unbind(enroll, chain); 
        ASSERT_ENSURE(ret == TRUE); 
    } 

    SPYER_MANAGER("The Event (Signal %d) is Un-Subscribed by the Active Object %X. TimeStamp %d", \\
                  signal, active, ticks_get()); 

    return TRUE; }

/***************************************************************************************
*   man_unsubscribe_all() Implementation.
***************************************************************************************/
int16_t man_unsubscribe_all(active_t *active)
{
    int16_t   ret; 
    event_t * event; 
    chain_t * chain; 
    chain_t * next; 
    chain_t **head; 
    chain_t **enroll; 
    manager_t *manager; 

    ASSERT_REQUIRE(active != (active_t *)0);
    if (active == (active_t *)0) { 
        return FAILURE; 
    } 

    /* Get the Manager */
    manager = (manager_t *)&man; 
    /* Get the Enroll Book */
    enroll = (chain_t **)&(manager -> enrool);
    /* Un-Subscribe All Event associated with the Active Object */
    chain = *enroll; 
    do { 
        /* Get List Head of Active Object Chain */
        event = (event_t *)(chain->builtin); 
        head  = (chain_t **)&(event->record); 
        ASSERT_REQUIRE(*head != (chain_t *)0); 
        /* Remove the Active Object from the Subscribes List */
        ret = achain_unbind(head, active); 
        ASSERT_ENSURE(ret == TRUE); 
        /* Backup the Next Chain Cell */
        next = chain->next; 
        /* The Subscribes List of Active Object become Empty ? */
        if (*head == (chain_t *)0) { 
            /* Unbind the Event from Register Chain */
            ret = rchain_unbind(enroll, chain); 
            ASSERT_ENSURE(ret == TRUE); 
        } 
        /* Pointer to Next Chain Cell */
        chain = next; 

        SPYER_MANAGER("The Event (Signal %d) is Un-Subscribed by the Active Object %X. TimeStamp %d", \\
                       event->signal, active, ticks_get()); 
    } while (chain != *enroll); 

    return TRUE; 
}

/***************************************************************************************
*   man_subscribe_find() Implementation.
***************************************************************************************/
chain_t *man_subscribe_find(event_t *event)
{
    chian_t * active; 
    chain_t * chain; 
    chain_t * enroll; 
    manager_t *manager; 

    ASSERT_REQUIRE(event != (event_t *)0);
    if (event == (event_t *)0) { 
        return (chain_t *)0; 
    } 

    /* Get the Manager */
    manager = (manager_t *)&man; 
    /* Get the Enroll Book */
    enroll  = manager->enrool;
    /* Find the Active Object Chain which Subscribe this Event */
    chain = enroll; 
    do { 
        /* Is the Want Event ? */
        if (((event_t *)(chain->builtin))->signal == event->signal) { 
            active = (chian_t *)(((event_t *)(chain->builtin))->record); 

            SPYER_MANAGER("The Event (Signal %d) is Find, The Active Object Chain %X is Return. TimeStamp %d", \\
                       event->signal, active, ticks_get()); 

            return active; 
        } 
        /* Pointer to Next Chain Cell */
        chain = chain->next; 
    } while (chain != enroll); 

    return (chain_t *)0; 
}

/***************************************************************************************
*   man_post() Implementation.
***************************************************************************************/
int16_t man_post(event_t *event) 
{
    int16_t   ret; 
    manager_t *manager; 

    ASSERT_REQUIRE(event != (event_t *)0); 
    if(event == (event_t *)0) { 
        return FAILURE; 
    }

    /* Get the Manager */
    manager = (manager_t *)&man; 
    ret = echain_post((chain_t **)&(manager -> event), event); 
    ASSERT_ENSURE(ret == TRUE); 
    if (ret != TRUE) { 
        return FAILURE; 
    } 
    return TRUE; 
}

/***************************************************************************************
*   man_publish() Implementation.
***************************************************************************************/
int16_t man_publish(event_t *event) 
{
    int16_t   ret; 
    event_t * event; 
    chain_t * chain; 
    chain_t * head; 
    active_t  *active; 
    manager_t *manager; 

    ASSERT_REQUIRE(event != (event_t *)0); 
    if(event == (event_t *)0) { 
        return FAILURE; 
    }

    /* Get the Manager */
    manager = (manager_t *)&man; 
    /* Get the Active Object Chain Head */
    head = (chain_t *)(manager -> head); 

    /***********************************************************************************
    *   Perform Multicasting with the Scheduler Locked.
    *   NOTE: 
    *   (1) man_publish() Uses Scheduler Locking to Prevent other Active Objects 
    *       from Running while the Event Multicasting is in Progress. 
    *   (2) During the Publication, the Subscription Lists are Assumed not to be 
    *       Changing. The Latter Assumption is True as Long as man_subscribe()/
    *       unsubscribe()/unsubscrib_all() are NOT Called from Interrupts, because 
    *       Only Interrupts can Preempt the Current Task wile the Scheduler is Locked.
    *   (3) Using Scheduler Lock at Here Can Avoid the Lose of External Event which 
    *       is Producted by External Peripheral of Device.
    ***********************************************************************************/
    /* Multicasting the Event to All the Avtive Object */
    chain = head; 
    do { 
        /* Get the Active Object */ 
        active = (active_t *)(chain -> builtin); 

        /*******************************************************************************
        *   Post the Event Directly to the Event Chain of the Acitve Object. 
        *   NOTE: 
        *   (1) Direct Event Posting is the Simplest Asynchronous Communication Method 
        *       Available in Framework. 
        *   (2) The Producer of the Event Only Require to "know" the Recipient through 
        *       a Generic Pointer, but the Specific Definition of the Active Object 
        *       Structure is not Required.
        *   (3) Direct Event Posting should not be Confused with Direct Event Dispatching. 
        *       In Contrast to Asynchronous Event Posting through Event Queues, Direct 
        *       Event Dispatching is Synchronous. Direct Event Dispatching Occurs when 
        *       You Call hsm_dispatch() Function.
        *******************************************************************************/
        ret = echain_post((chain_t **)&(active -> event), event); 
        ASSERT_ENSURE(ret == TRUE); 
        if (ret != TRUE) { 
            return FAILURE; 
        } 
        /* Increment the Reference Counter */
        ++event->dynamic_;

        SPYER_MANAGER("The Event (Signal %d) is Published to Active Object %X, dynamic %d. TimeStamp %d", \\
                       event->signal, active, event->dynamic_, ticks_get()); 
    } while (chain != head); 

    return TRUE; 
}

/***************************************************************************************
*   man_reset_context() Implementation. Internal
***************************************************************************************/
void_t man_reset_context(void_t) 
{
    manager -> priority = (uint16_t)65535;  /* Lowest Priority */
    manager -> head     = (chain_t *)0; 
    manager -> current  = (chain_t *)0; 
    manager -> stack    = (stack_t *)&man_stack; 
    ret = stack_init(manager->stack, (void_t **)&man_buffer[0], (uint16_t)LEN_BUFF_MSTACK); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_MANAGER("The Event Scheduler %X is Initialized Again.", manager); 
}

/***************************************************************************************
*   man_scheduler() Implementation.
***************************************************************************************/
int16_t man_scheduler(void_t)
{
    int16_t  ret; 
    event_t *event; 
    chain_t *chain; 
    active_t  *active; 
    manager_t *manager; 
    logger_t  *logger; 

    INT_LOCK_KEY_

    /* Get the Manager */
    manager = (manager_t *)&man; 
    /* Dispose the Fault Record, Post the Fault Event */
    /***********************************************************************************
    *   Failure Logging Event. 
    ***********************************************************************************/
    if (event_is_end((event_t *)&(manager -> logger_event)) == TRUE) { 
        /* Static Event is Handle Complete */
        ret = fault_get((logger_t *)&((manager -> logger_event).logger));
        if (ret == TRUE) { 
            /* Have the Fault Record */
            ret = man_post((event_t *)&(manager -> logger_event)); 
            ASSERT_ENSURE(ret == TRUE); 
            if (ret != TRUE) { 
                /* Deal With Fault at Here */

            } 
        } 
    } 
    /* Execute the Scheduler */
    switch(manager->state) { 
        case 0: 
        if (manager->event == (chain_t *)0) { 
            /* Have not Event need to Deal With */
            manager->state = 1; 
            break; 
        } 
        /* Get the Event Pointer*/
        event = (event_t *)(manager->event->builtin); 
        ASSERT_REQUIRE(event != (event_t *)0); 
        if (event == (event_t *)0) { 
            /* Deal With Fault at Here */

            /* Fault Tolerance at Here */
            /* Discard the Event Simply */
            echain_get((chain_t **)&(manager->event)); 
            manager->state = 1;
            break; 
        } 
        if (event->priority < manager->priority) { 
            /* Have the Higher Event need to Deal With */
            ret = (int16_t)stack_margin(manager->stack); 
            ASSERT_ENSURE(ret >= (int16_t)3); 
            if (ret < (int16_t)3) { 
                /* Deal With Fault at Here */

                /* Fault Tolerance at Here */
                /* Complete the Current Event Scheduler for Get the Stack Space */ 
                manager->state = 1; 
                break; 
            } 
            /* Get the Event from Event Chain */
            event = echain_get((chain_t **)&(manager->event)); 
            ASSERT_ENSURE(event != (event_t *)0); 
            /* Find the Active Object Chain which Subscribe this Event */
            chain = man_subscribe_find(event); 
            if (chain == (chain_t *)0) { 
                /* Not Find */
                manager->state = 1; 
                break; 
            } 
            /* Push Current Context into Stack */
            stack_push(manager->stack, (void_t *)(manager->priority)); 
            stack_push(manager->stack, (void_t *)(manager->head)); 
            stack_push(manager->stack, (void_t *)(manager->current)); 
            /* Set Higher Priority Context */
    `       manager->priority = event->threshold; 
            manager->head     = chain; 
            manager->current  = chain; 
            /* Publish the Event */
            ret = man_publish(event); 
            ASSERT_ENSURE(ret == TRUE); 
            if (ret != TRUE) { 
                /* Deal With Fault at Here */

                /* Fault Tolerance at Here */
                /* Continue Execute at Here */
            } 
        } 
        manager->state = 1;
        break; 
        case 1: 
        if ((manager->head == (chain_t *)0) && (manager->current == (chain_t *)0)) { 
            /* Have No Event Scheduler need to Deal With */
            manager->state = 2; 
            break; 
        } 
        ASSERT_REQUIRE((manager->head != (chain_t *)0) && (manager->current != (chain_t *)0)); 
        if ((manager->head == (chain_t *)0) || (manager->current == (chain_t *)0)) { 
            /* Deal With Fault at Here */

            /* Fault Tolerance at Here */ 
            /* Reset the Context and Continue... */
            man_reset_context(); 
            manager->state = 0; 
            break; 
        } 
        /* Scheduler the Active Object */
        active = (active_t *)(manager->current->builtin); 
        /* Callback the Active Object */
        (active->run)(active); 
        /* Pointer to Next Active Object */
        manager->current = manager->current->next; 
        /* Finish ? */
        if (manager->current == manager->head) { 
            /* */
            ret = (int16_t)stack_usage(manager -> stack); 
            ASSERT_ENSURE(ret >= (int16_t)3); 
            if (ret < (int16_t)3) { 
                /* Deal With Fault at Here */

                /* Fault Tolerance at Here */
                /* Reset the Context and Continue... */
                man_reset_context(); 
                manager->state = 0; 
                break; 
            } 
            /* Restore Previous Context from Stack */
            stack_pop(manager->stack, (void_t **)&(manager->current)); 
            stack_pop(manager->stack, (void_t **)&(manager->head)); 
            stack_pop(manager->stack, (void_t **)&(manager->priority)); 
        } 
        manager->state = 0; 
        break; 
        default: 
        if (manager->enroll == (chain_t *)0) { 
            /* Reset the Scan Scheduler */
            manager->scan_event   = (chain_t *)0; 
            manager->scan_head    = (chain_t *)0; 
            manager->scan_current = (chain_t *)0; 
            manager->idle  = (uint16_t)0; 
            manager->state = 0; 
            break; 
        } 
        if ((manager->scan_event == (chain_t *)0) || 
            (manager->scan_event->next->fore != manager->scan_event) || 
            (manager->scan_event->fore->next != manager->scan_event)
           ) { 
            /* Have Unbind Operations of Event Cell in the Scan Schedule Process */
            /* Initialize the Scan Scheduler */
            manager->scan_event = manager->enroll; 
            manager->scan_head    = (chain_t *)(manager->scan_event->builtin); 
            manager->scan_current = (chain_t *)(manager->scan_event->builtin); 
            manager->idle = (uint16_t)0; 
        } 
        if ((manager->scan_current->next->fore != manager->scan_current) || 
            (manager->scan_current->fore->next != manager->scan_current)
           ) { 
            /* Have Unbind Operations of Active Object Cell in the Scan Schedule Process */
            manager->scan_current = manager->scan_head; 
        } 
        active = (active_t *)(manager->scan_current->builtin); 
        ret = (active->run)(active); 
        if (ret != TRUE) { 
            ++(manager->idle); 
        } 
        manager->scan_current = manager->scan_current->next; 
        if (manager->scan_current == manager->scan_head) { 
            manager->scan_event   = manager->scan_event->next; 
            manager->scan_head    = (chain_t *)(manager->scan_event->builtin); 
            manager->scan_current = (chain_t *)(manager->scan_event->builtin); 
        } 
        manager->state = 0; 
        break; 
    } 
    INT_LOCK_(); 
    if ((manager->event == (chain_t *)0) { 
        if (manager->enroll == (chain_t *)0) { 
            /* No Event in Event Chain and the Enroll is Empty */
            on_idle(int_lock_key_); 
        } 
        if ((manager->scan_event == manager->enroll) && 
            (manager->idle == (uint16_t)0) 
           ) { 
            /* No Event in Event Chain and All the Active Object is in IDLE State */
            on_idle(int_lock_key_); 
        } 
    } 
    INT_UNLOCK_(); 
    if (manager->scan_event == manager->enroll) { 
        manager->idle = (uint16_t)0; 
    } 
    return TRUE; 
}



