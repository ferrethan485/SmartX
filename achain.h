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
*   File Name : achain.h
*   Version : 1.0
*   Description : Base for Priority-Based Active Object Chain
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-20-2005      Created Initial Version 1.0
*       JinHui Han      03-20-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _ACHAIN_H
#define _ACHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define ACHAIN_EXT
#else
    #define ACHAIN_EXT     extern
#endif

/***************************************************************************************
*   Priority-Based Active Object Chain.
*
*   This Priority-Based Active Object Chain is Chained to Bidirectional List.
*   Your Source Code Must Provide one (chain_t *) for Record the List Head of 
*   Priority-Based Active Object Chain, It Pointer to The Highest Priority's 
*   Active Object.
*
*   This Priority-Based Active Object Chain is not Capable of Blocking on the 
*   get() Operation, 
***************************************************************************************/

/***************************************************************************************
*   Initializes the Priority-Based Active Object Chain
*
*   Description: Initializes the Priority-Based Active Object Chain. 
*   Argument: me -- Pointer to the List Head of Active Object Chain to be Iitialized. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
ACHAIN_EXT int16_t achain_init(chain_t **me); 

/***************************************************************************************
*   Bind Active Object into Priority-Based Active Object Chain. 
*
*   Description: Bind Active Object into Priority-Based Active Object Chain, and Rank 
*                It According as the Active Object's Priority. 
*   Argument: me -- Pointer to the List Head of Active Object Chain. 
*             active -- The Active Object which is Bind.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: If the Lower Priority Active Object Has been Bind in Chain Pool, The Higher
*         Priority Active Object (Same as Previous) Bind Request will be Ignore, 
*         So ONLY the First Subscribed Request of Same Active Object is Valid, 
*         All the Subsequent Active Object (Same as Previous) Bind Request are Ignored.
***************************************************************************************/
ACHAIN_EXT int16_t achain_bind(chain_t **me, active_t *active);

/***************************************************************************************
*   Unbind Active Object from Priority-Based Active Object Chain. 
*
*   Description: Unbind Active Object from Priority-Based Active Object Chain. 
*   Argument: me -- Pointer to the List Head of Active Object Chain. 
*             active -- The Active Object which is Unbind.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
ACHAIN_EXT int16_t achain_unbind(chain_t **me, active_t *active);

#ifdef SPYER_ACHAIN_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_ACHAIN(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_ACHAIN(format, ...)   ((void_t)0)
#endif  /* End of SPYER_ACHAIN_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _ACHAIN_H */
