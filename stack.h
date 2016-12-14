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
*   Group : Stack 
*   File Name : stack.h                 
*   Version : 1.0
*   Description : Base for Stack 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-17-2005      Created Initial Version 1.0
*       JinHui Han      02-17-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _STACK_H
#define _STACK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define STACK_EXT
#else
    #define STACK_EXT      extern
#endif

/***************************************************************************************
*   Platform-Independent Stack Interface.
*
*   This File is needed when the "raw" Stack are used for Native Context-Switch.
***************************************************************************************/

/***************************************************************************************
*   Native Stack.
*
*   This Structure Describes the Native Stack, which can be Used as a Simple "raw" 
*   Stack for Native Context-Switch.
*
*   The stack_t Structure Contains Only Data Members for Managing an Stack, but 
*   Does not Contain the Storage for the Buffer, which must be Provided Externally 
*   during the Stack Initialization.
*
*   The Stack can Store the (void_t *) Data Entity. 
*   The Whole Data Entity of one Struct Object can be DeCompose into Many (void_t *) 
*   Data Entity, Push Every uint32_t Data Entity Respectively, Later ReCompose the 
*   Struct Object with Every uint32_t Data Entity that Pop from Stack. For This
*   Case, The Size of Stack Buffer Must be the Integer Multiple of Struct Object
*   Entity. 
*   You can implement the Context Switch Using the Struct Object Pointer Also.  
*
*   The "raw" Stack is not Thread-Safe because it Not Uses Critical Section to 
*   Protect its Integrity. 
***************************************************************************************/
typedef struct stack_tag { 
    /*  Pointer to the Start of the Buffer */
    void_t **buffer; 
    /*  Total Number of Entity in the Buffer */
    uint16_t total; 
    /*  Offset to where Next Entity will be Inserted into the Buffer */
    uint16_t head;
    /***********************************************************************************
    *   Maximum Number of Entity Ever Present in the Buffer.
    *
    *   This Attribute Remembers the High-Watermark of the Buffer, which Provides
    *   a Valuable Information for Sizing Buffer.
    ***********************************************************************************/
    uint16_t max_cnt;
} stack_t;

/***************************************************************************************
*   Initializes the Native Stack
*
*   Description: Initializes the Native Stack. 
*   Argument: me -- Pointer the stack_t Struct to be Initialized. 
*             stack_buffer[] -- The Buffer Storage. 
*             stack_len -- The Length of the Buffer in the Units of (void_t *).
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
STACK_EXT int16_t stack_init(stack_t *me, void_t *stack_buffer[], uint16_t stack_len);

/***************************************************************************************
*   "raw" Stack Implementation for the Pushing.
*
*   Description: Push into Stack. 
*   Argument: me -- Pointer the stack_t Struct to Push Into. 
*             data -- The Data to be Pushing. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) The Function Raises an Assertion if the Native Stack becomes Full and Cannot 
*       Accept the Entity. 
***************************************************************************************/
STACK_EXT int16_t stack_push(stack_t *me, void_t *data);

/***************************************************************************************
*   "raw" Stack Implementation for the Poping.
*
*   Description: Pop  from Stack. 
*   Argument: me -- Pointer the stack_t Struct to Pop from. 
*             data -- Returns the Entity if True, Or Else Returns NULL (Stack is Empty or 
*                     Exception is Arise). 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
STACK_EXT int16_t stack_pop(stack_t *me, void_t **data); 

/***************************************************************************************
*   Get the Usage of the Given Stack.
*
*   Description: Get the Usage of the Given Stack. 
*   Argument: me -- Pointer the stack_t Struct which Usage is Get from. 
*   Return: The Usage of the Given Stack. 
*
*   NOTE: 
***************************************************************************************/
STACK_EXT uint16_t stack_usage(stack_t *me)£» 

/***************************************************************************************
*   Get the Margin of the Given Stack.
*
*   Description: Get the Margin of the Given Stack. 
*   Argument: me -- Pointer the stack_t Struct which Margin is Get from. 
*   Return: The Margin of the Given Stack. 
*
*   NOTE: 
*   (1) The Margin is the Minimal Number of Free Entity Encountered in the Given 
*       Stack since System Startup.
***************************************************************************************/
STACK_EXT uint16_t stack_margin(stack_t *me); 

#ifdef SPYER_STACK_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_STACK(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spy Debuger Disabled */
    #define SPYER_STACK(format, ...)   ((void_t)0)
#endif  /* End of SPYER_STACK_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _STACK_H */
