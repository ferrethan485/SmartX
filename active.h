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
*   Group : Active Object Engine
*   File Name : active.h
*   Version : 1.0
*   Description : Based for Active Object 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-03-2005      Created Initial Version 1.0
*       JinHui Han      03-03-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _ACTIVE_H
#define _ACTIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define ACTIVE_EXT
#else
    #define ACTIVE_EXT  extern
#endif

/***************************************************************************************
*   The Stack Space must be Large Enough to Handle its Worst-Case Function Call 
*   Nesting, Local Variable Usage, and Interrupt Nesting.
*   The Additional Stack by Active Engine is XXXX Bytes.
***************************************************************************************/

/***************************************************************************************
*   Active Object Structure.
*
*   active_t is the Base Structure for Execute the Active Objects. 
*   Active Objects are Encapsulated Thread (each Embedding a State Machine and an Event 
*   Chain) that Communicate with one Another Asynchronously by Sending and Receiving 
*   Events. 
*   Within an Active Object, Events are Processed Sequentially in a Run-to-Completion 
*   (RTC) Fashion, while Framework Encapsulates all the Details of Thread-Safe Event 
*   Exchange and Priority Based Event Ranking. 
*
*   NOTE: Active Object Instance MUST be Static, Because achain_unbind() Does not execute
*         the Release Operation for Active Object Instance.
***************************************************************************************/
/* Forward Declaration  */
struct active_tag;
/* The Signature of Active Function  */
typedef int16_t (*active_function_t)(struct active_tag *);   /* True = 1, False = 0, Failure = -1 */
typedef int8_t const ROM_KEYWORDS * (*active_version_t)(void_t); 
typedef int8_t const ROM_KEYWORDS * (*active_name_t)(void_t); 

typedef struct active_tag { 
    /***********************************************************************************
    *   The Object's Attribute 
    ***********************************************************************************/
    /*  Quit Request */
    int16_t   exit;     /* Exit Request */
    /*  Reset Request */
    int16_t   reset;    /* Reset Request */
    /*  Pause Status */ 
    int16_t   pause;    /* Pause Request */ 
    /*  Priority Associated with the Active Object. */
    /*  Where Lower Numerical Values Correspond to Higher Priority (urgency) */
    /*  Range from 0 to 65535, Where 0 Represents the Highest Priority. */
    uint16_t  priority;
    /*  NOTE: me->hsm and me->fsm is Used for Indicate the Exit Status of State */
    /*  Machine Also, when me->hsm = NULL or me->fsm = NULL. */
    hsm_t * hsm;        /* The Pointer of HSM hsm_t */
    fsm_t * fsm;        /* The Pointer of FSM fsm_t */
    /*  Event Chain. */
    chain_t * event;    /* The Pointer of Event Chain */
    chain_t * defer;    /* The Pointer of Defer Event Chain */
    /***********************************************************************************
    *   The Object's Behavior.
    ***********************************************************************************/
    active_version_t version;     /* Get Object's Version */
    active_name_t    name;        /* Get Object's Name */
    active_function_t start;           /* Start */
    active_function_t probe;           /* Probe */
    active_function_t run;             /* Run, Return the IDLE Status */
    active_function_t reset;           /* Reset */
    active_function_t quit;            /* Force Quit */
    active_function_t pause;           /* Pause */
    active_function_t resume;          /* Resume */
    active_function_t is_idle;         /* Is in Idle State */
    active_function_t callback_exit;   /* Exit  Callback */
    active_function_t callback_reset;  /* Reset Callback */
} active_t; 

#ifdef __cplusplus
}
#endif

#endif      /* End of _ACTIVE_H */