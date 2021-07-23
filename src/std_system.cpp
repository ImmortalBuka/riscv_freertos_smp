#include "std_system.hpp"
//
void isr_jumps(void);
void call_ctors(const func_ptr* start, const func_ptr* end);
extern "C"
{
	void Reset_Handler(void);
	void clint_direct_mode_handler(void);
}
//
#if __clang__
volatile const uint8_t compiler_clang[4] __attribute__((section (".version"))) = {'c', __clang_major__, __clang_minor__, __clang_patchlevel__};
#else
	#if defined __GNUC__
volatile const uint8_t compiler_gcc[4] __attribute__((section (".version"))) = {'g', __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__};
	#endif
#endif
//
__attribute__((naked, noreturn)) void Reset_Handler(void)
{
	extern void sys_init(void);
	extern void main(void);
	extern void main_2(void);
	extern uint32_t __etext;
	extern uint32_t __data_start__;
	extern uint32_t __data_end__;
	extern uint32_t __bss_start__;
	extern uint32_t __bss_end__;
	extern func_ptr __cpp_begin[];
	extern func_ptr __cpp_end[];
	//
	uint32_t* pSrc;
	uint32_t* pDest;
	csr_write<csr::mtvec>((uint32_t)clint_direct_mode_handler);
	csr_write<csr::mstatus>(0xaa);
	csr_write<csr::mie>(0xaaa);
	switch(hart_id())
	{
		case 0:
			asm("la sp, __stack_top0");
			pSrc = &__etext;
			pDest = &__data_start__;
			while(pDest < &__data_end__) *pDest++ = *pSrc++;
			pDest = &__bss_start__;
			while(pDest < &__bss_end__) *pDest++ = 0;
			call_ctors(__cpp_begin, __cpp_end);
			asm("j main");
			break;
		case 1:
			asm("la sp, __stack_top1");
			break;
		case 2:
			asm("la sp, __stack_top2");
			break;
		default:
			break;
	}
	asm("j main_2");
}
void call_ctors(const func_ptr* start, const func_ptr* end)
{
	while(end > start)
	{
		if(*start) (*start)();
		start++;
	}
}
void enable_fpu(void)
{
	uint32_t temp_loc = csr_read<csr::mstatus>();
	temp_loc |= (1<<13);
	csr_write<csr::mstatus>(temp_loc);
}
void delay(uint32_t data)
{
	uint32_t temp1 = data;
	while (temp1--) asm("NOP");
}
void memset_word(uint32_t* ptr, uint32_t value, uint32_t size)
{
	for(uint32_t i=0; i<(size>>2); i++) ptr[i] = value;
}
__attribute__((always_inline)) uint32_t hart_id(void)
{
	uint32_t temp_loc = csr_read<csr::mhartid>();
	return temp_loc;
}
__attribute__((optimize("align-functions=4"))) void clint_direct_mode_handler(void)
{
	extern void project_default_handler(uint32_t mcause);
	uint32_t mcause = csr_read<csr::mcause>();
	project_default_handler(mcause);
}
__attribute__ ((section(".isr_vectors"),naked,noreturn)) void isr_jumps(void)
{
	asm("j Reset_Handler");				//init pc
}
