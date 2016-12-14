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
*   File Name : epool.h
*   Version : 1.0
*   Description : Base for Event Pool 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-20-2005      Created Initial Version 1.0
*       JinHui Han      02-20-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _EPOOL_H
#define _EPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define EPOOL_EXT
#else
    #define EPOOL_EXT   extern
#endif

/***************************************************************************************
*   Event Pool Initialization for Dynamic Allocation of Events.
*
*   Description: Initializes one Event Pool at a Time and Must be Called Exactly
*                once for Each Event Pool before the Pool can be Used. 
*   Argument: me -- Pointer the mpool_t Struct to be Initialized
*             begin -- The Begin Address of Event Pool Storage. 
*             size  -- The Size of the Event Pool Storage in Bytes. 
*             event_size -- The Event Block Size of This Pool in Bytes. 
*                           Which Determines the Maximum Size of Events that Can 
*                           be Allocated from the Pool.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) The Actual Number of Events Available in the Pool might be Actually Less 
*       Than ("size" / "event_size") due to the Internal Alignment of the Blocks 
*       that the Pool might Perform. You can Always Check the Capacity of the Pool 
*       by Calling empool_margin().
***************************************************************************************/
EPOOL_EXT int16_t epool_init(mpool_t *me, void_t *begin, uint32_t size, uint16_t event_size);

/***************************************************************************************
*   Allocate a Dynamic Event.
*
*   Description: Allocate a Dynamic Event from Event Pool, and The Event is 
*                Initialized with the Signal "signal" and the Priority "priority". 
*   Argument: me -- Pointer to the mpool_t from which the Event Block is Requested. 
*             signal -- Signal of Event. 
*             priority -- Priority of Event. 
*             threshold -- Threshold for Preemptive.
*   Return: Returns a Pointer to the Allocated Event Block or NULL if no Free 
*           Blocks are Available. 
*
*   NOTE: 
*   (1) The Dynamic Allocation of Events is Required for Using Framework. 
***************************************************************************************/
EPOOL_EXT event_t *epool_new(mpool_t *me, signal_t signal, uint16_t priority, uint16_t threshold);

/***************************************************************************************
*   Recycle a Dynamic Event.
*
*   Description: Recycle a Dynamic Event Back into a Event Pool. 
*   Argument: event -- Pointer to the Event which is Put Back. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) This Function Implements a Simple Garbage Collector for the Dynamic Events. 
*       Only Dynamic Events are Candidates for Recycling. (A Dynamic Event is One 
*       that is Allocated from an Event Pool). 
*   (2) This Function Decrements the Reference Counter of the Event, and Recycles 
*       the Event Only if the Counter Drops to Zero (Meaning that no More References 
*       are  Outstanding for This Event).
*
*   NOTE: 
*   (1) Framework Invokes the Garbage Collector at All Appropriate Contexts, When an 
*       Event can Become Garbage (Automatic Garbage Collection), so the Application 
*       Code Should have no Need to Call epool_release() Directly. 
*   (2) The epool_release() Function is Exposed Only for Special Cases when Your 
*       Application sends Dynamic Events to the "raw" Thread-Safe Queues 
*       (see equeue_t). Such Queues are Processed Outside of Framework and the 
*       Automatic Garbage Collection is CANNOT be Performed for These Events. 
*       In this Case you need to Call epool_release() Explicitly.
***************************************************************************************/
EPOOL_EXT int16_t epool_release(event_t *event);

/***************************************************************************************
*   Get the Margin of the Given Event Pool.
*
*   Description: Get the Margin of the Given Event Pool. 
*   Argument: me -- Pointer to the mpool_t Struct to be Inquired.  
*   Return: The Margin of the Given Event Pool. 
*
*   NOTE: 
*   () The Returned Pool Margin is the Minimal Number of Free Blocks Encountered in the 
*      Given Pool since System Startup.
***************************************************************************************/
EPOOL_EXT uint16_t epool_margin(mpool_t *me);

#ifdef SPYER_EPOOL_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_EPOOL(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                        /* Spyer Debuger Disabled */
    #define SPYER_EPOOL(format, ...)   ((void_t)0)
#endif  /* End of SPYER_EPOOL_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _EPOOL_H */