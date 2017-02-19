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
*   Group : Memory Pool 
*   File Name : mpool.c                 
*   Version : 1.0
*   Description : Base for Memory Pool 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-16-2005      Created Initial Version 1.0
*       JinHui Han      02-16-2005      Verified Version 1.0
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

#define  EXTERN_GLOBALS 
#include "mpool.h" 

ASSERT_THIS_FILE(mpool.c)
FAULT_THIS_FILE(mpool.c)

/***************************************************************************************
*   mpool_init() Implementation.
***************************************************************************************/
int16_t mpool_init(mpool_t *me, void_t *begin, uint32_t size, uint16_t block_size)
{
    free_mblock_t *free;
    uint16_t n;

    ASSERT_REQUIRE(me != (mpool_t *)0);
    ASSERT_REQUIRE(begin != (void_t *)0);
    ASSERT_REQUIRE((begin & (sizeof(void_t *) - 1)) == (void_t *)0); 
    ASSERT_REQUIRE(size != 0);
    /* The block_size must not be too Close to the Top of the Dynamic Range   */
    /* Such as, If the block_size Value is 65533, 65533 + 4 will equal to 1,  */ 
    /* This should be Avoided. 4 is The Byte Number of an Pointer at Here. */
    ASSERT_REQUIRE((uint16_t)(block_size + (uint16_t)sizeof(void_t *)) > block_size); 
    if ((me == (mpool_t *)0) || (begin == (void_t *)0)) { 
        return FAILURE; 
    }
    if (size == 0) {
        return FAILURE; 
    } 
    if ((uint16_t)(block_size + (uint16_t)sizeof(void_t *)) > block_size) { 
        return FAILURE; 
    }

    /* Round Up the block_size to Fit an Integer Number of Pointers */
    me->block_size = (uint16_t)sizeof(free_mblock_t);  /* Start with Just One */
    /* The Number of free_mblock_t * Pointers that Fit in one Memory Block */
    n = (uint16_t)1;
    while (me->block_size < block_size) {
        me->block_size += (uint16_t)sizeof(free_mblock_t);
        ++n;
    }
    block_size  = me->block_size;    /* Use the Rounded-Up Value from Here On */
    /* Reserve the Storage Location  for Remeber the Memory Pool which Block */
    /* is Allocated from, Size is (sizeof (void_t *). */
    block_size += (uint16_t)sizeof (void_t *); 

    /* The Whole Pool Buffer must Fit at least One Round Up Block */
    ASSERT_REQUIRE(size >= (uint32_t)block_size); 
    if (size < (uint32_t)block_size) { 
        return FAILURE; 
    } 

    me->start = begin;         /* The Start of Memory Managed by this Pool   */
    me->free  = begin;         /* Set the Head of Linked-List of Free Blocks */

    /* Chain all Blocks Together in a Free-List... */
    free = (free_mblock_t *)begin;
    me->total_cnt = (uint16_t)1;            /* One (the Last) Block in the Pool */
    size -= (uint32_t)block_size;           /* Don't Chain the Last Block */
    while (size >= (uint32_t)block_size) {
        free->next = &free[n];  /* Setup the Next Link   */
        free = free->next;      /* Advance to Next Block */
        ++me->total_cnt;
        size -= (uint32_t)block_size;
    }
    free->next = (free_mblock_t *)0;   /* The Last Link Points to NULL */
    me->end = free;   /* End of Memory Managed by This Pool (the Last Block) */

    me->free_cnt = me->total_cnt;           /* Store Number of Free Blocks */
    me->min_cnt  = me->total_cnt;           /* The Minimum Number of Free Blocks */

    SPYER_MPOOL("MPool %X is Initialized, Start %X, Block Size %d, Total %d.", \\
                 me, begin, me->block_size, me->total_cnt); 

    return TRUE; 
}

/***************************************************************************************
*   mpool_get() Implementation.
***************************************************************************************/
void_t *mpool_get(mpool_t *me)
{
    free_mblock_t *free;

    ASSERT_REQUIRE(me != (mpool_t *)0);
    if (me == (mpool_t *)0) { 
        return NULL; 
    }

    free = (free_mblock_t *)0;     /* No Free Block Available */

    INT_LOCK_KEY_
    INT_LOCK_();
    if (me->free_cnt > (uint16_t)0) {       /* Free Block Available ? Block is Depletion */
        free = (free_mblock_t *)me->free;   /* Get the Free Block */
        /* Adjust List Head to the Next Free Block */
        me->free = (void_t *)free->next;
        if (--me->free_cnt < me->min_cnt) { 
            /* One Less Block */
            me->min_cnt = me->free_cnt;     /* Remember the Minimum so far */
        }
    }
    INT_UNLOCK_();

    ASSERT_ENSURE(free != (free_mblock_t *)0); 
    if (free == (free_mblock_t *)0) { 
        return NULL; 
    } 

    /* Backup the Memory Pool Pointer */
    *((uint32_t *)free) = (uint32_t)me;     /* 32 Bits Address */ 

    SPYER_MPOOL("Block %X is Obtained from MPool %X, Free %d, Min %d. TimeStamp %d", \\ 
                free, me, me->free_cnt, me->min_cnt, ticks_get()); 

    /* Return the Block or NULL Pointer to the Caller */
    return (void_t *)(++free);
}

/***************************************************************************************
*   mpool_put() Implementation.
***************************************************************************************/
int16_t mpool_put(void_t *block)
{
    mpool_t * mpool; 

    ASSERT_REQUIRE(block != ( void_t *)0);
    if (block == ( void_t *)0) { 
        return FAILURE; 
    } 

    /* Get the Memory Pool Pointer */
    ((uint32_t *)block)--; 
    mpool = (mpool_t *)(*((uint32_t *)block));  /* 32 Bits Address */
    ASSERT_REQUIRE(mpool != (mpool_t *)0); 
    if (mpool == (mpool_t *)0) { 
        return FAILURE; 
    } 

    /* Must be in Range */
    ASSERT_REQUIRE((mpool->start <= block) && (block <= mpool->end));  
    if ((mpool->start > block) || (block > mpool->end)) { 
        return FAILURE; 
    }

    INT_LOCK_KEY_
    INT_LOCK_();
    /* Free Blocks must be < total_cnt */
    if(mpool->free_cnt >= mpool->total_cnt) {
        INT_UNLOCK_();
        ASSERT_ERROR();
        return FAILURE; 
    }
    /* Link into Free List */
    ((free_mblock_t *)block)->next = (free_mblock_t *)mpool->free;
    mpool->free = block;           /* Set as New Head of the Free List */
    ++mpool->free_cnt;             /* One more Free Block in this Pool */
    INT_UNLOCK_();

    SPYER_MPOOL("Block %X is Put Back into MPool %X, Free %d. TimeStamp %d", \\ 
                 block, mpool, mpool->free_cnt, ticks_get()); 

    return TRUE; 
}

/***************************************************************************************
*   mpool_all_recycled() Implementation.
***************************************************************************************/
int16_t mpool_all_recycled(mpool_t *me)
{
    if(me->free_cnt < me->total_cnt) { 
        return FALSE; 
    }
    /* Fault-Tolerant for "me->free_cnt > me->total_cnt" */
    return  TRUE; 
}

#ifdef SPYER_MPOOL_ENABLE   /* Spy Debuger Enabled */
/***************************************************************************************
*   mpool_margin() Implementation.
***************************************************************************************/
uint16_t mpool_margin(mpool_t *me)
{
    uint16_t margin;

    ASSERT_REQUIRE(me != (mpool_t *)0);
    if (me == (mpool_t *)0) { 
        return (uint16_t)0; 
    }

    INT_LOCK_KEY_
    INT_LOCK_();
    margin = (uint16_t)(me->min_cnt);
    INT_UNLOCK_();

    return margin;
}
#endif  /* End of SPYER_MPOOL_ENABLE */

