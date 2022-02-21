#include "stdint.h"
#include "projdefs.h"
//
#define portBYTE_ALIGNMENT									4
#define portMAX_DELAY 										0xffffffff
#define portSTACK_GROWTH									(-1)
#define portCRITICAL_NESTING_IN_TCB							1
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) 	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) 		void vFunction( void *pvParameters )
#define portENTER_CRITICAL()								vTaskEnterCritical()
#define portEXIT_CRITICAL()									vTaskExitCritical()
#define traceMALLOC											freertos_print_malloc
#define traceTASK_CREATE									freertos_print_task_create
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
void freertos_risc_v_trap_handler(void);
void timer_irq_handler(void);
uint32_t portGET_CORE_ID(void);
uint32_t portDISABLE_INTERRUPTS(void);
void portYIELD_CORE(uint32_t id);
BaseType_t portCHECK_IF_IN_ISR(void);
void portGET_TASK_LOCK(void);
void portGET_ISR_LOCK(void);
void portRELEASE_ISR_LOCK(void);
void portRELEASE_TASK_LOCK(void);
void portRESTORE_INTERRUPTS(uint32_t arg);
void portENABLE_INTERRUPTS(void);
void portYIELD(void);
void freertos_print_malloc(uint32_t address, uint32_t size);
void freertos_print_task_create(uint32_t in);
