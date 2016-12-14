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
*   File Name : cpool.h
*   Version : 1.0
*   Description : Base for Chain Pool
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-20-2005      Created Initial Version 1.0
*       JinHui Han      03-20-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _CPOOL_H
#define _CPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define CPOOL_EXT
#else
    #define CPOOL_EXT     extern
#endif

/***************************************************************************************
*   Chain List Struct.
***************************************************************************************/
typedef struct chain_tag {
    struct chain_tag *next;    /* Next     Location of Chain List */
    struct chain_tag *fore;    /* Previous Location of Chain List */
    void_t *builtin;           /* Built-In Object */
}chain_t;

/***************************************************************************************
*   Memory Pool for Chain List.
***************************************************************************************/
#ifndef  CPOOL_AMOUNT
    /***********************************************************************************
    *   Macro to Override the Default Amount of Chain Pool Array.
    *
    *   Default is 256. The Size of Memory Pool is 
    *   (sizeof(chain_t) + sizeof(void_t *)) * (CPOOL_AMOUNT + 1) 
    *   NOTE: 
    *   (1) The Chain Pool Array is Used in the Range of Priority Based Event Chain 
    *       and Enroll Book, So the Value of CPOOL_AMOUNT must Meet Their 
    *       Requirements. 
    ***********************************************************************************/
    #define CPOOL_AMOUNT    256
#endif

/*  Buffer of Chain Pool */
#define SIZE_BUFF_CPOOL    (sizeof(chain_t) + sizeof(void_t *)) * (CPOOL_AMOUNT + 1)
CPOOL_EXT uint8_t buff_cpool [SIZE_BUFF_CPOOL]; 
/*  Memory Pool */
CPOOL_EXT mpool_t cpool;

/***************************************************************************************
*   Initializes the Chain Pool
*
*   Description: Initializes the Chain Pool. 
*   Argument: none. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
CPOOL_EXT int16_t cpool_init(void_t); 

/***************************************************************************************
*   Get Chain Cell from Chain Pool. 
*
*   Description: Get Chain Cell from Chain Pool. 
*   Argument: none.
*   Return: The Pointer to Chain Cell, or else Return NULL if Have Not Chain Cell 
*           in that Chain Pool or no Free Blocks are Available.
*
*   NOTE: 
***************************************************************************************/
CPOOL_EXT chain_t *cpool_get(void_t);

/***************************************************************************************
*   Put Chain Cell into Chain Pool. 
*
*   Description: Put Chain Cell into Chain Pool. 
*   Argument: chain -- Pointer to the Chain Cell. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
CPOOL_EXT int16_t cpool_put(chain_t *chain);

/***************************************************************************************
*   Get the Margin of the Chain Pool.
*
*   Description: Get the Margin of the Chain Pool. 
*   Argument: none. 
*   Return: The Margin of the Chain Pool. If 0 is Return, FAILURE is Expressive. 
*
*   NOTE: 
*   (1) The Margin of Pool is the Minimal Number of Free Chain Cells Encountered 
*       in the Chain Pool since System Startup. 
*   (2) The System Designer must Keep Proper Margin for the Chain Pool. 
*       So, If 0 is Return, FAILURE is Expressive. 
***************************************************************************************/
CPOOL_EXT uint16_t cpool_margin(void_t); 

#ifdef SPYER_CPOOL_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_CPOOL(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_CPOOL(format, ...)   ((void_t)0)
#endif  /* End of SPYER_CPOOL_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _CPOOL_H */
