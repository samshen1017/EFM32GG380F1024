#include "drv_letimer.h"
#include "em_letimer.h"
#include "hdl_interrupt.h"
#include <rtthread.h>

#define RT_USING_HWTIMER
#if defined(RT_USING_HWTIMER)
#define RT_LETIMER_DEBUG
#ifdef RT_LETIMER_DEBUG
#define letimer_debug(format, args...) rt_kprintf(format, ##args)
#else
#define letimer_debug(format, args...)
#endif

struct efm32_letimer
{
    rt_hwtimer_t tim_dev;
    LETIMER_TypeDef *regs;
    LETIMER_Init_TypeDef init;
};

#ifdef RT_USING_LETIMER0
static struct efm32_letimer letimer0_instance =
    {
        .regs = LETIMER0,
        .init = LETIMER_INIT_DEFAULT,
};
static void LETIMER0_isr(void *usrdata)
{
    rt_device_hwtimer_isr((rt_hwtimer_t *)usrdata);
}
#endif

static void letimer_init(struct rt_hwtimer_device *timer, rt_uint32_t state)
{
    RT_ASSERT(timer != RT_NULL);
    letimer_debug("init.\n");
    /*  state参数，判定这个值是否等于1，其他情况不要进行硬件初始化。 */
    if (state)
    {
        efm32_irq_hook_init_t hook;
        struct efm32_letimer *htimer = (struct efm32_letimer *)timer->parent.user_data;
        LETIMER_TypeDef *regs = htimer->regs;
        LETIMER_Init_TypeDef *init = &htimer->init;
        init->enable = false;
        init->comp0Top = true;
        init->repMode = letimerRepeatFree;

        switch ((rt_uint32_t)regs)
        {
        case (rt_uint32_t)LETIMER0:
        {
            hook.unit = 0;
            hook.cbFunc = LETIMER0_isr;
            CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1);
            CMU_ClockEnable(cmuClock_LETIMER0, true);
            /* Enable underflow interrupt */
            LETIMER_IntClear(LETIMER0, LETIMER_IF_UF);
            LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);
            /* Enable LETIMER0 interrupt vector in NVIC */
            NVIC_ClearPendingIRQ(LETIMER0_IRQn);
            NVIC_SetPriority(LETIMER0_IRQn, EFM32_IRQ_PRI_DEFAULT);
            NVIC_EnableIRQ(LETIMER0_IRQn);
            break;
        }
        default:
            rt_kprintf("unknow LETIMER.\n");
            break;
        }
        /* Init LETIMERn */
        LETIMER_Init(regs, init);
        hook.type = efm32_irq_type_letimer;
        hook.userPtr = &htimer->tim_dev;
        efm32_irq_hook_register(&hook);
    }
}

static rt_err_t letimer_start(struct rt_hwtimer_device *timer, rt_uint32_t cnt, rt_hwtimer_mode_t mode)
{
    rt_err_t res = RT_EOK;
    RT_ASSERT(timer != RT_NULL);
    struct efm32_letimer *htimer = (struct efm32_letimer *)timer->parent.user_data;
    LETIMER_TypeDef *regs = htimer->regs;
    letimer_debug("start. %x\n", LETIMER_TopGet(regs));

    /* set tim cnt */
    LETIMER_TopSet(regs, cnt);

    if (mode == HWTIMER_MODE_ONESHOT)
    {

        regs->CTRL &= ~_LETIMER_CTRL_REPMODE_MASK;
        regs->CTRL |= _LETIMER_CTRL_REPMODE_ONESHOT << _LETIMER_CTRL_REPMODE_SHIFT;
        LETIMER_RepeatSet(regs, 0, 1);
    }
    LETIMER_Enable(regs, true);
    return res;
}

static void letimer_stop(struct rt_hwtimer_device *timer)
{
    RT_ASSERT(timer != RT_NULL);
    letimer_debug("stop.\n");
    struct efm32_letimer *htimer = (struct efm32_letimer *)timer->parent.user_data;
    LETIMER_TypeDef *regs = htimer->regs;

    /* stop timer */
    LETIMER_Enable(regs, false);
}

static rt_uint32_t letimer_count_get(struct rt_hwtimer_device *timer)
{
    RT_ASSERT(timer != RT_NULL);
    letimer_debug("count_get.\n");
    struct efm32_letimer *htimer = (struct efm32_letimer *)timer->parent.user_data;
    LETIMER_TypeDef *regs = htimer->regs;
    return LETIMER_CounterGet(regs);
}

static rt_err_t letimer_control(struct rt_hwtimer_device *timer, rt_uint32_t cmd, void *arg)
{
    RT_ASSERT(timer != RT_NULL);
    RT_ASSERT(arg != RT_NULL);
    letimer_debug("control.\n");
    rt_err_t result = RT_EOK;
    struct efm32_letimer *htimer = (struct efm32_letimer *)timer->parent.user_data;
    LETIMER_TypeDef *regs = htimer->regs;
    switch (cmd)
    {
    case HWTIMER_CTRL_FREQ_SET:
    {
        switch ((rt_uint32_t)regs)
        {
        case (rt_uint32_t)LETIMER0:
        {
            rt_uint32_t freq;
            freq = *((rt_uint32_t *)arg);
            rt_uint32_t div = 32768 / freq;
            rt_kprintf("freq:%d, div:%d\n", freq, div);
            CMU_ClockDivSet(cmuClock_LETIMER0, div);
            break;
        }
        }
        break;
    }
    default:
        letimer_debug("unknow cmd.\n");
        result = -RT_ENOSYS;
        break;
    }
    return result;
}

static const struct rt_hwtimer_info tim_info =
    {
        .maxfreq = 32768,
        .minfreq = 64,
        .maxcnt = 0xFFFF,
        .cntmode = HWTIMER_CNTMODE_DW,
};

static const struct rt_hwtimer_ops tim_ops =
    {
        .init = letimer_init,
        .start = letimer_start,
        .stop = letimer_stop,
        .count_get = letimer_count_get,
        .control = letimer_control,
};

void rt_hw_letimer_init(void)
{
#ifdef RT_USING_LETIMER0
    letimer0_instance.tim_dev.info = &tim_info;
    letimer0_instance.tim_dev.ops = &tim_ops;
    letimer0_instance.tim_dev.mode = HWTIMER_MODE_PERIOD;
    letimer0_instance.init.enable = false;
    rt_device_hwtimer_register(&(letimer0_instance.tim_dev), RT_LETIMER0_NAME, &letimer0_instance);
#endif
}

#endif
