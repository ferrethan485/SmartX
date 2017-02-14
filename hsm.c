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
*   File Name : hsm.c                 
*   Version : 1.0
*   Description : Base for HSM State Machine
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
#include "hsm.h"

ASSERT_THIS_FILE(hsm.c)
FAULT_THIS_FILE(hsm.c)

/***************************************************************************************
*   hsm_get_version() Implementation.
***************************************************************************************/
int8_t const ROM_KEYWORDS *hsm_get_version(void_t)
{
    static int8_t const ROM_KEYWORDS hsm_version[] = "HSM 1.0.00";
    return hsm_version;
}

/***************************************************************************************
*   hsm_destroyer() Implementation.
***************************************************************************************/
int16_t hsm_destroyer (hsm_t *me)
{
    ASSERT_REQUIRE(me != (hsm_t *)0);
    if(me == (hsm_t *)0) { 
        return FAILURE; 
    }
    /* For the Basal HSM, it is NOP Operation. */
    me->status_ = HSM_IDLE; 

    SPYER_HSM("HSM[%X] is Destroyed, Curr %d, Dest %d. TimeStamp %d", \\
               me, me->curr_state, me->dest_state, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   hsm_constructor() Implementation.
***************************************************************************************/
int16_t hsm_constructor (hsm_t *me) 
{
    ASSERT_REQUIRE(me != (hsm_t *)0);
    if(me == (hsm_t *)0) { 
        return FAILURE; 
    }
    /* For the Basal HSM, it is NOP Operation */
    me->status_ = HSM_CONSTRUCT; 

    SPYER_HSM("HSM[%X] is Construct, Curr %d, Dest %d. TimeStamp %d", \\
               me, me->curr_state, me->dest_state, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   hsm_initializer() Implementation.
***************************************************************************************/
int16_t hsm_initializer (hsm_t *me) 
{
    ASSERT_REQUIRE(me != (hsm_t *)0);
    if(me == (hsm_t *)0) { 
        return FAILURE; 
    }
    /* For the Basal HSM, Initialize the HSM Object Properties and Method */
    me->curr_state = me->curr_state;    /* NOP Operation */
    me->dest_state = me->dest_state;    /* NOP Operation */
    me->work_state = (hsm_state_t *)0; 
    me->external   = me->external;      /* NOP Operation */
    me->status_ = HSM_INITIALIZE; 
    me->constructor = me->constructor;  /* NOP Operation */
    me->destroyer   = me->destroyer;    /* NOP Operation */
    me->initializer = me->initializer;  /* NOP Operation */
    me->init_trans  = hsm_init_trans; 
    me->dispatch    = hsm_dispatch; 
    me->service     = me->service;      /* NOP Operation */
    me->is_in       = hsm_is_in; 
    me->is_idle     = me->is_idle;      /* NOP Operation */
    me->is_final    = hsm_is_final; 

    SPYER_HSM("HSM[%X] is Initialized, Curr %d, Dest %d. TimeStamp %d", \\
               me, me->curr_state, me->dest_state, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   Internal Function, hsm_entry()
***************************************************************************************/
int16_t hsm_entry(hsm_t *me)
{
    int16_t ret;

    ASSERT_REQUIRE(me != (hsm_t *)0);

    /* At this Point, The Current State has been Set */
    ret = TRUE; 
    if (me->curr_state->enter != (hsm_function_t *)0) { 
        ret = (*(me->curr_state->enter))(me); 
    } 
    ASSERT_ENSURE(ret == TRUE); 
    /* Enter Action Handled */
    SPYER_HSM("HSM[%X], State[%s] is Entered. TimeStamp %d", \\
               me, me->curr_state->name, ticks_get()); 
    return ret; 
}

/***************************************************************************************
*   Internal Function, hsm_exit()
***************************************************************************************/
int16_t hsm_exit(hsm_t *me)
{
    int16_t ret;

    ASSERT_REQUIRE(me != (hsm_t *)0);

    ret = TRUE; 
    if (me->curr_state->quit != (hsm_function_t *)0) { 
        ret = (*(me->curr_state->quit))(me); 
    } 
    ASSERT_ENSURE(ret == TRUE); 
    /* Exit Action Handled */
    SPYER_HSM("HSM[%X], State[%s] is Exited, Super[%s]. TimeStamp %d", \\
               me, me->curr_state->name, me->curr_state->top->name, ticks_get()); 
    me->curr_state = me->curr_state->top;   /* Points to Superstate */
    ASSERT_REQUIRE(me->curr_state != (hsm_state_t *)0); 
    if (me->curr_state == (hsm_state_t *)0) { 
        return FAILURE; 
    } 
    return ret; 
}

/***************************************************************************************
*   Internal Function, hsm_drill_target()
***************************************************************************************/
int16_t hsm_drill_target(hsm_t *me)
{
    ASSERT_REQUIRE(me != (hsm_t *)0); 
    ASSERT_REQUIRE(me->curr_state != (hsm_state_t *)0); 

    /* Drill into the Target... */
    while (me->curr_state->down != (hsm_state_t *)0) { 
        me->curr_state = me->curr_state->down; 
        ret = hsm_entry(me);
        if (ret != TRUE) { 
            return ret; 
        } 
    } 
    return TRUE; 
}

/***************************************************************************************
*   hsm_init_trans() Implementation.
***************************************************************************************/
int16_t hsm_init_trans(hsm_t *me)
{
    int16_t ret; 

    ASSERT_REQUIRE(me != (hsm_t *)0); 
    if (me == (hsm_t *)0) { 
        return FAILURE; 
    } 
    /* This State Machine Must be Initialized with hsm_initialize() */
    ASSERT_REQUIRE(me->curr_state != (hsm_state_t *)0);
    ASSERT_REQUIRE(me->dest_state == (hsm_state_t *)0);

    /* Set the Work Status */
    me->status_  = HSM_INIT_TRANS;
    /* Drill into the Target... */
    ret = hsm_drill_target(me);
    /* Set the Work Status */
    me->status_ = HSM_DISPATCH;

    SPYER_HSM("Initial Transition is Done, HSM[%X], Curr[%s]. TimeStamp %d", \\
                me, me->curr_state->name, ticks_get()); 
    return ret; 
}

/***************************************************************************************
*   Internal Function, hsm_transition()
***************************************************************************************/
/***************************************************************************************
*   According to the http://www.w3.org/TR/scxml/#SelectingTransitions:
*   (1) The Behavior of a Transition with 'type' of "external" (the Default) is 
*       Defined in Terms of the Transition's Source State (which is the State that 
*       Contains the Transition), the Transition's Target State(or States), and 
*       the Least Common Compound Ancestor (LCCA) of the Source and Target States 
*       When a Transition is Taken, the State Machine will Exit all Active States 
*       that are Proper Descendants of the LCCA, Starting with the Innermost One(s) 
*       and Working Up to the Immediate Descendant(s) of the LCCA. 
*       Then the State Machine Enters the Target State(s), plus Any States that are 
*       Between it and the LCCA, Starting with the Outermost One (i.e., the Immediate 
*       Descendant of the LCCA) and Working Down to the Target State(s). 
*       As States are Exited, Their "on exit" Handlers are Executed. Then the Executable 
*       Content in the Transition is Executed, Followed by the ¡°on entry¡± Handlers of 
*       the States that are Entered. 
*       If the Target State(s) of the Transition is not Atomic, the State Machine will 
*       Enter Their Default Initial States Recursively until it Reaches an Atomic State(s).
*       NOTE that the LCCA is neither Entered nor Exited. 
*   (2) When Target is a Descendant of Source....
*       "An Internal Transition will not Exit and Re-Enter its Source State, while an 
*       External one Will."
*   (3) For a Transition whose Target is its Source State.... 
*       "The State is Exited and Re-Entered, Triggering Execution of its "on entry"
*       and "on exit" Executable Content." 
*       Self-Transition: The Source Targeted Itself. 
***************************************************************************************/
int16_t hsm_transition(hsm_t *me)
{
    int16_t  ret; 
    hsm_state_t *track;                     /* Track State */ 
    hsm_state_t *stack[HSM_MAX_DEPTH+1];    /* Stack for Transition Entry Path */
    int16_t      index;                     /* Stack Index */

    ASSERT_REQUIRE(me != (hsm_t *)0);

    ASSERT_REQUIRE(me->dest_state->depth < HSM_MAX_DEPTH);
    index = (int16_t)0;     /* Clear the Stack Index */
    track = me->dest_state; 
    /* If Source Deep than Target, *exit* Up to the Same Level As Target */
    while(me->current_state->depth > track->depth) {
        ret = hsm_exit(me);  
        if (ret != TRUE) { 
            return ret; 
        } 
    }
    /* If Target Deep than Source, *record* its Path Up to the Same Level As Source */
    while(track->depth > me->current_state->depth ) {
        stack[index++]= track;
        track = track->top;
        ASSERT_REQUIRE(track != (hsm_state_t *)0); 
        if (track == (hsm_state_t *)0) { 
            return FAILURE; 
        } 
    } 
    /* Target is Itself of Source, or Source is the Ancestor of Target */
    if (track == me->current_state) { 
        if (me->external) {
            /* Self-Transition or External Transition */
            ret = hsm_exit(me);  
            if (ret != TRUE) { 
                return ret; 
            } 
            stack[index++]= track; 
            track = track->top;
        } 
    }
    /* Keep Going Up Together till Current and Track have Found Each Other. */
    /* Keep Exiting 'current' as it Goes Up, Keep Stracking 'track' as it Goes Up. */
    while (me->current_state != track) { 
        ret = hsm_exit(me);  
        if (ret != TRUE) { 
            return ret; 
        } 
        stack[index++]= track;
        track = track->top;
        ASSERT_REQUIRE(track != (hsm_state_t *)0); 
        if (track == (hsm_state_t *)0) { 
            return FAILURE; 
        } 
    }      
    /* <--- NOTE: In UML, Transitions Actions would Take Place Here */
    /* Turtles all the Way Down with Path that we Just Recorded */
    while (index > 0) {
        me->current_state = stack[--index]; 
        ret = hsm_entry(me);
        if (ret != TRUE) { 
            return ret; 
        } 
    }
    /* At this Point, curr_state == dest_state */
    ASSERT_ENSURE(me->current_state == me->dest_state); 
    if (me->current_state != me->dest_state) { 
        return FAILURE; 
    } 
    return TRUE; 
}

/***************************************************************************************
*   Internal Function, hsm_find()
*   Bubble the Event Up the Hierarchy Until We Get a Valid Reaction or Until We 
*   Run Off the Top of the Tree. 
***************************************************************************************/
hsm_reaction_t * hsm_find (hsm_t *me, signal_t signal)
{
    int16_t index; 
    hsm_reaction_t *reaction; 

    ASSERT_REQUIRE(me != (hsm_t *)0);

    me->work_state = me->current_state; 
    /* Recursively Find the Dispatch Entry */
    while(me->work_state != (hsm_state_t *)0) { 
        /* Search for Real Action */
        reaction = me->work_state->reaction; 
        if (reaction != (hsm_reaction_t *)0) { 
            ASSERT_REQUIRE(me->work_state->rtotal > (int16_t)0);
            for (index = 0; index < me->work_state->rtotal; index++) { 
                if (reaction[index].signal == signal) { 

                    SPYER_HSM("Signal[%d]'s Reaction[%X] is Find. TimeStamp %d", 
                               signal, &reaction[index], ticks_get()); 

                    return (hsm_reaction_t *)&reaction[index];
                } 
            }
        } 
        me->work_state = me->work_state->top; 
    }
    return (hsm_reaction_t *)0; 
}

/***************************************************************************************
*   Internal Function, hsm_guard()
***************************************************************************************/
int16_t hsm_guard (hsm_t *me, hsm_reaction_t *reaction) 
{ 
    int16_t ret; 

    ASSERT_REQUIRE(me       != (hsm_t *)0);
    ASSERT_REQUIRE(reaction != (hsm_reaction_t *)0);

    ret = TRUE; 
    if (reaction->guard != (hsm_function_t)0) { 
        ret = (*(reaction->guard))(me); 
    }

    SPYER_HSM("Reaction[%X]'s Guard is %d. TimeStamp %d", reaction, ret, ticks_get()); 
    return ret;
}

/***************************************************************************************
*   Internal Function, hsm_action()
***************************************************************************************/
int16_t hsm_action (hsm_t *me, hsm_reaction_t *reaction) 
{ 
    int16_t ret; 

    ASSERT_REQUIRE(me       != (hsm_t *)0);
    ASSERT_REQUIRE(reaction != (hsm_reaction_t *)0);

    ret = TRUE; 
    if (reaction->action != (hsm_function_t)0) { 
        ret = (*(reaction->action))(me); 
    }

    SPYER_HSM("Reaction[%X]'s Action[%X] is Executed. TimeStamp %d", 
               reaction, reaction->action, ticks_get()); 
    return ret; 
}

/***************************************************************************************
*   hsm_dispatch() Implementation.
***************************************************************************************/
int16_t hsm_dispatch(hsm_t *me, event_t *event)
{
    int16_t ret; 
    hsm_reaction_t *reaction; 

    ASSERT_REQUIRE(me    != (hsm_t *)0); 
    ASSERT_REQUIRE(event != (event_t *)0); 
    if((me == (hsm_t *)0) || (event == (event_t *)0)) { 
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
    me->dest_state = (hsm_state_t *)0; 

    /***********************************************************************************
    *   Process the Event Hierarchically... 
    ***********************************************************************************/
    /* Find the Reaction */
    reaction = hsm_find (me, event->signal); 
    if (reaction == (hsm_reaction_t *)0) { 
        /* Not Find the Reaction that can Response this Event */
        return FALSE; 
    } 
    /* Check the Guard Condition */
    if (hsm_guard(me, reaction) != TRUE) { 
        /* Guard Condition is Not Satisfy, MUST Return TRUE at Here */
        return TRUE; 
    } 
    /* Recover the Context of Super State */
    /* Above (1) Specifically Designates the LCCA of 'source' and 'target'. which can 
     * Only, then, be the Same Node as 'source' or, More Likely, a Node Less Deep than 
     * 'source'. Since 'source' is the Node that Handled the Event, and Since Events 
     * Propogate from 'current' Up to the Top of the Tree that Also Means 'source' is 
     * Either the Same Node as, or Higher Than, 'current'.
     * ie. LCCA->depth <= source->depth <= current->depth
     * Therefore: As a First Step, Bring 'current' to 'source', and Work Towards LCCA 
     * from There. 
     */
    /* NOTE: In UML, The External Transitions Actions would Take after "on exit" 
     *       Handlers. 
     */
    while (me->work_state != me->current_state) { 
        hsm_exit(me); 
    } 
    /* Invoke the Event Action */
    ret = hsm_action (me, reaction); 
    ASSERT_ENSURE(ret == TRUE); 
    /* Set the Target State */
    me->dest_state = reaction->target; 
    me->external   = reaction->external; 
    /***********************************************************************************
    *   Process the State Transition... 
    ***********************************************************************************/
    /* Do the Transitions */
    ret = hsm_transition(hsm_t *me); 
    ASSERT_ENSURE(ret == TRUE); 
    if ((ret != TRUE)) { 
        return ret; 
    }
    /* Drill into the Target Hierarchy... */
    ret = hsm_drill_target(me);
    ASSERT_ENSURE(ret == TRUE); 
    if ((ret != TRUE)) { 
        return ret; 
    }

    /* Entire Transition is Completed ( MUST NOT REMOVE IT ) */
    me->dest_state = (hsm_state_t *)0;  /* Clear the Target State */
                                      
    /* Set the Work Status  ( MUST NOT REMOVE IT ) */
    me->status_ = HSM_DISPATCH;

    SPYER_HSM("Event Dispatch is Completed, HSM[%X]. TimeStamp %d", me, ticks_get()); 
    return TRUE; 
}

/***************************************************************************************
*   hsm_is_in() Implementation.
***************************************************************************************/
int16_t hsm_is_in(hsm_t *me, hsm_state_t *state)
{
    hsm_state_t *start;

    ASSERT_REQUIRE(me != (hsm_t *)0); 
    ASSERT_REQUIRE(state != (hsm_state_t *)0); 
    if (me == (hsm_t *)0) { 
        return FAILURE; 
    } 
    if (state == (hsm_state_t *)0) { 
        return FAILURE; 
    } 

    start = me->curr_state;
    while (start != (hsm_state_t *)0) { 
        if(start == state) { 
            return TRUE; 
        } 
        start = start->top;
    } 
    /* Match not Found, Return FALSE */
    return FALSE; 
}

/***************************************************************************************
*   hsm_is_final() Implementation.
***************************************************************************************/
int16_t hsm_is_final(hsm_t *me)
{
    ASSERT_REQUIRE(me != (hsm_t *)0); 
    if (me == (hsm_t *)0) { 
        return FAILURE; 
    } 

    if (me->current_state->top == (hsm_state_t *)0) { 
        /* The State Machine is in Final State */
        return TRUE; 
    } 
    return FALSE; 
}

/***************************************************************************************
*   hsm_topmost_enter() Implementation. 
*   Used to start the statemachine going. 
*   The Spec says Enter Runs before Init. 
*   The Specified Starting State isn't Necessarily the Topmost State: We Need to 
*   Walk Up to the Topmost State, then Walk Down to and Including Our First State. 
***************************************************************************************/
int16_t hsm_topmost_enter (hsm_t *me, hsm_state_t *state)
{
    if (state != (hsm_state_t *)0) {
        hsm_topmost_enter(me, state->top); 
        me->curr_state = state->down; 
        hsm_entry(me); 
    } 
    return TRUE; 
}
