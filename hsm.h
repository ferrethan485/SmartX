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
*   Group : State Machine
*   File Name : hsm.h
*   Version : 1.0
*   Description : Base for HSM State Machine
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-09-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _HSM_H
#define _HSM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define HSM_EXT
#else
    #define HSM_EXT   extern
#endif

/***************************************************************************************
*   Get the Current HSM State Machine Version Number String
*
*   Return Version of the HSM State Machine as a Constant 10-Character String of the
*   Form "HSM x.y.zz", Where x is a 1-Digit Major Version Number, y is a 1-Digit Minor
*   Version Number, and zz is a 2-Digit Release Number.
***************************************************************************************/
HSM_EXT int8_t const ROM_KEYWORDS * hsm_get_version(void_t);

/***************************************************************************************
*   Hierarchical State Machine
***************************************************************************************/
/* Forward Declaration  */
struct hsm_state_tag;
struct hsm_tag;
/* The Signature of HSM Function  */
typedef int16_t (*hsm_function_t)(struct hsm_tag *);   /* True = 1, False = 0, Failure = -1 */
typedef int16_t (*hsm_dispatch_t)(struct hsm_tag *, event_t const *);       /* True = 1, False = 0, Failure = -1 */
typedef int16_t (*hsm_is_in_t)(struct hsm_tag *, hsm_state_tag const *);    /* True = 1, False = 0, Failure = -1 */

/***************************************************************************************
*   HSM Reaction Structure.
*
*   hsm_action_t Represents the Event Triggered Action. It Include Event Signal, 
*   Guard Function, Action Function, Target State, etc. 
*   NOTICE:
*   () Self-Transition is External Transition, So the Exit and Entry Must be Handled. 
*   () Internal Transition, Don't Need State Transition, Keep Current State ONLY. 
***************************************************************************************/
typedef struct hsm_reaction_tag { 
    signal_t        signal;         /* Signal of the Event Instance */
    hsm_function_t  guard;          /* Guard  Function */
    hsm_function_t  action;         /* Action Function */
    int16_t         external;       /* Distinguish, 1 = External Transition, 0 = Internal Transition */ 
    struct hsm_state_tag * target;  /* Target State */
} hsm_reaction_t; 

/***************************************************************************************
*   HSM State Structure.
*
*   hsm_state_t Represents the State in Hierarchical State Machine (HSM), it Include
*   the Entry/Exit/Do Function and the Event Handle Tables, etc. 
*   NOTICE: 
*   (1) The Ultimate Root of State Hierarchy in all HSMs Derived from hsm_t, Must be 
*       Defined as "top"=NULL, and It¡®s "down" is the Path of Initial Transition.
***************************************************************************************/
typedef struct hsm_state_tag { 
    int16_t  depth;                 /* Depth in HSM Layer */
    struct hsm_state_tag * top;     /* Top   Level State  */
    struct hsm_state_tag * down;    /* Down  Level State  */
    hsm_function_t  enter;          /* Entry Action Function */
    hsm_function_t  quit;           /* Exit  Action Function */
    hsm_function_t  service;        /* Do    Action Function */
    hsm_reaction_t * reaction;      /* Event Reaction Table  */
    int16_t  rtotal;                /* Total of Items in Event Reaction Table */
    int8_t*  name;                  /* Strings of State Name, Used for Spyer  */
} hsm_state_t; 

/***************************************************************************************
*   hsm_t Represents a Hierarchical State Machine (HSM) with Full Support for 
*   Hierarchical Nesting of States, Entry/Exit actions, and Initial Transitions in
*   Any Composite State.
*
*   ham_t is not Intended to be Instantiated Directly, but Rather Serves as the 
*   Base Structure for Derivation of State Machines in the Application Code.
*
*   The Following Example Illustrates how to Derive a State Machine Structure from 
*   hsm_t.
*       typedef struct hsm_calc_tag
*       {
*           hsm_t super;      // Derives from hsm_t
*           // Object Properties 
*           flost32_t   operand1;
*           float32_t   operand2;
*           int8_t      display[DISP_WIDTH + 1];
*           uint8_t     len;
*           uint8_t     op_key;
*           // Object Method
*           
*       } hsm_calc_t;
*
*   Please Note that the hsm_t Member "super" is Defined as the FIRST Member of 
*   the Derived Struct. 
*
*   The Following Example Illustrates how to Define a State Machine Object Instance. 
*   hsm_calc_t hsm_calc = {
*       {
*           // Object Properties
*           (hsm_t *)tbl_hsm_calc[0],   // Overload the Current State 
*           (hsm_t *)0,         // Overload the Target State
*           (hsm_t *)0,         // Overload the Work State
*           0,                  // Overload the External Transition
*           HSM_IDLE,           // Work Status of State Machine
*           // Object Method
*           (hsm_function_t) calc_constructor,  // Overload the Constructor
*           (hsm_function_t) calc_destroyer,    // Overload the Destroyer
*           (hsm_function_t) calc_initializer,  // Overload the Initializer
*           (hsm_function_t) hsm_init_trans, 
*           (hsm_dispatch_t) hsm_dispatch, 
*           (hsm_is_in_t) hsm_is_in, 
*           (hsm_function_t) calc_is_idle,      // Overload the Idle Check
*           (hsm_function_t) hsm_is_final
*       }, 
*       // Object Properties 
*       // Object Method
*   }; 
***************************************************************************************/
enum hsm_status {
    HSM_IDLE = 0,            /* Default Status, Must Equal to 0 */
    HSM_CONSTRUCT,           /* Construct  Status */
    HSM_INITIALIZE,          /* Initialize Status */
    HSM_INIT_TRANS,          /* Initialize Transition Status */
    HSM_DISPATCH             /* Normal Dispatch Status */
};

#ifndef HSM_MAX_DEPTH
    /***********************************************************************************
    *   This Constant Defines the Maximum Depth of HSM State Nesting.
    *   Topmost Depth is 0. 
    ***********************************************************************************/
    #define HSM_MAX_DEPTH    16     /* Default is 16 */
#endif      /* End of HSM_MAX_DEPTH */

typedef struct hsm_tag
{
    /* Object Properties, Need be Overload by Inherit */
    hsm_state_t * curr_state;     /* Current Active State */
    hsm_state_t * dest_state;     /* Target State of Transitions*/
    hsm_state_t * work_state;     /* Temporary Work State */
    int16_t       external;       /* Distinguish, 1 = External Transition, 0 = Internal Transition */ 
    /* Object Properties, Public */
    int16_t       status_;        /* Work Status of State Machine, Internal Use Only */
    /* Object Method, Need be Overload by Inherit */
    hsm_function_t constructor;   /* Constructor of HSM State Machine */
    hsm_function_t destroyer;     /* Destroyer   of HSM State Machine */
    hsm_function_t initializer;   /* Initializer of HSM State Machine */
    /* Object Method, Public */
    hsm_function_t init_trans;    /* Initial Transition of HSM State Machine */
    hsm_dispatch_t dispatch;      /* Dispatches an Event to HSM */
    hsm_is_in_t    is_in;         /* Tests a Given State */
    hsm_function_t is_idle;       /* Is in IDLE State, MUST BE IMPLEMENT BY USER */
    hsm_function_t is_final;      /* Is in FINAL State */
} hsm_t;

/***************************************************************************************
*   Destroyer of a HSM.
*
*   Description: Performs the HSM Destroy by Release the Needed Resource which is 
*                Necessary for HSM Run. 
*   Argument: me -- Pointer the State Machine Structure Derived from hsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: (1) The "Destroyer" is Used to Release the Resource Mostly, e.g., the 
*             Memory Block which Get from Memory Pool.
*         (2) Must be Called Only by the "Destroyer" of the Derived State Machines.
*
*   The Following Example Illustrates How to Invoke hsm_destroyer() in the 
*   "Destroyer" of a Derived State Machine:
*       int16_t calc_destroyer(hsm_calc_t *me)
*       {
*           // Release the Resource of calc HSM at Here
*           ...
*           // Super Class Destroyer
*           hsm_destroyer((hsm_t *)me);
*           return TRUE; 
*       }
*
*       calc_destroyer(&hsm_calc);
***************************************************************************************/
HSM_EXT int16_t hsm_destroyer (hsm_t *me); 

/***************************************************************************************
*   The First Step of HSM Initialization.
*
*   Description: Construct the Needed Resource which is Necessary for HSM Run. 
*   Argument: me -- Pointer the State Machine Structure Derived from hsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: (1) The "Constructor" is Used to Allocate the Resource Mostly, e.g., Get the
*             Memory Block from the Memory Pool.
*         (2) Must be Called Only by the "Constructors" of the Derived State Machines.
*         (3) Must be Called Before hsm_initialize(). 
*
*   The Following Example Illustrates How to Invoke hsm_construct() in the "Constructor"
*   of a Derived State Machine:
*       int_t calc_constructor(hsm_calc_t *me)
*       {
*           // Call the Super Class Constructor
*           hsm_constructor((hsm_t *)me);
*           // Allocate the Resource of calc HSM at Here
*           ...
*       } 
*
*       calc_constructor(&hsm_calc); 
***************************************************************************************/
HSM_EXT int16_t hsm_constructor (hsm_t *me); 

/***************************************************************************************
*   The Second Step of HSM Initialization.

*   Description: Initialize the HSM Object. 
*   Argument: me -- Pointer the State Machine Structure Derived from hsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*   
*   NOTE: (1) After Initialize the hsm_t Structure and the Structure Derived 
*             from hsm_t, the Currently Active State of the State Machine is the 
*             Initial Pseudostate. 
*         (2) Must be Called Only by the "Initializer" of the Derived State Machines.
*         (3) Must be Called Before hsm_init_trans() and After hsm_construct().
*
*   The Following Example Illustrates How to Invoke hsm_initialize() in the "Initializer"
*   of a Derived State Machine:
*       int16_t calc_initializer(hsm_calc_t *me)
*       {
*           // Call the Super Class Initializer
*           hsm_initializer((hsm_t *)me); 
*           // Initialize the Variables of calc HSM at Here
*           ...
*       }
*
*       calc_initializer(&hsm_calc);
***************************************************************************************/
HSM_EXT int16_t hsm_initializer (hsm_t *me); 

/***************************************************************************************
*   The Third Step of HSM Initialization. 
*
*   Description: Triggering the Top-Most Initial Transition.
*   Argument: me -- Pointer the State Machine Structure Derived from hsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: (1) Must be Called Only ONCE After the hsm_constructor() and hsm_initializer(), 
*             and Before hsm_dispatch().
*         (2) This Does Not Implement the Initialization Event, Because It is Trashy
*             in Generic Application.
*
*   The Following Example Illustrates How to Triggering the Top-Most Initial Transition:
*       // Trigger Initial Transition
*       hsm_init_trans((hsm_t *)&hsm_calc);
***************************************************************************************/
HSM_EXT int16_t hsm_init_trans(hsm_t *me);

/***************************************************************************************
*   Description: Dispatches an Event to a HSM. Processes one Event at a Time in 
*                Run-to-Completion Fashion.
*   Argument: me    -- Pointer the State Machine Structure Derived from hsm_t
*             event -- Constant Pointer the event_t or a Structure Derived from event_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: Must be Called After the "Constructor" hsm_construct(), hsm_initialize() and
*         hsm_init_trans().
*
*   The Following Example Illustrates How to Dispatch Events to It:
*       // Dispatch Event to hsm_calc
*       hsm_dispatch((hsm_t *)&hsm_calc, (event_t const *)&event);
***************************************************************************************/
HSM_EXT int16_t hsm_dispatch(hsm_t *me, event_t *event);

/***************************************************************************************
*   Description: Tests if a Given State is Part of the Current Active State 
*                Configuratioin
*   Argument: me    -- Pointer the State Machine Structure Derived from hsm_t
*             state -- Pointer to the State, e.g., &tbl_hsm_calc[5].
*   Return: 1 is True, 0 is False, -1 is Failure
***************************************************************************************/
HSM_EXT int16_t hsm_is_in(hsm_t *me, hsm_state_t *state);

/***************************************************************************************
*   Description: Tests if State Machine is in Final State. 
*   Argument: me    -- Pointer the State Machine Structure Derived from hsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
***************************************************************************************/
HSM_EXT int16_t hsm_is_final(hsm_t *me); 

#ifdef SPYER_HSM_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_HSM(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
    #define HSM_STATE_NAME(name)    ,"name"
#else                       /* Spyer Debuger Disabled */
    #define SPYER_HSM(format, ...)   ((void_t)0)
    #define HSM_STATE_NAME(name) 
#endif  /* End of SPYER_HSM_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _HSM_H */
