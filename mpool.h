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
*   File Name : mpool.h
*   Version : 1.0
*   Description : Base for Memory Pool 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-16-2005      Created Initial Version 1.0
*       JinHui Han      02-16-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _MPOOL_H
#define _MPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define MPOOL_EXT
#else
    #define MPOOL_EXT   extern
#endif

/***************************************************************************************
*   Native Memory Pool
*
*   This Structure Describes the Native Memory Pool, which can be Used as the Event
*   Pool for Dynamic Event Allocation, or As a Fast, Deterministic Fixed Block-Size
*   Heap for Any Other Objects in Your Application.
*
*   The mpool_t Structure Contains Only Data Members for Managing a Memory Pool, 
*   but Does not Contain the Pool Storage, which must be Provided Externally during
*   the Pool Initialization. 
*
*   Note: 
*   () The Storage Space for Memory Block and Memory Pool Itself MUST be Allocated
*      Statically, So the Transferred Memory Block or Event to Other Active Object
*      Can be Release Accurately when The Active Object which Own this Memory Pool
*      or Event Pool has been Abolished (This is, the Active Object is Exits).
***************************************************************************************/
typedef struct mpool_tag
{
    void_t * start;     /* Start Block of the Memory Managed by this Memory Pool */
    void_t * end;       /* End   Block of the Memory Managed by this Memory Pool */
    void_t * free;      /* The Head of the Linked List of Free Blocks */
    uint16_t block_size;    /* Block Size (in Bytes) */
    uint16_t total_cnt;     /* Total Number of Blocks */
    uint16_t free_cnt;      /* Number of Free Blocks Remaining */
    /***********************************************************************************
    *   Minimum Number of Free Blocks ever Present in this Pool.
    *
    *   NOTE: This Attribute Remembers the Low Water-Mark of the Pool, which Provides
    *   a Valuable Information for Sizing Memory Pools.
    ***********************************************************************************/
    uint16_t min_cnt;
} mpool_t;

/***************************************************************************************
*   Structure Representing a Free Block in the Native Memory Pool
***************************************************************************************/
typedef struct free_mblock_tag
{
    struct free_mblock_tag *next;
}free_mblock_t;

/***************************************************************************************
*   Initializes the Native Memory Pool
*
*   Description: Initializes the Native Memory Pool. 
*   Argument: me -- Pointer the mpool_t Struct to be Initialized
*             begin -- The Begin Address of Pool Storage. 
*             size  -- The Size of the Pool Storage in Bytes. 
*             block_size -- The Block Size of This Pool in Bytes. 
*                           Which Determines the Maximum Size of Block that Can 
*                           be Allocated from the Pool.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   The Caller of This Method must Make Sure that The "begin" Pointer is Properly
*   aligned. In Particular, it must be Possible to Efficiently Store a Pointer at the
*   Location Pointed to by "begin".
*   Illustrates Described as Below (Such as 4-Bytes Pointer):
*
*               0x000000  ----------------
*                         |   void_t *   |
*               0x000004  ----------------
*                         |   void_t *   |
*               0x000008  ----------------
*                         |   void_t *   |
*               0x00000C  ----------------
*                         |   void_t *   |
*               0x000010  ----------------
*                         |   ...  ...   |
*
*                         |   ...  ...   |
*               0x100000  ----------------
*                         |   void_t *   |
*               0x100004  ----------------
*                         |   ...  ...   |
*
*   Internally, the mpool_init() Function Rounds Up the "block_size" so that it can 
*   Fit an Integer Number of Pointers. 
*   When "block_size" is 0, The Actual Size of Block is sizeof(free_mblock_t).
*   This is Done to Achieve Proper Alignment of the Blocks within the Pool.
*
*   NOTE: (1) Due to the Rounding of Block Size (Blocks Alignment) and the Reserved 
*             Storage Location (sizeof (void_t *) for Remeber the Memory Pool 
*             which Block is Allocated from), the Actual Number of Blocks Available 
*             in the Memory Pool might be Actually Less Than ("size" /"block_size"). 
*             Always, You can Check the Capacity of the Memory Pool by Calling the 
*             mpool_margin() Function.
*         (2) For "block_size", The Actual Size Requirement of Memory is, 
*             ("block_size" + sizeof(void_t *))*(Blocks Amount you Need). 
*         (3) The Whole Pool Buffer must Fit at least One Block. 
*         (4) Must be Called Exactly once before the Memory Pool can be Used. 
***************************************************************************************/
MPOOL_EXT int16_t mpool_init(mpool_t *me, void_t *begin, uint32_t size, uint16_t block_size);

/***************************************************************************************
*   Obtains a Memory Block from a Memory Pool.
*
*   Description: Obtains a Memory Block from a Memory Pool. 
*   Argument: me -- Pointer to the mpool_t from which the Block is Requested. 
*   Return: Returns a Pointer to the Allocated Memory Block or NULL if no Free 
*           Blocks are Available.
*
*   NOTE: 
*   (1) The Memory Pool "me" must be Initialized before Any Blockss can be Requested 
*       from it. 
*   (2) When the Return Value of this Function is NULL, You Must Product one Assert 
*       Error for it, So You Can Detect this Error by Sper at Debug Case. To Resolve 
*       this Error, Increment the Memory-Space Managed by the Pool.
*   (3) This Function can be Called from Any Task Level or ISR Level.
*   (4) The mpool_get() Function Uses Internally a Critical Section, so You should 
*       be Careful not to Call it within a Critical Section when Nesting of 
*       Critical Section is not Supported.
*   (5) The Obtained Block must be Put Back to the Same Memory Pool which it is 
*       Allocated from. The Originated Memory Pool Information is Stored in the 
*       Returned Pointer Minus sizeof(void_t *). 
***************************************************************************************/
MPOOL_EXT void_t *mpool_get(mpool_t *me);

/***************************************************************************************
*   Put a Memory Block Back to a Memory Pool.
*
*   Description: Put a Memory Block Back to a Memory Pool. 
*   Argument: block -- The Pointer to the Memory Block which will be Put Back. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) The mpool_put() Function Raises an Assertion if the Put Back Pointer Outside 
*       of the Original Memory Buffer Managed by the Memory Pool. 
*   (2) This Function can be Called from Any Task Level or ISR Level.
*   (3) The mpool_put() Function Uses Internally a Critical Section, so You should 
*       be Careful not to Call it from within a Critical Section when Nesting of 
*       Critical Section is not Supported.
***************************************************************************************/
MPOOL_EXT int16_t mpool_put(void_t *block);

/***************************************************************************************
*   All Block are Recycled for the Given Memory Pool ? 
*
*   Description:  All Block are Recycled for the Given Memory Pool ? 
*   Argument: me -- Pointer to the mpool_t Struct to be Inquired. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
MPOOL_EXT int16_t mpool_all_recycled(mpool_t *me); 

/***************************************************************************************
*   Get the Margin of the Given Memory Pool. 
*
*   Description: Get the Margin of the Given Memory Pool. 
*   Argument: me -- Pointer to the mpool_t Struct to be Inquired. 
*   Return: The Margin of the Given Memory Pool. If 0 is Return, FAILURE is Expressive. 
*
*   NOTE: 
*   (1) The Margin of Pool is the Minimal Number of Free Blocks Encountered in the 
*       Given Pool since System Startup. 
*   (2) The System Designer must Keep Proper Margin for the Given Memory Pool. 
*       So, If 0 is Return, FAILURE is Expressive. 
*   (3) The mpool_margin() Function Uses Internally a Critical Section, so You should
*       be Careful not to Call it from within a Critical Section when Nesting of 
*       Critical Section is not Supported.
***************************************************************************************/
MPOOL_EXT uint16_t mpool_margin(mpool_t *me); 

#ifdef SPYER_MPOOL_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_MPOOL(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_MPOOL(format, ...)   ((void_t)0)
#endif  /* End of SPYER_MPOOL_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _MPOOL_H */

