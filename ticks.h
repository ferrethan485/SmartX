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
*   Group : System Ticks
*   File Name : ticks.h
*   Version : 1.0
*   Description : Base for System Ticks 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-09-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _TICKS_H
#define _TICKS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define TICKS_EXT
#else
    #define TICKS_EXT   extern
#endif

/**************************************************************************************
*    The Current System Time in Clock Ticks, Timestamp. 
**************************************************************************************/
TICKS_EXT uint32_t volatile sys_ticks; 

/**************************************************************************************
*   Increment the Current System Time in Time-Tick. 
*
*   Description: Increment the Current System Time in Time-Tick. 
*   Argument: none
*   Return: none. 
*
*   NOTE: ticks_increase() is NOT Using a Critical Section Internal, It must be 
*         Finished in the Manner of RTC (Run to Completion). It's Assmed Here that 
*         ticks_increment() is Invoked from Only one ISR (The "Clock Ticker" 
*         Interrupt).
**************************************************************************************/
TICKS_EXT void_t ticks_increase(void_t); 

/**************************************************************************************
*   Pickup the Current System Time in Time-Tick. 
*
*   Description: Pickup the Current System Time in Time-Tick. 
*   Argument: none
*   Return: The Current System Time in Time-Tick, Timestamp. 
*
*   NOTE: (1) ticks_get() is Using a Critical Section Internal.
**************************************************************************************/
TICKS_EXT uint32_t ticks_get(void_t); 


#ifdef SPYER_TICKS_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_TICKS(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_TICKS(format, ...)   ((void_t)0)
#endif 

#ifdef __cplusplus
}
#endif

#endif      /* End of _TICKS_H */
