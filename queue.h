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
*   Group : Data Pointer Queue 
*   File Name : queue.h                 
*   Version : 1.0
*   Description : Base for Data Pointer Queue 
*   Author : JinHui Han
*   History : 
*          Name            Date                    Remarks
*       JinHui Han      02-17-2005      Created Initial Version 1.0
*       JinHui Han      02-17-2005      Verified Version 1.0
***************************************************************************************/

#ifndef _QUEUE_H
#define _QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  EXTERN_GLOBALS
    #define QUEUE_EXT
#else
    #define QUEUE_EXT      extern
#endif

/***************************************************************************************
*   Platform-Independent Pointer Queue Interface.
*
*   This File is needed when the "raw" Thread-Safe Pointer Queues are used for 
*   Communication between Active Objects and Non-Framework Entities, such as ISRs, 
*   Device Drivers, or Legacy Code.
***************************************************************************************/

/***************************************************************************************
*   Native Pointer Queue.
*
*   This Structure Describes the Native Pointer Queue, which can be Used as a Simple 
*   "raw" Pointer Queue for Thread-Safe Data Passing Among Non-Framework Entities, 
*   such as ISRs, Device Drivers, or Other Third-Party Components.
*
*   The queue_t Structure Contains Only Data Members for Managing an Poniter Queue, 
*   but Does not Contain the Storage for the Buffer, which must be Provided Externally 
*   during the Queue Initialization.
*
*   The Pointer Queue can Store Only Data Entity Pointers, not the Whole Data Entity. 
*   The Internal Implementation Uses the Standard Ring-Buffer Plus one External 
*   Location that Optimizes the Queue Operation for the Most Frequent Case of 
*   Empty Queue.
*
*   The "raw" Pointer Queue is not Capable of Blocking on the get() Operation, but is 
*   still Thread-Safe because it uses Critical Section to Protect its Integrity. 
*
*   NOTE: Most Queue Operations Internally Use the Critical Section. You should
*   be Careful not to Invoke those Operations from Other Critical Sections when
*   Nesting of Critical Sections is not Supported.
***************************************************************************************/
typedef struct queue_tag { 
    /***********************************************************************************
    *   Pointer at the Front of the Queue
    *
    *   All Incoming and Outgoing Data Entity Pointer Pass through the "front" Location. 
    *   When the Queue is Empty (which is Most of the Time), the Extra "front" Location
    *   Allows to Bypass the Ring Buffer Altogether, Greatly Optimizing the Performance
    *   of the Queue. Only Bursts of Events Engage the Ring Buffer.
    *
    *   The Additional Role of This Attribute is to Indicate the Empty Status of the 
    *   Queue. The Queue is Empty if the "front" Location is NULL.
    ***********************************************************************************/
    void_t * front;

    /*  Pointer to the Start of the Ring Buffer */
    void_t **ring;
    /*  Total Number of Pointers in the Ring Buffer */
    uint16_t total;
    /*  Offset to where Next Pointer will be Inserted into the Buffer */
    uint16_t head;
    /*  Offset of where Next Pointer will be Extracted from the Buffer */
    uint16_t tail;
    /*  Number of Pointers Currently Present in the Ring Buffer */
    uint16_t used_cnt;
    /***********************************************************************************
    *   Maximum Number of Pointers Ever Present in the Ring Buffer.
    *
    *   This Attribute Remembers the High-Watermark of the Ring Buffer, which Provides
    *   a Valuable Information for Sizing Pointer Queues.
    ***********************************************************************************/
    uint16_t max_cnt;
} queue_t;

/***************************************************************************************
*   Initializes the Native Pointer Queue
*
*   Description: Initializes the Native Pointer Queue. 
*   Argument: me -- Pointer the queue_t Struct to be Initialized. 
*             queue_buffer[] -- The Ring Buffer Storage. 
*             queue_len -- The Length of the Ring Buffer in the Units of Pointer.
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: The Actual Capacity of the Queue is queue_len + 1, Because of the Extra 
*         Location front.
***************************************************************************************/
QUEUE_EXT int16_t queue_init(queue_t *me, void_t *queue_buffer[], uint16_t queue_len);

/***************************************************************************************
*   "raw" Thread-Safe Pointer Queue Implementation for the First-In-First-Out (FIFO)
*   Pointer Posting. 
*
*   Description: Post Pointer into Queue in FIFO Way. 
*   Argument: me -- Pointer the queue_t Struct to be Post Into. 
*             data -- The Data Pointer to be Posting. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) The Function Raises an Assertion if the Native Queue becomes Full and 
*       Cannot Accept the Pointer.
*   (2) You can Call this Function from Any Task Context or ISR Context.
*       Please note that This Function uses Internally a Critical Section.
***************************************************************************************/
QUEUE_EXT int16_t queue_post_FIFO(queue_t *me, void_t *data);

/***************************************************************************************
*   "raw" Thread-Safe Poniter Queue Implementation for the Last-In-First-Out (LIFO) 
*   Pointer Posting.
*
*   Description: Post Pointer into Queue in LIFO Way. 
*   Argument: me -- Pointer the queue_t Struct to be Post Into. 
*             data -- The Data Pointer to be Posting. 
*   Return: 1 is True, 0 is False, -1 is Failure
*
*   NOTE: 
*   (1) The Function Raises an Assertion if the Native Queue becomes Full and Cannot 
*       Accept the Pointer. 
*   (2) The LIFO Policy should be Used Only with Great Caution because it Alters 
*       Order of Events in the Queue.
*   (3) You can Call this Function from Any Task Context or ISR context.
*       Please Note that This Function uses Internally a Critical Section.
***************************************************************************************/
QUEUE_EXT int16_t queue_post_LIFO(queue_t *me, void_t *data);

/***************************************************************************************
*   "raw" Thread-Safe Pointer Queue Implementation for the Get Operation.
*
*   Description: Get Pointer from Queue. 
*   Argument: me -- Pointer the queue_t Struct which Pointer is Get from. 
*   Return: Returns the Pointer if True, Or Else Returns NULL (Queue is Empty or 
*           Exception is Arise). 
*
*   NOTE: 
*   (1) If the Queue has One or more Pointers, the Function Returns the Pointer at 
*       the Front of the Queue, Otherwise it Returns NULL. 
*   (2) You can Call this Function from Any Task Context or ISR Context. 
*       Please Note that This Function uses Internally a Critical Section.
***************************************************************************************/
QUEUE_EXT void_t *queue_get(queue_t *me); 

/***************************************************************************************
*   Inquire The Front Value of the Queue.
*
*   Description: Get the Front of the Queue.
*   Argument: me -- Pointer the queue_t Struct which Front is Get from. 
*   Return: The Front Value of the Queue. 
*
*   NOTE: 
*   (1) You can Call this Function from Any Task Context or ISR Context. 
*       Please Note that This Function uses Internally a Critical Section. 
*   (2) When the Return Value of queue_front() is Not (void_t *)0, But the Return 
*       Value of queue_get() is (void_t *)0, You Must Assert one ERROR by Yourself.
***************************************************************************************/
QUEUE_EXT void_t *queue_front(queue_t *me); 

/***************************************************************************************
*   Get the Margin of the Given Queue.
*
*   Description: Get the Margin of the Given Queue. 
*   Argument: me -- Pointer the queue_t Struct which Margin is Get from. 
*   Return: The Margin of the Given Queue. 
*
*   NOTE: 
*   (1) The Margin is the Minimal Number of Free Pointers Encountered in the Given 
*       Queue since System Startup.
***************************************************************************************/
QUEUE_EXT uint16_t queue_margin(queue_t *me); 

#ifdef SPYER_QUEUE_ENABLE   /* Spy Debuger Enabled */
    #define SPYER_QUEUE(format, ...)   spyer_((const int8_t *)format, ##__VA_ARGS__)
#else                       /* Spy Debuger Disabled */
    #define SPYER_QUEUE(format, ...)   ((void_t)0)
#endif  /* End of SPYER_QUEUE_ENABLE */

#ifdef __cplusplus
}
#endif

#endif      /* End of _QUEUE_H */
