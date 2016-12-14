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
*   File Name : debug.h                 
*   Version : 1.0
*   Description : Debugger Serial Port Driver used for Assert and Spyer.
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-14-2005      Created Initial Version 1.0
*       JinHui Han      02-14-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define DEBUG_EXT
#else
    #define DEBUG_EXT    extern
#endif

#ifndef DEBUG_BUFFER_SIZE
    /***********************************************************************************
    *   This Constant Defines the Size of the Debug Buffer in Bytes. 
    *   Currently the Size of the Debug Buffer cannot Exceed 64KB.
    *   If the Debug Buffer is too Small for the Bursts of Logging Activity, You will 
    *   Start Losing Data.
    *   The Right Size of the Buffer Depends on the Data Production Rate and the Data 
    *   Output Rate. 
    ***********************************************************************************/
    #define DEBUG_BUFFER_SIZE     4*1024      /* Default is 4KB */
#endif      /* End of DEBUG_BUFFER_SIZE */

/***************************************************************************************
*   Buffer for Debug Serial Port
***************************************************************************************/
DEBUG_EXT uint8_t debug_buffer[DEBUG_BUFFER_SIZE];

/***************************************************************************************
*   Vairables for Manager Debug Ring Buffer
***************************************************************************************/
DEBUG_EXT uint16_t debug_buff_size_;   /* The Size of Ring Buffer */
DEBUG_EXT uint16_t debug_buff_head_;   /* Offset to where Next Byte will be Inserted   */
DEBUG_EXT uint16_t debug_buff_tail_;   /* Offset of where Next Byte will be Extracted  */
DEBUG_EXT uint16_t debug_buff_used_;   /* Number of Bytes Currently in the Ring Buffer */

/***************************************************************************************
*   Initialize the Debug Facility.
*
*   Description: Initialize (Open and Configure) the Debugger Serial Port and the 
*                Debug Buffer. 
*   Argument: none
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
DEBUG_EXT int16_t debug_init(void_t);

/***************************************************************************************
*   Query the Empty Status of Debug Buffer. 
*
*   Description: Query the Empty Status of Debug Buffer. Debug Buffer Empty Imply
*                that the Debug Trace Data has been Flush to the Host. 
*   Argument: none
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
DEBUG_EXT int16_t debug_is_empty(void_t);

/***************************************************************************************
*   Put One Strings into Debug Buffer. 
*
*   Description: Put One Strings into Debug Buffer. 
*   Argument: string -- The Pointer of String Which will be Put into Debug Buffer. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) If the Data Output Rate cannot Keep Up With the Data Production Rate, 
*       The Debug Buffer will Start to be Overwriting the Older Data with Newer Data. 
*       This is Consistent with the "Last-is-Best" Debug Policy. The Displays in 
*       Host's Serial Terminal will Easy to Judge Out the Data Loss. 
*   (2) debug_puts() is Protected with a Critical Section.
***************************************************************************************/
DEBUG_EXT int16_t debug_puts(int8_t * string);

/***************************************************************************************
*   Byte-Oriented Interface to the Debug Buffer.
*
*   Description: This Function Delivers One Byte at a Time from the Debug Buffer.
*   Argument: none
*   Return: The Function Returns the Byte in the Least-Significant 8-Bits of the 
*           16-Bit Return Value if the Byte is Available. 
*           If No More Data is Available at that Time, the Function Returns 
*           DEBUG_EOD (End-Of-Data).
*   NOTE: debug_get_byte() is NOT Protected with a Critical Section, So It Can ONLY 
*         be Used in Interrupt Service Routine. 
***************************************************************************************/
#define DEBUG_EOD     ((uint16_t)0xFFFF)

DEBUG_EXT uint16_t debug_get_byte(void_t);

/***************************************************************************************
*   Block-Oriented Interface to the Debug Buffer.
*
*   Description: This Function Delivers a Contiguous Block of Data from the Debug 
*                Buffer.
*   Argument: byte_count -- 
*   Return: The Function Returns the Pointer to the Beginning of the Block, and 
*           Writes the Number of Bytes in the Block to the Location Pointed to 
*           by "byte_count".
*           The Argument "byte_count" is Also Used as Input to Provide the Maximum 
*           Size of the Data Block that the Caller can Accept.
*           If no Bytes are Available in the Debug Buffer when the Function is Called,
*           the Function Returns a NULL Pointer and Sets the Value Pointed to by 
*           "byte_count" to Zero.
*
*   NOTE: 
*   (1) Only the NULL Return from debug_get_block() Indicates that the Debug Buffer 
*       is Empty at the Time of the Call. The non-NULL Return Often Means that the 
*       Block is at the End of the Buffer and You need to Call debug_get_block() 
*       Again to Obtain the Rest of the Data that "Wrapped Around" to the Beginning 
*       of the Debug Buffer.
*   (2) debug_get_block() is NOT Protected with a Critical Section, So It Can ONLY 
*       be Used in Interrupt Service Routine. 
***************************************************************************************/
DEBUG_EXT uint8_t const *debug_get_block(uint16_t *byte_count);

/***************************************************************************************
*   Interrupt Service Routine Implementation for Debug Serial Port.
***************************************************************************************/
DEBUG_EXT

#ifdef __cplusplus
}
#endif

#endif      /* End of _DEBUG_H */

