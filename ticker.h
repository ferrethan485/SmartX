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
*   Group : Ticker
*   File Name : ticker.h
*   Version : 1.0
*   Description : Base for Ticker
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-09-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _TICKER_H
#define _TICKER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define TICKER_EXT
#else
    #define TICKER_EXT   extern
#endif

/**************************************************************************************
*   Initialize the Ticker.
*
*   Description: Initialize Ticker, Include the Initialization of System Ticks and 
*                the Time-Tick ISR (or the Highest-Priority Task). 
*   Argument: none
*   Return: none
*
*   NOTE: 
**************************************************************************************/
TICKER_EXT void_t ticker_init(void_t);

/**************************************************************************************
*   Processes all Armed Time Events at Every Clock Tick.
*
*   Description: Performs the Ticks Operatios at Every Clock Tick. 
*   Argument: none
*   Return: none
*
*   NOTE: 
*   (1) This Function must be Called Periodically from a Time-Tick ISR or from the 
*       Highest-Priority Task so that FrameWork can Manage the Timeout Events.
*   (2) The ticker() Function is not Reentrant Meaning that It must Run To Completion 
*       before it is Called Again. Also, ticker() Assumes that It never will Get 
*       Preempted by a Task, which is Always the Case when It is Called from an ISR 
*       or the Highest-Priority Task.
**************************************************************************************/
TICKER_EXT void_t ticker(void_t);

#ifdef __cplusplus
}
#endif

#endif      /* End of _TICKER_H */
