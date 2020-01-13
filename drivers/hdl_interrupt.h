/***************************************************************************/ /**
 * @file    hdl_interrupt.h
 * @brief   Interrupt handler of RT-Thread RTOS for EFM32
 *  COPYRIGHT (C) 2012, RT-Thread Development Team
 * @author  onelife
 * @version 1.0
 *******************************************************************************
 * @section License
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rt-thread.org/license/LICENSE
 *******************************************************************************
 * @section Change Logs
 * Date         Author      Notes
 * 2010-12-29   onelife     Initial creation for EFM32
 * 2011-12-09   onelife     Add LEUART module support
 * 2019-06-21	墨鸿苍穹	 Add efm32_irq_type_uart
 ******************************************************************************/
#ifndef __HDL_INTERRUPT_H__
#define __HDL_INTERRUPT_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
enum efm32_irq_hook_type_t
{
	efm32_irq_type_dma = 0,
	efm32_irq_type_rtc,
	efm32_irq_type_timer,
	efm32_irq_type_letimer,
	efm32_irq_type_gpio,
	efm32_irq_type_acmp,
	efm32_irq_type_usart,
	efm32_irq_type_uart,
	efm32_irq_type_leuart,
	efm32_irq_type_iic
};

typedef void (*efm32_irq_callback_t)(void *usrdata);

typedef struct
{
	enum efm32_irq_hook_type_t type;
	rt_uint8_t unit;
	efm32_irq_callback_t cbFunc;
	void *userPtr;
} efm32_irq_hook_init_t;

typedef struct
{
	efm32_irq_callback_t cbFunc;
	void *userPtr;
} efm32_irq_hook_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/***************************************************************************/ /**
 * @brief
 *   Perform bit-band operation on peripheral memory location.
 *
 * @details
 *   Bit-banding provides atomic read-modify-write cycle for single bit
 *   modification. Please refer to the reference manual for further details
 *   about bit-banding.
 *
 * @param[in,out] addr Peripheral address location to modify bit in.
 *
 * @param[in] bit Bit position to modify, 0-31.
 *
 * @param[in] val Value to set bit to, 0 or 1.
 ******************************************************************************/
__STATIC_INLINE void BITBAND_Peripheral(volatile uint32_t *addr,
										uint32_t bit,
										uint32_t val)
{
	uint32_t tmp = BITBAND_PER_BASE + (((uint32_t)addr - PER_MEM_BASE) * 32) + (bit * 4);

	*((volatile uint32_t *)tmp) = (uint32_t)val;
}

/***************************************************************************/ /**
 * @brief
 *   Perform bit-band operation on SRAM memory location.
 *
 * @details
 *   Bit-banding provides atomic read-modify-write cycle for single bit
 *   modification. Please refer to the reference manual for further details
 *   about bit-banding.
 *
 * @param[in,out] addr SRAM address location to modify bit in.
 *
 * @param[in] bit Bit position to modify, 0-31.
 *
 * @param[in] val Value to set bit to, 0 or 1.
 ******************************************************************************/
__STATIC_INLINE void BITBAND_SRAM(uint32_t *addr, uint32_t bit, uint32_t val)
{
	uint32_t tmp = BITBAND_RAM_BASE + (((uint32_t)addr - RAM_MEM_BASE) * 32) + (bit * 4);

	*((volatile uint32_t *)tmp) = (uint32_t)val;
}

/* Exported functions ------------------------------------------------------- */
//void NMI_Handler(void);
//void MemManage_Handler(void);
//void BusFault_Handler(void);
//void UsageFault_Handler(void);
//void SVC_Handler(void);
//void DebugMon_Handler(void);
void DMA_IRQHandler_All(rt_uint32_t channel, rt_bool_t primary, void *user);
void efm32_irq_hook_register(efm32_irq_hook_init_t *hook);

#endif /* __HDL_INTERRUPT_H__ */
