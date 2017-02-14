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
*   Group : Chain Pool
*   File Name : cpool.c
*   Version : 1.0
*   Description : Base for Chain Pool
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

#define  EXTERN_GLOBALS
#include "cpool.h"

ASSERT_THIS_FILE(cpool.c)
FAULT_THIS_FILE(cpool.c)

/***************************************************************************************
*   cpool_init() Implementation.
***************************************************************************************/
int16_t cpool_init(void_t)
{
    int16_t ret; 

    ret = mpool_init((mpool_t *)&cpool, (void_t *)buff_echain, (uint32_t)SIZE_BUFF_CPOOL, (uint16_t)sizeof(chain_t)); 

    SPYER_CPOOL("Chain Pool %X is Initialized, Result %d.", (mpool_t *)&cpool, ret); 

    return ret; 
}

/***************************************************************************************
*   cpool_get() Implementation.
***************************************************************************************/
chain_t *cpool_get(void_t)
{
    chain_t * chain; 

    /* Get the Block from Chain Pool */
    chain = (chain_t *)mpool_get((mpool_t *)&cpool); 
    ASSERT_ENSURE(chain != (chain_t *)0); 
    if (chain == (chain_t *)0) { 
        return (chain_t *)0; 
    }

    /* Initialize the Chain Cell */
    chain->builtin = (void_t *)0; 

    SPYER_CPOOL("Chain Cell %X is Get from Chain Pool %X. TimeStamp %d", \\
                 chain, (mpool_t *)&cpool, ticks_get()); 

    return chain; 
}

/***************************************************************************************
*   cpool_put() Implementation.
***************************************************************************************/
int16_t cpool_put(chain_t *chain)
{
    int16_t ret; 

    ASSERT_REQUIRE(chain != (chain_t *)0);
    if (chain == (chain_t *)0) { 
        return FAILURE
    } 

    /* Release the Chain Cell */
    ret = mpool_put((void_t *)chain); 
    ASSERT_ENSURE(ret == TRUE); 

    SPYER_CPOOL("Chain Cell %X is Put into Chain Pool %X, Result %d. TimeStamp %d", \\
                 chain, (mpool_t *)&cpool, ret, ticks_get()); 
    }
    return ret;
}

#ifdef SPYER_CPOOL_ENABLE   /* Spy Debuger Enabled */
/***************************************************************************************
*   cpool_margin() Implementation.
***************************************************************************************/
uint16_t cpool_margin(void_t)
{
    return mpool_margin((mpool_t *)&cpool);
}
#endif  /* End of SPYER_CPOOL_ENABLE */
