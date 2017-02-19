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
*   Group : Active Object Engine
*   File Name : activex.c
*   Version : 1.0
*   Description : Based for Active Object Engine
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-03-2005      Created Initial Version 1.0
*       JinHui Han      03-03-2005      Verified Version 1.0
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

#define  EXTERN_GLOBALS
#include "activex.h" 

ASSERT_THIS_FILE(activex.c)
FAULT_THIS_FILE(activex.c)

/***************************************************************************************
*   active_version() Implementation.
***************************************************************************************/
int8_t const ROM_KEYWORDS * active_version(void_t)
{
    /* Implement by Inherit Objects */
    /* For Example: 
    static int8_t const ROM_KEYWORDS object_version[] = "OBJECT_NAME 1.0.00";
    return object_version;
    */
    return (int8_t const ROM_KEYWORDS *)0; 
}

/***************************************************************************************
*   active_name() Implementation.
***************************************************************************************/
int8_t const ROM_KEYWORDS * active_name(void_t)
{
    /* Implement by Inherit Objects */
    /* For Example: 
    static int8_t const ROM_KEYWORDS object_name[] = "OBJECT_NAME";
    return object_name;
    */
    return (int8_t const ROM_KEYWORDS *)0; 
}

/***************************************************************************************
*   active_flushout() Implementation. 
***************************************************************************************/
int16_t active_flushout(active_t *me) 
{
    event_t *event;

    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 

    /* Flush Out the Defer Event */
    while (me->defer != (chain_t *)0) { 
        /* Get One Event from Defer Event Chain */
        event = echain_get((chain_t **)&(me->defer)); 
        /* Release the Event */
        epool_release(event); 
        SPYER_ACTIVEX("Event[%X] is Flush Out from Defer Chain[%X] of Active Object[%X]. TimeStamp %d", \\
                       event, me->defer, me, ticks_get()); 
    } 
    while (me->event != (chain_t *)0) { 
        /* Get One Event from Event Chain */
        event = echain_get((chain_t **)&(me->event)); 
        /* Release the Event */
        epool_release(event); 
        SPYER_ACTIVEX("Event[%X] is Flush Out from Event Chain[%X] of Active Object[%X]. TimeStamp %d", \\
                       event, me->event, me, ticks_get()); 
    } 
    return  TRUE; 
}

/***************************************************************************************
*   active_run() Implementation.
***************************************************************************************/
int16_t active_run(active_t *me)
{
    int16_t  ret; 
    event_t *event;

    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 

    if (me->exit == (int16_t)1) { 
        ASSERT_REQUIRE(me->callback_exit != (active_function_t)0); 
        if (me->callback_exit != (active_function_t)0) { 
            if ((*(me->callback_exit))(me) == TRUE) { 
                me -> exit == (int16_t)0; 
                return TRUE; 
            } 
        } 
    } 

    if (me->reset == (int16_t)1) { 
        ASSERT_REQUIRE(me->callback_reset != (active_function_t)0); 
        if (me->callback_reset != (active_function_t)0) { 
            if ((*(me->callback_reset))(me) == TRUE) { 
                me -> reset == (int16_t)0; 
                return TRUE; 
            } 
        } 
    } 

    if (me->pause == (int16_t)1) { 
        /* Bypass the Run, Return Directly */
        SPYER_ACTIVEX("Active Object %X is Pause. TimeStamp %d", me, ticks_get()); 
        return TRUE; 
    } 

    event = (event_t *)0; 
    ASSERT_REQUIRE(me->hsm->is_idle != (hsm_function_t)0); 
    if (me->hsm->is_idle != (hsm_function_t)0) { 
        if ((*(me->hsm->is_idle))(me->hsm) == TRUE) { 
            if (me->defer != (chain_t *)0) { 
                event = echain_get((chain_t **)&(me->defer)); 
                /* Dispatch the Event to Object's HSM */
                if (me->hsm->dispatch != (hsm_dispatch_t)0) { 
                    (*(me->hsm->dispatch))(me->hsm, event); 
                } 
                /* Release the Event */
                epool_release(event); 
                /* Scan HSM the Service */
                if (me->hsm->service != (hsm_function_t)0) { 
                    (*(me->hsm->service))(me->hsm); 
                } 
                SPYER_ACTIVEX("Event[%X] is Dispatch to HSM[%X] of Active Object[%X]. TimeStamp %d", \\
                    event, me->hsm, me, ticks_get()); 
                return TRUE; 
            } 
        } 
    } 
    if (me->event != (chain_t *)0) { 
        event = echain_get((chain_t **)&(me->event)); 
        /* Dispatch the Event to Object's HSM */
        ret = TRUE;  
        if (me->hsm->dispatch != (hsm_dispatch_t)0) { 
            ret = (*(me->hsm->dispatch))(me->hsm, event); 
        } 
        if (ret != TRUE) { 
            /* Post into Defer Chain, Wait Idle State to Solve It */
            echain_post((chain_t **)&(me->defer), event)
            return TRUE; 
        } 
        /* Release the Event */
        epool_release(event); 
        if (me->hsm->service != (hsm_function_t)0) { 
            (*(me->hsm->service))(me->hsm); 
        } 
        SPYER_ACTIVEX("Event[%X] is Dispatch to HSM[%X] of Active Object[%X]. TimeStamp %d", \\
                   event, me->hsm, me, ticks_get()); 
    } 
    if (me->hsm->curr_state->service != (hsm_function_t)0) { 
        ret = (*(me->hsm->curr_state->service))(me->hsm); 
        if (ret != TRUE) { 
            return FAILURE; 
        } 
    } 
    return TRUE; 
}

/***************************************************************************************
*   active_reset() Implementation.
***************************************************************************************/
int16_t active_reset(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /* Request to Reset */ 
    me -> reset = (int16_t)1; 

    SPYER_ACTIVEX("Request to Reset the Active Object %X. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_quit() Implementation.
***************************************************************************************/
int16_t active_quit(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /* Request to Exit */
    me -> exit = (int16_t)1; 
    SPYER_ACTIVEX("Request to Exit the Active Object %X. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_pause() Implementation.
***************************************************************************************/
int16_t active_pause(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /* Request to Pause */ 
    me -> pause = (int16_t)1; 
    SPYER_ACTIVEX("Request to Pause the Active Object %X. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_resume() Implementation.
***************************************************************************************/
int16_t active_resume(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /* Clear the Pause */ 
    me -> pause = (int16_t)0; 
    SPYER_ACTIVEX("Request to Resume the Active Object %X. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_on_idle() Implementation.
***************************************************************************************/
int16_t active_on_idle(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /***********************************************************************************
    *   When None Event in the Event Chain and the HSM is in IDLE State, The Active 
    *   Object is be IDLE State. 
    ***********************************************************************************/
    ASSERT_REQUIRE(me->hsm->is_idle != (hsm_function_t)0); 
    if (me->hsm->is_idle != (hsm_function_t)0) { 
        if ((*(me->hsm->is_idle))(me->hsm) != TRUE) { 
            return FALSE; 
        } 
    } 
    if (me->event != (chain_t *)0) { 
        return FALSE; 
    } 
    if (me->defer != (chain_t *)0) { 
        return FALSE; 
    } 
    SPYER_ACTIVEX("Active Object %X is Idle. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_start() Implementation.
***************************************************************************************/
int16_t active_start(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /***********************************************************************************
    *   Construct and Initialize the Resource of Active Object. 
    *   Allocate the Rsource Associated with This Object at Here. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Construct and Initialize the HSM, and Execute the Initial Conversion, Entry 
    *   the Begin State of HSM.
    ***********************************************************************************/

    /***********************************************************************************
    *   At Here, The Hardware of Active Object Worked on is Opened by Object Itself.
    *   NOTE: 
    *   The Open of Hardware Associated with the Object MUST Located at Here, Because 
    *   It Must be Executed after Object is Enter Ready Work Status, Initialization 
    *   has been Completed.
    ***********************************************************************************/

    /***********************************************************************************
    *   Subscribe the Event "Signal" Required by Active Object. 
    *   NOTE: 
    *   (1) MUST Not Flush Out the Event Queue of This Active, Because It is Possible 
    *   that the Events is Generated by Hardware or Other High Priority Object when 
    *   Execute this Function.
    ***********************************************************************************/

    SPYER_ACTIVEX("Start the Active Object %X. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_probe() Implementation.
***************************************************************************************/
int16_t active_probe(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
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

/***************************************************************************************
*   active_on_quit() Implementation. 
***************************************************************************************/
int16_t active_on_quit(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /* Check the Quit Condition at Here. */
    if (0) { 
        /* Don't Meet the Quit Condition */
        return FALSE; 
    } 
    /* If Meet the Quit Condition, Execute the Quit Operation at Here. */
    /***********************************************************************************
    *   Unsubscribe the Event "Signal" Required by Active Object. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Close the Hardware Associated with This Active Object.
    *   NOTE: 
    *   The Close Action of Hardware Associated with the Active Object MUST Located
    *   at Here, Because It Must be Executed to Set the Hardware into Security Status 
    *   before Active Object is Destoryed.
    ***********************************************************************************/

    /***********************************************************************************
    *   Destroy the HSM. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Destroy the Resource of Active Object. 
    *   Release All the Resource of Active Object, e.g., the Memory Block which Get 
    *   from Memory Pool. 
    ***********************************************************************************/

    /***********************************************************************************
    *   Flush Out the Event Chain. 
    *   Release the Dynamic Event Allocated from the Event Pools.    
    ***********************************************************************************/

    SPYER_ACTIVEX("The Active Object %X is Exit. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   active_on_reset_() Implementation. 
***************************************************************************************/
int16_t active_on_reset(active_t *me)
{
    ASSERT_REQUIRE(me != (active_t *)0); 
    if (me == (active_t *)0) { 
        return FAILURE; 
    } 
    /* Check the Reset Condition at Here. */
    if (0) { 
        /* Don't Meet the Reset Condition */
        return FALSE; 
    } 
    /* If Meet the Reset Condition, Execute the Reset Operation at Here. */

    SPYER_ACTIVEX("The Active Object %X is Reset. TimeStamp %d", me, ticks_get()); 
} 
