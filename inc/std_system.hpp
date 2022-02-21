#ifndef _STD_SYSTEM_H_
#define _STD_SYSTEM_H_
#include <stdint.h>
#include <stdbool.h>
//
class csr
{
public:
	static const uint32_t mstatus = 0x300;
	static const uint32_t mie = 0x304;
	static const uint32_t mtvec = 0x305;
	static const uint32_t mepc = 0x341;
	static const uint32_t mcause = 0x342;
	static const uint32_t mip = 0x344;
	static const uint32_t pmcfg0 = 0x3a0;
	static const uint32_t pmpaddr0 = 0x3b0;
	static const uint32_t mhartid = 0xf14;
};
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
//
template<const uint32_t addr> inline uint32_t csr_read(void) __attribute__((always_inline));
template<const uint32_t addr> inline void csr_write(uint32_t data) __attribute__((always_inline));
template<const uint32_t addr> inline void csr_clear_bits(uint32_t data) __attribute__((always_inline));
template<const uint32_t addr> inline void csr_set_bits(uint32_t data) __attribute__((always_inline));
//
template<const uint32_t addr> inline uint32_t csr_read(void)
{
	uint32_t temp_loc;
	asm("csrr %0, %1" : "=r"(temp_loc) : "i"(addr));
	return temp_loc;
}
template<const uint32_t addr> inline void csr_write(uint32_t data)
{
	asm("csrw %0, %1" :: "I"(addr), "r"(data));
}
template<const uint32_t addr> inline void csr_clear_bits(uint32_t data)
{
	asm("csrc %0, %1" :: "I"(addr), "r"(data));
}
template<const uint32_t addr> inline void csr_set_bits(uint32_t data)
{
	asm("csrs %0, %1" :: "I"(addr), "r"(data));
}
//
#endif//_STD_SYSTEM_H_
