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
*   Group : Non-Preemptive Scheduler
*   File Name : scheduler.h
*   Version : 1.0
*   Description : Base for Non-Preemptive Scheduler Set
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-24-2005      Created Initial Version 1.0
*       JinHui Han      03-24-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _SCHE_H
#define _SCHE_H

#ifdef  EXTERN_GLOBALS
    #define SCHE_EXT
#else
    #define SCHE_EXT    extern
#endif

#if (MAX_ACTIVE <= 8)
    #define SCHE_BITSET_8
#endif

/***************************************************************************************
*   Priority Set Useful for Building Non-Preemptive Schedulers
*
*   The Priority Set Represents the Set of Active Object that are Ready to Run and 
*   Need to be Considered by Scheduling Processing. 
***************************************************************************************/
#ifdef (SCHE_BITSET_8)

    /***********************************************************************************
    *   The set is Capable of Storing up to 8 Priority Levels.
    *   The Priority Set Allows to Build Cooperative Multitasking Schedulers to Manage 
    *   Up to 8 Tasks.
    ***********************************************************************************/
    typedef struct set_tag {
        uint8_t     bits;   /* Bitmask Representing Elements of the Set */
    } set_t;

    /***********************************************************************************
    *   The Macro Initialize the Priority Set "me_"
    ***********************************************************************************/
    #define set_init(me_)           (((set_t *)me_)->bits = (uint8_t)0)

    /***********************************************************************************
    *   The Macro Evaluates to TRUE if the Priority Set "me_" is Empty
    ***********************************************************************************/
    #define set_is_empty(me_)       (((set_t *)me_)->bits == (uint8_t)0)

    /***********************************************************************************
    *   The Macro Evaluates to TRUE if the Priority Set "me_" has Elements
    ***********************************************************************************/
    #define set_has_elements(me_)   (!set_is_empty(me_))

    /***********************************************************************************
    *   The Macro Evaluates to TRUE if the Priority Set "me_" has Element "n_".
    ***********************************************************************************/
    #define set_has_element(me_, n_)    \
            ((((set_t *)me_)->bits & ROM_BYTE(&lookup_bit_or[n_])) != 0)

    /***********************************************************************************
    *   Insert Element "n_" into the Set "me_", n_ = 1..8
    ***********************************************************************************/
    #define set_insert(me_, n_)     \
            (((set_t *)me_)->bits |= ROM_BYTE(&lookup_bit_or[n_]))

    /***********************************************************************************
    *   Remove Element n_ from the Set "me_", n_ = 1..8
    ***********************************************************************************/
    #define set_remove(me_, n_)     \
            (((set_t *)me_)->bits &= ROM_BYTE(&lookup_bit_and[n_]))

    /***********************************************************************************
    *   Find the Maximum Element in the Set "me_", and Assign it to n_,
    *   NOTE: The Set cannot be Empty
    ***********************************************************************************/
    #define set_find_max(me_, n_)   \
            ((n_) = ROM_BYTE(&lookup_unmap[((set_t *)me_)->bits]))

#else

    /***********************************************************************************
    *   The Set is Capable of Storing up to 64 Priority Levels.
    *
    *   The Priority Set Allows to Build Cooperative Multitasking Schedulers to Manage 
    *   Up to 64 Tasks. 
    ***********************************************************************************/
    typedef struct set_tag { 
        /*******************************************************************************
        *   Condensed Representation of the Priority Set
        *
        *   Each Bit in the Bytes Attribute Represents a Byte (8-Bits) in the bits[] 
        *   Array.
        *   More Specifically:
        *       bit 0 in bytes is 1 when any bit in bits[0] is 1
        *       bit 1 in bytes is 1 when any bit in bits[1] is 1
        *       bit 2 in bytes is 1 when any bit in bits[2] is 1
        *       bit 3 in bytes is 1 when any bit in bits[3] is 1
        *       bit 4 in bytes is 1 when any bit in bits[4] is 1
        *       bit 5 in bytes is 1 when any bit in bits[5] is 1
        *       bit 6 in bytes is 1 when any bit in bits[6] is 1
        *       bit 7 in bytes is 1 when any bit in bits[7] is 1
        *******************************************************************************/
        uint8_t     bytes;

        /*******************************************************************************
        *   Bitmasks Representing Elements in the Set. 
        *   Specifically: 
        *       bits[0] represent elements  0..7
        *       bits[1] represent elements  8..15
        *       bits[2] represent elements 16..23
        *       bits[3] represent elements 24..31
        *       bits[4] represent elements 32..39
        *       bits[5] represent elements 40..47
        *       bits[6] represent elements 48..55
        *       bits[7] represent elements 56..63
        *******************************************************************************/
        uint8_t     bits[8];
    } set_t;

    /***********************************************************************************
    *   The Macro Initialize the Priority Set "me_"
    ***********************************************************************************/
    #define set_init(me_)   \
        do  \
        {   \
            ((set_t *)me_)->bytes   = (uint8_t)0;   \
            ((set_t *)me_)->bits[0] = (uint8_t)0;   \
            ((set_t *)me_)->bits[1] = (uint8_t)0;   \
            ((set_t *)me_)->bits[2] = (uint8_t)0;   \
            ((set_t *)me_)->bits[3] = (uint8_t)0;   \
            ((set_t *)me_)->bits[4] = (uint8_t)0;   \
            ((set_t *)me_)->bits[5] = (uint8_t)0;   \
            ((set_t *)me_)->bits[6] = (uint8_t)0;   \
            ((set_t *)me_)->bits[7] = (uint8_t)0;   \
        }while(0)

    /***********************************************************************************
    *   The Macro Evaluates to TRUE if the Priority Set "me_" is Empty
    ***********************************************************************************/
    #define set_is_empty(me_)       (((set_t *)me_)->bytes == (uint8_t)0)

    /***********************************************************************************
    *   The Macro Evaluates to TRUE if the Priority Set "me_" has Elements
    ***********************************************************************************/
    #define set_has_elements(me_)   (!set_is_empty(me_))

    /***********************************************************************************
    *   The Macro Evaluates to TRUE if the Priority Set "me_" has Element "n_".
    ***********************************************************************************/
    #define set_has_element(me_, n_)    \
    ((((set_t *)me_)->bits[ROM_BYTE(&lookup_div8[n_])] & ROM_BYTE(&lookup_bit_or[n_])) != 0)

    /***********************************************************************************
    *   Insert Element "n_" into the Set "me_", n_ = 0..63
    ***********************************************************************************/
    #define set_insert(me_, n_)     \
    do  \
    {   \
        ((set_t *)me_)->bits[ROM_BYTE(&lookup_div8[n_])] |= ROM_BYTE(&lookup_bit_or[n_]);   \
        ((set_t *)me_)->bytes |= ROM_BYTE(&lookup_bit_or[ROM_BYTE(&lookup_div8[n_])]);      \
    } while(0)

    /***********************************************************************************
    *   Remove Element n_ from the Set "me_", n_ = 0..63
    ***********************************************************************************/
    #define set_remove(me_, n_)     \
    do  \
    {   \
        ((set_t *)me_)->bits[ROM_BYTE(&lookup_div8[n_])] &= ROM_BYTE(&lookup_bit_and[n_]);  \
        if (((set_t *)me_)->bits[ROM_BYTE(&lookup_div8[n_])] == (uint8_t)0)      \
        {   \
            ((set_t *)me_)->bytes &= ROM_BYTE(&lookup_bit_and[ROM_BYTE(&lookup_div8[n_])]); \
        }   \
    } while(0)

    /***********************************************************************************
    *   Find the Maximum Element in the Set, and Assign it to n_, 
    *   NOTE: The Set cannot be Empty
    ***********************************************************************************/
    #define set_find_max(me_, n_)   \
    do  \
    {   \
        (n_) = (uint8_t)(ROM_BYTE(&lookup_unmap[((set_t *)me_)->bytes]));    \
        (n_) = (uint8_t)(((n_)<<3) + ROM_BYTE(&lookup_unmap[((set_t *)me_)->bits[n_]]));    \
    } while(0)

#endif      /* End of SCHE_BITSET_8 */

/***************************************************************************************
*   Ready Set of Active Objects.
***************************************************************************************/
SCHE_EXT set_t volatile os_ready_set_;

#endif      /* End of _SCHE_H */
