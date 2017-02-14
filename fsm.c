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
*   File Name : fsm.c                 
*   Version : 1.0
*   Description : Base for FSM State Machine
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-09-2005      Created Initial Version 1.0
*       JinHui Han      02-13-2005      Verified Version 1.0
***************************************************************************************/

#include "std_type.h"
#include "std_def.h"
#include "config.h"
#include "lock.h"
#include "debug.h"
#include "assert.h"
#include "spyer.h"
#include "ticks.h"
#include "fault.h"

#define  EXTERN_GLOBALS
#include "fsm.h"

ASSERT_THIS_FILE(fsm.c)
FAULT_THIS_FILE(fsm.c)

/***************************************************************************************
*   fsm_get_version() Implementation.
***************************************************************************************/
int8_t const ROM_KEYWORDS *fsm_get_version(void_t)
{
    static int8_t const ROM_KEYWORDS fsm_version[] = "FSM 1.0.00";
    return fsm_version;
}

/***************************************************************************************
*   fsm_destroyer() Implementation.
***************************************************************************************/
int16_t fsm_destroyer (fsm_t *me)
{
    ASSERT_REQUIRE(me != (fsm_t *)0);
    if(me == (fsm_t *)0) { 
        return FAILURE; 
    }
    /* For the Basal FSM, it is NOP Operation. */
    me->status_ = FSM_IDLE; 

    SPYER_FSM("FSM[%X] is Destroyed, Curr %d, Dest %d. TimeStamp %d", \\
               me, me->curr_state, me->dest_state, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   fsm_constructor() Implementation.
***************************************************************************************/
int16_t fsm_constructor (fsm_t *me) 
{
    ASSERT_REQUIRE(me != (fsm_t *)0);
    if(me == (fsm_t *)0) { 
        return FAILURE; 
    }
    /* For the Basal FSM, it is NOP Operation */
    me->status_ = FSM_CONSTRUCT; 

    SPYER_FSM("FSM[%X] is Construct, Curr %d, Dest %d. TimeStamp %d", \\
               me, me->curr_state, me->dest_state, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   fsm_initializer() Implementation.
***************************************************************************************/
int16_t fsm_initializer (fsm_t *me) 
{
    ASSERT_REQUIRE(me != (fsm_t *)0);
    if(me == (fsm_t *)0) { 
        return FAILURE; 
    }
    /* For the Basal FSM, Initialize the FSM Object Properties and Method */
    me->curr_state = me->curr_state;    /* NOP Operation */
    me->dest_state = me->dest_state;    /* NOP Operation */
    me->external   = me->external;      /* NOP Operation */
    me->status_ = FSM_INITIALIZE; 
    me->constructor = me->constructor;  /* NOP Operation */
    me->destroyer   = me->destroyer;    /* NOP Operation */
    me->initializer = me->initializer;  /* NOP Operation */
    me->init_trans  = fsm_init_trans; 
    me->dispatch    = fsm_dispatch; 
    me->is_in       = fsm_is_in; 
    me->is_idle     = me->is_idle;      /* NOP Operation */
    me->is_final    = fsm_is_final; 

    SPYER_FSM("FSM[%X] is Initialized, Curr %d, Dest %d. TimeStamp %d", \\
               me, me->curr_state, me->dest_state, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   Internal Function, fsm_entry()
***************************************************************************************/
int16_t fsm_entry(fsm_t *me)
{
    int16_t ret;

    ASSERT_REQUIRE(me != (fsm_t *)0);

    /* At this Point, The Current State has been Set */
    ret = TRUE; 
    if (me->curr_state->enter != (fsm_function_t *)0) { 
        ret = (*(me->curr_state->enter))(me); 
    } 
    ASSERT_ENSURE(ret == TRUE); 
    /* Enter Action Handled */
    SPYER_FSM("FSM[%X], State[%s] is Entered. TimeStamp %d", \\
               me, me->curr_state->name, ticks_get()); 
    return ret; 
}

/***************************************************************************************
*   Internal Function, fsm_exit()
***************************************************************************************/
int16_t fsm_exit(fsm_t *me)
{
    int16_t ret;

    ASSERT_REQUIRE(me != (fsm_t *)0);

    ret = TRUE; 
    if (me->curr_state->quit != (fsm_function_t *)0) { 
        ret = (*(me->curr_state->quit))(me); 
    } 
    ASSERT_ENSURE(ret == TRUE); 
    /* Exit Action Handled */
    SPYER_FSM("FSM[%X], State[%s] is Exited, Super[%s]. TimeStamp %d", \\
               me, me->curr_state->name, me->curr_state->top->name, ticks_get()); 
    me->curr_state = me->curr_state->top;   /* Points to Superstate */
    ASSERT_REQUIRE(me->curr_state != (fsm_state_t *)0); 
    if (me->curr_state == (fsm_state_t *)0) { 
        return FAILURE; 
    } 
    return ret; 
}

/***************************************************************************************
*   fsm_init_trans() Implementation.
***************************************************************************************/
int16_t fsm_init_trans(fsm_t *me)
{
    int16_t ret; 

    ASSERT_REQUIRE(me != (fsm_t *)0); 
    if (me == (fsm_t *)0) { 
        return FAILURE; 
    } 
    /* This State Machine Must be Initialized with fsm_initialize() */
    ASSERT_REQUIRE(me->curr_state != (fsm_state_t *)0);
    ASSERT_REQUIRE(me->dest_state == (fsm_state_t *)0);

    /* Set the Work Status */
    me->status_  = FSM_INIT_TRANS;
    /* Drill into the Target... */
    if (me->curr_state->down != (fsm_state_t *)0) { 
        me->curr_state = me->curr_state->down; 
        ret = fsm_entry(me);
        if (ret != TRUE) { 
            return ret; 
        } 
    } 
    /* Set the Work Status */
    me->status_ = FSM_DISPATCH;

    SPYER_FSM("Initial Transition is Done, FSM[%X], Curr[%s]. TimeStamp %d", \\
                me, me->curr_state->name, ticks_get()); 
    return ret; 
}

/***************************************************************************************
*   Internal Function, fsm_find()
***************************************************************************************/
fsm_reaction_t * fsm_find (fsm_t *me, signal_t signal)
{
    int16_t index; 
    fsm_reaction_t *reaction; 

    ASSERT_REQUIRE(me != (fsm_t *)0);

    /* Search for Real Action */
    reaction = me->work_state->reaction; 
    if (reaction != (fsm_reaction_t *)0) { 
        ASSERT_REQUIRE(me->work_state->rtotal > (int16_t)0);
        for (index = 0; index < me->work_state->rtotal; index++) { 
            if (reaction[index].signal == signal) { 

                SPYER_FSM("Signal[%d]'s Reaction[%X] is Find. TimeStamp %d", 
                               signal, &reaction[index], ticks_get()); 

                return (fsm_reaction_t *)&reaction[index];
            } 
        }
    } 
    return (fsm_reaction_t *)0; 
}

/***************************************************************************************
*   Internal Function, fsm_guard()
***************************************************************************************/
int16_t fsm_guard (fsm_t *me, fsm_reaction_t *reaction) 
{ 
    int16_t ret; 

    ASSERT_REQUIRE(me       != (fsm_t *)0);
    ASSERT_REQUIRE(reaction != (fsm_reaction_t *)0);

    ret = TRUE; 
    if (reaction->guard != (fsm_function_t)0) { 
        ret = (*(reaction->guard))(me); 
    }

    SPYER_FSM("Reaction[%X]'s Guard is %d. TimeStamp %d", reaction, ret, ticks_get()); 
    return ret;
}

/***************************************************************************************
*   Internal Function, fsm_action()
***************************************************************************************/
int16_t fsm_action (fsm_t *me, fsm_reaction_t *reaction) 
{ 
    int16_t ret; 

    ASSERT_REQUIRE(me       != (fsm_t *)0);
    ASSERT_REQUIRE(reaction != (fsm_reaction_t *)0);

    ret = TRUE; 
    if (reaction->action != (fsm_function_t)0) { 
        ret = (*(reaction->action))(me); 
    }

    SPYER_FSM("Reaction[%X]'s Action[%X] is Executed. TimeStamp %d", 
               reaction, reaction->action, ticks_get()); 
    return ret; 
}

/***************************************************************************************
*   fsm_dispatch() Implementation.
***************************************************************************************/
int16_t fsm_dispatch(fsm_t *me, event_t *event)
{
    int16_t ret; 
    fsm_reaction_t *reaction; 

    ASSERT_REQUIRE(me    != (fsm_t *)0); 
    ASSERT_REQUIRE(event != (event_t *)0); 
    if((me == (fsm_t *)0) || (event == (event_t *)0)) { 
        return FAILURE; 
    }
    ASSERT_REQUIRE(me->current_state != (hsm_state_t *)0); 
    if (me->current_state == (hsm_state_t *)0) { 
        return FAILURE; 
    } 

    /* State Machine is Running ? */
    if (me->current_state->top == (hsm_state_t *)0) { 
        /* The State Machine is in Final State */
        return TRUE; 
    } 
    /* Make Sure that a Transition will be Noticed */
    me->dest_state = (fsm_state_t *)0; 

    /***********************************************************************************
    *   Process the Event Hierarchically... 
    ***********************************************************************************/
    /* Find the Reaction */
    reaction = fsm_find (me, event->signal); 
    if (reaction == (fsm_reaction_t *)0) { 
        /* Not Find the Reaction that can Response this Event */
        return FALSE; 
    } 
    /* Check the Guard Condition */
    if (fsm_guard(me, reaction) != TRUE) { 
        /* Guard Condition is Not Satisfy, MUST Return TRUE at Here */
        return TRUE; 
    } 
    /* Invoke the Event Action */
    ret = fsm_action (me, reaction); 
    ASSERT_ENSURE(ret == TRUE); 
    /* Set the Target State */
    me->dest_state = reaction->target; 
    me->external   = reaction->external; 
    /***********************************************************************************
    *   Process the State Transition... 
    ***********************************************************************************/
    /* Do the Transitions */
    /* Target is Itself of Source, or Source is the Ancestor of Target */
    if ((me->dest_state != me->current_state) || (me->external == (int16_t)1)) { 
        /* Self-Transition or External Transition */
        ret = fsm_exit(me);  
        if (ret != TRUE) { 
            return ret; 
        } 
        if (me->dest_state->top != (hsm_state_t *)0) {  /* Target to Topmost State ? */
            /* Not Target to Topmost State */ 
            me->current_state = me->dest_state; 
            ret = fsm_entry(me);
            if (ret != TRUE) { 
                return ret; 
            } 
        } 
    }
    /* At this Point, curr_state == dest_state */
    ASSERT_ENSURE(me->current_state == me->dest_state); 
    if (me->current_state != me->dest_state) { 
        return FAILURE; 
    } 

    /* Entire Transition is Completed ( MUST NOT REMOVE IT ) */
    me->dest_state = (fsm_state_t *)0;  /* Clear the Target State */
                                      
    /* Set the Work Status  ( MUST NOT REMOVE IT ) */
    me->status_ = FSM_DISPATCH;

    SPYER_FSM("Event Dispatch is Completed, FSM[%X]. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   fsm_is_in() Implementation.
***************************************************************************************/
int16_t fsm_is_in(fsm_t *me, fsm_state_t *state)
{
    fsm_state_t *start;

    ASSERT_REQUIRE(me != (fsm_t *)0); 
    ASSERT_REQUIRE(state != (fsm_state_t *)0); 
    if (me == (fsm_t *)0) { 
        return FAILURE; 
    } 
    if (state == (fsm_state_t *)0) { 
        return FAILURE; 
    } 

    if(state == me->curr_state) { 
        return TRUE; 
    } 
    /* Match not Found, Return FALSE */
    return FALSE; 
}

/***************************************************************************************
*   fsm_is_final() Implementation.
***************************************************************************************/
int16_t fsm_is_final(fsm_t *me)
{
    ASSERT_REQUIRE(me != (fsm_t *)0); 
    if (me == (fsm_t *)0) { 
        return FAILURE; 
    } 

    if (me->current_state->top == (fsm_state_t *)0) { 
        /* The State Machine is in Final State */
        return TRUE; 
    } 
    return FALSE; 
}
