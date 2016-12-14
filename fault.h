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
*   Group : Failure Logger
*   File Name : fault.h              
*   Version : 1.0
*   Description : Bsae for Failure Logger.
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-07-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _FAULT_H
#define _FAULT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define FAULT_EXT
#else
    #define FAULT_EXT     extern
#endif

/***************************************************************************************
*   Maximum Length of File Name Strings.
***************************************************************************************/
#ifndef  FAULT_NAME
    /***********************************************************************************
    *   Macro to Override the Default Length of File Name Strings.
    *   Default is 16. 
    ***********************************************************************************/
    #define FAULT_NAME    16
#endif

/***************************************************************************************
*   Logger Amount.
***************************************************************************************/
#ifndef  FAULT_AMOUNT
    /***********************************************************************************
    *   Macro to Override the Default Amount of Failure Logging Array.
    *   Default is 256. 
    ***********************************************************************************/
    #define FAULT_AMOUNT    256
#endif

/***************************************************************************************
*   Logger Struct.
***************************************************************************************/
typedef struct logger_tag {
    int8_t   file[FAULT_NAME];      /* File Name */
    int16_t  line;                  /* Line Number */
    uint32_t time;                  /* TimeStamp */
} logger_t; 

/***************************************************************************************
*   Fault Manager Struct.
***************************************************************************************/
typedef struct fault_tag { 
    /*  Pointer to the Start of the Ring Buffer */
    logger_t *ring;
    /*  Total Number of Records in the Ring Buffer */
    int16_t total;
    /*  Offset to where Next Record will be Inserted into the Buffer */
    int16_t head;
    /*  Offset of where Next Record will be Extracted from the Buffer */
    int16_t tail;
} fault_t;

#ifdef FAULT_ENABLE      /* Failure Logger Enabled */
    /*  Buffer of Failure Logging */
    FAULT_EXT logger_t buff_logger [FAULT_AMOUNT + 1]; 
    /*  Fault Manager Object */
    FAULT_EXT fault_t  fault; 

    /***********************************************************************************
    *   Description: This Callback Invoked in FAULT() 
    *   Argument: file -- File Name where the Fault Happen
    *             line -- Line Number at which the Fault Happen
    *             time -- TimeStamp When the Fault Happen
    *   Return: none. 
    *   NOTE:
    ***********************************************************************************/
    FAULT_EXT void_t fault_(int8_t const ROM_KEYWORDS * const file, int32_t line, uint32_t time);

    /***********************************************************************************
    *   Place This Macro at the Top of Each C Source File to Define the File Name 
    *   String as the Argument "name_". This File Name will be Used in Reporting 
    *   Fault in This File.
    *   NOTE: Don't Using the __FILE__, Because It Might Contain Lengthy Path Name.
    ***********************************************************************************/
    #define FAULT_THIS_FILE(name_) static int8_t const ROM_KEYWORDS fault_file_name[] = #name_;

    /***********************************************************************************
    *   General Purpose Fault Recorder.
    *   Calls the fault_() Callback.
    ***********************************************************************************/
    #define FAULT()   fault_(fault_file_name, __LINE__, ticks_get())

#else       /* Failure Logger Disabled */

    #define FAULT_THIS_FILE(name_)
    #define FAULT()   ((void_t)0)

#endif      /* End of FAULT_ENABLE */

/***************************************************************************************
*   Description: Initialize Failure Logging. 
*   Argument: none. 
*   Return: 1 is True, 0 is False, -1 is Failure
*   NOTE: 
***************************************************************************************/
FAULT_EXT int16_t fault_init(void_t); 

/***************************************************************************************
*   Description: This Callback Invoked in FAULT() 
*   Argument: logger -- The Pointer of Fault Record Copy To. 
*   Return: 1 is True, 0 is False, -1 is Failure 
*   NOTE:
***************************************************************************************/
FAULT_EXT int16_t fault_get(logger_t *logger);

#ifdef __cplusplus
}
#endif

#endif      /* End of _FAULT_H */
