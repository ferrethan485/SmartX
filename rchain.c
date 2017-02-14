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
*   File Name : rchain.c
*   Version : 1.0
*   Description : Base for Priority-Based Event Register Chain
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
#include "rchain.h"

ASSERT_THIS_FILE(rchain.c)
FAULT_THIS_FILE(rchain.c)

/***************************************************************************************
*   rchain_init() Implementation.
***************************************************************************************/
int16_t rchain_init(chain_t **me)
{
    int16_t ret; 

    ASSERT_REQUIRE(me != (chain_t **)0); 
    if (me == (chain_t **)0) { 
        return FAILURE; 
    } 

    *me = (chain_t *)0;    /* Initialize the List Head to 0 */

    SPYER_RCHAIN("Priority Based Event Register Chain %X is Initialized", me); 

    return ret; 
}

/***************************************************************************************
*   rchain_find() Implementation. 
***************************************************************************************/
chain_t *rchain_find(chain_t **me, signal_t signal)
{
    chain_t *chain;

    ASSERT_REQUIRE(me  != (chain_t **)0);
    if (me == (chain_t **)0) { 
        return (chain_t *)0; 
    } 

    if (*me == (chain_t *)0) { 
        /* Chain Pool is Empty */
        return (chain_t *)0; 
    } 
    chain = *me; 
    if ((event_t *)(chain->builtin)->signal == signal) { 
        /* Find It */
        return chain; 
    } 
    chain = chain->next; 
    while ((chain != *me) && ((event_t *)(chain->builtin)->signal != signal)) { 
        chain = chain->next; 
    } 
    if (chain == *me) { 
        /* Not Find It */
        return (chain_t *)0; 
    } 
    /* Find It */
    return chain; 
}

/***************************************************************************************
*   rchain_bind() Implementation.
***************************************************************************************/
int16_t rchain_bind(chain_t **me, event_t *event)
{
    chain_t *chain;
    chain_t *current;

    ASSERT_REQUIRE(me  != (chain_t **)0);
    ASSERT_REQUIRE(event != (event_t *)0);
    if ((me == (chain_t **)0) || (event == (event_t *)0)) { 
        return FAILURE; 
    } 

    /* The Active Object Has Bind in Chain Pool ? */
    if (rchain_find(me, event) != (chain_t *)0) { 
        /* Find It */
        SPYER_RCHAIN("Event %X Has Bind in Event Register Chain %X. TimeStamp %d", \\
                      event, me, ticks_get()); 
        return TRUE; 
    } 

    /* Get the Block from Event Register Chain Pool */
    chain = (chain_t *)cpool_get(); 
    ASSERT_ENSURE(chain != (chain_t *)0); 
    if (chain == (chain_t *)0) { 
        return FAILURE; 
    }

    /* Initialize the Event Register Chain Cell */
    chain->builtin = (void_t *)event; 

    if(*me == (chain_t *)0) {   /* If Priority-Based Event Register Chain is NULL */
        *me = chain;            /* Set the List Head */
        chain->next = chain;
        chain->fore = chain;
    }
    else {  /* The Priority-Based Event Register Chain is Not NULL */
        /* The Priority of "event" is High than The Highest Priority Event ? */
        if(event->priority < ((event_t *)(current->builtin))->priority) {   
            /* Yes, It High Than The Highest Priority Event */
            /* Get the Pointer of the Highest Priority Event */
            current = *me;
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

    SPYER_RCHAIN("Event %X is Bind into Event Register Chain %X. TimeStamp %d", \\
                  event, me, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   rchain_unbind() Implementation.
***************************************************************************************/
int16_t rchain_unbind(chain_t **me, chain_t *chain)
{
    int16_t ret; 
    chain_t *current; 

    ASSERT_REQUIRE(me  != (chain_t **)0);
    ASSERT_REQUIRE(chain != (chain_t *)0);
    if ((me == (chain_t **)0) || (chain == (chain_t *)0)) { 
        return FAILURE; 
    } 

    /***********************************************************************************
    *   NOTE: For Acceptance the Error, This Point Not Use the "&&", But "||".
    ***********************************************************************************/
    if((chain->fore == chain) || (chain->next == chain)) {
        *me = (chain_t *)0;
    }
    else {
        chain->next->fore = chain->fore; 
        chain->fore->next = chain->next;
        if (*me == chain) { 
            *me = chain->next; 
        } 
    }

    /* Release the Block of Event */
    ret = epool_release((event_t *)(current->builtin)); 
    ASSERT_ENSURE(ret == TRUE); 
    /* Release the Block of Event Register Chain Cell */
    ret = cpool_put((chain_t *)chain); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_RCHAIN("Event %X is Unbind from Event Register Chain %X. TimeStamp %d", \\
                  event, me, ticks_get()); 

    return TRUE;
}
