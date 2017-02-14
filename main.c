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
*   Group : Main
*   File Name : main.c                 
*   Version : 1.0
*   Description : Base for Main
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      04-05-2005      Created Initial Version 1.0
*       JinHui Han      04-05-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h"
#include "std_def.h"
#include "config.h"
#include "system.h"
#include "lock.h"
#include "fault.h"
#include "debug.h"
#include "assert.h"
#include "spyer.h"
#include "ticks.h"
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
#include "stack.h"
#include "callback.h"
#include "manager.h"
#include "activex.h" 
#include "timer.h"
#include "ticker.h"
#include "watchdog.h"
#include "hpool.h"
#include "queue.h"

/***************************************************************************************
*   main_() Implementation.
***************************************************************************************/
int32_t main_(void_t)
{
    /* Initialize the System Hardware */
    system_init(); 
    /* Initialize the Failure Logging */
    fault_init();
    /* Initialize the Debug Port */
    debug_init();
    /* Initialize the Ticks, TimeStamp */
    ticks_init(); 
    /* Initialize the Chain Pools */
    cpool_init(); 
    /* Initialize the Manager */
    man_init(void_t);
    /* Initialize the Ticker, Ticks and Timer */
    ticker_init(); 
    /* Enable the Interrupt */
    interrupt_enable(); 

    /* Initialize the Watchdog */
    watchdog_init(); 
    while (1) { 
        man_scheduler(); 
        watchdog_feed(); 
    } 
    /* Fault Return */
    return FAILURE; 
}