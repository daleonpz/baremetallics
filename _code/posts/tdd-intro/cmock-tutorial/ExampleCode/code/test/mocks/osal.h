#ifndef _OSAL_H
#define _OSAL_H

#include <stdint.h> 

typedef uint16_t TickType_t;
typedef void (*TaskFunction_t)(void *param);
typedef int *TaskHandle_t;
typedef long BaseType_t;

typedef enum {
    eNoAction = 0,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite,
    eSetValueWithoutOverwrite,
} eNotifyAction;


void xTaskCreate(const char* name, TaskFunction_t task_func, 
        void *arg, size_t stack_size,  
        int priority, 
        TaskHandle_t task_handle);

void xTaskNotify( TaskHandle_t handle, uint32_t value, eNotifyAction eAction);
BaseType_t xTaskNotifyWait( uint32_t, uint32_t, uint32_t *, TickType_t );

#define portMAX_DELAY                   ( TickType_t )0xffff
#define configASSERT( x )

#define OS_ASSERT               configASSERT
#define OS_BASE_TYPE            BaseType_t
#define OS_TASK                 TaskHandle_t
#define OS_TASK_PRIORITY_NORMAL 2
#define OS_TASK_NOTIFY_FOREVER  portMAX_DELAY
#define OS_NOTIFY_SET_BITS      eSetBits


#define OS_TASK_NOTIFY_WAIT(entry_bits, exit_bits, value, ticks_to_wait) \
                                xTaskNotifyWait((entry_bits), (exit_bits), (value), \
                                                                                (ticks_to_wait))

#define OS_TASK_CREATE(name, task_func, arg, stack_size, priority, task) \
    xTaskCreate( (name), (task_func), (arg), (stack_size), \
            (priority), (task))

#define OS_TASK_NOTIFY(task, value, action) xTaskNotify((task), (value), (action))


#endif  /* _OSAL_H*/
