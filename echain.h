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
*   Copyright (C) 2004 - 2099 Silver Fox Security System Limited, All Right Reserved
*
*   Component : Active Object Application FrameWork
*   Group : Active Object Engine
*   File Name : echain.h
*   Version : 1.0
*   Description : Base for Priority-Based Event Chain
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-20-2005      Created Initial Version 1.0
*       JinHui Han      03-20-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _ECHAIN_H
#define _ECHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define ECHAIN_EXT
#else
    #define ECHAIN_EXT     extern
#endif

/***************************************************************************************
*   Priority-Based Event Chain.
*
*   This Priority-Based Event Chain is Chained to Bidirectional List.
*   Your Source Code Must Provide one (chain_t *) for Record the List Head of 
*   Priority-Based Event Chain, It Pointer to The Highest Priority's Event.
*
*   This Priority-Based Event Chain is not Capable of Blocking on the get() Operation, 
***************************************************************************************/

/***************************************************************************************
*   Initializes the Priority-Based Event Chain
*
*   Description: Initializes the Priority-Based Event Chain. 
*   Argument: me -- Pointer to the List Head of Event Chain to be Iitialized. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
ECHAIN_EXT int16_t echain_init(chain_t **me); 

/***************************************************************************************
*   Post Event into Priority-Based Event Chain. 
*
*   Description: Post Event into Priority-Based Event Chain, and Rank It According as 
*                the Event's Priority. 
*   Argument: me -- Pointer to the List Head of Event Chain. 
*             event -- The Event which is be Appended.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) This Function can be Called from Object Itself.
*   (2) This Function Uses Internally a Critical Section, so You should be Careful 
*       not to Call it from within a Critical Section when Nesting of Critical 
*       Section is not Supported.
***************************************************************************************/
ECHAIN_EXT int16_t echain_post(chain_t **me, event_t *event);

/***************************************************************************************
*   Get Event from Priority-Based Event Chain. 
*
*   Description: Get Event from Priority-Based Event Chain. It Always Get the Event 
*                which Have Highest Priority. 
*   Argument: me -- Pointer to the List Head of Event Chain. 
*   Return: The Pointer to Highest Priority Event, or else Return NULL if Have Not 
*           Event in that Chain.
*
*   NOTE: 
*   (1) This Function is Called from Object Itself.
*   (2) This Function Uses Internally a Critical Section, so You should be Careful 
*       not to Call it from within a Critical Section when Nesting of Critical 
*       Section is not Supported.
***************************************************************************************/
ECHAIN_EXT event_t *echain_get(chain_t **me);

/***************************************************************************************
*   Flush Out the Priority-Based Event Chain. 
*
*   Description: Flush Empty the Priority-Based Event Chain and If Event's dynamic_ is 0, 
*                Release It into Memory Pool.  
*   Argument: me -- Pointer to the List Head of Event Chain to be Fluah Out. 
*   Return: 1 is True, 0 is False, -1 is Failure.
*
*   NOTE: 
*   (1) This Function is Called from Object Itself. 
*   (2) If has ISR Post the Event into Event Chain, You Must Disable this ISR before
*       Call echain_flush(). 
***************************************************************************************/
ECHAIN_EXT int16_t echain_flush(chain_t **me);

#ifdef SPYER_ECHAIN_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_ECHAIN(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_ECHAIN(format, ...)   ((void_t)0)
#endif  /* End of SPYER_ECHAIN_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _ECHAIN_H */
