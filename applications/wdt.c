#include "wdt.h"

#ifdef RT_USING_WDT
static rt_device_t wdg_dev = RT_NULL;
static rt_thread_t wdt_th = RT_NULL;

static void wdt_thread_entry(void *parameter)
{
    while (1)
    {
        rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
        rt_thread_mdelay(1000);
    }
}

rt_err_t wdt_init(void)
{
    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(WDT_NAME);
    RT_ASSERT(wdg_dev != RT_NULL);
    rt_device_init(wdg_dev);
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    wdt_th = rt_thread_create("wdt", wdt_thread_entry, RT_NULL, 384, 24, 20);
    rt_thread_startup(wdt_th);
    return RT_EOK;
}
#endif
