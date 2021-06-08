#ifndef _STD_SYSTEM_H_
#define _STD_SYSTEM_H_
#include <stdint.h>
#include <stdbool.h>
//
#define CSR_MSTATUS						0x300
#define CSR_MIE 						0x304
#define CSR_MTVEC 						0x305
#define CSR_MEPC 						0x341
#define CSR_MCAUSE 						0x342
#define CSR_pmcfg0 						0x3a0
#define CSR_pmpaddr0					0x3b0
#define CSR_MHARTID 					0xf14
//
typedef struct __attribute__((packed, aligned(4)))
{
	uint8_t number;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} version_t;
typedef void(*func_ptr)(void);
//
void delay(uint32_t data);
uint32_t hart_id(void);
void enable_fpu(void);
//
template<uint32_t addr> inline uint32_t csr_read(void) __attribute__((always_inline));
template<const uint32_t addr> inline void csr_write(uint32_t data) __attribute__((always_inline));
template<uint32_t addr> inline uint32_t csr_read(void)
{
	uint32_t temp_loc;
	asm("csrr %0, %1" : "=r"(temp_loc) : "i"(addr));
	return temp_loc;
}
template<const uint32_t addr> inline void csr_write(uint32_t data)
{
	asm("csrw %0, %1" :: "I"(addr), "r"(data));
}

//
#endif//_STD_SYSTEM_H_
