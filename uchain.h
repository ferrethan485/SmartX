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
*   Group : General Chain
*   File Name : uchain.h
*   Version : 1.0
*   Description : Base for General Chain
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-20-2005      Created Initial Version 1.0
*       JinHui Han      03-20-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _UCHAIN_H
#define _UCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define UCHAIN_EXT
#else
    #define UCHAIN_EXT     extern
#endif

/***************************************************************************************
*   General Chain.
*
*   Note: 
*   () The General Chain is not Thread-Safe because it Not Uses Critical Section to 
*      Protect its Integrity. 
*   () The General Chain ONLY be Used Inside General Internal. MUST Not Use It
*      Inside Interrupt Service Routine. 
***************************************************************************************/

/***************************************************************************************
*   Initializes the General Chain
*
*   Description: Initializes the General Chain. 
*   Argument: me -- Pointer to the List Head of General Chain to be Iitialized. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
UCHAIN_EXT int16_t uchain_init(chain_t **me); 

/***************************************************************************************
*   Bind Cell into General Chain. 
*
*   Description: Bind Cell into General Chain. 
*   Argument: me -- Pointer to the List Head of General Chain. 
*             cell -- The Cell which need to be Bind.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
UCHAIN_EXT int16_t uchain_bind(chain_t **me, void_t *cell);

/***************************************************************************************
*   Unbind Cell from General Chain. 
*
*   Description: Unbind Cell from General Chain. 
*   Argument: me -- Pointer to the List Head of General Chain. 
*             cell -- The Cell which need to be Unbind.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
***************************************************************************************/
UCHAIN_EXT int16_t uchain_unbind(chain_t **me, void_t *cell);

#ifdef SPYER_UCHAIN_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_UCHAIN(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                        /* Spyer Debuger Disabled */
    #define SPYER_UCHAIN(format, ...)   ((void_t)0)
#endif  /* End of SPYER_UCHAIN_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _UCHAIN_H */
