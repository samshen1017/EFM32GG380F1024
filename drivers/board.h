/***************************************************************************/ /**
 * @file 	board.h
 * @brief 	Board support of RT-Thread RTOS for EFM32
 *  COPYRIGHT (C) 2012, RT-Thread Development Team
 * @author 	onelife
 * @version 1.0
 *******************************************************************************
 * @section License
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rt-thread.org/license/LICENSE
 *******************************************************************************
 * @section Change Logs
 * Date			Author		Notes
 * 2010-12-21	onelife		Initial creation for EFM32
 * 2011-05-06	onelife		Add EFM32 development kit and SPI Flash support
 * 2011-07-12	onelife		Add prototype for SWO output enable and interrupt
 *  context check functions
 * 2011-12-08	onelife		Add giant gecko development kit support
 * 2011-12-09	onelife		Add giant gecko support
 * 2011-12-09   onelife     Add LEUART module support
 * 2011-12-14   onelife     Add LFXO enabling routine in driver initialization
 *  function
 * 2011-12-20   onelife     Move SPI Auto-CS setting to "rtconfig.h"
 * 2012-05-15	onelife		Modified to compatible with CMSIS v3
 ******************************************************************************/
#ifndef __BOARD_H__
#define __BOARD_H__

/* Includes ------------------------------------------------------------------*/
#include <efm32.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_rmu.h>
#include <em_emu.h>
#include <em_dma.h>
#include <em_ebi.h>
#include <em_rtc.h>
#include <em_timer.h>
#include <em_letimer.h>
#include <em_gpio.h>
#include <em_acmp.h>
#include <em_adc.h>
#include <em_usart.h>
#include <em_leuart.h>
#include <em_i2c.h>

#include <rtthread.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern volatile rt_uint32_t rt_system_status;

#define EFM32_NO_DATA (0)
#define EFM32_NO_POINTER (RT_NULL)
#define EFM32_NO_OFFSET (-1)
#define EFM32_NO_DMA (-1)

/* SECTION: SYSTEM */
#define EFM32_SRAM_END (SRAM_BASE + SRAM_SIZE)
#define EFM32_BASE_PRI_DEFAULT (0x0UL << 5)
#define EFM32_IRQ_PRI_DEFAULT (0x4UL << 5)

#define RT_USING_EXT_SRAM
#ifdef RT_USING_EXT_SRAM
#define EFM32_EXT_SRAM_BEGIN 0x84000000
#define EFM32_EXT_SRAM_END 0x84080000
/* Use 192kB for external heap */
#define EFM32_EXT_HEAP_BEGIN (EFM32_EXT_SRAM_BEGIN)
#define EFM32_EXT_HEAP_END (EFM32_EXT_HEAP_BEGIN + 384 * 1024)
struct rt_memheap ext_heap;
#endif

/* SECTION: CLOCK */
#define EFM32_USING_HFXO
#define EFM32_USING_LFXO

#if defined(EFM32_USING_HFXO)
#define EFM32_HFXO_FREQUENCY (48000000)
#endif

#if defined(EFM32_USING_LFXO)
#define EFM32_LETIMER_TOP_100HZ (41)
#endif

#define RT_DEVICE_CTRL_USART_RBUFFER (0xF1)  /*!< set USART/UART rx buffer */
#define RT_DEVICE_CTRL_LEUART_RBUFFER (0xF2) /*!< set LEUART rx buffer */

/* Exported functions ------------------------------------------------------- */
void rt_hw_board_init(void);
void rt_hw_driver_init(void);
rt_uint32_t rt_hw_interrupt_check(void);
rt_uint32_t SysTick_Configuration(void);
#endif /*__BOARD_H__ */
