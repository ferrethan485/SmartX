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
*   Group :  Variable Size Block Heap
*   File Name : hpool.h
*   Version : 1.0
*   Description : Base for Variable Size Block Heap (ONLY Used in Object Internal)
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-08-2005      Created Initial Version 1.0
*       JinHui Han      03-08-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _HPOOL_H
#define _HPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define HPOOL_EXT
#else
    #define HPOOL_EXT   extern
#endif

/***************************************************************************************
*   Section for Variable Size Block Heap.
*
*   The hpool_section_t Structure Contains Only Data Members for Managing a Pool 
*   Section, but Does not Contain the Pool Section's Storage.
***************************************************************************************/
typedef struct hpool_section_tag { 
    void_t * free;        /* The Head of the Linked List of Free Blocks */
    uint32_t block_size;  /* Block Size (in Bytes) */
    uint32_t total_cnt;   /* Total Number of Blocks Currently in This Section */
    uint32_t free_cnt;    /* Number of Free  Blocks Currently in This Section */
    /* Number of Allocated Blocks Currently in This Section */
    uint32_t allocate_cnt;
    /***********************************************************************************
    *   Maximum Number of Allocated Blocks ever Present in this Section.
    *
    *   NOTE: This Attribute Remembers the High Water-Mark of the Amount of Allocated 
    *   Blocks, which Provides a Valuable Information for Sizing Memory Space of Pools
    *   and the Amount of Section (This is For Improve the Speed Performance of Search
    *   in This Manager).
    ***********************************************************************************/
    uint32_t max_cnt;
} hpool_section_t;

#ifndef  HPOOL_FACTOR
    /***********************************************************************************
    *   This Constant Define the Scale Factor for Block-Size. 
    *
    *   The Units of Block-Size is: HPOOL_FACTOR * (sizeof(free_hblock_t *)).
    *
    *   The Units of Block-Size can be The Discretional Multiple of (void_t *) for 
    *   the Requirement of Application. You Should Select this Multiple with Various 
    *   Block-Size Requirement of Your Application, So You can Reduce the Waste of 
    *   Memory Space.
    *   NOTE: For 8-Bits MCU, The Multiple Must Be the Power of 2, The Speed of Code 
    *         Execute can be Quicken.
    ***********************************************************************************/
    #define HPOOL_FACTOR  (1L)
#endif

#ifndef  HPOOL_AMOUNT
    /***********************************************************************************
    *   Macro to Override the Default Amount of Memory Pool's Section Array.
    *
    *   Default is 10. The Largest Size of Block is 
    *   2^(10-1) * (HPOOL_FACTOR * sizeof(free_hblock_t *)). When HPOOL_FACTOR is 1, 
    *   For 8-Bit MCU, It is 1KB, For 32-Bit MCU, It is 2KB.
    *
    *   For Save the Memory, You Should Reduce this Constant Value.
    *   For the Purpose which Need Fixed-Size Large Block and Variable-Size Small
    *   Block Synchronously, You Should Manage the Variable-Size Small Block using this 
    *   hpool_t, and Manage the Fixed-Size Large Block Using the mpool_t.
    *
    *   NOTE: The Units of be Managed Block is HPOOL_FACTOR * (sizeof(free_hblock_t *)). 
    *         The Formula for MPOOL_AMOUNT is: 
    *         log2((Max-Size of Block)/(HPOOL_FACTOR * (sizeof(free_hblock_t *))))+1.
    ***********************************************************************************/
    #define HPOOL_AMOUNT    10
#endif

#if (HPOOL_AMOUNT > 32)
    #error "HPOOL_AMOUNT Defined Incorrectly, Expected 1 to 32"
#endif

/***************************************************************************************
*   Memory Pool for Variable Size Block Heap.
*
*   This Structure Describes the Native Memory Pool, which can be Used as the Block
*   Pool for Dynamic Block Allocation, or As a Fast, Deterministic Variable Size Block
*   Heap for Any Other Objects in Your Application.
*
*   The hpool_t Structure Contains Only Data Members for Managing a Memory Pool, 
*   but Does not Contain the Pool Storage, which must be Provided Externally during
*   the Pool Initialization. 
*
*   Variable Size Block Heap is Defined in Global Range, But Dynamic Block Allocate 
*   and Release Operation can ONLY Used in Every Object Internal. 
***************************************************************************************/
typedef struct hpool_tag
{
    void_t *  start;      /* Start Address of the Memory Managed by this Memory Pool */
    void_t *  end;        /* End   Address of the Memory Managed by this Memory Pool */
    int8_t    max_index;  /* Maximum Index of Section Array, It is Determine by the */
                          /* Maximum Block Size when Initialize */
    hpool_section_t     section[HPOOL_AMOUNT];  /* Subsection Array of Pool */
} hpool_t;

/***************************************************************************************
*   Structure Representing a Free Block in the Native Memory Pool
***************************************************************************************/
typedef struct free_hblock_tag free_hblock_t;

typedef struct free_hblock_tag
{
    free_hblock_t *next;
};

/***************************************************************************************
*   Initializes the Native Variable Size Block Heap.
*
*   Description: Initializes the Native Variable Size Block Heap. 
*   Argument: begin -- The Begin Address of Block Heap Storage. 
*             size  -- The Size of the Block Heap Storage in Bytes. 
*             max_block_size -- The Maximum Size of Block that can be Allocated, 
*                               It MUST be Less Than 2^31 = 2147483648. 
*                               When 0 is Specified, It will Compute the Biggest 
*                               Possible Size Base on "size" Internally. 
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
*   Internally, the hpool_init() Function Rounds Up the Size of Block into 
*   (2^"Index of Section")*(HPOOL_FACTOR * (sizeof(free_hblock_t *))).
*
*   NOTE: (1) Due to the Rounds Up of Block Size, the Actual Total Capacity of the 
*             Pool might be Less than "size", But the Losed Capacity is Small than 
*             The Size of One Pointer.
*         (2) Due to the Rounding of Maximum Block Size, the Actual Capacity of the 
*             Maximum Size Section of Pool might be Less than 
*             ("size" / "max_block_size"). 
*         (3) The Whole Pool Buffer must Fit at least One Block which Size is 
*             (2^"Maximum Index of Section")*(HPOOL_FACTOR * (sizeof(free_hblock_t *))).
*         (4) The Maximum Size of Block is Calculate Using "Maximum Index of Section" 
*             Internally.
*         (5) The "max_block_size" MUST Accord With the Formula:
*             (max_block_size-1) / (HPOOL_FACTOR * sizeof(free_hblock_t *)) < 32768
***************************************************************************************/
HPOOL_EXT int16_t hpool_init(void_t *begin, uint32_t size, uint32_t max_block_size);

/***************************************************************************************
*   Obtains a Block from a Variable Size Block Heap.
*
*   Description: Obtains a Block from a Variable Size Block Heap.
*   Argument: block_size -- The Block-Size of Requested Block. 
*   Return: Pointer to the Allocated Block or NULL if no Free Blocks are Available.
*
*   NOTE: 
*   (1) A Allocated Block must be Returned to the Same Subsection from which 
*       it has been Allocated.
*   (2) This Function can be Called from Any Task Level or ISR Level.
*   (3) The Variable Size Block Heap must be Initialized before Any Block can be 
*       Requested from it. 
*   (4) The hpool_get() Function Uses Internally a Critical Section, so You should
*       be Careful not to Call it from within a Critical Section when Nesting
*       of Critical Section is not Supported.
*   (5) The "block_size" MUST Accord With the Formula:
*       (block_size-1) / (HPOOL_FACTOR * sizeof(free_hblock_t *)) < 32768
*   (6) When the Return Value of this Function is NULL, You Must Product one Assert
*       Error for it, So You Can Detect this Error by Spyer at Debug Case.
*       To Resolve this Error, Increment the Memory-Space Managed by the Pool.
***************************************************************************************/
HPOOL_EXT void_t *hpool_get(uint16_t block_size);

/***************************************************************************************
*   Put a Block Back to a Variable-Size Block Heap.
*
*   Description: Put a Block Back to a Variable-Size Block Heap.
*   Argument: block -- Pointer to the Memory Block will be Put Back.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) This Function can be Called from Any Task Level or ISR Level.
*   (2) The Block must be Put Back to the Same Memory SubSection to which
*       it is Allocated. 
*   (3) The hpool_put() Function Raises an Assertion if the Put Back Pointer Outside 
*       of the Original Memory Buffer Managed by the Memory Pool. 
*   (4) The hpool_put() Function Uses Internally a Critical Section, so You should
*       be Careful not to Call it from within a Critical Section when Nesting of
*       Critical Section is not Supported.
***************************************************************************************/
HPOOL_EXT int16_t hpool_put(void_t *block);

/***************************************************************************************
*   Spy the Blocks Usage Snap View of the Given Heap.
*
*   Description: Spy the Blocks Usage Snap View of the Given Heap.
*   Argument: none.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   You can Account the Memory Requirement of This Heap Using the Block Distributing
*   Form which Spy Returned.
*
*   NOTE: (1) The hpool_snap() Uses Internally a Critical Section, so 
*             You should be Careful not to Call it from within a Critical Section 
*             when Nesting of Critical Section is not Supported.
*         (2) Stack Space Requirement of This Function' Local Variable is 
*             ((sizeof(uint16_t) * 3) * MPOOL_AMOUNT + 1) Bytes
***************************************************************************************/
HPOOL_EXT int16_t hpool_snap(void_t);

#ifdef SPYER_HPOOL_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_HPOOL(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_HPOOL(format, ...)   ((void_t)0)
#endif  /* End of SPYER_HPOOL_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _HPOOL_H */

