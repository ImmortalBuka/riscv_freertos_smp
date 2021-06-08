#include <periph.h>
#include "std_system.hpp"
#include "print.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
//
typedef float float32_t;
class base
{
public:
	uint32_t a;
	void func(void)
	{
		a++;
	}
	base(void) : a(10)
	{};
};
class child : public base
{
	public:
		uint32_t a;
		void func(void)
		{
			base::func();
			a++;
		}
		child(void) : a(20)
		{};
		child(uint32_t in) : a(in)
		{};
};
//
extern "C"
{
	void main(void);
	void main_2(void);
	uint32_t read_mstatus(void);
	void print_malloc(uint32_t address, uint32_t size);
	void write_mtvec(uint32_t in);
	uint32_t read_mcause(void);
	void print_task_create(uint32_t in);
	void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName);
}
void project_default_handler(uint32_t mcause);
void uart_init(sifive_uart_t* uart);
void uart_print_char(void* uart, uint8_t data);
void task_01(void* pvParameters);
void task_02(void* pvParameters);
void task_03(void* pvParameters);
void print_uarts(void);
void uart_print_sized(void* uart, uint8_t* string, uint8_t size);
void uart_irq(uint8_t i);
void sys_wr(uint32_t addr, uint32_t data);
void func_01(void);
uint16_t func_02(uint16_t in);
void func_03(void);
//
volatile const version_t version __attribute__ ((section (".version"))) =
{
	.number = 0,
	.day = 0x04,
	.month = 0x05,
	.year = 0x21,
};
child one(30);
void* uart_ptrs[] = {UART0, UART1, UART2, UART3};
uint32_t parameter = 0;
//
__attribute__((noreturn)) void main(void)
{
	BaseType_t ret_values;
	enable_fpu();
	for(uint8_t i=0; i<4; i++)
	{
		uart_init((sifive_uart_t*)uart_ptrs[i]);
		uart_print_string(uart_ptrs[i], (uint8_t*)"init done\n");
	}
#if 0
	//timer irq for all cores
	PLIC->TARGET_ENABLES[1].ENABLES[0] = (1<<1)|(1<<5);
	PLIC->TARGET_ENABLES[3].ENABLES[0] = 1<<2;
	PLIC->TARGET_ENABLES[5].ENABLES[0] = 1<<3;
	uart_print_string(uart_ptrs[0], (uint8_t*)"back\r\n");
	TIMER0->ARR = 999;
	TIMER0->PSC = 9999;
	TIMER0->DIER = 1;
	TIMER0->CR1 = (TIM_CR1_CEN|TIM_CR1_ARPE);
#endif
	ret_values = xTaskCreate(task_01, "01", configMINIMAL_STACK_SIZE * 2U, &parameter,
		(tskIDLE_PRIORITY + 2), NULL);
	if(ret_values != pdPASS) uart_print_string(UART0, (uint8_t*)"task_01 start error\r\n");
	ret_values = xTaskCreate(task_02, "02", configMINIMAL_STACK_SIZE * 2U, &parameter,
		(tskIDLE_PRIORITY + 1), NULL);
	if(ret_values != pdPASS) uart_print_string(UART0, (uint8_t*)"task_02 start error\r\n");
	ret_values = xTaskCreate(task_03, "03", configMINIMAL_STACK_SIZE * 2U, &parameter,
		(tskIDLE_PRIORITY + 2), NULL);
	if(ret_values != pdPASS) uart_print_string(UART0, (uint8_t*)"task_03 start error\r\n");
	vTaskStartScheduler();
	uart_print_string(UART0, (uint8_t*)"insufficient RAM\r\n");
	while(1){}
}
void func_01(void)
{
	uint8_t string[256];
	one.func();
	uint32_to_string(one.a, string);
	uart_print_string(UART0, string);
	uart_print_string(UART0, (uint8_t*)"\r\n");
	uint32_to_string(one.base::a, string);
	uart_print_string(UART0, string);
	uart_print_string(UART0, (uint8_t*)"\r\n");
}
uint16_t func_02(uint16_t in)
{
	uint16_t temp_loc_1 = (in & 0xff) << 8;
	uint16_t temp_loc_2 = (in & 0xff00) >> 8;
	return (temp_loc_1|temp_loc_2);
}
void func_03(void)
{
	float32_t temp_loc = 1.0f;
	uint8_t string[256];
	float32_to_string(temp_loc, string, 3);
	uart_print_string(UART0, (uint8_t*)"print float: ");
	uart_print_string(UART0, string);
	uart_print_string(UART0, (uint8_t*)"\r\n");
}
void print_uarts(void)
{
	uint32_t this_id = hart_id();
	uart_init(UART0);
	if(this_id < 3)
	{
		uart_init((sifive_uart_t*)uart_ptrs[this_id]);
		uart_print_string(uart_ptrs[this_id], (uint8_t*)"cpu ");
		uart_print_char(uart_ptrs[this_id], this_id + 0x30);
		uart_print_string(uart_ptrs[this_id], (uint8_t*)"\r\n");
	}
	else
	{
		uart_print_string(UART0, (uint8_t*)"unknown cpu\r\n");
	}
}
void task_01(void* pvParameters)
{
	while(1)
	{
		(*((uint32_t*)pvParameters))++;
		uart_print_string(uart_ptrs[hart_id()], (uint8_t*)"task_01\r\n");
		vTaskDelay(2);
	}
}
void task_02(void* pvParameters)
{
	while(1)
	{
		(*((uint32_t*)pvParameters))++;
		uart_print_string(uart_ptrs[hart_id()], (uint8_t*)"task_02\r\n");
		vTaskDelay(1);
	}
}
void task_03(void* pvParameters)
{
	uint8_t string[11];
	uint32_t this_id;
	while(1)
	{
		this_id = hart_id();
		uart_print_string(uart_ptrs[this_id], (uint8_t*)"task_03\r\nparameter: ");
		uint32_to_hex_string(*((uint32_t*)pvParameters), string);
		uart_print_string(uart_ptrs[this_id], string);
		uart_print_string(uart_ptrs[this_id], (uint8_t*)"\r\n");
		vTaskDelay(3);
	}
}
void project_default_handler(uint32_t mcause)
{
	uint32_t this_id = hart_id();
	uint8_t string[11];
	uint32_t temp_loc;
	uart_print_string(uart_ptrs[this_id], (uint8_t*)"IRQ(core ");
	uart_print_char(uart_ptrs[this_id], this_id + 0x30);
	uart_print_string(uart_ptrs[this_id], (uint8_t*)"): ");
	uint32_to_hex_string(mcause, string);
	uart_print_string(uart_ptrs[this_id], string);
	uart_print_string(uart_ptrs[this_id], (uint8_t*)"\r\n");
	if(mcause & (1<<31))//interrupt
	{
		switch(mcause & 0xffff)
		{
			case 3:
				CLINT[this_id] = 0;
			break;
			case 11:
				this_id = (this_id << 1) + 1;
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
						TIMER0->SR = 0;
					break;
					default:
					break;
				}
				uart_print_string(uart_ptrs[this_id], (uint8_t*)"interrupt: ");
				uint32_to_string(temp_loc, string);
				PLIC->TARGET[this_id].CLAIM_COMPLETE = temp_loc;
				uart_print_string(uart_ptrs[this_id], string);
				uart_print_string(uart_ptrs[this_id], (uint8_t*)"\r\n");
			break;
		}
	}
	else//exception
	{
		uart_print_string(uart_ptrs[this_id], (uint8_t*)"exception\r\nPC: ");
		temp_loc = csr_read<CSR_MEPC>();
		uint32_to_hex_string(temp_loc, string);
		uart_print_string(uart_ptrs[this_id], string);
		uart_print_string(uart_ptrs[this_id], (uint8_t*)"\r\n");
		csr_write<CSR_MEPC>(temp_loc + 4);
	}
}
void uart_irq(uint8_t i)
{
	sifive_uart_t* ptr = (sifive_uart_t*)uart_ptrs[i];
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
	sifive_uart_t* ptr = (sifive_uart_t*)uart;
	while(ptr->txdata & (1<<31)){};
	ptr->txdata=data;
}
void uart_print_sized(void* uart, uint8_t* string, uint8_t size)
{
	for(uint8_t i=0; i<size; i++) uart_print_char(uart, string[i]);
}
void sys_wr(uint32_t addr, uint32_t data)
{
	volatile uint32_t* ptr = (uint32_t*)addr;
	*ptr = data;
}
__attribute__((noreturn, naked)) void main_2(void)
{
	while(1)
	{
		asm("wfi");
		//asm("ecall");
	}
}
uint32_t read_mstatus(void)
{
	return csr_read<CSR_MSTATUS>();
}
uint32_t read_mcause(void)
{
	return csr_read<CSR_MCAUSE>();
}
void write_mtvec(uint32_t in)
{
	csr_write<CSR_MTVEC>(in);
}
void print_malloc(uint32_t address, uint32_t size)
{
	uint8_t string[11];
	uart_print_string(UART3, (uint8_t*)"malloc ");
	uint32_to_hex_string(address, string);
	uart_print_string(UART3, string);
	uart_print_string(UART3, (uint8_t*)" size ");
	uint32_to_string(size, string);
	uart_print_string(UART3, string);
	uart_print_string(UART3, (uint8_t*)"\n\r");
}
void print_task_create(uint32_t in)
{
	uint8_t string[11];
	uart_print_string(UART3, (uint8_t*)"task create ");
	uint32_to_hex_string(in, string);
	uart_print_string(UART3, string);
	uart_print_string(UART3, (uint8_t*)"\n\r");
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
	(void)xTask;
	uart_print_string(UART3, (uint8_t*)"stack overflow for ");
	uart_print_string(UART3, (uint8_t*)pcTaskName);
	uart_print_string(UART3, (uint8_t*)"\n\r");
}
