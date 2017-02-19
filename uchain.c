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
*   Group : General Chain
*   File Name : uchain.c
*   Version : 1.0
*   Description : Base for General Chain
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
#include "uchain.h"

ASSERT_THIS_FILE(uchain.c)
FAULT_THIS_FILE(uchain.c)

/***************************************************************************************
*   uchain_init() Implementation.
***************************************************************************************/
int16_t uchain_init(chain_t **me)
{
    ASSERT_REQUIRE(me != (chain_t **)0); 
    if (me == (chain_t **)0) { 
        return FAILURE; 
    } 

    *me = (chain_t *)0;    /* Initialize the List Head to 0 */

    SPYER_UCHAIN("General Chain %X is Initialized", me); 

    return TRUE; 
}

/***************************************************************************************
*   uchain_find() Implementation. Internally
***************************************************************************************/
chain_t *uchain_find(chain_t **me, void_t *cell)
{
    chain_t *chain;

    if (*me == (chain_t *)0) { 
        /* Chain Pool is Empty */
        return (chain_t *)0; 
    } 
    chain = *me; 
    if ((void_t *)(chain->builtin) == cell) { 
        /* Find It */
        return chain; 
    } 
    chain = chain->next; 
    while ((chain != *me) && ((void_t *)(chain->builtin) != cell)) { 
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
*   uchain_bind() Implementation.
***************************************************************************************/
int16_t uchain_bind(chain_t **me, void_t *cell)
{
    chain_t *chain;
    chain_t *current;

    ASSERT_REQUIRE(me   != (chain_t **)0);
    ASSERT_REQUIRE(cell != (chain_t *)0);
    if ((me == (chain_t **)0) || (cell == (chain_t *)0)) { 
        return FAILURE; 
    } 

    /* The Cell Has Bind in GeneralChain ? */
    if (uchain_find(me, cell) != (chain_t *)0) { 
        /* Find It */
        SPYER_UCHAIN("Cell %X Has Bind in General Chain %X. TimeStamp %d", 
                     cell, me, ticks_get()); 
        return TRUE; 
    } 

    /* Get the Block from Chain Pool */
    chain = (chain_t *)cpool_get(); 
    ASSERT_ENSURE(chain != (chain_t *)0); 
    if (chain == (chain_t *)0) { 
        return FAILURE; 
    }

    /* Initialize the General Chain Cell */
    chain->builtin = (void_t *)cell; 

    if(*me == (chain_t *)0) {   /* If General Chain is NULL */
        *me = chain;            /* Set the List Head */
        chain->next = chain;
        chain->fore = chain;
    }
    else {  /* The General Chain is Not NULL */
        /* Bind into the End of the General Chain */
        current = (*me)->fore;
        /* Append this Cell into The General Chain */
        chain->next  = current->next;
        chain->fore  = current;
        current->next = chain;
        chain->next->fore = chain;
    }

    SPYER_UCHAIN("Data %X is Bind into General Chain %X. TimeStamp %d", 
                 cell, me, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   uchain_unbind() Implementation.
***************************************************************************************/
int16_t uchain_unbind(chain_t **me, void_t *cell)
{
    int16_t ret; 
    chain_t *chain; 
    chain_t *current; 

    ASSERT_REQUIRE(me   != (chain_t **)0);
    ASSERT_REQUIRE(cell != (void_t *)0);
    if ((me == (chain_t **)0) || (cell == (void_t *)0)) { 
        return FAILURE; 
    } 

    /* The Cell Has Live in General Chain ? */
    chain = uchain_find(me, cell); 
    if (chain == (chain_t *)0) { 
        /* Dont' Find It */
        SPYER_UCHAIN("Cell %X Has Unbind from General Chain %X. TimeStamp %d", 
                     cell, me, ticks_get()); 
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

    /* Release the Block of General Chain Cell */
    ret = cpool_put((chain_t *)chain); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_UCHAIN("Cell %X is Unbind from General Chain %X. TimeStamp %d", 
                 cell, me, ticks_get()); 

    return TRUE;
}
