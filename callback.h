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
*   File Name : callback.h                 
*   Version : 1.0
*   Description : Base for Callback
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-05-2005      Created Initial Version 1.0
*       JinHui Han      04-05-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _CALLBACK_H
#define _CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define CALLBACK_EXT
#else
    #define CALLBACK_EXT     extern
#endif

/***************************************************************************************
*   Scheduler Initialize Callback (Customized Based on Application)
*
*   Description: Initialize Callback. 
*   Argument: none. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*   
*   NOTE: 
*   (1) on_init() Initialize the Active Objects Defined by Application.
***************************************************************************************/
CALLBACK_EXT int16_t on_init(void_t); 

/***************************************************************************************
*   Scheduler Idle Callback (Customized Based on Application)
*
*   Description: Idle Callback. 
*   Argument: int_lock_key -- The Interrupt Status need to be Resume. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*   
*   NOTE: 
*   (1) on_idle() is Invoked with Interrupts LOCKED Because the Idle Condition 
*       can be Asynchronously Changed at Any Time by an Interrupt, Such As the Idle 
*       Condition "No Events in the Event Chain" could Change at Any Time by an 
*       Interrupt Posting Events to the Event Chain.
*   (2) on_idle() MUST Unlock the Interrupts Internally, Perhaps at the Same Time 
*       as Putting the CPU into a Low-Power Mode, but not Before Putting the CPU 
*       into the Low-Power Mode. (Ideally, Unlocking Interrupts and Low-Power Mode 
*       should Happen Atomically). At the Very Least, the Function MUST Unlock 
*       Interrupts, Otherwise Interrups will be Locked Permanently.
*   (3) on_idle() MUST Enable Interrups Internally, Perhaps at the Same Time as 
*       Putting the CPU into a Power-Saving Mode.
*   (4) on_idle() is Only Used by the Non-Preemptive Scheduler Built into Manager 
*       in the "Bare Metal" Port, and is NOT Used in any Other Ports. 
***************************************************************************************/
CALLBACK_EXT int16_t on_idle(INT_KEY_TYPE int_lock_key); 

#ifdef __cplusplus
}
#endif

#endif      /* End of _CALLBACK_H */
