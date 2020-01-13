#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include <stdio.h>
#include <string.h>
#include "dev_sram.h"
#include "dev_zsc31014.h"
#include "drv_pm.h"
#include "drv_pin.h"

/* ***************************************************
 * **************** Base Test Command ****************  
 * *************************************************** */
static void getRTTick(void)
{
    rt_kprintf("SysTick:%u\n", rt_tick_get());
}
FINSH_FUNCTION_EXPORT(getRTTick, get systick);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(getRTTick, get systick);
#endif

void getCoreClock(void)
{
    rt_kprintf("CoreClock:%uHz\n", SystemCoreClockGet());
}
FINSH_FUNCTION_EXPORT(getCoreClock, get CMU frequency);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(getCoreClock, get CMU frequency);
#endif
/* ***************************************************
 * *************************************************** */
