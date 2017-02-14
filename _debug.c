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
*   Group : Debugger Serial Port Driver
*   File Name : debug.c                 
*   Version : 1.0
*   Description : Debugger Serial Port Driver used for Assert and debuger.
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-14-2005      Created Initial Version 1.0
*       JinHui Han      02-14-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h"
#include "std_def.h"
#include "config.h"
#include "lock.h"

#define  EXTERN_GLOBALS 
#include "debug.h"

/***************************************************************************************
*   debug_init() Implementation.
***************************************************************************************/
int16_t debug_init(void_t)
{
    /* Initialize the Debug Buffer */
    debug_buff_size_ = (uint16_t)DEBUG_BUFFER_SIZE;     /* The Size of Ring Buffer */
    debug_buff_head_ = (uint16_t)0;          /* Offset to where Next Byte will be Inserted   */
    debug_buff_tail_ = (uint16_t)0;          /* Offset of where Next Byte will be Extracted  */
    debug_buff_used_ = (uint16_t)0;          /* Number of Bytes Currently in the Ring Buffer */
    /* Initialize the Debugger UART */

}

/***************************************************************************************
*   debug_is_empty() Implementation.
***************************************************************************************/
int16_t debug_is_empty(void_t)
{
    uint16_t used;

    INT_LOCK_KEY_

    INT_LOCK_();
    used = debug_buff_used_; 
    INT_UNLOCK_(); 
    if (used) { 
        return FALSE; 
    }
    return (TRUE); 
}

/***************************************************************************************
*   debug_puts() Implementation.
***************************************************************************************/
int16_t debug_puts(int8_t * string) 
{
    uint8_t byte; 

    INT_LOCK_KEY_

    INT_LOCK_();
    do {
        byte = (uint8_t)*string++; 
        debug_buffer[debug_buff_head_] = byte; 
        ++debug_buff_head_; 
        if (debug_buff_head_ == debug_buff_size_) { 
            debug_buff_head_ = (uint16_t)0; 
        } 
        ++debug_buff_used_; 
    } while (byte != (uint8_t)'\0'); 

    if (debug_buff_used_ > debug_buff_size_)
    {   /* Overrun over the Old Data ? */
        debug_buff_tail_ = debug_buff_head_;    /* Shift the Tail to the Old Data */
        debug_buff_used_ = debug_buff_size_;    /* The whole Buffer is Used */
    }
    INT_UNLOCK_(); 
    
    return TRUE; 
}

/***************************************************************************************
*   debug_get_byte() Implementation.
***************************************************************************************/
uint16_t debug_get_byte(void_t)
{
    uint8_t byte;

    if (debug_buff_used_ == (uint16_t)0)
    {
        return DEBUG_EOD;                     /* Return End-Of-Data */
    }
    byte = debug_buffer[debug_buff_tail_++];    /* Set the Byte to Return */
    if (debug_buff_tail_ == debug_buff_size_)   /* Tail Wrap Around ? */
    {
        debug_buff_tail_ = (uint16_t)0;
    }
    --debug_buff_used_;       /* One Less Byte Used */
    return (uint16_t)byte;  /* Return the byte */
}

/***************************************************************************************
*   debug_get_block() Implementation.
*   Get Up to *byte_count of Contiguous Memory
***************************************************************************************/
uint8_t const *debug_get_block(uint16_t *byte_count)
{
    uint16_t count;
    uint16_t tail;

    /* Is Empty ? */
    if (debug_buff_used_ == (uint16_t)0)
    {   /* No Bytes to Return Right Now */
        *byte_count = (uint16_t)0;
        return (uint8_t *)0;
    } 
    /* Truncation to the Maximum Size of Debug Buffer */
    count = (uint16_t)(debug_buff_size_ - debug_buff_tail_);
    if (count > debug_buff_used_)
    {
        count = debug_buff_used_; 
    }
    /* Limit to the Maximum Request Length */
    if (count > (uint16_t)(*byte_count))
    {
        count = (uint16_t)(*byte_count);
    }
    *byte_count = (uint16_t)count; 
    /* Update the Used Count of Debug Buffer */
    debug_buff_used_ = (uint16_t)(debug_buff_used_ - count); 
    /* Backup the Begin Position of Get Operation */
    tail = debug_buff_tail_; 
    /* Update the Tail of Debug Buffer */
    debug_buff_tail_ = (uint16_t)(debug_buff_tail_ + count);
    if (debug_buff_tail_ == debug_buff_size_)
    {
        debug_buff_tail_ = (uint16_t)0;
    }
    /* Return the Begin Pointer of Get Operation */
    return (uint8_t const *)&debug_buffer[tail];
}

/***************************************************************************************
*   Interrupt Service Routine Implementation for Debug Serial Port.
***************************************************************************************/

