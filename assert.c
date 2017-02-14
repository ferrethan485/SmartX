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
*   Group : Assert Checker
*   File Name : assert.c              
*   Version : 1.0
*   Description : Base for Assert Checker
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
#include "assert.h"

#ifdef ASSERT_ENABLE      /* Assertion Checking Enabled */

/***************************************************************************************
*   assert_() Implementation.
***************************************************************************************/
void_t assert_(int8_t const ROM_KEYWORDS * const file, int32_t line)
{
    // 255 is the Maximum Length of File Name in Linux and Windows
    //  10 is the Maximum Strings Length Associated with int32_t
    int8_t buff_assert[255 + 10 + 1 + 30]; 

    INT_LOCK_KEY_

    INT_LOCK_();
    if (sprintf(buff_assert, "\nAssertion Failed in %s, Line %d.", file, line) > 0) { 
        debug_puts (buff_assert); 
    }
    INT_UNLOCK_(); 
}

#endif      /* End of ASSERT_ENABLE */
