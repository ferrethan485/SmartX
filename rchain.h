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
*   Group : Active Object Manager
*   File Name : rchain.h
*   Version : 1.0
*   Description : Base for Priority-Based Event Register Chain
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-20-2005      Created Initial Version 1.0
*       JinHui Han      03-20-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _RCHAIN_H
#define _RCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define RCHAIN_EXT
#else
    #define RCHAIN_EXT     extern
#endif

/***************************************************************************************
*   Priority-Based Event Regiater Chain.
*
*   This Priority-Based Event Regiater Chain is Chained to Bidirectional List.
*   Your Source Code Must Provide one (chain_t *) for Record the List Head of 
*   Priority-Based Event Regiater Chain, It Pointer to The Highest Priority's 
*   Event.
*
*   This Priority-Based Event Regiater Chain is not Capable of Blocking on the 
*   get() Operation, 
***************************************************************************************/

/***************************************************************************************
*   Initializes the Priority-Based Event Regiater Chain
*
*   Description: Initializes the Priority-Based Event Regiater Chain. 
*   Argument: me -- Pointer to the List Head of Event Regiater Chain to be Iitialized. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
RCHAIN_EXT int16_t rchain_init(chain_t **me); 

/***************************************************************************************
*   Find Event from the Priority-Based Event Regiater Chain
*
*   Description: Find Event from the Priority-Based Event Regiater Chain. 
*   Argument: me -- Pointer to the List Head of Event Regiater Chain to be Iitialized. 
*             signal -- The Signal of Event which Require to be Find.
*   Return:  Return the Pointer of Event Regiater Cell if the Event has be Find, 
*            Or Else Return NULL.  
*
*   NOTE: 
***************************************************************************************/
RCHAIN_EXT chain_t *rchain_find(chain_t **me, signal_t signal); 

/***************************************************************************************
*   Bind Event into Priority-Based Event Regiater Chain. 
*
*   Description: Bind Event into Priority-Based Event Regiater Chain, and Rank 
*                It According as the Event's Priority. 
*   Argument: me -- Pointer to the List Head of Event Regiater Chain. 
*             event -- The Event which is Bind.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: If the Lower Priority Event Has been Bind in Chain Pool, The Higher
*         Priority Event (Same as Previous) Bind Request will be Ignore, 
*         So ONLY the First Subscribed Request of Same Event is Valid, 
*         All the Subsequent Event (Same as Previous) Bind Request are Ignored.
***************************************************************************************/
RCHAIN_EXT int16_t rchain_bind(chain_t **me, event_t *event);

/***************************************************************************************
*   Unbind Event from Priority-Based Event Regiater Chain. 
*
*   Description: Unbind Event from Priority-Based Event Regiater Chain. 
*   Argument: me -- Pointer to the List Head of Event Regiater Chain. 
*             chain -- The Event Cell which is Unbind.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
RCHAIN_EXT int16_t rchain_unbind(chain_t **me, chain_t *chain);

#ifdef SPYER_RCHAIN_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_RCHAIN(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_RCHAIN(format, ...)   ((void_t)0)
#endif  /* End of SPYER_RCHAIN_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _RCHAIN_H */
