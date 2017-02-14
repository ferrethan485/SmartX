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
*   Group : Variable Size Block Heap
*   File Name : hpool.c                 
*   Version : 1.0
*   Description : Base for Variable Size Block Heap (ONLY Used in Object Internal)
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-08-2005      Created Initial Version 1.0
*       JinHui Han      03-08-2005      Verified Version 1.0
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
#include "hpool.h"

ASSERT_THIS_FILE(hpool.c)
FAULT_THIS_FILE(hpool.c)

/***************************************************************************************
*   hpool_index_lookup[] definition.
***************************************************************************************/
int8_t const ROM_KEYWORDS hpool_index_lookup[256] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

/***************************************************************************************
*   hpool_size_lookup[] definition.
***************************************************************************************/
uint32_t const ROM_KEYWORDS hpool_size_lookup[32] = {
    1,        2,        4,        8,         16,        32,        64,         128, 
    256,      512,      1024,     2048,      4096,      8192,      16384,      32768, 
    65536,    131072,   262144,   524288,    1048576,   2097152,   4194304,    8388608, 
    16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648
}

/***************************************************************************************
*   Internal Constant to Limit the Maximum Block Size that User Can Specified.
***************************************************************************************/
#define MAX_BLOCK_SIZE_  2147483648    /* 2^31 = 2147483648 */

/***************************************************************************************
*   Variable Size Block Heap.
***************************************************************************************/
hpool_t hpool; 

/***************************************************************************************
*   power() Internal Implementation. Compute the Round Up Power. 
***************************************************************************************/
inline uint32_t power (uint32_t value) 
{
    uint32_t ret; 

    if (value & 0xFFFFFF00 == 0) {
        ret = hpool_index_lookup[value];
    }
    else if (value & 0xFFFF0000 == 0) { 
        ret = hpool_index_lookup[value >>  8] +  8;
    }
    else if (value & 0xFF000000 == 0) { 
        ret = hpool_index_lookup[value >> 16] + 16;
    }
    else {
        ret = hpool_index_lookup[value >> 24] + 24;
    }
    return ret; 
}

/***************************************************************************************
*   hpool_init() Implementation.
***************************************************************************************/
int16_t hpool_init(void_t *begin, uint32_t size, uint32_t max_block_size)
{
    int8_t    index;
    uint32_t  n; 
    hpool_t * me; 

    ASSERT_REQUIRE(begin != (void_t *)0);
    ASSERT_REQUIRE((begin & (sizeof(void_t *) - 1)) == (void_t *)0);
    ASSERT_REQUIRE(size >= HPOOL_FACTOR * sizeof(free_hblock_t *)); 
    if (begin == (void_t *)0) { 
        return FAILURE; 
    }
    if ((begin & (SIZE_OF_PTR - 1)) != (void_t *)0) { 
        return FAILURE; 
    }
    if (size < HPOOL_FACTOR * sizeof(free_hblock_t *)) { 
        return FAILURE; 
    }

    /* Calculate the max_block_size when It is 0 */
    if (max_block_size == 0) { 
        n = power (size); 
        max_block_size = hpool_size_lookup [n - 1];  /* Round Down */
    } 
    else { 
        max_block_size += (uint32_t)sizeof(void_t *); 
        /* The max_block_size must not be too Close to the Top of the Dynamic Range. */
        /* Such as, If the max_block_size Value is 2147483645, */
        /* 2147483645 + 4 = 2147483649 > MAX_BLOCK_SIZE_, */ 
        /* This should be Avoided. 4 is sizeof(void_t *) at Here. */
        ASSERT_REQUIRE(max_block_size <= (uint32_t)MAX_BLOCK_SIZE_); 
        if (max_block_size > (uint32_t)MAX_BLOCK_SIZE_) {
            return FAILURE; 
        } 
    }

    me = (hpool_t *)&hpool; 
    /* Inialize the Memory Start and End Address of this Pool */
    me->start = begin;
    /***********************************************************************************
    *   For Some Compiler, the Memory Unit of uint8_t is Two Byte, and is Properly
    *   Aligned at Half-Word Boundary. So At Here, Can't Transform the "begin"
    *   into (uint8_t *) for Compute the End Memory Address.
    ***********************************************************************************/
    me->end   = (void_t *)(((uint32_t)begin) + size);

    /* Check for Address Wrap-Back Around */
    ASSERT_REQUIRE(me->end > me->start);
    if (me->end <= me->start) { 
        return FAILURE; 
    } 
    /* Check for Address Wrap-Back Around on One Times or Many Times */
    ASSERT_REQUIRE(((uint32_t)me->end - (uint32_t)me->start) == (uint32_t)size); 
    if (((uint32_t)me->end - (uint32_t)me->start) != (uint32_t)size) { 
        return FAILURE; 
    } 

    /* Calculate the Section Index Associate with max_block_size */
    n = (max_block_size - 1) / (HPOOL_FACTOR * sizeof(free_hblock_t *));
    ASSERT_REQUIRE(n < (MAX_BLOCK_SIZE_/(HPOOL_FACTOR * sizeof(free_hblock_t *)))); 
    if (n >= (MAX_BLOCK_SIZE_/(HPOOL_FACTOR * sizeof(free_hblock_t *)))) { 
        return FAILURE; 
    }
    me->max_index = power (n);
    ASSERT_REQUIRE(me->max_index < HPOOL_AMOUNT); 
    if (me->max_index >= HPOOL_AMOUNT) { 
        return FAILURE; 
    } 

    /* Round Up the Block Size to Fit the (2^index)*HPOOL_FACTOR*sizeof(free_hblock_t *) */
    n = (uint32_t)(hpool_size_lookup[me->max_index] * (HPOOL_FACTOR * sizeof(free_hblock_t *)));
    ASSERT_REQUIRE(n >= max_block_size); 
    if (n < max_block_size) { 
        return FAILURE; 
    }

    /* The Whole Pool Buffer must Fit at least One Largest Block (Round Up) */
    ASSERT_REQUIRE(size >= (uint32_t)n); 
    if (size < (uint32_t)n) { 
        return FAILURE; 
    } 

    for (index = me->max_index; index >= 0; --index) {
        /* Calculate the Amount of (void_t*) that Block-Size of This Section Contained */
        n = (uint32_t)(hpool_size_lookup[index] * HPOOL_FACTOR);
        /* Use the Rounded-Up Value from Here On */
        max_block_size = n * sizeof(free_hblock_t *);
        /* Initialize the Block Size of This Section */
        me->section[index].block_size = max_block_size;
        /* The Whole Pool Buffer must Fit at least One Largest Block */
        if(size < (uint32_t)max_block_size) {
            /* Clear All the Attributes of This Section */
            me->section[index].free      = (void_t *)0;
            me->section[index].total_cnt = (uint32_t)0;
            me->section[index].free_cnt  = (uint32_t)0;
            me->section[index].allocate_cnt = (uint32_t)0;
            me->section[index].max_cnt   = (uint32_t)0;
        }
        else {
            /* Set the Head of Linked-List of Free Blocks */
            me->section[index].free = (void_t *)begin;
            /***************************************************************************
            *   Chain all Blocks Together in a Free-List...
            ***************************************************************************/
            /* One (the Last) Block in the Pool */
            me->section[index].total_cnt = (uint32_t)1;
            /* Don't Chain the Last Block */
            size -= (uint32_t)max_block_size;
            while (size >= (uint32_t)max_block_size) {
                /* Setup the Next Link   */
                ((free_hblock_t *)begin)->next = &((free_hblock_t *)begin)[n];
                /* Advance to Next Block */
                (free_hblock_t *)begin) = ((free_hblock_t *)begin)->next;
                ++me->section[index].total_cnt;
                size -= (uint32_t)max_block_size;
            }
            /* The Last Link Points to 0 */
            ((free_hblock_t *)begin)->next = (free_hblock_t *)0;
            /* Store Number of Free Blocks */
            me->section[index].free_cnt = me->section[index].total_cnt;
            /* Number of Allocated Blocks Currently in This Section */
            me->section[index].allocate_cnt = (uint32_t)0;
            /* Maximum Number of Allocated Blocks ever Present in this Section */
            me->section[index].max_cnt  = (uint32_t)0;
            /* Setup the Memory Space for Next Section */
            ((free_hblock_t *)begin) = &((free_hblock_t *)begin)[n];

            SPYER_HPOOL("HPool %X is Initialized, Section %d, Start %X, Block Size %d, Total %d.", \\ 
                         me, index, \\ 
                         me->section[index].free, \\ 
                         me->section[index].block_size, \\ 
                         me->section[index].total_cnt); 
        }
    }
    SPYER_HPOOL("HPool %X Initialization is Done.", me); 
    return TRUE; 
}

/***************************************************************************************
*   hpool_get() Implementation.
***************************************************************************************/
void_t *hpool_get(uint32_t block_size)
{
    int8_t    current;
    int8_t    index;
    uint32_t  n;
    hpool_t * me; 
    free_hblock_t *free_block; 

    ASSERT_REQUIRE(block_size <= (uint32_t)MAX_BLOCK_SIZE_); 
    if (block_size > (uint32_t)MAX_BLOCK_SIZE_) { 
        return NULL; 
    }

    /* Calculate the Index Associate with block_size */
    n = (block_size - 1) / (HPOOL_FACTOR * sizeof(free_hblock_t *));
    ASSERT_REQUIRE(n < (MAX_BLOCK_SIZE_/(HPOOL_FACTOR * sizeof(free_hblock_t *)))); 
    if (n >= (MAX_BLOCK_SIZE_/(HPOOL_FACTOR * sizeof(free_hblock_t *)))) { 
        return NULL; 
    }
    index = power (n);
    ASSERT_REQUIRE(index <= me->max_index);
    if (index > me->max_index) { 
        return NULL; 
    } 

    me = (hpool_t *)&hpool; 

    INT_LOCK_KEY_
    INT_LOCK_();

    /* Free Block Available ? */
    if (me->section[index].free_cnt > (uint32_t)0) {
        /* Get the Free Block */
        free_block = (free_hblock_t *)me->section[index].free;
        /* Adjust List Head to the Next Free Block */
        me->section[index].free = (void_t *)free_block->next;
        /* Reduce the Number of Free Blocks */
        --me->section[index].free_cnt;
        if (++me->section[index].allocate_cnt > me->section[index].max_cnt)
        {
            /* Remember the Maximum so far */
            me->section[index].max_cnt = me->section[index].allocate_cnt;
        }
        INT_UNLOCK_();
        /* The Section Index is in "index" */
    }
    else {
        INT_UNLOCK_();
        current = index++;      /* Backup the Section Index */
        /* Search the Larger Size Block, Find Larger Size Free Block in It. */
        INT_LOCK_();
        while((index <= me->max_index) && (me->section[index].free_cnt == (uint32_t)0)) {
            ++index;
        }
        /* Find The Larger Size Block ? */
        if (index <= me->max_index) {   /* Find It */
            /* Get the Free Block */
            free_block = (free_hblock_t *)me->section[index].free;
            /* Adjust List Head to the Next Free Block */
            me->section[index].free = (void_t *)free_block->next;
            /* Reduce the Number of Free Blocks */
            --me->section[index].free_cnt;
            /* Reduce the Total Number of Blocks */
            --me->section[index].total_cnt;

            SPYER_HPOOL("Move Block %X from Section %d in HPool %X, Total %d, Free %d. TimeStamp %d", \\ 
                         free_block, index, me, \\ 
                         me->section[index].total_cnt, \\ 
                         me->section[index].free_cnt, \\ 
                         ticks_get()); 

            for(--index; index >= current; --index) {
                /* Put the Free Block into Smaller Size Section */
                free_block->next = (free_hblock_t *)me->section[index].free;
                /* Adjust List Head to the Next Free Block */
                me->section[index].free = (void_t *)free_block;
                /* Increase the Number of Free Blocks */
                ++me->section[index].free_cnt;
                /* Increase the Total Number of Blocks */
                ++me->section[index].total_cnt;

                SPYER_HPOOL("Move Block %X to Section %d in HPool %X, Total %d, Free %d. TimeStamp %d", \\ 
                            free_block, index, me, \\ 
                            me->section[index].total_cnt, \\ 
                            me->section[index].free_cnt, \\ 
                            ticks_get()); 

                /***********************************************************************
                *   For Some Compiler, the Memory Unit of uint8_t is Two Byte, and is 
                *   Properly Aligned at Half-Word Boundary. So At Here, Can't Transform 
                *   the "begin" into (uint8_t *) for Compute the End Memory 
                *   Address.
                ***********************************************************************/
                free_block = (free_hblock_t *)(((uint32_t)free_block) + 
                                               me->section[index].block_size);
            }
            /* Increase the Total Number of Blocks */
            ++me->section[++index].total_cnt;
            if (++me->section[index].allocate_cnt > me->section[index].max_cnt)
            {
                /* Remember the Maximum so far */
                me->section[index].max_cnt = me->section[index].allocate_cnt;
            }
            INT_UNLOCK_();
            /* The Section Index is in "index" */
        }
        else {   /* Not Find It */
            INT_UNLOCK_();
            free_block = (free_hblock_t *)0;     /* No Free Block Available */
            index = -1;            /* Set The Section Index to Invalid Value */
        }
    }
    /***********************************************************************************
    *   If Not Find one Free Block, the Section Index Below is -1,
    *   If Find one Free Block or Move one Block for Larger Size Section, the Section
    *   Index is Correct.
    ***********************************************************************************/
    if ((index != -1) && (free_block != (free_hblock_t *)0)) { 
        SPYER_HPOOL("Get Block %X from Section %d in HPool %X, Total %d, Free %d, Allocated %d, Max %d. TimeStamp %d", \\ 
                    free_block, index, me, \\ 
                    me->section[index].total_cnt, \\ 
                    me->section[index].free_cnt, \\ 
                    me->section[index].allocate_cnt, \\ 
                    me->section[index].max_cnt, \\
                    ticks_get()); 

    /* Backup the Section that the Block Get from */
    *((uint32_t *)free_block) = (uint32_t)index;    /* 32 Bits Address */
    /* Return the Block or NULL Pointer to the Caller */
    return (void_t *)(++free_block);
}

/***************************************************************************************
*   hpool_put() Implementation.
***************************************************************************************/
int16_t hpool_put(void_t *block)
{
    hpool_t * me; 
    int8_t    section; 

    ASSERT_REQUIRE(block != (void_t *)0); 
    if (block == (void_t *)0) { 
        return FAILURE; 
    } 

    /* Get the Section that the Block Get from */
    ((uint32_t *)block)--;      /* 32 Bits Address */
    section = (int8_t)(*((uint32_t *)block)); 

    me = (hpool_t *)&hpool; 

    ASSERT_REQUIRE(section <= me->max_index);    /* Must be in Range */
    if (section > me->max_index) { 
        return FAILURE; 
    }

    ASSERT_REQUIRE((me->start <= block) && (block < me->end));    /* Must be in Range */
    if ((me->start > block) && (block >= me->end)) { 
        return FAILURE; 
    }

    INT_LOCK_KEY_
    INT_LOCK_();

    /* Free Blocks must be < total_cnt */
    if(me->section[section].free_cnt >= me->section[section].total_cnt)) {
        INT_UNLOCK_();
        ASSERT_ERROR();
        return FAILURE; 
    }
    /* Link into Free List */
    ((free_hblock_t *)block)->next = (free_hblock_t *)me->section[section].free;
    me->section[section].free = block;      /* Set as New Head of the Free List */
    ++me->section[section].free_cnt;        /* One more Free Block in this Pool */
    --me->section[section].allocate_cnt;    /* Reduce the Allocated Number of Blocks */
    INT_UNLOCK_();

    SPYER_HPOOL("Put Block %X into Section %d in HPool %X, Free %d, Allocated %d. TimeStamp %d", \\ 
                block, section, me, \\ 
                me->section[section].free_cnt, \\ 
                me->section[section].allocate_cnt, \\ 
                ticks_get()); 
    return TRUE; 
}

#ifdef SPYER_HPOOL_ENABLE /* Spy Debuger Enabled */
/***************************************************************************************
*   hpool_snap() Implementation.
***************************************************************************************/
int16_t hpool_snap(void_t)
{
    hpool_t * me; 
    int8_t  index;
    struct {
        uint32_t block_size;
        uint32_t total_cnt;
        uint32_t max_cnt;
    } record_snap[HPOOL_AMOUNT];

    me = (hpool_t *)&hpool; 

    INT_LOCK_KEY_
    INT_LOCK_();
    /* Get the Block Usage Snap View of All Section, MUST be Operated at one Time */
    for(index = 0; index <= me->max_index; ++index) {
        record_snap[index].block_size = me->section[index].block_size;
        record_snap[index].total_cnt  = me->section[index].total_cnt;
        record_snap[index].max_cnt    = me->section[index].max_cnt;
    }
    INT_UNLOCK_();
    /* Spyer it Output */
    for(index = 0; index <= me->max_index; ++index) {
        SPYER_HPOOL("Snap Section %d in HPool %X (Maximum %d), Block Size %d, Total %d, Max %d. TimeStamp %d", \\ 
                     index, me, me->max_index, \\ 
                     record_snap[index].block_size, \\
                     me->section[index].total_cnt, \\ 
                     me->section[index].max_cnt, \\ 
                     ticks_get()); 
    }
}
#endif  /* End of SPYER_HPOOL_ENABLE */
