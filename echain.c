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
*   File Name : echain.c
*   Version : 1.0
*   Description : Base for Priority-Based Event Chain
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-20-2005      Created Initial Version 1.0
*       JinHui Han      03-20-2005      Verified Version 1.0
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
#include "cpool.h"

#define  EXTERN_GLOBALS
#include "echain.h"

ASSERT_THIS_FILE(echain.c)
FAULT_THIS_FILE(echain.c)

/***************************************************************************************
*   echain_init() Implementation.
***************************************************************************************/
int16_t echain_init(chain_t **me)
{
    int16_t ret; 

    ASSERT_REQUIRE(me != (chain_t **)0); 
    if (me == (chain_t **)0) { 
        return FAILURE; 
    } 

    *me = (chain_t *)0;    /* Initialize the List Head to 0 */

    SPYER_ECHAIN("Priority Based Event Chain %X is Initialized", me); 

    return ret; 
}

/***************************************************************************************
*   echain_post() Implementation.
***************************************************************************************/
int16_t echain_post(chain_t **me, event_t *event)
{
    chain_t *chain;
    chain_t *current;

    ASSERT_REQUIRE(me  != (chain_t **)0);
    ASSERT_REQUIRE(event != (event_t *)0);
    if ((me == (chain_t **)0) || (event == (event_t *)0)) { 
        return FAILURE; 
    } 
    
    /* Get the Block from Event Chain Pool */
    chain = (chain_t *)cpool_get(); 
    ASSERT_ENSURE(chain != (chain_t *)0); 
    if (chain == (chain_t *)0) { 
        return FAILURE; 
    }

    /* Initialize the Event Chain Cell */
    chain->builtin = (void_t *)event; 

    INT_LOCK_KEY_
    INT_LOCK_();
    if(*me == (chain_t *)0) {   /* If Priority-Based Event Chain is NULL */
        *me = chain;            /* Set the List Head */
        chain->next = chain;
        chain->fore = chain;
    }
    else {  /* The Priority-Based Event Chain is Not NULL */
        /* Get the Pointer of the Highest Priority Event */
        current = *me;
        /* The Priority of "event" is High than The Highest Priority Event ? */
        if(event->priority < ((event_t *)(current->builtin))->priority) {   
            /* Yes, It High Than The Highest Priority Event */
            /* Append this Event into The Head of Event Chain */
            chain->next  = current;
            chain->fore  = current->fore;  /* Circulated Bidirectional List */
            current->fore = chain;
            chain->fore->next = chain;    /* Circulated Bidirectional List */
            /* Set the List Head */
            *me = chain;
        }
        else {   
            /* Not, It Low or Equal Than The Highest Priority Event */
            /* Get the Pointer of the Lowest Priority Event */
            current = (*me)->fore;
            /* Search the List of Event, Find the Insert Point */
            while((current != (*me)) && (event->priority < ((event_t *)(current->builtin))->priority)) {
                current = current->fore;     /* Point to Fore Event */
            }
            /***************************************************************************
            *   This NOT Need to ASSERT the (current == *me) , Because the 
            *   Event Should Append the Back-Side of *me when Not Find the 
            *   Insert Point in This while() Loop.
            ***************************************************************************/
            /* Append this Event into The Event Chain */
            chain->next  = current->next;
            chain->fore  = current;
            current->next = chain;
            chain->next->fore = chain;
        }
    }
    INT_UNLOCK_();

    SPYER_ECHAIN("Event %X is Post into Event Chain %X, Signal %d. TimeStamp %d", \\
                  event, me, event->signal, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   echain_get() Implementation.
***************************************************************************************/
event_t *echain_get(chain_t **me)
{
    int16_t ret; 
    event_t *event; 
    chain_t *current; 

    ASSERT_REQUIRE(me != (chain_t **)0);
    if (me == (chain_t **)0) { 
        return (event_t *)0; 
    } 

    INT_LOCK_KEY_
    event = (event_t *)0; 
    /* If Priority-Based Event Chain is not NULL */
    if(*me != (chain_t *)0) {   
        INT_LOCK_();
        /* Get the Highest Priority Event from Event Chain */
        current = *me; 
        /*******************************************************************************
        *   NOTE: For Acceptance the Error, This Point Not Use the "&&", But "||".
        *******************************************************************************/
        if((current->fore == current) || (current->next == current)) {
            *me = (chain_t *)0;
        }
        else {
            current->next->fore = current->fore; 
            current->fore->next = current->next;
            *me = current->next; 
        }

        /* Get The Pointer of Event */
        event = (event_t *)(current->builtin); 

        /* Release the Block of Event Chain Cell */
        ret = cpool_put((chain_t *)current); 
        INT_UNLOCK_();

        ASSERT_ENSURE(ret == TRUE); 

        SPYER_ECHAIN("Event %X is Get from Event Chain %X, Signal %d, Dynamic %d. TimeStamp %d", \\
                  event, me, event->signal, event->dynamic_, ticks_get()); 
    }
    return event;
}

/***************************************************************************************
*   echain_flush() Implementation.
***************************************************************************************/
int16_t echain_flush(chain_t **me)
{
    int16_t ret; 
    event_t *event; 
    chain_t *current; 

    ASSERT_REQUIRE(me != (chain_t **)0);
    if (me == (chain_t **)0) { 
        return FAILURE; 
    } 

    /* If Priority-Based Event Chain is NULL */
    if(*me == (chain_t *)0) { 
        return TRUE; 
    }
    /* Get the Head of Event Chain */
    current = *me; 
    /***********************************************************************************
    *   NOTE: For Acceptance the Error, This Point Not Use the "&&", But "||".
    ***********************************************************************************/
    while ((current->fore != current) && (current->next != current)) { 
        /* Tear Down the "current" Event Chain Cell */
        current->next->fore = current->fore; 
        current->fore->next = current->next;
        /* Release the Block of Event */
        ret = epool_release((event_t *)(current->builtin)); 
        ASSERT_ENSURE(ret == TRUE); 
        /* Release the Block of Event Chain Cell */
        ret = cpool_put((chain_t *)current); 
        ASSERT_ENSURE(ret == TRUE); 
        /* Pointer to the Next Event Chain Cell */
        current = current->next; 
    }
    /* Release the Block of Event */
    ret = epool_release((event_t *)(current->builtin)); 
    ASSERT_ENSURE(ret == TRUE); 
    /* Release the Block of Event Chain Cell */
    ret = cpool_put((chain_t *)current); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_ECHAIN("Event Chain %X is Flush Out. TimeStamp %d", me, ticks_get()); 

    *me = (chain_t *)0;
    return TRUE;
}
