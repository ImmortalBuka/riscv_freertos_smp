#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include <periph.h>
//
uint32_t portDISABLE_INTERRUPTS(void);
//
uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute((aligned(4)));
//pxCurrentTCBs
//
uint32_t portDISABLE_INTERRUPTS(void)
{
	return 0;
}
void portRESTORE_INTERRUPTS(uint32_t arg)
{
	(void)arg;
}
BaseType_t portCHECK_IF_IN_ISR(void)
{
	return pdFALSE;
}
void portRELEASE_ISR_LOCK(void)
{
}
void portRELEASE_TASK_LOCK(void)
{
}
void portENABLE_INTERRUPTS(void)
{
}
void portGET_TASK_LOCK(void)
{
}
void portGET_ISR_LOCK(void)
{
}
void portYIELD_CORE(uint32_t id)
{
	(void)id;
}
void portYIELD(void)
{
}
StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack, TaskFunction_t pxCode, void* pvParameters)
{
	(void)pxTopOfStack;
	(void)pxCode;
	(void)pvParameters;
	return 0;
}
BaseType_t xPortStartScheduler(void)
{
	return pdFALSE;
}
TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
	return 0;
}
