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
*   Group : Active Object Template
*   File Name : template.c
*   Version : 1.0
*   Description : Based for Active Object Template
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-19-2005      Created Initial Version 1.0
*       JinHui Han      03-19-2005      Verified Version 1.0
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
#include "manager.h"
#include "activex.h" 
#include "timer.h"
#include "hpool.h"
#include "queue.h"

#define  EXTERN_GLOBALS
#include "template.h"

ASSERT_THIS_FILE(temp.c)
FAULT_THIS_FILE(temp.c)

typedef struct temp_tag {
    /***********************************************************************************
    *   Inherit the Super Active Object Type.
    ***********************************************************************************/
    active_t  active;
    
    /***********************************************************************************
    *   Protected Member Used In State Machine.
    ***********************************************************************************/
    mpool_t * epool;    /* Event Pool */ 
    /***********************************************************************************
    *   Hardware Configure.
    ***********************************************************************************/
    int16_t   uart;     /* UART IDs */
} temp_t;

/***************************************************************************************
*   Local Instance. 
***************************************************************************************/
/* Event Pool Instance */ 
mpool_t epool_temp;
/* Template Object Instance */
temp_t temp = { 
    {
        (int16_t)0,     /* Exit Request */
        (int16_t)0,     /* Reset Request */
        (int16_t)0,     /* Pause Request */ 
        (uint16_t)10,   /* Priority */
        (hsm_t *)hsm,   /* The Pointer of HSM hsm_t */
        (fsm_t *)fsm,   /* The Pointer of FSM fsm_t */
        (chain_t *)0,   /* The Pointer of Event Chain */
        (chain_t *)0,   /* The Pointer of Defer Event Chain */
        temp_version,   /* Get Object's Version */
        temp_name,      /* Get Object's Name */
        temp_start,     /* Start */
        temp_probe,     /* Probe */
        active_run,     /* Run, Return the IDLE Status */
        active_reset,   /* Reset */
        active_quit,    /* Force Quit */
        active_pause,   /* Pause */
        active_resume,  /* Resume */
        active_is_idle, /* Is in Idle State */
        temp_exit,      /* Exit  Callback */
        temp_reset,     /* Reset Callback */
    }, 
    &epool_temp,    /* Event Pool */
    0               /* UART IDs */
};

/***************************************************************************************
*   This Function Return the Pointer of Template's Version.
*
*   This Function Returns Constant Version String in the Format x.y.zz, 
*   Where x (One Digit) is the Major Version, y (One Digit) is the Minor Version, 
*   and zz (Two Digits) is the Maintenance Release Version.
*   An Example of the Version String is "TEMPLATE 1.0.00".
***************************************************************************************/
int8_t const ROM_KEYWORDS * temp_version(void_t)
{
    static int8_t const ROM_KEYWORDS object_version[] = "TEMP 1.0.00";
    return object_version;
}

/***************************************************************************************
*   This Function Return the Pointer of Template's Name.
***************************************************************************************/
int8_t const ROM_KEYWORDS * temp_name(void_t)
{
    static int8_t const ROM_KEYWORDS object_name[] = "TEMP";
    return object_name;
}

/***************************************************************************************
*   temp_start() Implementation. 
***************************************************************************************/
int16_t temp_start(temp_t *me)
{
    ASSERT_REQUIRE(me != (temp_t *)0); 
    if (me == (temp_t *)0) { 
        return FALSE; 
    } 
    /***********************************************************************************
    *   Construct and Initialize the Resource of Active Object. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Initialize the HSM, and Execute the Initial Conversion, Entry the Begin State 
    *   of HSM.
    ***********************************************************************************/

    /***********************************************************************************
    *   At Here, The Hardware of Active Object Worked on is Opened by Object Itself.
    ***********************************************************************************/
    uart_open(me->uart);    /* Open the Uart Port */

    /***********************************************************************************
    *   Subscribe the Event "Signal" Required by Active Object. 
    ***********************************************************************************/

    SPYER_TEMP("Start the Active Object %X. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   temp_exit() Implementation. 
***************************************************************************************/
int16_t temp_exit(temp_t *me)
{
    ASSERT_REQUIRE(me != (temp_t *)0); 
    if (me == (temp_t *)0) { 
        return FALSE; 
    } 
    /* Check the Quit Condition at Here. */
    if () { 
        /* Don't Meet the Quit Condition */
        return FALSE; 
    } 
    /***********************************************************************************
    *   Unsubscribe the Event "Signal" Required by Active Object. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Close the Hardware Associated with This Active Object.
    ***********************************************************************************/
    uart_close(me->uart);   /* Close the Uart Port */

    /***********************************************************************************
    *   Destroy the Resource of Active Object. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Flush Out the Event Chain. 
    ***********************************************************************************/

    SPYER_TEMP("The Active Object %X is Exit. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   temp_reset() Implementation. 
***************************************************************************************/
int16_t temp_reset(temp_t *me)
{
    ASSERT_REQUIRE(me != (temp_t *)0); 
    if (me == (temp_t *)0) { 
        return FALSE; 
    } 
    /* Check the Reset Condition at Here. */
    if () { 
        /* Don't Meet the Reset Condition */
        return FALSE; 
    } 
    /* If Meet the Reset Condition, Execute the Reset Operation at Here. */

    SPYER_TEMP("The Active Object %X is Reset. TimeStamp %d", me, ticks_get()); 
} 

/***************************************************************************************
*   temp_probe() Implementation. 
***************************************************************************************/
int16_t temp_probe(temp_t *me)
{
    ASSERT_REQUIRE(me != (temp_t *)0); 
    if (me == (temp_t *)0) { 
        return FALSE; 
    } 
    /***********************************************************************************
    *   Open the Hardware Associated with the Active Object. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Direct Send Command (Get Version or Get Status, etc) to the Hardware for 
    *   Identificat It. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Close the Hardware Associated with the Active Object. 
    ***********************************************************************************/

    return TRUE; 
}
/* Return to Super State to Handle the Event£¬Legal */
/* Renovate the Hardware Status Continually at Here. */

