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
*   Group : Spyer Debuger
*   File Name : spyer.c              
*   Version : 1.0
*   Description : Base for Spyer Debuger
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-13-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "std_type.h"
#include "std_def.h"
#include "config.h"
#include "lock.h"
#include "fault.h"
#include "debug.h"

#define  EXTERN_GLOBALS
#include "spyer.h"

#ifdef SPYER_ENABLE      /* Spyer Enabled */

/***************************************************************************************
*   spyer_() Implementation.
***************************************************************************************/
void_t spyer_(const int8_t * format, ...)
{
    int8_t buff_spyer[SPYER_BUFFER_SIZE]; 

    INT_LOCK_KEY_

    INT_LOCK_();
    if (sprintf(buff_spyer, "\n"format, ##__VA_ARGS__) > 0) { 
        debug_puts (buff_spyer); 
    }
    INT_UNLOCK_(); 
}

#endif      /* End of SPYER_ENABLE */
