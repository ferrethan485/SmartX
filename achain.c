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
*   File Name : achain.c
*   Version : 1.0
*   Description : Base for Priority-Based Active Object Chain
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
#include "cpool.h"
#include "active.h"

#define  EXTERN_GLOBALS
#include "achain.h"

ASSERT_THIS_FILE(achain.c)
FAULT_THIS_FILE(achain.c)

/***************************************************************************************
*   achain_init() Implementation.
***************************************************************************************/
int16_t achain_init(chain_t **me)
{
    ASSERT_REQUIRE(me != (chain_t **)0); 
    if (me == (chain_t **)0) { 
        return FAILURE; 
    } 

    *me = (chain_t *)0;    /* Initialize the List Head to 0 */

    SPYER_ACHAIN("Priority Based Active Object Chain %X is Initialized", me); 

    return TRUE; 
}

/***************************************************************************************
*   achain_find() Implementation. Internally
***************************************************************************************/
chain_t *achain_find(chain_t **me, active_t *active)
{
    chain_t *chain;

    if (*me == (chain_t *)0) { 
        /* Chain Pool is Empty */
        return (chain_t *)0; 
    } 
    chain = *me; 
    if ((active_t *)(chain->builtin) == active) { 
        /* Find It */
        return chain; 
    } 
    chain = chain->next; 
    while ((chain != *me) && ((active_t *)(chain->builtin) != active)) { 
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
*   achain_bind() Implementation.
***************************************************************************************/
int16_t achain_bind(chain_t **me, active_t *active)
{
    chain_t *chain;
    chain_t *current;

    ASSERT_REQUIRE(me  != (chain_t **)0);
    ASSERT_REQUIRE(active != (active_t *)0);
    if ((me == (chain_t **)0) || (active == (active_t *)0)) { 
        return FAILURE; 
    } 

    /* The Active Object Has Bind in Chain Pool ? */
    if (achain_find(me, active) != (chain_t *)0) { 
        /* Find It */
        SPYER_ACHAIN("Active Object %X Has Bind in Active Object Chain %X. TimeStamp %d", \\
                      active, me, ticks_get()); 
        return TRUE; 
    } 

    /* Get the Block from Active Object Chain Pool */
    chain = (chain_t *)cpool_get(); 
    ASSERT_ENSURE(chain != (chain_t *)0); 
    if (chain == (chain_t *)0) { 
        return FAILURE; 
    }

    /* Initialize the Active Object Chain Cell */
    chain->builtin = (void_t *)active; 

    if(*me == (chain_t *)0) {   /* If Priority-Based Active Object Chain is NULL */
        *me = chain;            /* Set the List Head */
        chain->next = chain;
        chain->fore = chain;
    }
    else {  /* The Priority-Based Active Object Chain is Not NULL */
        /* The Priority of "active" is High than The Highest Priority Active Object ? */
        if(active->priority < ((active_t *)(current->builtin))->priority) {   
            /* Yes, It High Than The Highest Priority Active Object */
            /* Get the Pointer of the Highest Priority Active Object */
            current = *me;
            /* Append this Active Object into The Head of Active Object Chain */
            chain->next  = current;
            chain->fore  = current->fore;  /* Circulated Bidirectional List */
            current->fore = chain;
            chain->fore->next = chain;    /* Circulated Bidirectional List */
            /* Set the List Head */
            *me = chain;
        }
        else {   
            /* Not, It Low or Equal Than The Highest Priority Active Object */
            /* Get the Pointer of the Lowest Priority Active Object */
            current = (*me)->fore;
            /* Search the List of Active Object, Find the Insert Point */
            while((current != (*me)) && (active->priority < ((active_t *)(current->builtin))->priority)) {
                current = current->fore;     /* Point to Fore Active Object */
            }
            /***************************************************************************
            *   This NOT Need to ASSERT the (current == *me) , Because the 
            *   Event Should Append the Back-Side of *me when Not Find the 
            *   Insert Point in This while() Loop.
            ***************************************************************************/
            /* Append this Active Object into The Active Object Chain */
            chain->next  = current->next;
            chain->fore  = current;
            current->next = chain;
            chain->next->fore = chain;
        }
    }

    SPYER_ACHAIN("Active Object %X is Bind into Active Object Chain %X. TimeStamp %d", \\
                  active, me, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   achain_unbind() Implementation.
***************************************************************************************/
int16_t achain_unbind(chain_t **me, active_t *active)
{
    int16_t ret; 
    chain_t *chain; 
    chain_t *current; 

    ASSERT_REQUIRE(me  != (chain_t **)0);
    ASSERT_REQUIRE(active != (active_t *)0);
    if ((me == (chain_t **)0) || (active == (active_t *)0)) { 
        return FAILURE; 
    } 

    /* The Active Object Has Live in Chain Pool ? */
    chain = achain_find(me, active); 
    if (chain == (chain_t *)0) { 
        /* Dont' Find It */
        SPYER_ACHAIN("Active Object %X Has Unbind from Active Object Chain %X. TimeStamp %d", \\
                      active, me, ticks_get()); 
        return TRUE; 
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

    /* Release the Block of Active Object Chain Cell */
    ret = cpool_put((chain_t *)chain); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_ACHAIN("Active Object %X is Unbind from Active Object Chain %X. TimeStamp %d", \\
                  active, me, ticks_get()); 

    return TRUE;
}
