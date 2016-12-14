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
*   File Name : assert.h              
*   Version : 1.0
*   Description : Base for Assert Checker
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-07-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define ASSERT_EXT
#else
    #define ASSERT_EXT     extern
#endif

/***************************************************************************************
*   The Preprocessor Switch ASSERT_ENABLE Enables Assertion Checking.
***************************************************************************************/

#ifdef ASSERT_ENABLE      /* Assertion Checking Enabled */

    /***********************************************************************************
    *   Description: This Callback Invoked in Case the Condition Passed to ASSERT() 
    *                Evaluates to FALSE.
    *   Argument: file -- File Name where the Assertion Failed
    *             line -- Line Number at which the Assertion Failed
    *   Return: none
    *   NOTE:
    ***********************************************************************************/
    ASSERT_EXT void_t assert_(int8_t const ROM_KEYWORDS * const file, int32_t line);

    /***********************************************************************************
    *   Place This Macro at the Top of Each C Source File to Define the File Name 
    *   String as the Argument "name_". This File Name will be Used in Reporting 
    *   Assertions in This File.
    *   NOTE: Don't Using the __FILE__, Because It Might Contain Lengthy Path Name.
    ***********************************************************************************/
    #define ASSERT_THIS_FILE(name_) static int8_t const ROM_KEYWORDS local_file_name[] = #name_;

    /***********************************************************************************
    *   General Purpose Assertion that Makes Sure the "test_" Argument is TRUE.
    *   Calls the assert_() Callback if the "test_" Evaluates to FALSE.
    ***********************************************************************************/
    #define ASSERT(test_)   if (!(test_)   assert_(local_file_name, __LINE__)

    /***********************************************************************************
    *   Assertion that Always Calls the assert_handler() Callback if Ever Executed.
    ***********************************************************************************/
    #define ASSERT_ERROR()    assert_(local_file_name, __LINE__)

#else                       /* Assertion Checking Disabled */

    #define ASSERT_THIS_FILE(name_)
    #define ASSERT(ignore_)         ((void_t)0)
    #define ASSERT_ERROR()          ((void_t)0)

#endif      /* End of ASSERT_ENABLE */

/***************************************************************************************
*   These Macros is Equivalent to ASSERT(), Except the Name Provides a Better 
*   Understand of the Intention of these Assertion.
***************************************************************************************/

/* Assertion that Checks for a Pre-Condition  */
#define ASSERT_REQUIRE(test_)   ASSERT(test_)

/* Assertion that Checks for a Post-Condition */
#define ASSERT_ENSURE(test_)    ASSERT(test_)

/* Assertion that Checks for an Invariant */
#define ASSERT_INVARIANT(test_) ASSERT(test_)

/***************************************************************************************
*   Compile-Time Assertion Exploits the Fact that in C/C++ a Dimension of an Array Must
*   be non-zero. The Following Declaration Causes a Compilation Error if the Compile-Time
*   Expression is not TRUE. The Assertion has no Runtime Side Effects.
***************************************************************************************/
#define ASSERT_COMPILE(test_)     int8_t assert_compile[(test_)]

#ifdef __cplusplus
}
#endif

#endif      /* End of _ASSERT_H */
