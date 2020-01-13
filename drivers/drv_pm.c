/*
 * @Author: ShenQuan 
 * @Date: 2019-06-14 13:03:11 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-06-14 17:22:09
 */

#include "em_emu.h"
#include "emodes.h"
#include "board.h"
#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_PM)

#ifdef RT_PM_DEBUG
#define pm_debug(format, args...) rt_kprintf(format, ##args)
#else
#define pm_debug(format, args...)
#endif

/**
 * This function will enter sleep mode.
 *
 * @param pm pointer to power manage structure
 */
static void sleep(struct rt_pm *pm, uint8_t mode)
{
    switch (mode)
    {
    case PM_SLEEP_MODE_NONE:
    case PM_SLEEP_MODE_IDLE:
        break;
    case PM_SLEEP_MODE_LIGHT:
        em_EM1_Hfrco(cmuHFRCOBand_1MHz);
        break;
    case PM_SLEEP_MODE_DEEP:
        em_EM2_Lfxo();
        break;
    case PM_SLEEP_MODE_STANDBY:
        em_EM3();
        break;
    case PM_SLEEP_MODE_SHUTDOWN:
        em_EM4();
        break;
    default:
        pm_debug("unknown pm sleep mode.\n");
        RT_ASSERT(0);
    }
}

static void run(struct rt_pm *pm, uint8_t mode)
{
    static uint8_t last_mode;
    if (mode == last_mode)
        return;
    last_mode = mode;

    switch (mode)
    {
    case PM_RUN_MODE_HIGH_SPEED:
        em_EM0_Hfxo();
        break;
    case PM_RUN_MODE_NORMAL_SPEED:
        em_EM1_Hfrco(cmuHFRCOBand_28MHz);
        break;
    case PM_RUN_MODE_MEDIUM_SPEED:
        em_EM1_Hfrco(cmuHFRCOBand_11MHz);
        break;
    case PM_RUN_MODE_LOW_SPEED:
        em_EM1_Hfrco(cmuHFRCOBand_1MHz);
        break;
    default:
        pm_debug("unknown pm run mode.\n");
        RT_ASSERT(0);
    }
    pm_debug("switch to  frequency = %d Hz\n", CMU_ClockFreqGet(cmuClock_HF));
}

static void emu_EM23init(void)
{

    /* Use default settings for EM23 */
    EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
    EMU_EM23Init(&em23Init);
}

//#include "drv_pin.h"
static void emu_EM4init(void)
{
    // rt_uint16_t wakeup_pin = get_PinNumber(GPIO_F_PORT, 2);
    // rt_pin_mode(wakeup_pin, PIN_MODE_INPUT_PULLUP);

    // GPIO_EM4EnablePinWakeup(GPIO_EM4WUEN_EM4WUEN_F2, 0);

    /* Use default settings for EM4 */
    EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
    //EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;
    //GPIO_EM4SetPinRetention(true);
    EMU_EM4Init(&em4Init);
}

/**
 * This function initialize the power manager
 */
int rt_hw_pm_init(void)
{
    emu_EM23init();
    emu_EM4init();

    static const struct rt_pm_ops _ops =
        {
            sleep,
            run,
            RT_NULL,
            RT_NULL,
            RT_NULL,
        };
    rt_uint8_t timer_mask = 0;

    /* initialize timer mask */
    //timer_mask = 1UL << PM_SLEEP_MODE_DEEP;
    //timer_mask = 1UL << PM_SLEEP_MODE_STANDBY;
    /* initialize system pm module */
    rt_system_pm_init(&_ops, timer_mask, RT_NULL);

    return 0;
}
//INIT_BOARD_EXPORT(rt_hw_pm_init);

#include <drv_pin.h>
static void peripheral_pow(uint8_t argc, char **argv)
{
    rt_uint16_t pwr_pin = get_PinNumber(GPIO_D_PORT, 7);
    rt_pin_mode(pwr_pin, PIN_MODE_OUTPUT);
    if (argc < 2)
    {
        rt_kprintf("Usage: peripheral_pow 0 or 1 (off/on)\n");
    }
    else
    {
        bool state = strtol(argv[1], NULL, 0);
        if (state)
        {
            rt_kprintf("peripheral power on \n");
            rt_pin_write(pwr_pin, PIN_HIGH); //高使能
        }
        else
        {
            rt_kprintf("peripheral power off \n");
            rt_pin_write(pwr_pin, PIN_LOW);
        }
    }
}
FINSH_FUNCTION_EXPORT(peripheral_pow, peripheral device power control);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(peripheral_pow, peripheral device power control);
#endif

static void em2(void)
{
    rt_kprintf("enter EM2 mode.\n");
    rt_pm_request(PM_SLEEP_MODE_DEEP);
}
FINSH_FUNCTION_EXPORT(em2, enter em2 mode);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(em2, enter em2 mode);
#endif

static void em4(void)
{
    rt_kprintf("enter EM4 mode.\n");
    rt_uint16_t wakeup_pin = get_PinNumber(GPIO_F_PORT, 2);
    rt_pin_mode(wakeup_pin, PIN_MODE_INPUT_PULLUP);
    GPIO_EM4EnablePinWakeup(GPIO_EM4WUEN_EM4WUEN_F2, 0);

    rt_pm_release(PM_SLEEP_MODE_DEEP);
    rt_pm_request(PM_SLEEP_MODE_SHUTDOWN);
}
FINSH_FUNCTION_EXPORT(em4, enter em4 mode);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(em4, enter em4 mode);
#endif

#endif
