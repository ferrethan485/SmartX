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
*   File Name : fsm.h
*   Version : 1.0
*   Description : Base for FSM State Machine
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-09-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _FSM_H
#define _FSM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define FSM_EXT
#else
    #define FSM_EXT   extern
#endif

/***************************************************************************************
*   FSM can be Advantageous in Resource-Constraint Applications because Avoiding HSM 
*   Functions Saves about 1KB of Code Space (typically ROM), and FSM Functions are 
*   Slightly Faster than the More General HSM Functions.
***************************************************************************************/

/***************************************************************************************
*   Get the Current FSM State Machine Version Number String
*
*   Return Version of the FSM State Machine as a Constant 10-Character String of the
*   Form "FSM x.y.zz", Where x is a 1-Digit Major Version Number, y is a 1-Digit Minor
*   Version Number, and zz is a 2-Digit Release Number.
***************************************************************************************/
FSM_EXT int8_t const ROM_KEYWORDS * fsm_get_version(void_t);

/***************************************************************************************
*   Finite State Machine
***************************************************************************************/
/* Forward Declaration  */
struct fsm_state_tag;
struct fsm_tag;
/* The Signature of FSM Function  */
typedef int16_t (*fsm_function_t)(struct fsm_tag *);   /* True = 1, False = 0, Failure = -1 */
typedef int16_t (*fsm_dispatch_t)(struct fsm_tag *, event_t *);       /* True = 1, False = 0, Failure = -1 */
typedef int16_t (*fsm_is_in_t)(struct fsm_tag *, fsm_state_tag *);    /* True = 1, False = 0, Failure = -1 */

/***************************************************************************************
*   FSM Reaction Structure.
*
*   fsm_action_t Represents the Event Triggered Action. It Include Event Signal, 
*   Guard Function, Action Function, Target State, etc. 
*   NOTICE:
*   () Self-Transition is External Transition, So the Exit and Entry Must be Handled. 
*   () Internal Transition, Don't Need State Transition, Keep Current State ONLY. 
***************************************************************************************/
typedef struct fsm_reaction_tag { 
    signal_t        signal;         /* Signal of the Event Instance */
    fsm_function_t  guard;          /* Guard  Function */
    fsm_function_t  action;         /* Action Function */
    int16_t         external;       /* Distinguish, 1 = External Transition, 0 = Internal Transition */ 
    struct fsm_state_tag * target;  /* Target State */
} fsm_reaction_t; 

/***************************************************************************************
*   FSM State Structure.
*
*   fsm_state_t Represents the State in Finite State Machine (FSM), it Include
*   the Entry/Exit/Do Function and the Event Handle Tables, etc. 
*   NOTICE: 
***************************************************************************************/
typedef struct fsm_state_tag { 
    fsm_function_t  enter;          /* Entry Action Function */
    fsm_function_t  quit;           /* Exit  Action Function */
    fsm_function_t  service;        /* Do    Action Function */
    fsm_reaction_t * reaction;      /* Event Reaction Table  */
    int16_t  rtotal;                /* Total of Items in Event Reaction Table */
    int8_t*  name;                  /* Strings of State Name, Used for Spyer  */
} fsm_state_t; 

/***************************************************************************************
*   fsm_t Represents a Traditional Non-Hierarchical Finite State Machine (FSM) 
*   without State Hierarchy, but with Entry/Exit Actions.
*
*   ham_t is not Intended to be Instantiated Directly, but Rather Serves as the 
*   Base Structure for Derivation of State Machines in the Application Code.
*
*   The Following Example Illustrates how to Derive a State Machine Structure from 
*   fsm_t.
*       typedef struct fsm_bomb_tag
*       {
*           fsm_t super;      // Derives from fsm_t
*           // Object Properties 
*           uint8_t     timeout;    // Number of Seconds Till Explosion
*           uint8_t     defuse;     // The Secret  Defuse Code
*           uint8_t     code;       // The Current Defuse Code Entry
*           // Object Method
*           
*       } fsm_bomb_t;
*
*   Please Note that the fsm_t Member "super" is Defined as the FIRST Member of 
*   the Derived Struct. 
*
*   The Following Example Illustrates how to Define a State Machine Object Instance. 
*   fsm_bomb_t fsm_bomb = {
*       {
*           // Object Properties
*           (fsm_t *)&tbl_fsm_bomb[0],   // Overload the Current State 
*           (fsm_t *)0,         // Overload the Target  State
*           0,                  // Overload the External Transition
*           FSM_IDLE,           // Work Status of State Machine
*           // Object Method
*           (fsm_function_t) bomb_constructor,  // Overload the Constructor
*           (fsm_function_t) bomb_destroyer,    // Overload the Destroyer
*           (fsm_function_t) bomb_initializer,  // Overload the Initializer
*           (fsm_function_t) fsm_init_trans, 
*           (fsm_dispatch_t) fsm_dispatch, 
*           (fsm_is_in_t) fsm_is_in, 
*           (fsm_function_t) bomb_is_idle,      // Overload the Idle Check
*           (fsm_function_t) fsm_is_final, 
*       }, 
*       // Object Properties 
*       // Object Method
*   }; 
***************************************************************************************/
enum fsm_status {
    FSM_IDLE = 0,            /* Default Status, Must Equal to 0 */
    FSM_CONSTRUCT,           /* Construct  Status */
    FSM_INITIALIZE,          /* Initialize Status */
    FSM_INIT_TRANS,          /* Initialize Transition Status */
    FSM_DISPATCH             /* Normal Dispatch Status */
};

typedef struct fsm_tag
{
    /* Object Properties, Need be Overload by Inherit */
    fsm_state_t * curr_state;     /* Current Active State */
    fsm_state_t * dest_state;     /* Target State of Transitions*/
    int16_t       external;       /* Distinguish, 1 = External Transition, 0 = Internal Transition */ 
    /* Object Properties, Public */
    int16_t       status_;        /* Work Status of State Machine, Internal Use Only */
    /* Object Method, Need be Overload by Inherit */
    fsm_function_t constructor;   /* Constructor of FSM State Machine */
    fsm_function_t destroyer;     /* Destroyer   of FSM State Machine */
    fsm_function_t initializer;   /* Initializer of FSM State Machine */
    /* Object Method, Public */
    fsm_function_t init_trans;    /* Initial Transition of FSM State Machine */
    fsm_dispatch_t dispatch;      /* Dispatches an Event to FSM */
    fsm_is_in_t    is_in;         /* Tests a Given State */
    fsm_function_t is_idle;       /* Is in IDLE State, MUST BE IMPLEMENT BY USER */
    hsm_function_t is_final;      /* Is in FINAL State */
} fsm_t;

/***************************************************************************************
*   Destroyer of a FSM.
*
*   Description: Performs the FSM Destroy by Release the Needed Resource which is 
*                Necessary for FSM Run. 
*   Argument: me -- Pointer the State Machine Structure Derived from fsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: (1) The "Destroyer" is Used to Release the Resource Mostly, e.g., the 
*             Memory Block which Get from Memory Pool.
*         (2) Must be Called Only by the "Destroyer" of the Derived State Machines.
*
*   The Following Example Illustrates How to Invoke fsm_destroyer() in the 
*   "Destroyer" of a Derived State Machine:
*       int16_t bomb_destroyer(fsm_bomb_t *me)
*       {
*           // Release the Resource of bomb FSM at Here
*           ...
*           // Super Class Destroyer
*           fsm_destroyer((fsm_t *)me);
*           return TRUE; 
*       }
*
*       bomb_destroyer(&fsm_bomb);
***************************************************************************************/
FSM_EXT int16_t fsm_destroyer (fsm_t *me); 

/***************************************************************************************
*   The First Step of FSM Initialization.
*
*   Description: Construct the Needed Resource which is Necessary for FSM Run. 
*   Argument: me -- Pointer the State Machine Structure Derived from fsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: (1) The "Constructor" is Used to Allocate the Resource Mostly, e.g., Get the
*             Memory Block from the Memory Pool.
*         (2) Must be Called Only by the "Constructors" of the Derived State Machines.
*         (3) Must be Called Before fsm_initialize(). 
*
*   The Following Example Illustrates How to Invoke fsm_construct() in the "Constructor"
*   of a Derived State Machine:
*       int_t bomb_constructor(fsm_bomb_t *me)
*       {
*           // Call the Super Class Constructor
*           fsm_constructor((fsm_t *)me);
*           // Allocate the Resource of calc FSM at Here
*           ...
*       } 
*
*       bomb_constructor(&fsm_bomb); 
***************************************************************************************/
FSM_EXT int16_t fsm_constructor (fsm_t *me); 

/***************************************************************************************
*   The Second Step of FSM Initialization.

*   Description: Initialize the FSM Object. 
*   Argument: me -- Pointer the State Machine Structure Derived from fsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*   
*   NOTE: (1) After Initialize the fsm_t Structure and the Structure Derived 
*             from fsm_t, the Currently Active State of the State Machine is the 
*             Initial Pseudostate. 
*         (2) Must be Called Only by the "Initializer" of the Derived State Machines.
*         (3) Must be Called Before fsm_init_trans() and After fsm_construct().
*
*   The Following Example Illustrates How to Invoke fsm_initialize() in the "Initializer"
*   of a Derived State Machine:
*       int16_t bomb_initializer(fsm_bomb_t *me)
*       {
*           // Call the Super Class Initializer
*           fsm_initializer((fsm_t *)me); 
*           // Initialize the Variables of calc FSM at Here
*           ...
*       }
*
*       bomb_initializer(&fsm_bomb);
***************************************************************************************/
FSM_EXT int16_t fsm_initializer (fsm_t *me); 

/***************************************************************************************
*   The Third Step of FSM Initialization. 
*
*   Description: Triggering the Top-Most Initial Transition.
*   Argument: me -- Pointer the State Machine Structure Derived from fsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: (1) Must be Called Only ONCE After the fsm_constructor() and fsm_initializer(), 
*             and Before fsm_dispatch().
*         (2) This Does Not Implement the Initialization Event, Because It is Trashy
*             in Generic Application.
*
*   The Following Example Illustrates How to Triggering the Top-Most Initial Transition:
*       // Trigger Initial Transition
*       fsm_init_trans((fsm_t *)&fsm_bomb);
***************************************************************************************/
FSM_EXT int16_t fsm_init_trans(fsm_t *me);

/***************************************************************************************
*   Description: Dispatches an Event to a FSM. Processes one Event at a Time in 
*                Run-to-Completion Fashion.
*   Argument: me    -- Pointer the State Machine Structure Derived from fsm_t
*             event -- Constant Pointer the event_t or a Structure Derived from event_t
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: Must be Called After the "Constructor" fsm_construct(), fsm_initialize() and
*         fsm_init_trans().
*
*   The Following Example Illustrates How to Dispatch Events to It:
*       // Dispatch Event to fsm_bomb
*       fsm_dispatch((fsm_t *)&fsm_bomb, (event_t *)&event);
***************************************************************************************/
FSM_EXT int16_t fsm_dispatch(fsm_t *me, event_t *event);

/***************************************************************************************
*   Description: Tests if a Given State is the Current Active State. 
*   Argument: me    -- Pointer the State Machine Structure Derived from fsm_t
*             state -- Pointer to the State, e.g., &tbl_fsm_bomb[5].
*   Return: 1 is True, 0 is False, -1 is Failure
***************************************************************************************/
FSM_EXT int16_t fsm_is_in(fsm_t *me, fsm_state_t *state);

/***************************************************************************************
*   Description: Tests if State Machine is in Final State. 
*   Argument: me    -- Pointer the State Machine Structure Derived from fsm_t
*   Return: 1 is True, 0 is False, -1 is Failure
***************************************************************************************/
FSM_EXT int16_t fsm_is_final(fsm_t *me); 

#ifdef SPYER_FSM_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_FSM(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
    #define FSM_STATE_NAME(name)    ,"name"
#else                       /* Spyer Debuger Disabled */
    #define SPYER_FSM(format, ...)   ((void_t)0)
    #define FSM_STATE_NAME(name) 
#endif  /* End of SPYER_FSM_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _FSM_H */
