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
*   File Name : timer.h
*   Version : 1.0
*   Description : Base for Timer 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-11-2005      Created Initial Version 1.0
*       JinHui Han      03-11-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define TIMER_EXT
#else
    #define TIMER_EXT   extern
#endif

/***************************************************************************************
*   Timer Structure.
*
*   Timer Events are Special Events Equipped with the Notion of Timer Passage. 
*   NOTICE: Timer and Timer is Static Allocated in Active Object. 
*
*   The Basic Usage Model of the Timer is as Follows. An Active Object Static¡¡
*   Allocates one or more timer_t Objects (Provides the Storage for Them). When the 
*   Active Object Needs to Arrange for a Timeout, It Arms one of Its Timer to Fire 
*   either Just Once (One-Shot) or Periodically. Each Timer Counter Out Independently 
*   from the Others, so a Application can Make Multiple Parallel Timeout Requests 
*   (From the Same or Different Active Objects). When Detects that the Appropriate 
*   Moment has Arrived, It Inserts the Timer Event Directly into the Recipient's 
*   Event Queue. The Recipient then Processes the Timer Event Just Like any Other 
*   Event. 
*
*   Internally, the Armed Timer are Organized into a Bi-Directional Linked List. 
*   This Linked List is Scanned in Every Invocation of the Ticker. Only Armed 
*   (Timing Out) Timer are in the List, so Only Armed Timer Consume CPU Cycles. 
*
*   timer_tag for the Description of the Data Members Derivation.
*
*   NOTE: 
*   (1) Framework Manages the Timer in the Function timer(), which must be Called 
*       Periodically Every Clock Ticker.
*   (2) In This Version of timer_t Objects should be Allocated Statically. If timer_t 
*       Objects is Allocated Dynamically from Memory Pools, You Should Release It 
*       in the Entire Lifetime of Object. Currently, Framework will not Correctly 
*       Recycle the Dynamically Allocated Timer Events.
*   (3) In the Future Releases of Framework, Timer Events Actually could be Allocated 
*       Dynamically. However, for Simplicity in This Release of Framework, Timer 
*       Events are Limited to be Statically Allocated.
*   (4) For Static Allocated Timer Object, You can Reuse the Timer Any Number of Times, 
*       and You can Time-Division-Multiplexing Reuse It. 
***************************************************************************************/
typedef struct timer_tag
{
    /***********************************************************************************
    *   The Internal Down-Counter of the Timer. The Down-Counter is Decremented by
    *   1 in Every Ticker Invocation. The Timer Fires (Posted or Published) when 
    *   the Down-Counter Reaches Zero.
    ***********************************************************************************/
    int32_t  counter;

    /***********************************************************************************
    *   The Interval for the Periodic Timer (Zero for the One-Shot Timer). 
    *   The Value of the Interval is Re-Loaded to the Internal Down-Counter when the
    *   Timer Expires, so that the Timer keeps Timing Out Periodically.
    ***********************************************************************************/
    int32_t  interval;

    /***********************************************************************************
    *   The Defer Event. If this Pointer is NULL, No Event is Post.
    ***********************************************************************************/
    event_t *event;

    /***********************************************************************************
    *   Link to the Previous Timer in the List.
    ***********************************************************************************/
    struct timer_tag *prev;

    /***********************************************************************************
    *   Link to the Next Timer in the List.
    ***********************************************************************************/
    struct timer_tag *next;
} timer_t;

TIMER_EXT timer_t * timer_list_;     /* List Head of Timer */

/***************************************************************************************
*   Initialize the Timer Manager.
*
*   Description: Initialize the Timer Manager.
*   Argument: none. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
TIMER_EXT int16_t timer_init(void_t);

/***************************************************************************************
*   Arm a Timer (One-Shot or Periodic).
*
*   Description: Arm a Timer (One-Shot or Periodic).
*   Argument: me --- The Timer Pointer. 
*             count --- The Counter Value for One-Shot Timer, The Initial Counter 
*                       Value for Periodic Timer. 
*             interval --- The Interval Time for Periodic Timer, MUST be 0 for 
*                          One-Shot Timer. 
*             event --- Post Event when Expired.
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) For One-Shot Timer, Arms a Timer "me" to Fire in "count" Clock Ticks. 
*       After Posting, the Timer Gets Automatically Disarmed and can be Reused for
*       a One-Shot or Periodic Timeout Requests.
*   (2) A One-Shot Timer can be Disarmed at Any time by Calling the timer_disarm() 
*       Function. Also, a One-Shot Timer can be Re-Armed to Fire in a Different 
*       Number of Clock Ticks by Calling the timer_rearm() Function.
*   (3) For Periodic Timer, Arms a Timer "me" to Fire Every "interval" Clock Ticks. 
*       After Posting, the Timer Gets Automatically Re-Armed to Fire again in the 
*       Specified "interval" Clock Ticks.
*   (4) A Periodic Timer can be Disarmed Only by Calling the timer_disarm() function. 
*       After Disarming, the Timer can be Reused for a One-Shot or Periodic Timeout 
*       Requests.
*   (5) An Attempt to Reuse (Arm Again) a Running Periodic Timer Raises an Assertion.
*   (6) Also, a Periodic Timer can be Re-Armed to Shorten or Extend the Current 
*       Period by Calling the timer_rearm() Function. After Adjusting the Current 
*       Period, The Periodic Timer Goes Back Timing Out at the Original Rate.
***************************************************************************************/
TIMER_EXT int16_t timer_arm(timer_t *me, int32_t count, int32_t interval, event_t *event);

/***************************************************************************************
*   Rearm a Timer (One-Shot or Periodic).
*
*   Description: Rearm a Timer (One-Shot or Periodic).
*   Argument: me --- The Timer Pointer. 
*             count --- The Counter Value for One-Shot Timer, The Initial Counter 
*                       Value for Periodic Timer. 
*             interval --- The Interval Time for Periodic Timer, MUST be 0 for 
*                          One-Shot Timer. 
*   Return: Returns 1 (TRUE) if the Timer was Running as it was Re-Armed. 
*           Return of 0 (FALSE) Means that the Timer was not Rearmed Because 
*           It was not Running. The FALSE Return is Only Possible for One-Shot Timer 
*           that Have been Automatically Disarmed Upon Expiration. In this case the 
*           FALSE Return Means that the Timer Event has Already been Posted or 
*           Published and Should be Expected in the Manager's Event Chain or the 
*           Active Object's Event Chain.
*
*   NOTE: 
*   (1) The Timer "me" Gets Rearmed with a New Number of Clock Ticks "tick_count". 
*       This Facility can be Used to Prevent a One-Shot Timer from Expiring (e.g., 
*       a Watchdog Timer), or to Adjusts the Current Period of a Periodic Timer. 
*   (2) Rearming a Periodic Timer Leaves the Interval Unchanged and is a Convenient 
*       Method to Adjust the Phasing of the Periodic Timer.
***************************************************************************************/
TIMER_EXT int16_t timer_rearm(timer_t *me, int32_t count, int32_t interval);

/***************************************************************************************
*   Disarm a Timer.
*
*   Description: Disarm a Timer (One-Shot or Periodic). 
*                The Timer "me" Gets Disarmed and Can be Reused. 
*   Argument: me --- The Timer Pointer. 
*   Return:  Returns 1 (TRUE) if the Timer was Truly Disarmed, that is, It was Running. 
*            Return of 0 (FALSE) Means that the Timer was not Truly Disarmed because 
*            It was Not Running. 
*   
*   NOTE: 
***************************************************************************************/
TIMER_EXT int16_t timer_disarm(timer_t *me);

/***************************************************************************************
*   Timer Task.
*
*   Timer Task Decrment the Counter of Timer which has be Linked on "timer_list_"
*   Very Ticks, If the Counter Reduce to 0, This Task will Execute the Function
*   Defined by "interval", It Could be One-Shot Timer or Periodic Timer.
*
*   NOTE: 
*   (1) When the Event which Active Object is Waiting and Timer Timeout are Arrived 
*       at the Same Time, The Event will be Publish into Event Chain of Active 
*       Object, and the Timeout Event is Publish into Event Chain Following It. 
*       At this Time, after Event is Handled, Timer' Rearm will FALSE, the 
*       Application Design should Avoid This through Increment the "counter" 
*       Value of Timeout Timer. 
*   (2) For Avoid The Case of (1):
*       (A) The Timer MUST be Handled at Task Level, but not ISR Level.
*       (B) The Priority of Timer Task MUST Low than Very Task, but Higher than 
*           Idle Task.
*       (C) The Interrupt MUST be Disabled When Handle Every timer_t.
*   (3) timer() is not Reentrant, which Means that timer() Function must Finish 
*       (Run to Completion) before the "Clock Tick" Interrupt Fires Again. 
*   (4) When The Active Object Disarm one Timer (Which Remove the Pointer of Timer 
*       from Timer List) which timer() has Got It's Pointer, The Error would be 
*       Occured After the Active Object Compltete the Operation and Waiting for 
*       Next Event, At This Time, timer() will Run Error Because It Using One 
*       Timer Pointer which Has Been Removed from the Timer List. For Avoid This 
*       Error, You Must not Use the timer_disarm() Function During the Normal
*       Operation, timer_disarm() can Only be Used During the Active Object is 
*       Exiting, and Active Object Has Got the Expired Event Associate with that 
*       Timer.
***************************************************************************************/
TIMER_EXT int16_t timer(void_t); 

#ifdef SPYER_TIMER_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_TIMER(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spy Debuger Disabled */
    #define SPYER_TIMER(format, ...)   ((void_t)0)
#endif  /* End of SPYER_TIMER_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _TIMER_H */