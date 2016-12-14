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
*   File Name : event.h
*   Version : 1.0
*   Description : Base for State Machine Event
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-08-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _EVENT_H
#define _EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define EVENT_EXT
#else
    #define EVENT_EXT     extern
#endif

/***************************************************************************************
*   signal Represents the Signal of an Event.
*   The Relationship between an Event and a Signal is as Follows. A Signal in
*   UML is the Specification of an Asynchronous Stimulus that Triggers Reactions
*   Referrence UML Document: "http://www.omg.org/docs/ptc/03-08-02.pdf",
*   and as such is an Essential Part of an Event.
*   (The Signal Conveys the Type of the Occurrence-What Happened?) However, an
*   Event can Also Contain Additional Quantitative Information about the
*   Occurrence in Form of Event Parameters. 
***************************************************************************************/
typedef uint16_t  signal_t;     /* Signal of the Event */

/***************************************************************************************
*   Signal Internal Constants
***************************************************************************************/
enum {
    SIG_EMPTY_ = 0;   /* Empty Signal for Internal Use Only */
};

/***************************************************************************************
*   Event Structure.
*
*   event_t Represents Events without Parameters and Serves as the Base Structure
*   for Derivation of Events with Parameters.
*
*   The Following Example Illustrates How to Add an Event Parameter by Derivation of
*   the event_t Structure. 
*       typedef struct event_calc_tag
*       {
*           event_t super;      // Derives from event_t
*           uint8_t key_ids;    // IDs of the Key Depressed
*       } event_calc_t;
*
*   Please NOTE that the event_t Member "super" MUST Defined as the FIRST Member
*   of the Derived Struct.
*
*   For the Design, If Need the Defer Event have the Priority Mechanism, You Must 
*   Using the Event Chain (echain_t)to Defer the Event. If the Defer Event have not 
*   Priority Mechanism, You Can Using the Event Queue (equeue_t FIFO ) to Defer the 
*   Event.
***************************************************************************************/
typedef struct event_tag {
    signal_t  signal;           /* Signal   of the Event Instance */
    uint16_t  priority;         /* Priority of the Event Instance */
                                /* Range from 0 to 65535, Where 0 Represents */
                                /* the Highest Priority. */
                                /* Where Lower Numerical Values Correspond to */
                                /* Higher Priority (urgency) */
    uint16_t  threshold;        /* Highest Priority Level of Disable Preempt. */
                                /* Only Priorities Higher than "threshold" Level are */
                                /* Allow to Preempt this Event. */
                                /* The Value of "threshold" must be Less Than or */
                                /* Equal to the Specified "priority". */
                                /* Typically, It is the Same As the "priority". */
    void_t *  record;           /* Record for the Data Pointer or Data */
                                /* NOTE: epool_release() Don't Release the Resource whitch */
                                /* Associated with "record", You MUST Do it by yourself */
    int16_t   dynamic_;         /* Dynamic Attributes of the Event (0 for Static), Internal Use Only */
    int16_t   static_;          /* Static Allocate Flag, 1 is Static, 0 is Dynamic */
} event_t;

/***************************************************************************************
*   Query whether the External Event or Not. 
*
*   Description: Query whether the External Event or Not. 
*   Argument: event -- Pointer to the Event to be Inquired. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
EVENT_EXT int16_t event_is_external(event_t *event); 

/***************************************************************************************
*   Query whether the Event is End. 
*
*   Description: Query whether the Event is End. 
*   Argument: event -- Pointer to the Event to be Inquired. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
EVENT_EXT int16_t event_is_end(event_t *event); 

/***************************************************************************************
*   Initialize the Static Event. 
*
*   Description: Initialize the Static Event.
*   Argument: event -- Pointer to the Static Event which be Initialized. 
*             signal -- Signal of Event. 
*             priority -- Priority of Event. 
*             threshold -- Threshold for Preemptive.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
EVENT_EXT int16_t event_init(event_t *event, signal_t signal, uint16_t priority, uint16_t threshold); 

#ifdef __cplusplus
}
#endif

#endif      /* End of _EVENT_H */

