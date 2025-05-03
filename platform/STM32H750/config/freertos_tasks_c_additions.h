// From Gabriel Staples here: https://stackoverflow.com/a/77568267/4561887
// 
// NB: THIS IS TYPICALLY NOT A HEADER FILE YOU SHOULD INCLUDE IN YOUR CODE. 
// - Treat this like a .c source file, not a header file. 
// - FreeRTOS includes this file instead, at the bottom of tasks.c. 
//   - This way, all our code in this file gets direct access to 
//     private `static` functions and variables in `tasks.c`, 
//     and we can extend the FreeRTOS API without modifying kernel code.
// - To get access to the public API herein, include 
//   "freertos_tasks_c_additions_include.h" instead.

#pragma once

#include "FreeRTOS.h"


#if ((configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H == 1) \
    && (configRECORD_STACK_HIGH_ADDRESS == 1))

// Get the stack size in words, not bytes, first passed to `xTaskCreate()` 
// when the task was created. This is the size of the stack allocated for 
// the task. 
// - Note: due to the alignment that FreeRTOS enforces to the top (end) of
//   the stack via `portBYTE_ALIGNMENT_MASK` in tasks.c, the available stack
//   size is actually one or two bytes or so less than what you input at
//   task creation. FreeRTOS adjusts `pxTopOfStack`, which gets assigned to
//   `pxEndOfStack`, for stack alignment. So, if you passed in 200 words to
//   `xTaskCreate()` (see: https://www.freertos.org/a00125.html) as the
//   `usStackDepth` value, this function may return 199 instead, which is
//   the actual, correct, stack size available. 
// - Partially learned from:
//   https://www.freertos.org/FreeRTOS_Support_Forum_Archive/January_2019/freertos_Retrieve_the_size_and_maximum_usage_of_the_stack_per_task_7ab5c6eb05j.html
// - On a 32-bit mcu, the word size is probably 4 bytes. Check 
//   `sizeof(StackType_t)`, or call `taskGetStackSizeBytes()` to be sure. 
uint32_t taskGetStackSizeWords(TaskHandle_t taskHandle)
{
    uint32_t stackSizeWords;

    TCB_t * tcb = prvGetTCBFromHandle(taskHandle);
    // critical section to access protected TCB (Task Control Block)
    // members from this task's `struct tskTaskControlBlock`
    taskENTER_CRITICAL();
    {
        // +1 to count both the end and start in the calculation
        stackSizeWords = tcb->pxEndOfStack - tcb->pxStack + 1;
    }
    taskEXIT_CRITICAL();

    // Optionally [but not recommended], you may add +1 again to give 
    // us the same value that we input when we created the task; 
    // technically, `portBYTE_ALIGNMENT_MASK` removed one of our bytes 
    // due to alignment, so actually our stack is 1 byte less
    // than our input at creation.
    // stackSizeWords += 1; 

    return stackSizeWords;
}

// Get the stack size in bytes. 
// - See details in the `taskGetStackSizeWords()` function above.
uint32_t taskGetStackSizeBytes(TaskHandle_t taskHandle)
{
    uint32_t stackSizeBytes = 
        taskGetStackSizeWords(taskHandle) * sizeof(StackType_t);
    return stackSizeBytes;
}

#endif // ((configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H == 1)
       //   && (configRECORD_STACK_HIGH_ADDRESS == 1))