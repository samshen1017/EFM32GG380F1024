#include "emodes.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "board.h"
#include <stdint.h>
#include <stdbool.h>

void em_EM0_Hfxo(void)
{
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  SysTick_Configuration();
}

void em_EM1_Hfrco(CMU_HFRCOBand_TypeDef band)
{
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_HFRCOBandSet(band);
  EMU_EnterEM1();
  SysTick_Configuration();
}

void em_EM2_Lfxo(void)
{
  // Enter EM2.
  EMU_EnterEM2(true);
  SysTick_Configuration();
}

void em_EM3(void)
{
  // Enter EM3.
  EMU_EnterEM3(false);
}

void em_EM4(void)
{
  // Enter EM4.
  EMU_EnterEM4();
}

#if 0
/***************************************************************************/ /**
 * @brief   Disable high frequency clocks
 ******************************************************************************/
static void disableHFClocks(void)
{
  // Disable High Frequency Peripheral Clocks
  CMU_ClockEnable(cmuClock_HFPER, false);
  CMU_ClockEnable(cmuClock_USART0, false);
  CMU_ClockEnable(cmuClock_USART1, false);
  CMU_ClockEnable(cmuClock_USART2, false);
  CMU_ClockEnable(cmuClock_UART0, false);
  CMU_ClockEnable(cmuClock_UART1, false);
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER1, false);
  CMU_ClockEnable(cmuClock_TIMER2, false);
  CMU_ClockEnable(cmuClock_TIMER3, false);
  CMU_ClockEnable(cmuClock_ACMP0, false);
  CMU_ClockEnable(cmuClock_ACMP1, false);
  CMU_ClockEnable(cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_DAC0, false);
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_VCMP, false);
  CMU_ClockEnable(cmuClock_ADC0, false);
  CMU_ClockEnable(cmuClock_I2C0, false);
  CMU_ClockEnable(cmuClock_I2C1, false);

  // Disable High Frequency Core/Bus Clocks
  CMU_ClockEnable(cmuClock_AES, false);
  CMU_ClockEnable(cmuClock_DMA, false);
  CMU_ClockEnable(cmuClock_HFLE, false);
  CMU_ClockEnable(cmuClock_EBI, false);
  CMU_ClockEnable(cmuClock_USB, false);

  // Disable USBC clock by choosing unused oscillator (LFXO)
  CMU_ClockEnable(cmuClock_USBC, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_LFXO);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
  CMU_ClockEnable(cmuClock_USBC, false);
}

/***************************************************************************/ /**
 * @brief   Disable low frequency clocks
 ******************************************************************************/
static void disableLFClocks(void)
{
  // Enable LFXO for Low Frequency Clock Disables
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Disable Low Frequency A Peripheral Clocks
  // Note: LFA clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTC, false);
  CMU_ClockEnable(cmuClock_LETIMER0, false);
  //CMU_ClockEnable(cmuClock_LCD, false);
  CMU_ClockEnable(cmuClock_PCNT0, false);
  CMU_ClockEnable(cmuClock_PCNT1, false);
  CMU_ClockEnable(cmuClock_PCNT2, false);
  CMU_ClockEnable(cmuClock_LESENSE, false);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_Disabled);

  // Disable Low Frequency B Peripheral Clocks
  // Note: LFB clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, false);
  CMU_ClockEnable(cmuClock_LEUART1, false);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  // Disable Low Frequency Oscillator
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
}

/***************************************************************************/ /**
 * @brief   Disable all clocks to achieve lowest current consumption numbers.
 ******************************************************************************/
static void disableClocks(void)
{
  // Disable DBG clock by selecting AUXHFRCO
  CMU_ClockSelectSet(cmuClock_DBG, cmuSelect_AUXHFRCO);

  // Disable High Frequency Clocks
  disableHFClocks();

  // Disable Low Frequency Clocks
  disableLFClocks();
}
#endif
