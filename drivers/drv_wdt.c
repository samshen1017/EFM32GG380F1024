#include "drv_wdt.h"

#ifdef RT_USING_WDT

static struct rt_watchdog_device _device;

static rt_err_t efm32gg_wdt_init(rt_watchdog_t *watchdog)
{
    return RT_EOK;
}
static rt_err_t efm32gg_wdt_control(rt_watchdog_t *watchdog, int cmd, void *arg)
{
    switch (cmd)
    {
    case RT_DEVICE_CTRL_WDT_START:
    {
        WDOG_Enable(RT_TRUE);
    }
    break;
    case RT_DEVICE_CTRL_WDT_GET_TIMEOUT:
    {
        RT_ASSERT(arg != RT_NULL);
        /* Max timeout is 8 seconds */
        *(int *)arg = 8;
    }
    break;
    case RT_DEVICE_CTRL_WDT_KEEPALIVE:
    {
        WDOG_Feed();
    }
    break;
    case RT_DEVICE_CTRL_WDT_STOP:
    {
        WDOG_Enable(RT_FALSE);
    }
    break;
    default:
    {
        return (RT_ERROR);
    }
    }

    return (RT_EOK);
}

static struct rt_watchdog_ops _ops =
    {
        efm32gg_wdt_init,
        efm32gg_wdt_control};

void rt_hw_wdt_init(void)
{
    //看门狗定时器属于低频外设，访问其寄存器需要开启 LE 时钟
    CMU_ClockEnable(cmuClock_CORELE, true);

    /*看门狗定时期配置结构体*/
    WDOG_Init_TypeDef wtdInit;
    //使能看门狗
    wtdInit.enable = RT_FALSE;
    //看门狗在调试时不计数
    wtdInit.debugRun = RT_FALSE;
    //在EM2模式下看门狗继续计数
    wtdInit.em2Run = RT_TRUE;
    //在EM3模式下看门狗继续计数
    wtdInit.em3Run = RT_TRUE;
    //禁能阻塞进入EM4功能
    wtdInit.em4Block = RT_FALSE;
    //禁能阻塞CMU关闭WDOG所选振荡器
    wtdInit.swoscBlock = RT_FALSE;
    //不锁定看门狗配置
    wtdInit.lock = RT_FALSE;
    //选择1kHz作为看门狗时钟
    wtdInit.clkSel = wdogClkSelULFRCO;
    //设置看门狗周期为8个时钟周期
    wtdInit.perSel = wdogPeriod_16k;
    //初始化看门狗定时器
    WDOG_Init(&wtdInit);

    _device.ops = &_ops;
    //注册看门狗设备
    rt_hw_watchdog_register(&_device, WDT_NAME, RT_DEVICE_FLAG_WRONLY, RT_NULL);
}

#endif