#include <board.h>
#include <drv_pin.h>
#include "wdt.h"

void rt_led_thread_entry(void *parameter)
{
    rt_kprintf("led thread start!\n");
    //rt_uint16_t led_pin = get_PinNumber(GPIO_A_PORT, 11);
    //rt_pin_mode(led_pin, PIN_MODE_OUTPUT);
    /* 让MAX3471进去节能状态 */
    rt_uint16_t max3471_de_pin = get_PinNumber(GPIO_D_PORT, 1);
    rt_pin_mode(max3471_de_pin, PIN_MODE_OUTPUT);
    rt_pin_write(max3471_de_pin, PIN_LOW);

    /* 关闭外设供电 */
    rt_uint16_t pwr_pin = get_PinNumber(GPIO_D_PORT, 7);
    rt_pin_mode(pwr_pin, PIN_MODE_OUTPUT);
    rt_pin_write(pwr_pin, PIN_LOW);

    while (1)
    {
        // rt_pin_write(led_pin, PIN_HIGH);
        // rt_thread_sleep(1000);
        // rt_pin_write(led_pin, PIN_LOW);
        rt_thread_mdelay(1000);
    }
}

void rt_init_thread_entry(void *parameter)
{
#if defined(RT_USING_W25QXX)
    rt_hw_w25qxx_init();
#endif

#if defined(RT_USING_BLUEMOD_SR)
    rt_hw_bluemod_sr_init();
#endif

#if defined(RT_USING_WDT)
    wdt_init();
#endif

#if defined(RT_USING_PM)
    rt_pm_request(PM_SLEEP_MODE_DEEP);
    rt_pm_run_enter(PM_RUN_MODE_LOW_SPEED);
#endif

    rt_thread_t led_thread;
    led_thread = rt_thread_create("led", rt_led_thread_entry, RT_NULL, 256, 24, 20);
    rt_thread_startup(led_thread);
}

int rt_application_init()
{
    rt_thread_t init_thread;
    init_thread = rt_thread_create("init", rt_init_thread_entry, RT_NULL, 2048, 5, 20);
    rt_thread_startup(init_thread);
    return 0;
}

/***************************************************************************/ /**
 * @}
 ******************************************************************************/
