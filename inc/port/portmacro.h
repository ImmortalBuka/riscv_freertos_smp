#include "stdint.h"
#include "projdefs.h"
//
#define portBYTE_ALIGNMENT									4
#define portMAX_DELAY 										0xffffffff
#define portSTACK_GROWTH									(-1)
#define portCRITICAL_NESTING_IN_TCB							1
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) 	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) 		void vFunction( void *pvParameters )
#define portDISABLE_INTERRUPTS()							asm("csrc mstatus, 8")
#define portENABLE_INTERRUPTS()								asm("csrs mstatus, 8")
#define portENTER_CRITICAL()								vTaskEnterCritical()
#define portEXIT_CRITICAL()									vTaskExitCritical()
#define portYIELD() 										asm("ecall")
#define traceMALLOC											print_malloc
#define traceTASK_CREATE									print_task_create
//
typedef uint32_t				StackType_t;
typedef int32_t					BaseType_t;
typedef uint32_t				UBaseType_t;
typedef uint32_t 				TickType_t;
//
StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack, TaskFunction_t pxCode, void* pvParameters);
void vPortSetupTimerInterrupt(void);
void print_malloc(uint32_t address, uint32_t size);
void vTaskEnterCritical(void);
void vTaskExitCritical(void);
BaseType_t xPortStartScheduler(void);
void xPortStartFirstTask(void);
void freertos_risc_v_trap_handler(void);
void timer_irq_handler(void);
void print_task_create(uint32_t in);
