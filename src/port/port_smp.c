#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include <periph.h>
//
extern void* volatile pxCurrentTCBs[configNUM_CORES];
extern uint32_t read_mcause(void);
//
uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute((aligned(4)));
uint32_t message[3] = {0};
//
StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack, TaskFunction_t pxCode, void* pvParameters)
{
	extern uint32_t read_mstatus(void);
	uint32_t reg = read_mstatus();
	reg &= ~(1<<3);
	reg |= 0x1880;
	pxTopOfStack--;
	*pxTopOfStack = reg;
	pxTopOfStack -= 22;
	*pxTopOfStack = (uint32_t)pvParameters;
	pxTopOfStack -= 6;
	*pxTopOfStack = 0;
	pxTopOfStack--;
	*pxTopOfStack = (uint32_t)pxCode;
	return pxTopOfStack;
}
void vPortSetupTimerInterrupt(void)
{
	PLIC->TARGET_ENABLES[1].ENABLES[0] = (1<<1)|(1<<5);
	TIMER0->ARR = 999;
	TIMER0->PSC = 9999;
	TIMER0->DIER = 1;
	TIMER0->CR1 = (TIM_CR1_CEN|TIM_CR1_ARPE);
}
BaseType_t xPortStartScheduler(void)
{
	extern void write_mtvec(uint32_t in);
	vPortSetupTimerInterrupt();
	asm("csrs mie, %0" :: "r"(0x880));
	write_mtvec((uint32_t)&freertos_risc_v_trap_handler);
	xPortStartFirstTask();
	asm("wfi");
	return pdFAIL;
}
void timer_irq_handler(void)
{
	uint32_t temp_loc = PLIC->TARGET[1].CLAIM_COMPLETE;
	TIMER0->SR = 0;
	xTaskIncrementTick();
	vTaskSwitchContext(0);
	PLIC->TARGET[1].CLAIM_COMPLETE = temp_loc;
}
__attribute__((naked, optimize("align-functions=4"))) void freertos_risc_v_trap_handler(void)
{
	asm("addi sp, sp, -120");
	asm("sw x1, 1*4(sp)");
	asm("sw x5, 2*4(sp)");
	asm("sw x6, 3*4(sp)");
	asm("sw x7, 4*4(sp)");
	asm("sw x8, 5*4(sp)");
	asm("sw x9, 6*4(sp)");
	asm("sw x10, 7*4(sp)");
	asm("sw x11, 8*4(sp)");
	asm("sw x12, 9*4(sp)");
	asm("sw x13, 10*4(sp)");
	asm("sw x14, 11*4(sp)");
	asm("sw x15, 12*4(sp)");
	asm("sw x16, 13*4(sp)");
	asm("sw x17, 14*4(sp)");
	asm("sw x18, 15*4(sp)");
	asm("sw x19, 16*4(sp)");
	asm("sw x20, 17*4(sp)");
	asm("sw x21, 18*4(sp)");
	asm("sw x22, 19*4(sp)");
	asm("sw x23, 20*4(sp)");
	asm("sw x24, 21*4(sp)");
	asm("sw x25, 22*4(sp)");
	asm("sw x26, 23*4(sp)");
	asm("sw x27, 24*4(sp)");
	asm("sw x28, 25*4(sp)");
	asm("sw x29, 26*4(sp)");
	asm("sw x30, 27*4(sp)");
	asm("sw x31, 28*4(sp)");
	asm("csrr t0, mstatus");/* Required for MPIE bit. */
	asm("sw t0, 29*4(sp)");
	asm("lw t0, %[data]":: [data] "m" (pxCurrentTCBs[0]));/* Load pxCurrentTCB. */
	asm("sw sp, 0( t0 )");/* Write sp to first TCB member. */
	asm("csrr a0, mcause");
	asm("csrr a1, mepc");
	if(read_mcause() & (1<<31))
	{
		asm("sw a1, 0(sp)");
		asm("la sp, __stack_top0");/* Switch to ISR stack before function call. */
		timer_irq_handler();/* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
	}
	else
	{
		asm("addi a1, a1, 4");/* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
		asm("sw a1, 0(sp)");/* Save updated exception return address. */
		asm("li t0, 11");/* 11 == environment call. */
		asm("la sp, __stack_top0");/* Switch to ISR stack before function call. */
		vTaskSwitchContext(0);
		//CLINT[1] = 1;
		//CLINT[2] = 1;
		/*vTaskSwitchContext();
		pxCurrentTCB_by_core[1] = pxCurrentTCB;
		vTaskSwitchContext();
		pxCurrentTCB_by_core[2] = pxCurrentTCB;*/
	}
	asm("lw t1, %[data]":: [data] "m" (pxCurrentTCBs[0]));/* Load pxCurrentTCB. */
	asm("lw sp, 0(t1)");/* Read sp from first TCB member. */
	asm("lw t0, 0(sp)");/* Load mret with the address of the next instruction in the task to run next. */
	asm("csrw mepc, t0");
	asm("lw t0, 29*4(sp)");/* Load mstatus with the interrupt enable bits used by the task. */
	asm("csrw mstatus, t0");/* Required for MPIE bit. */
	asm("lw x1, 1*4(sp)");
	asm("lw x5, 2*4(sp)");/* t0 */
	asm("lw x6, 3*4(sp)");/* t1 */
	asm("lw x7, 4*4(sp)");/* t2 */
	asm("lw x8, 5*4(sp)");/* s0/fp */
	asm("lw x9, 6*4(sp)");/* s1 */
	asm("lw x10, 7*4(sp)");/* a0 */
	asm("lw x11, 8*4(sp)");/* a1 */
	asm("lw x12, 9*4(sp)");/* a2 */
	asm("lw x13, 10*4(sp)");/* a3 */
	asm("lw x14, 11*4(sp)");/* a4 */
	asm("lw x15, 12*4(sp)");/* a5 */
	asm("lw x16, 13*4(sp)");/* a6 */
	asm("lw x17, 14*4(sp)");/* a7 */
	asm("lw x18, 15*4(sp)");/* s2 */
	asm("lw x19, 16*4(sp)");/* s3 */
	asm("lw x20, 17*4(sp)");/* s4 */
	asm("lw x21, 18*4(sp)");/* s5 */
	asm("lw x22, 19*4(sp)");/* s6 */
	asm("lw x23, 20*4(sp)");/* s7 */
	asm("lw x24, 21*4(sp)");/* s8 */
	asm("lw x25, 22*4(sp)");/* s9 */
	asm("lw x26, 23*4(sp)");/* s10 */
	asm("lw x27, 24*4(sp)");/* s11 */
	asm("lw x28, 25*4(sp)");/* t3 */
	asm("lw x29, 26*4(sp)");/* t4 */
	asm("lw x30, 27*4(sp)");/* t5 */
	asm("lw x31, 28*4(sp)");/* t6 */
	asm("addi sp, sp, 120");
	asm("mret");
}
__attribute__((naked)) void xPortStartFirstTask(void)
{
	asm("lw sp, %[data]":: [data] "m" (pxCurrentTCBs[0]));/* Load pxCurrentTCB. */
	asm("lw sp, 0(sp)");/* Read sp from first TCB member. */
	asm("lw x1, 0(sp)");/* Note for starting the scheduler the exception return address is used as the function return address. */
	asm("lw x6, 3*4(sp)");/* t1 */
	asm("lw x7, 4*4(sp)");/* t2 */
	asm("lw x8, 5*4(sp)");/* s0/fp */
	asm("lw x9, 6*4(sp)");/* s1 */
	asm("lw x10, 7*4(sp)");/* a0 */
	asm("lw x11, 8*4(sp)");/* a1 */
	asm("lw x12, 9*4(sp)");/* a2 */
	asm("lw x13, 10*4(sp)");/* a3 */
	asm("lw x14, 11*4(sp)");/* a4 */
	asm("lw x15, 12*4(sp)");/* a5 */
	asm("lw x16, 13*4(sp)");/* a6 */
	asm("lw x17, 14*4(sp)");/* a7 */
	asm("lw x18, 15*4(sp)");/* s2 */
	asm("lw x19, 16*4(sp)");/* s3 */
	asm("lw x20, 17*4(sp)");/* s4 */
	asm("lw x21, 18*4(sp)");/* s5 */
	asm("lw x22, 19*4(sp)");/* s6 */
	asm("lw x23, 20*4(sp)");/* s7 */
	asm("lw x24, 21*4(sp)");/* s8 */
	asm("lw x25, 22*4(sp)");/* s9 */
	asm("lw x26, 23*4(sp)");/* s10 */
	asm("lw x27, 24*4(sp)");/* s11 */
	asm("lw x28, 25*4(sp)");/* t3 */
	asm("lw x29, 26*4(sp)");/* t4 */
	asm("lw x30, 27*4(sp)");/* t5 */
	asm("lw x31, 28*4(sp)");/* t6 */
	asm("lw x5, 29*4(sp)");/* Initial mstatus into x5 (t0) */
	asm("addi x5, x5, 0x08");/* Set MIE bit so the first task starts with interrupts enabled - required as returns with ret not eret. */
	asm("csrrw x0, mstatus, x5");/* Interrupts enabled from here! */
	asm("lw x5, 2*4(sp)");/* Initial x5 (t0) value. */
	asm("addi	sp, sp, 120");
	asm("ret");
}
uint32_t portGET_CORE_ID(void)
{
	extern uint32_t get_core_id(void);
	return get_core_id();
}
uint32_t portDISABLE_INTERRUPTS(void)
{
	asm("csrc mstatus, 8");
	return 0;
}
void portYIELD_CORE(uint32_t id)
{
	portYIELD();
}
BaseType_t portCHECK_IF_IN_ISR(void)
{
	if(read_mcause()) return pdTRUE;
	else return pdFALSE;
}
void portGET_TASK_LOCK(void)
{
}
void portGET_ISR_LOCK(void)
{
}
void portRELEASE_ISR_LOCK(void)
{
}
void portRELEASE_TASK_LOCK(void)
{
}
void portRESTORE_INTERRUPTS(uint32_t arg)
{
	(void)arg;
	portENABLE_INTERRUPTS();
}
TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
	return pxCurrentTCBs[0];
}
