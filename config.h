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
*   Group : All the Group of Framework
*   File Name : config.h                 
*   Version : 1.0
*   Description : Configures for All the Part of Framework
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-05-2005      Created Initial Version 1.0
*       JinHui Han      04-05-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CORTEX_M  /* The Type of CPU Core */

/***************************************************************************************
* The Size of CPU Status Register, CPU-Specific. 
***************************************************************************************/
typedef uint32_t cpu_sr_t; 

/***************************************************************************************
*   Macros to Specify Compiler-Specific Directive.
***************************************************************************************/
#ifndef ROM_KEYWORDS
    /***********************************************************************************
    *   Macro to Specify Compiler-Specific Directive for Placing a Constant Object in
    *   ROM.
    *
    *   Many Compilers for 8-bit Harvard-Architecture MCUs Provide Non-Stanard 
    *   Extensions to Support Placement of Objects in Different Memories.
    *   In Order to Conserve the Precious RAM, Here Uses the ROM_KEYWORDS Macro for All 
    *   Constant Objects that Can be Allocated in ROM.
    *
    *   To Override the Following Empty Definition, You need to Define the ROM_KEYWORDS
    *   Macro in this Header File. Some Examples of Valid ROM_KEYWORDS Macro
    *   Definitions are: __code (IAR 8051 compiler), code (Keil Cx51 compiler), 
    *   PROGMEM (gcc for AVR), __flash (IAR for AVR).
    ***********************************************************************************/
    #define ROM_KEYWORDS
#endif  /* End of ROM_KEYWORDS */

#ifndef ROM_BYTE
    /***********************************************************************************
    *   Macro to Access a Byte Allocated in ROM
    *
    *   Some Compilers for Harvard-architecture MCUs, such as Gcc for AVR, do not 
    *   Generate Correct Code for Accessing Data Allocated in the Program Space (ROM). 
    *   The Workaround for Such Compilers is to Explictly Add Assembly Code to Access 
    *   Each Data Element Allocated in the Program Space. The Macro ROM_BYTE() Retrieves
    *   a Byte from the Given ROM Address.
    *
    *   The ROM_BYTE() Macro Should be Defined in the This Header File for Each Compiler 
    *   that cannot Handle Correctly Data Allocated in ROM (Such as the Gcc). If the 
    *   Macro is Left Undefined, The Default Definition Simply Returns the Argument and 
    *   Lets the Compiler Synthesize the Correct Code.
    ***********************************************************************************/
    #define ROM_BYTE(rom_addr_)      (*(uint8_t const ROM_KEYWORDS *)rom_addr_)

#endif  /* End of ROM_BYTE */

/**************************************************************************************
*   The Ticks of Desktop Linux is 10ms. 
*   The Ticks of This System is 2ms, 500 ticks/s.
**************************************************************************************/
#define TICKER     500     /* UNIT: ticks/s */

/***************************************************************************************
*   Configure Definations for Failure Logger.
***************************************************************************************/
#define FAULT_NAME     16
#define FAULT_AMOUNT   128

/***************************************************************************************
*   Enable or Disable for Failure Logger.
***************************************************************************************/
#undef  FAULT_ENABLE

/***************************************************************************************
*   The Size of the Debug Buffer in Bytes. 
*   NOTE: If not Using the Debuger, Setting it into (0*1024)
***************************************************************************************/
#define DEBUG_BUFFER_SIZE  (2*1024)

/***************************************************************************************
*   Enable or Disable the Assert Checker.
***************************************************************************************/
#undef  ASSERT_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer Debuger.
***************************************************************************************/
#undef  SPYER_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of System Ticks.
***************************************************************************************/
#undef  SPYER_TICKS_ENABLE

/***************************************************************************************
*   Configure Definations for Watchdog.
***************************************************************************************/
#define WATCHDOG_ENABLE 

/***************************************************************************************
*   Enable or Disable for Spyer of Watchdog.
***************************************************************************************/
#undef  SPYER_WATCHDOG_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Memory Pools.
***************************************************************************************/
#undef  SPYER_MPOOL_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Event Pools.
***************************************************************************************/
#undef  SPYER_EPOOL_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Variable Size Memory Pools.
***************************************************************************************/
#undef  SPYER_HPOOL_ENABLE

/***************************************************************************************
*   Configure Definations for Variable Size Memory Pools.
***************************************************************************************/
#define HPOOL_FACTOR    (1L)
#define HPOOL_AMOUNT     12

/***************************************************************************************
*   Configure Definations for Chain Pool.
***************************************************************************************/
#define CPOOL_AMOUNT   256

/***************************************************************************************
*   Enable or Disable for Spyer of Priority Based Event Chain.
***************************************************************************************/
#undef  SPYER_ECHAIN_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Priority Based Active Object Chain.
***************************************************************************************/
#undef  SPYER_ACHAIN_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Priority Based Event Register Chain.
***************************************************************************************/
#undef  SPYER_RCHAIN_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Enroll Book.
***************************************************************************************/
#undef  SPYER_ENROLL_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Data Pointer Queue.
***************************************************************************************/
#undef  SPYER_QUEUE_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Stack.
***************************************************************************************/
#undef  SPYER_STACK_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Active Object Manager.
***************************************************************************************/
#undef  SPYER_MANAGER_ENABLE

/***************************************************************************************
*   Configure Definations for Active Object Manager.
***************************************************************************************/
#define MSTACK_AMOUNT   4

/***************************************************************************************
*   Enable or Disable for Spyer of Active Object Engine.
***************************************************************************************/
#undef  SPYER_ACTIVEX_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of Timer.
***************************************************************************************/
#undef  SPYER_TIMER_ENABLE

/***************************************************************************************
*   Configure Definations for HSM.
***************************************************************************************/
#define HSM_MAX_DEPTH   8

/***************************************************************************************
*   Enable or Disable for Spyer of HSM.
***************************************************************************************/
#undef  SPYER_HSM_ENABLE

/***************************************************************************************
*   Enable or Disable for Spyer of FSM.
***************************************************************************************/
#undef  SPYER_FSM_ENABLE

#ifdef __cplusplus
}
#endif

#endif      /* End of _CONFIG_H */
