#ifndef _INC_H_
#define _INC_H_
#include <stdint.h>
//
constexpr uint32_t PLIC_NUM_SOURCES = 64;
//
typedef struct
{
	volatile uint32_t PRIORITY_THRESHOLD;
	volatile uint32_t CLAIM_COMPLETE;
	volatile uint32_t reserved[1022];
} IRQ_Target_Type;
typedef struct
{
	volatile uint32_t ENABLES[32];
} Target_Enables_Type;
typedef struct
{
	volatile uint32_t SOURCE_PRIORITY[1024];
	volatile /*const*/ uint32_t PENDING_ARRAY[32];
	volatile uint32_t RESERVED1[992];
	volatile Target_Enables_Type TARGET_ENABLES[15808];
	volatile uint32_t RESERVED2[16384];
	IRQ_Target_Type TARGET[15872];
} PLIC_Type;
typedef struct
{
	volatile uint32_t txdata;//Transmit data register
	volatile uint32_t rxdata;//Receive data register
	volatile uint32_t txctrl;//Transmit control register
	volatile uint32_t rxctrl;//Receive control register
	volatile uint32_t ie;//UART interrupt enable
	volatile uint32_t ip;//UART Interrupt pending
	volatile uint32_t div;//Baud rate divisor
} sifive_uart_t;
typedef struct
{
	volatile uint32_t CR1;         /*!< TIM control register 1,              Address offset: 0x00 */
	volatile uint32_t CR2;         /*!< TIM control register 2,              Address offset: 0x04 */
	volatile uint32_t SMCR;        /*!< TIM slave mode control register,     Address offset: 0x08 */
	volatile uint32_t DIER;        /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
	volatile uint32_t SR;          /*!< TIM status register,                 Address offset: 0x10 */
	volatile uint32_t EGR;         /*!< TIM event generation register,       Address offset: 0x14 */
	volatile uint32_t CCMR1;       /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
	volatile uint32_t CCMR2;       /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
	volatile uint32_t CCER;        /*!< TIM capture/compare enable register, Address offset: 0x20 */
	volatile uint32_t CNT;         /*!< TIM counter register,                Address offset: 0x24 */
	volatile uint32_t PSC;         /*!< TIM prescaler,                       Address offset: 0x28 */
	volatile uint32_t ARR;         /*!< TIM auto-reload register,            Address offset: 0x2C */
	volatile uint32_t RCR;         /*!< TIM repetition counter register,     Address offset: 0x30 */
	volatile uint32_t CCR1;        /*!< TIM capture/compare register 1,      Address offset: 0x34 */
	volatile uint32_t CCR2;        /*!< TIM capture/compare register 2,      Address offset: 0x38 */
	volatile uint32_t CCR3;        /*!< TIM capture/compare register 3,      Address offset: 0x3C */
	volatile uint32_t CCR4;        /*!< TIM capture/compare register 4,      Address offset: 0x40 */
	volatile uint32_t BDTR;        /*!< TIM break and dead-time register,    Address offset: 0x44 */
	volatile uint32_t DCR;         /*!< TIM DMA control register,            Address offset: 0x48 */
	volatile uint32_t DMAR;        /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
	volatile uint32_t OR;          /*!< TIM option register,                 Address offset: 0x50 */
} stm32_timer_t;
//
class tim_cr1
{
	public:
		static const uint32_t TIM_CR1_CEN = 1;
		static const uint32_t TIM_CR1_ARPE = 1<<7;
};
//
PLIC_Type* const PLIC = reinterpret_cast<PLIC_Type*>(0x20000000);
volatile uint32_t* const CLINT = reinterpret_cast<uint32_t*>(0x24000000);
sifive_uart_t* const UART[4] =
{
	reinterpret_cast<sifive_uart_t*>(0x30000000),
	reinterpret_cast<sifive_uart_t*>(0x30001000),
	reinterpret_cast<sifive_uart_t*>(0x30002000),
	reinterpret_cast<sifive_uart_t*>(0x30003000),
};
stm32_timer_t* const TIMER[4] =
{
	reinterpret_cast<stm32_timer_t*>(0x30004000),
	reinterpret_cast<stm32_timer_t*>(0x30005000),
	reinterpret_cast<stm32_timer_t*>(0x30006000),
	reinterpret_cast<stm32_timer_t*>(0x30007000),
};
//
#endif

