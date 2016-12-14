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
*   Group : Interrupt Lock 
*   File Name : lock.h
*   Version : 1.0
*   Description : Base for Interrupt Lock
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-08-2005      Created Initial Version 1.0
*       JinHui Han      04-08-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _LOCK_H
#define _LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define LOCK_EXT
#else
    #define LOCK_EXT   extern
#endif

/***************************************************************************************
*   Enable the Interrupt.
*
*   Description: Enable the Interrupt and Return the Value of CPU Status Register.
*   Argument: none
*   Return: The Value of CPU Status Register
***************************************************************************************/
LOCK_EXT  cpu_sr_t interrupt_enable(void_t); 

/***************************************************************************************
*   Disable the Interrupt.
*
*   Description: Disable the Interrupt and Return the Value of CPU Status Register.
*   Argument: none
*   Return: The Value of CPU Status Register
***************************************************************************************/
LOCK_EXT  cpu_sr_t interrupt_disable(void_t); 

/***************************************************************************************
*   Restore the Interrupt Status.
*
*   Description: Restore the Interrupt Status to Previous Value before Disbale It.
*   Argument: cpu_sr -- The Backup CPU Status Register Value when Disable It Previously.
*   Return: none
***************************************************************************************/
LOCK_EXT  void_t   interrupt_restore(cpu_sr_t cpu_sr); 

/***************************************************************************************
*   Interrupt Enable/Disable Control.
*
*   For Support Nesting of Critical Sections MUST Using the INT_LOCK_KEY.
*   The Interrupt Lock Key is a Temporary Variable that Holds the Interrupt Status. 
*   The Status is Stored Upon Locking the Interrupts in INT_LOCK() and then 
*   Restored from the Key in INT_UNLOCK().
*
*   NOTE: (1) The INT_UNLOCK() Macro Must be Always Paired with INT_LOCK().
*         (2) You Should Keep the Code Between These Two Macros to the Minimum.
***************************************************************************************/
/* This is an Internal Macro for Defining the Interrupt Lock Key */
#define INT_LOCK_KEY_       cpu_sr_t  int_lock_key_;
/* This is an Internal Macro for Locking Interrupts */
#define INT_LOCK_()         ((int_lock_key_) = interrupt_disable())
/* This is an Internal Macro for Unlocking Interrupts */
#define INT_UNLOCK_()       interrupt_restore(int_lock_key_)

/***************************************************************************************
*   The Control Method of Interrupt.
*
*   (1) Disable/Enable Interrupts Using Simple Instructions.  
*       After Critical Section, Interrupts will be Enabled Even if They Were 
*       Disabled before Entering the Critical Section.
*
*   (2) Disable/Enable Interrupts by Preserving the State of Interrupts into Stack. 
*       In other Words, if Interrupts were Disabled before Entering the Critical 
*       Section, They will be Disabled when Leaving the Critical Section.
*           #define  INT_LOCK_()     asm {PUSHF; CLI}  // Disable interrupts
*           #define  INT_UNLOCK_()   asm  POPF         // Restore interrupts
*
*   (3) Disable/Enable Interrupts by Preserving the State of Interrupts into the 
*       Local Variable.  
*       Generally Speaking you would Store the State of the Interrupt Disable Flag 
*       in the Local Variable 'cpu_sr' and then Disable Interrupts.  'cpu_sr' is 
*       Allocated in All of Functions that Need to Disable Interrupts.  You would 
*       Restore the Interrupt Disable State by Copying Back 'cpu_sr' into the CPU's 
*       Status Register.
***************************************************************************************/

/***************************************************************************************
*   Scheduler Lock/Unlock Control for Preemptive RTOS.
*
*   The Thread that Locks the Scheduler Keeps Control of the CPU Even Though Other 
*   Higher-Priority Threads might be Ready to Run. 
*   However, Interrupts are Still Serviced.
*
*   NOTE: (1) The Scheduler Lock and Unlock must be Always Paired Using.
*         (2) You Should Keep the Code Minimum between Scheduler Lock and Unlock.
***************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif      /* End of _LOCK_H */
