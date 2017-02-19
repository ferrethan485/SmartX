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
*   File Name : manager.h
*   Version : 1.0
*   Description : Based for Active Object Manager
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-01-2005      Created Initial Version 1.0
*       JinHui Han      04-01-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _MANAGER_H
#define _MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define MANAGER_EXT
#else
    #define MANAGER_EXT  extern
#endif

/***************************************************************************************
*   Thread's Extended Context Switch (e.g., Uses a Floating Point Co-Processor). 
***************************************************************************************/



/***************************************************************************************
*   Manager for the Active Object Application FrameWork
*
*   Manager Using the Non-Preemptive Scheduling Mechanism, It Implement below 
*   Functions: 
*   (1) Priority Based Scheduler. First, It Find All the Active Objects those Need 
*       to be Handled Based on the Highest Priority Event in the Event Chain "event";  
*       Second, It Dispatch the Highest Priority Event into Event Chain of All Active 
*       Objects those Fond Foregoing; Next, It Call the Active Object One at One Time 
*       According to the Priority of Active Object, If Higher Priority Event has
*       Arrived when Call Someone, It Will Backup the Current Context into Stack, 
*       and Handle this Higher Priority Event in the Same Manner Described as Above,
*       when the Higher Priority Event Scheduling is Complete, It Resume the Context
*       for Previous Event and Continue. 
*   (2) Priority Based Scan Scheduler.
*   (3) Failure Logging. 
***************************************************************************************/
typedef struct manager_tag { 
    /* State Machine */
    int16_t   state; 
    /***********************************************************************************
    *   Priority Based Enroll Book. 
    *   This Enroll Book is Used for the Publish-Subscribe Pattern. This Pattern is 
    *   Used for Event Delivery Mechanism in the Framework. 
    ***********************************************************************************/
    chain_t * enrool; 
    /***********************************************************************************
    *   Priority Based Event Scheduler.
    ***********************************************************************************/
    /* Priority Based Event Chain */
    chain_t * event; 
    /* Priority of Current Executed Event */
    uint16_t  priority; 
    /* Active Object Chain Head */
    chain_t * head; 
    /* Active Object Chain Cell */
    chain_t * current; 
    /* Stack for Priority Based Event Schedule */
    stack_t * stack; 
    /***********************************************************************************
    *   Priority Based Scan Scheduler.
    ***********************************************************************************/
    /* Event Cell in Enroll Book */
    chain_t * scan_event; 
    /* Active Object Chain Head */
    chain_t * scan_head; 
    /* Active Object Chain Cell */
    chain_t * scan_current; 
    /* Idle Counter */
    uint16_t  idle; 
    /***********************************************************************************
    *   Failure Logging Event. 
    ***********************************************************************************/
    struct { 
        event_t  super; 
        logger_t logger; 
    } logger_event; 
} manager_t;

/***************************************************************************************
*   Global Objects Defined for Active Object Manager
***************************************************************************************/
/*  Manager */
MANAGER_EXT manager_t man; 
/*  Stack Used by Manager for Context Switch */
#ifndef  MSTACK_AMOUNT
    /***********************************************************************************
    *   Macro to Override the Default Amount of Manager Stack Buffer.
    *
    *   Default is 8. The Size of Manager Stack Buffer is 
    *   (2*sizeof(chain_t *) + sizeof(uint16_t)) * (MSTACK_AMOUNT + 1) 
    *   NOTE: 
    *   (1) The Value of MSTACK_AMOUNT must Meet the Manager's Requirements. 
    ***********************************************************************************/
    #define MSTACK_AMOUNT    8
#endif
#define LEN_BUFF_MSTACK     (2*sizeof(chain_t *) + sizeof(uint16_t)) * (MSTACK_AMOUNT + 1)
MANAGER_EXT void_t * man_buffer[LEN_BUFF_MSTACK]; 
MANAGER_EXT stack_t  man_stack; 

/***************************************************************************************
*   Signal Internal Constants for System Use Only
***************************************************************************************/
enum {
    SIGNAL_QUIT_ = 1;      /* Signal for Quit   the Active Object */
    SIGNAL_START_;         /* Signal for Start  the Active Object */
    SIGNAL_RESET_;         /* Signal for Reset  the Active Object */
    SIGNAL_PAUSE_;         /* Signal for Pause  the Active Object */
    SIGNAL_RESUME_;        /* Signal for Resume the Active Object */
    SIGNAL_FAULT_;         /* Signal for Fault */
};

#define PRIO_EVT_FAULT      ((uint16_t)0)   /* Event Priority for Fault, Highest */

/***************************************************************************************
*   Used Define the Subscribed Event for the Active Object, Used ONLY by User 
*   Application. 
***************************************************************************************/
typedef struct subscribe_tag { 
    signal_t  signal;       /* Event Signal */
    uint16_t  priority;     /* Event Priority */
    uint16_t  threshold;    /* Event Threshold for Preempt */
}subscribe_t;

/***************************************************************************************
*   Event Pool Used for Event Subscribe. 
***************************************************************************************/
#ifndef MAX_SUBSCRIBE_EVENTS
    /***********************************************************************************
    *   This Constant Defines the Maximum Event Quantity of Event Subscribe. 
    ***********************************************************************************/
    #define MAX_SUBSCRIBE_EVENTS    256     /* Default is 256 */
#endif      /* End of MAX_SUBSCRIBE_EVENTS */

/* Buffer for Subscribe Event Pool */
MANAGER_EXT  uint8_t buff_subscribe[(sizeof(event_t) + sizeof(void_t *)) * MAX_SUBSCRIBE_EVENTS + 4]; 
MANAGER_EXT  mpool_t epool_subscribe; 

/***************************************************************************************
*   Return the Pointer of Active Object Manager's Version.
*
*   This Function Returns Constant Version String in the Format x.y.zz, 
*   Where x (One Digit) is the Major Version, y (One Digit) is the Minor Version, 
*   and zz (Two Digits) is the Maintenance Release Version.
***************************************************************************************/
MANAGER_EXT int8_t const ROM_KEYWORDS * man_version(void_t); 

/***************************************************************************************
*   Initialize the Active Object Manager.
*
*   Description: Initialize the Active Object Manager and Must be Called Exactly once 
*                Before Any Other Operation. 
*   Argument: none. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Function Must be Called Before Any Initialization and Open Action of 
*       Hardware. Or Else, The Hardware maybe Post the Event into the Active Object 
*       Manager.
*   (2) This is, The Interrupt Unlock is Enabled Only after All the Initialize is 
*       Completed (Include Software and Hardware and System Ticker, etc), and Before 
*       Call the man_run(). 
*   (3) This Function Call the External on_init() for Initialize the Active Objects 
*       Defined by Application. on_init() is Defined by Application, Not Defined in 
*       Manager. 
***************************************************************************************/
MANAGER_EXT int16_t man_init(void_t);

/***************************************************************************************
*   Subscribes for the Delivery of Event "signal" to the Active Object.
*
*   Description: Subscribing to an Event, Append the Active Object into Event's 
*                Subscribes List. 
*   Argument: event  -- The Pointer of Event which Active Object will Subscribed. 
*             active -- The Pointer of Active Object which Request to Subscribed. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) Subscribing to an Event means that the Framework will Start Posting
*       Published Events with a Given Signal to the Event Chain of the Active Object. 
***************************************************************************************/
MANAGER_EXT int16_t man_subscribe(event_t *event, active_t *active);

/***************************************************************************************
*   Un-Subscribes for the Delivery of Event "signal" to the Active Object.
*
*   Description: Un-Subscribing from an Event, Remove the Active Object from Event's 
*                Subscribes List. 
*   Argument: me -- The Pointer of Subscribes List. 
*             event  -- The Pointer of Event which Active Object will Un-Subscribed. 
*             active -- The Pointer of Active Object which Request to Un-Subscribed. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) Un-Subscribing from an Event means that the Framework will Stop Posting 
*       Published Events with a Given Signal to the Event Chain of the Active Object.
*   (2) Un-Subscribing from a Event that has Never been Subscribed in the First Place 
*       is Considered an Error and Framework will Rise an Assertion.
*   (3) Due to the Latency of Event Chain, an Active Object should NOT Assume that 
*       a Given Event will Never be Dispatched to the State Machine of the Active 
*       Object after Un-Subscribing from that Event. 
*       The Event might be Already in the Chain, or Just about to be Posted and the 
*       Un-Subscribe Operation will not Flush such Events.
***************************************************************************************/
MANAGER_EXT int16_t man_unsubscribe(event_t *event, active_t *active);

/***************************************************************************************
*   Un-Subscribes from the Delivery of All Event "signal" to the Active Object.
*
*   Description: Un-Subscribing from all Event, Remove the Active Object from All 
*                Event's Subscribes List. 
*   Argument: me -- The Pointer of Subscribes List. 
*             active -- The Pointer of Active Object which Request to Un-Subscribed. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) Un-Subscribing from All Events means that the Framework will Stop Posting 
*       any Published Events to the Event Chain of the Active Object.
*   (2) Due to the Latency of Event Queues, an Active Object should NOT Assume that 
*       no Events will Never be Dispatched to the State Machine of the Active Object 
*       after Un-Subscribing from All Events. 
*       The Events might be Already in the Chain, or Just about to be Posted and the 
*       Un-Subscribe Operation will not Flush Such Events. Also, the Alternative 
*       Event-Delivery Mechanisms, Such as Direct Event Posting or Timer Events, 
*       can be Still Delivered to the Event Chain of the Active Object.
***************************************************************************************/
MANAGER_EXT int16_t man_unsubscribe_all(active_t *active);

/***************************************************************************************
*   Find the Active Object Chain which Subscribe that Event "signal".
*
*   Description: Travel all the Enroll, Find the Active Object Chain which Subscribe 
*                that Event "signal". 
*   Argument: event -- The Pointer of Event which Request to Find. 
*   Return: Return the Pointer of the Active Object Chain, or else NULL is Return.  
*
*   NOTE: 
*   (1) This Function Call by Manager Internally ONLY.
***************************************************************************************/
MANAGER_EXT chain_t *man_subscribe_find(event_t *event);

/***************************************************************************************
*   Post an Event to the Manager.
*
*   Description: Post an Event into the Ecent Chain of Manager. 
*   Argument: event  -- The Pointer of Event which will be Post. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
MANAGER_EXT int16_t man_post(event_t *event); 

/***************************************************************************************
*   Publish an Event to the Active Objects.
*
*   Description: Posts the "event" to ALL Active Object that Has Subscribed to the 
*                "event". 
*   Argument: event  -- The Pointer of Event which will be Post. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) In the General Case, Event Publishing Requires Multi-Casting the Event to 
*       Multiple Subscribers. 
*   (2) This Function ONLY Used in Active Object Manager.
*   (2) The Subscriber Lists is Indexed by Event's Signal. 
***************************************************************************************/
MANAGER_EXT int16_t man_publish(event_t *event); 

/***************************************************************************************
*   Transfers Control to Active Object to Run the Active Task.
*
*   Description: Posts the "event" to ALL Active Object that Has Subscribed to the 
*                "event". 
*   Argument: event  -- The Pointer of Event which will be Post. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Implemenation of man_scheduler() Represents the Non-Preeemptive 
*       Scheduler Built into Framework. 
*   (2) This Implementation is Used Only in the "Vanilla" Framework Ports to 
*       "Bare Metal" Target Systems without any Underlying RTOS or Kernel.
*   (3) man_scheduler() is Typically Called from Your Startup Code after You 
*       Initialize the Framework and the Active Objects. 
*   (4) man_scheduler() Typically Never Returns to the Caller. 
*   (5) When the Manager Detects that no Events are Available for Active Objects 
*       and All the Active Object has been Idle State (the Idle Condition), 
*       Manager will Call the External on_idle(). on_idle() is Defined by 
*       Application, Not Defined in Manager. on_idle() Callback Gives the 
*       Application an Opportunity to Enter a CPU Power-Saving Mode, or 
*       Perform Some Other Idle Processing (such as SPY Output).
*   (6) on_idle() MUST be Called With Interrupts LOCKED Because the Determination
*       of the Idle Condition (No Events in the Chain and All the Active Object 
*       has been Idle State) can Change at Any Time by an Interrupt Posting 
*       Events to a Event Chain of Active Object. 
*   (7) The Scheduler is C-Operative, which Means that All Active Objects 
*       Co-Operate to Share a Single CPU and Implicitly Yield to Each Other after 
*       Every RTC Step. 
*   (8) The Scheduler is Non-Preemptive, Meaning that Every Active Object must 
*       Completely Process an Event before Any Other Active Object can Start 
*       Processing Another Event. The ISRs can Preempt the Execution of Active 
*       Objects at Any Time, but Every ISR Returns to Exactly the Preemption 
*       Point. If the ISR Posts an Event to Any Active Object, the Processing 
*       of this Event won't Start until the Current RTC Step Completes. 
*       The Maximum Time an Highest-Priority Event for the Highest-Priority 
*       Active Object could be Delayed this Way is Called the Task-Level 
*       Response. With the Non-Preemptive Scheduler, the Task-Level Response
*	    is Equal to the Longest RTC Step of All Active Objects in the System.
*   (9) The Scheduler is Engaged After Each Event is Processed in the 
*       Run-to-Completion (RTC) Fashion to Choose the Next Highest-Priority 
*       Active Object Ready to Process the Next Event. 
***************************************************************************************/
MANAGER_EXT int16_t man_scheduler(void_t);

#ifdef SPYER_MANAGER_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_MANAGER(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                         /* Spy Debuger Disabled */
    #define SPYER_MANAGER(format, ...)   ((void_t)0)
#endif  /* End of SPYER_MANAGER_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _MANAGER_H */


/***************************************************************************************
*   Spy record types.
*
*   This Enumeration Specifies the Record Types Used in the Framework Components.
*   You can Specify Your Own Record Types Starting from SPY_USER Offset.
*   Currently, the Maximum of All Records cannot Exceed 256.
***************************************************************************************/
enum spy_record_type
{
    /* Assert Records */
    SPY_ASSERT = 0,                                             /* Assert has Entered */
    /* RTOS Records */
    SPY_INT_LOCK_T,                                         /* Interrupts were Locked */
    SPY_INT_UNLOCK_T,                                     /* Interrupts were Unlocked */
    SPY_ISR_ENTRY_T,                                            /* An ISR was Entered */
    SPY_ISR_EXIT_T,                                              /* An ISR was Exited */
    /* State Machine Records */
    SPY_STATE_EMPTY_T,
    SPY_STATE_CONSTRUCT_T,                                 /* A State was Constructed */
    SPY_STATE_INIT_T,                                      /* A State was Initialized */
    SPY_STATE_DESTORY_T,                                     /* A State was Destoryed */
    SPY_STATE_ENTRY_T,                                         /* A State was Entered */
    SPY_STATE_EXIT_T,                                          /* A State was Exited  */
    SPY_INIT_TRANS_T,                    /* The Top-Most Initial Transition was Taken */
    SPY_INTERNAL_TRANS_T,                         /* An Internal Transition was Taken */
    SPY_STATE_TRANS_T,                              /* A Regular Transition was Taken */
    SPY_STATE_IGNORED_T,                 /* An Event was Ignored (Silently Discarded) */
    /* Fixed-Size Memory Pool Records */
    SPY_MPOOL_INIT_T,                     /* A Fixed-Size Memory Pool was Initialized */
    SPY_MPOOL_GET_T,               /* A Memory Block was Allocated from a Memory Pool */
    SPY_MPOOL_PUT_T,                  /* A Memory Block was Released to a Memory Pool */
    /* Event Pool Records */
    SPY_EVENT_NEW_T,                                            /* New Event Creation */
    SPY_EVENT_ATTEMPT_T,                                /* Garbage Collection Attempt */
    SPY_EVENT_GC_T,                                             /* Garbage Collection */
    /* Variable-Size Memory Pool Records */
    SPY_MPOOL_VAR_INIT_T,              /* A Variable-Size Memory Pool was Initialized */
    SPY_MPOOL_VAR_FROM_T,            /* A Memory Block is Moved Out from Pool Section */
    SPY_MPOOL_VAR_TO_T,                  /* A Memory Block is Moved into Pool Section */
    SPY_MPOOL_VAR_GET_T,                   /* A Memory Block is Get from Pool Section */
    SPY_MPOOL_VAR_PUT_T,                   /* A Memory Block is Put into Pool Section */
    SPY_MPOOL_VAR_SNAP_T,                      /* Snap the View of Variable-Size Pool */
    /* Event Heap Records */
    SPY_HEAP_NEW_T,                                             /* New Event Creation */
    SPY_HEAP_ATTEMPT_T,                                 /* Garbage Collection Attempt */
    SPY_HEAP_GC_T,                                              /* Garbage Collection */
    /* Event Queue Records */
    SPY_EQUEUE_INIT_T,                              /* An Event Queue was Initialized */
    SPY_EQUEUE_POST_FIFO_T,              /* An Event was Posted (FIFO) to a Raw Queue */
    SPY_EQUEUE_POST_LIFO_T,              /* An Event was Posted (LIFO) to a Raw Queue */
    SPY_EQUEUE_GET_T,                       /* Get an Event and Queue Still not Empty */
    SPY_EQUEUE_GET_LAST_T,                       /* Get the Last Event from the Queue */
    /* Priority-Based Defer Queue Records */
    SPY_CQUEUE_INIT_T,               /* An Priority-Based Defer Queue was Initialized */
    SPY_CQUEUE_POST_T,         /* An Defer Event was Posted to a Priority-Based Queue */
    SPY_CQUEUE_ATTEMPT_T,       /* Attempt to Get One Event from Priority-Based Queue */
    SPY_CQUEUE_GET_T,                  /* One Event has Get from Priority-Based Queue */
    /* Pattern Records */
    SPY_PATTERN_INIT_T,                                /* The Pattern was Initialized */
    SPY_PATTERN_SUBSCRIBE_T,               /* An Active Object Subscribed to an Event */
    SPY_PATTERN_UNSUBSCRIBE_T,           /* An Active Object Unsubscribed to an Event */
    SPY_PATTERN_PUBLISH_T,        /* An Event was Truly Published to Some Subscribers */
    SPY_PUBLISH_ATTEMPT_T,                     /* An Unsubscribed Event was Published */
    SPY_PATTERN_APPEND_T,               /* An Active Object has been Added to Pattern */
    SPY_PATTERN_REMOVE_T,           /* An Active Object has been Removed from Pattern */
    /* Active Engine Records */
    SPY_ACTIVE_INIT_T,                            /* An Active Object was Iniaialized */
    SPY_ACTIVE_POST_FIFO_T,   /* An Event was Posted (FIFO) Directly to Active Object */
    SPY_ACTIVE_POST_LIFO_T,   /* An Event was Posted (LIFO) Directly to Active Object */
    SPY_ACTIVE_WAIT_T,                  /* The Active Object is Waiting for the Event */
    SPY_ACTIVE_TIMEOUT_T,     /* The Active Object is Timeouted for Waiting the Event */
    SPY_ACTIVE_GET_T,                                   /* Active Object Got an Event */
    /* Timer Records */
    SPY_TIMER_INIT_T,                                      /* An Timer is Initialized */
    SPY_TIMER_ARM_T,                                             /* A Timer was Armed */
    SPY_TIMER_REARM_T,                                        /* A Timer was Re-Armed */
    SPY_TIMER_DISARM_T,                           /* True Disarming of an Armed Timer */
    SPY_TIMER_DISARM_ATTEMPT_T,               /* Attempt to Disarmed a Disarmed Timer */
    SPY_TIMER_AUTO_REARM_T,                       /* A Timer Expired and was Re-Armed */
    SPY_TIMER_AUTO_DISARM_T,                      /* A Timer Expired and was Disarmed */
    SPY_TIMER_POST_T,     /* A Timer Posted Itself Event Directly to an Active Object */
    SPY_TIMER_PUBLISH_T,                            /* A Timer Published Itself Event */
    SPY_TIMER_TASK_T,                                      /* task_timer() was Called */
    /* Manager Object Records */
    SPY_MANAGER_INIT_T,                         /* The Manager Object Was Initialized */
    SPY_MANAGER_EXIT_T,                              /* The Manager Object was Exited */
    SPY_MANAGER_CALLBACK_T,                           /* Callback Function was Called */
    SPY_MANAGER_DEFER_T,                       /* An Event was Defered to Defer Quque */
    SPY_MANAGER_RECALL_T,                   /* An Event was Recalled from Defer Queue */
    SPY_MANAGER_START_T,              /* The Thread of Application Object was Started */
    /* Dictionary Records */
    SPY_OBJECT_DICT,                                       /* Object Dictionary Entry */
    SPY_FUNCTION_DICT,                                   /* Function Dictionary Entry */
    SPY_SIGNAL_DICT,                                       /* Signal Dictionary Entry */
    /* User Records */
    SPY_USER                       /* The First Record Available for User SPY Records */
};




