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
*   File Name : activex.h
*   Version : 1.0
*   Description : Based for Active Object Engine
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      03-03-2005      Created Initial Version 1.0
*       JinHui Han      03-03-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _ACTIVEX_H
#define _ACTIVEX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define ACTIVEX_EXT
#else
    #define ACTIVEX_EXT  extern
#endif

/***************************************************************************************
*   Return the Pointer of Active Object's Version.
*
*   This Function Returns Constant Version String in the Format x.y.zz, 
*   Where x (One Digit) is the Major Version, y (One Digit) is the Minor Version, 
*   and zz (Two Digits) is the Maintenance Release Version.
***************************************************************************************/
MANAGER_EXT int8_t const ROM_KEYWORDS * active_version(void_t); 

/***************************************************************************************
*   Return the Pointer of Active Object's Name.
*
*   This Function Returns Constant Version String in the Format x.y.zz, 
*   Where x (One Digit) is the Major Version, y (One Digit) is the Minor Version, 
*   and zz (Two Digits) is the Maintenance Release Version.
***************************************************************************************/
MANAGER_EXT int8_t const ROM_KEYWORDS * active_name(void_t); 

/***************************************************************************************
*   Quit the Active Object.
*
*   Description: Quit the Active Object and Flush Out the Event Chain. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) The Preferred Way of Exit the Active Object is from within the Active Object 
*       that Needs to Exit. In other words, an Active Object should Exit itself Rather 
*       than being Stopped by Some Other Entity (But Other Entity can Trigger one Exit 
*       Request). This Policy Works Best, Because Only the Active Object itself "knows" 
*       when it has Reached the Appropriate State for the Shutdown.
*   (2) The Exit of the Active Object is Optional, Because the Active Objects never 
*       Need to be Exit in Generic Embedded Systems.
*   (3) Due to the Latency of Event Queues, After Un-Subscribing from All Events, 
*       The Events might be Already in the Queue, or Just about to be Posted and 
*       the Un-Subscribe Operation will not Flush Such Events. So This Function 
*       Flush Out the Event Queue After Remove the Object's Register Internally.
*   (4) Also, the Alternative Event-Delivery Mechanisms, Such as Direct Event Posting 
*       or Time Events, can be Still Delivered to the Event Queue of the Active Object. 
*       So the Object Should Remove the Associated Resource, Such as Timer etc, 
*       By Itself.
*   (5) When the Error is Generated During Program Execute Due to Some Reason, Such As:
*       EMI etc. The Assert Mechanism will Capture this Error or Exception, The Error 
*       Manager Will Call This Force Object Exit, Set the Hardware into Security Status,
*       and Release the Resource which Allocated at Object Initialize.
***************************************************************************************/
ACTIVEX_EXT int16_t active_quit(active_t *me);

/***************************************************************************************
*   Reset the Active Object. 
*
*   Description: Reset the Active Object. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Function MUST be Called after active_start().
***************************************************************************************/
ACTIVEX_EXT int16_t active_reset(active_t *me);

/***************************************************************************************
*   Pause the Run of Active Object. 
*
*   Description: Pause the Run of Active Object. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Function MUST be Called after active_start().
***************************************************************************************/
ACTIVEX_EXT int16_t active_pause(active_t *me);

/***************************************************************************************
*   Resume the Run of Active Object. 
*
*   Description: Resume the Run of Active Object. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Function MUST be Called after active_start().
***************************************************************************************/
ACTIVEX_EXT int16_t active_resume(active_t *me);

/***************************************************************************************
*   Query the Idle State of Active Object. 
*
*   Description: Query the Idle State of Active Object. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  Return 1 If the Active Object is in Idle State, Or Else 0 is Return. 
*
*   NOTE: 
*   (1) This Function MUST be Called after active_start().
***************************************************************************************/
ACTIVEX_EXT int16_t active_is_idle(active_t *me);

/***************************************************************************************
*   Start the Active Object.
*
*   Description: Start the Active Object and Must be Called Exactly once Before Any 
*                Other Operation. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
***************************************************************************************/
ACTIVEX_EXT int16_t active_start(active_t *me);

/***************************************************************************************
*   Probe the Suitability of Active Object. 
*
*   Description: Probe the Suitability of Active Object for Dynamic Load Active Object. 
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Function MUST be Called after active_start().
***************************************************************************************/
ACTIVEX_EXT int16_t active_probe(active_t *me); 

/***************************************************************************************
*   Runner for the Active Object.
*
*   Description: Run the Active Object, The Manager Schedule the Run of Active Object
*                through This Function.
*   Argument: me --- The Pointer of Active Object. 
*   Return:  1 is True, 0 is False, -1 is Failure 
*
*   NOTE: 
*   (1) This Function MUST be Called after active_start().
***************************************************************************************/
ACTIVEX_EXT int16_t active_run(active_t *me);

#ifdef SPYER_ACTIVEX_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_ACTIVEX(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spyer Debuger Disabled */
    #define SPYER_ACTIVEX(format, ...)   ((void_t)0)
#endif  /* End of SPYER_ACTIVEX_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _ACTIVEX_H */