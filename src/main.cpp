#include <periph.hpp>
#include "std_system.hpp"
#include "print.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "fifo.hpp"
#include <string.h>
//
typedef float float32_t;
//
extern "C"
{
	void main(void);
	void freertos_print_malloc(uint32_t address, uint32_t size);
	void freertos_print_task_create(uint32_t in);
	void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName);
	uint32_t portGET_CORE_ID(void);
	/*void portENABLE_INTERRUPTS(void);
	void portRESTORE_INTERRUPTS(uint32_t arg);
	void portYIELD(void);
	void portGET_TASK_LOCK(void);
	void portGET_ISR_LOCK(void);
	void portRELEASE_ISR_LOCK(void);
	void portRELEASE_TASK_LOCK(void);
	void portYIELD_CORE(uint32_t id);
	BaseType_t portCHECK_IF_IN_ISR(void);
	TaskHandle_t xTaskGetCurrentTaskHandle(void);
	StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack, TaskFunction_t pxCode, void* pvParameters);
	BaseType_t xPortStartScheduler(void);
	uint32_t read_mcause(void);*/
}
void project_default_handler(uint32_t mcause);
void uart_init(sifive_uart_t* uart);
void uart_print_char(void* uart, uint8_t data);
void task_01(void* pvParameters);
void task_02(void* pvParameters);
void task_03(void* pvParameters);
void uart_print_sized(void* uart, const char* string, uint8_t size);
void uart_irq(uint8_t i);
void func_02(uint8_t test_num);
void start_freertos(void);
void uart32_print(uint32_t id, char* text, uint32_t data);
//
volatile const version_t version __attribute__ ((section (".version"))) =
{
	.number = 0,
	.day = 0x19,
	.month = 0x04,
	.year = 0x22,
};
uint32_t parameter = 0;
volatile uint32_t clint_flag[4] = {0};
//
void start_freertos(void)
{
	BaseType_t ret_values;
	ret_values = xTaskCreate(task_01, "01", configMINIMAL_STACK_SIZE, &parameter, 1, NULL);
	if(ret_values != pdPASS) uart_print_string(UART[0], "task_01 start error\r\n");
	ret_values = xTaskCreate(task_02, "02", configMINIMAL_STACK_SIZE, &parameter, 2, NULL);
	if(ret_values != pdPASS) uart_print_string(UART[0], "task_02 start error\r\n");
	ret_values = xTaskCreate(task_03, "03", configMINIMAL_STACK_SIZE, &parameter, 3, NULL);
	if(ret_values != pdPASS) uart_print_string(UART[0], "task_03 start error\r\n");
	vTaskStartScheduler();
	uart_print_string(UART[0], "insufficient RAM\r\n");
}
__attribute__((noreturn, naked)) void main(void)
{
	if(hart_id() == 0)
	{
		for(uint8_t i=0; i<4; i++)
		{
			uart_init(static_cast<sifive_uart_t*>(UART[i]));
			PLIC->TARGET_ENABLES[i].ENABLES[0] = (1<<(i+1))|PLIC->TARGET_ENABLES[i].ENABLES[0];
			uart_print_string(UART[i], "init done\n");
		}
		for(uint8_t i=0; i<4; i++)
		{
			//vPortSetupTimerInterrupt(i);
			CLINT[i] = 1;
		}
		start_freertos();
	}
	while(1)
	{
		asm("wfi");
	}
}
void task_01(void* pvParameters)
{
	while(1)
	{
		(*static_cast<uint32_t*>(pvParameters))++;
		uart_print_string(UART[hart_id()], "task_01\r\n");
		vTaskDelay(2);
	}
}
void task_02(void* pvParameters)
{
	while(1)
	{
		(*static_cast<uint32_t*>(pvParameters))++;
		uart_print_string(UART[hart_id()], "task_02\r\n");
		vTaskDelay(1);
	}
}
void task_03(void* pvParameters)
{
	char string[11];
	uint32_t this_id;
	while(1)
	{
		this_id = hart_id();
		uart_print_string(UART[this_id], "task_03\r\nparameter: ");
		uint32_to_hex_string(*static_cast<uint32_t*>(pvParameters), string);
		uart_print_string(UART[this_id], string);
		uart_print_string(UART[this_id], "\r\n");
		vTaskDelay(3);
	}
}
void project_default_handler(uint32_t mcause)
{
	uint32_t this_id = hart_id();
	char string[11];
	uint32_t temp_loc;
	uart_print_string(UART[this_id], "IRQ(core ");
	uart_print_char(UART[this_id], this_id + 0x30);
	uart_print_string(UART[this_id], "): ");
	uint32_to_hex_string(mcause, string);
	uart_print_string(UART[this_id], string);
	uart_print_string(UART[this_id], "\r\n");
	if(mcause & (1<<31))//interrupt
	{
		switch(mcause & 0xffff)
		{
			case 3:
				CLINT[this_id] = 0;
				clint_flag[this_id] = 1;
			break;
			case 9:
			case 11:
				temp_loc = PLIC->TARGET[this_id].CLAIM_COMPLETE;
				switch(temp_loc)
				{
					case 1:
					case 2:
					case 3:
					case 4:
						uart_irq(temp_loc-1);
					break;
					case 5:
					case 6:
					case 7:
					case 8:
						TIMER[temp_loc-5]->SR = 0;
					break;
					default:
					break;
				}
				uart_print_string(UART[this_id], "interrupt ");
				uint32_to_string(temp_loc, string);
				uart_print_string(UART[this_id], string);
				uart_print_string(UART[this_id], "\r\n");
				PLIC->TARGET[this_id].CLAIM_COMPLETE = temp_loc;
			break;
		}
	}
	else//exception
	{
		uint16_to_string(mcause & 0xffff, string);
		uart_print_string(UART[this_id], "exception ");
		uart_print_string(UART[this_id], string);
		uart_print_string(UART[this_id], "\r\nSP ");
		asm("mv %0, sp" : "=r"(temp_loc));
		uint32_to_hex_string(temp_loc, string);
		uart_print_string(UART[this_id], string);
		uart_print_string(UART[this_id], "\r\nPC ");
		temp_loc = csr_read<csr::mepc>();
		uint32_to_hex_string(temp_loc, string);
		uart_print_string(UART[this_id], string);
		uart_print_string(UART[this_id], "\r\n");
		csr_write<csr::mepc>(temp_loc + 4);
	}
}
void uart_irq(uint8_t i)
{
	sifive_uart_t* ptr = static_cast<sifive_uart_t*>(UART[i]);
	while(ptr->ip)
	{
		(void)ptr->rxdata;
	}
}
void uart_init(sifive_uart_t* uart)
{
	uart->txctrl = 1;
	uart->rxctrl = 1;
	uart->ie = 1<<1;
}
void uart_print_char(void* uart, uint8_t data)
{
	sifive_uart_t* ptr = static_cast<sifive_uart_t*>(uart);
	while(ptr->txdata & (1<<31)){};
	ptr->txdata=data;
}
void uart_print_sized(void* uart, const char* string, uint8_t size)
{
	for(uint8_t i=0; i<size; i++) uart_print_char(uart, string[i]);
}
void func_02(uint8_t test_num)
{
	constexpr uint8_t size = 4;
	uint8_t test_fifo_buffer[256];
	fifo<uint8_t> test_fifo(test_fifo_buffer, size);
	uint32_t temp_loc;
	uint8_t idx = 0xaa;
	test_fifo.print_debug(UART[0]);
	if(test_num == 0)
	{
		for(uint32_t i=0; i<(size+2); i++)
		{
			temp_loc = test_fifo.write(&idx, 1);
			test_fifo.print_debug(UART[0]);
			if(temp_loc == 0) uart_print_string(UART[0], "fail to wr - not enough space\r\n");
		}
		for(uint32_t i=0; i<10; i++)
		{
			temp_loc = test_fifo.read(&idx, 1);
			test_fifo.print_debug(UART[0]);
			if(temp_loc == 0) uart_print_string(UART[0], "fail to rd - no more data\r\n");
		}
	}
	if(test_num == 1)
	{
		for(uint32_t i=0; i<size; i++)
		{
			temp_loc = test_fifo.write(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "wr skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.write(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "wr skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.write(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "wr skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.read(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "rd skip\r\n");
			else test_fifo.print_debug(UART[0]);
		}
	}
	if(test_num == 2)
	{
		for(uint32_t i=0; i<size; i++)
		{
			temp_loc = test_fifo.write(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "wr skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.write(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "wr skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.read(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "rd skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.read(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "rd skip\r\n");
			else test_fifo.print_debug(UART[0]);
			temp_loc = test_fifo.read(&idx, 1);
			if(temp_loc == 0) uart_print_string(UART[0], "rd skip\r\n");
			else test_fifo.print_debug(UART[0]);
		}
	}
}
void uart32_print(uint32_t id, char* text, uint32_t data)
{
	char string[11];
	uart_print_string(UART[id], text);
	uint32_to_hex_string(data, string);
	uart_print_string(UART[id], string);
	uart_print_string(UART[id], "\n\r");
}
//FreeRTOS port
void freertos_print_malloc(uint32_t address, uint32_t size)
{
	char string[11];
	uart_print_string(UART[3], "malloc ");
	uint32_to_hex_string(address, string);
	uart_print_string(UART[3], string);
	uart_print_string(UART[3], " size ");
	uint32_to_string(size, string);
	uart_print_string(UART[3], string);
	uart_print_string(UART[3], "\n\r");
}
void freertos_print_task_create(uint32_t in)
{
	char string[11];
	uart_print_string(UART[3], "task create ");
	uint32_to_hex_string(in, string);
	uart_print_string(UART[3], string);
	uart_print_string(UART[3], "\n\r");
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
	(void)xTask;
	uart_print_string(UART[3], "stack overflow for ");
	uart_print_string(UART[3], pcTaskName);
	uart_print_string(UART[3], "\n\r");
}
uint32_t portGET_CORE_ID(void)
{
	return hart_id();
}
