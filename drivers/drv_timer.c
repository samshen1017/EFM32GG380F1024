#include "drv_timer.h"

struct efm32_timer
{
	TIMER_TypeDef *regs;
	TIMER_Init_TypeDef init;
};
#ifdef RT_USING_TIMER0
static rt_hwtimer_t timer0;
static struct efm32_timer htimer0 =
	{
		TIMER0,
		TIMER_INIT_DEFAULT,
};
#endif

#ifdef RT_USING_TIMER1
static rt_hwtimer_t timer1;
static struct efm32_timer htimer1 =
	{
		TIMER1,
		TIMER_INIT_DEFAULT,
};
#endif

#ifdef RT_USING_TIMER2
static rt_hwtimer_t timer2;
static struct efm32_timer htimer2 =
	{
		TIMER2,
		TIMER_INIT_DEFAULT,
};
#endif
#ifdef RT_USING_TIMER0
void TIMER0_isr(void *usrdata)
{
	rt_interrupt_enter();
	rt_device_hwtimer_isr(&timer0);
	rt_interrupt_leave();
}
#endif
#ifdef RT_USING_TIMER1
void TIMER1_isr(void *usrdata)
{
	rt_interrupt_enter();
	// TIMER_IntClear(TIMER1, TIMER_IF_OF);
	rt_device_hwtimer_isr(&timer1);
	rt_interrupt_leave();
}
#endif
#ifdef RT_USING_TIMER2
void TIMER2_isr(void *usrdata)
{
	rt_interrupt_enter();
	// TIMER_IntClear(TIMER2, TIMER_IF_OF);
	rt_device_hwtimer_isr(&timer2);
	rt_interrupt_leave();
}
#endif
static void timer_init(rt_hwtimer_t *timer, rt_uint32_t state)
{
	efm32_irq_hook_init_t hook;
	if (state == 1)
	{
		struct efm32_timer *htimer = (struct efm32_timer *)timer->parent.user_data;
		TIMER_TypeDef *regs = htimer->regs;
		TIMER_Init_TypeDef tim = htimer->init;
		switch ((uint32_t)regs)
		{
		case (uint32_t)TIMER0:
#ifdef RT_USING_TIMER0
			CMU_ClockEnable(cmuClock_TIMER0, true);
			hook.unit = 0;
			hook.cbFunc = TIMER0_isr;
			break;
#endif
		case (uint32_t)TIMER1:
#ifdef RT_USING_TIMER1
			CMU_ClockEnable(cmuClock_TIMER1, true);
			hook.unit = 1;
			hook.cbFunc = TIMER1_isr;
			break;
#endif
		case (uint32_t)TIMER2:
#ifdef RT_USING_TIMER2
			CMU_ClockEnable(cmuClock_TIMER2, true);
			hook.unit = 2;
			hook.cbFunc = TIMER2_isr;
			break;
#endif
		default:
			break;
		}
		timer->freq = 1500000;
		//初始化TIMER计数器
		TIMER_Init(regs, &tim);
		hook.type = efm32_irq_type_timer;
		hook.userPtr = RT_NULL;
		efm32_irq_hook_register(&hook);
	}
}

static rt_err_t timer_start(rt_hwtimer_t *timer, rt_uint32_t t, rt_hwtimer_mode_t opmode)
{
	struct efm32_timer *htimer = (struct efm32_timer *)timer->parent.user_data;
	TIMER_TypeDef *regs = htimer->regs;
	
	//写TIMER的TOP值
	TIMER_TopSet(regs, t);
	//使能TIMER溢出中断
	TIMER_IntEnable(regs, TIMER_IF_OF);
	switch ((uint32_t)regs)
	{
	case (uint32_t)TIMER0:
		//使能TIMER0中断
		NVIC_EnableIRQ(TIMER0_IRQn);
		//启动计数器
		TIMER0->CMD = TIMER_CMD_START;
		break;
	case (uint32_t)TIMER1:
		//使能TIMER0中断
		NVIC_EnableIRQ(TIMER1_IRQn);
		//启动计数器
		TIMER1->CMD = TIMER_CMD_START;
		break;
	case (uint32_t)TIMER2:
		//使能TIMER0中断
		NVIC_EnableIRQ(TIMER2_IRQn);
		//启动计数器
		TIMER2->CMD = TIMER_CMD_START;
		break;
	default:
		break;
	}
	return RT_EOK;
}

void timer_stop(rt_hwtimer_t *timer)
{
	struct efm32_timer *htimer = (struct efm32_timer *)timer->parent.user_data;
	TIMER_TypeDef *regs = htimer->regs;
	//失能TIMER0溢出中断
	TIMER_IntDisable(regs, TIMER_IF_OF);
	switch ((uint32_t)regs)
	{
	case (uint32_t)TIMER0:
		//失能TIMER0中断
		NVIC_DisableIRQ(TIMER0_IRQn);
		//关闭计数器
		TIMER0->CMD = TIMER_CMD_STOP;
		// TIMER0->CNT  = _TIMER_CNT_RESETVALUE;
		break;
	case (uint32_t)TIMER1:
		//失能TIMER1中断
		NVIC_DisableIRQ(TIMER1_IRQn);
		//关闭计数器
		TIMER1->CMD = TIMER_CMD_STOP;
		break;
	case (uint32_t)TIMER2:
		//失能TIMER2中断
		NVIC_DisableIRQ(TIMER2_IRQn);
		//关闭计数器
		TIMER2->CMD = TIMER_CMD_STOP;
		break;
	default:
		break;
	}
}

static rt_err_t timer_ctrl(rt_hwtimer_t *timer, rt_uint32_t cmd, void *arg)
{
	uint32_t freq = *((rt_uint32_t *)arg);
	struct efm32_timer *htimer = (struct efm32_timer *)timer->parent.user_data;
	TIMER_TypeDef *regs = htimer->regs;
	TIMER_Init_TypeDef tim = htimer->init;
	if (cmd == HWTIMER_CTRL_FREQ_SET)
	{
		switch (freq)
		{
		case TIMER_FREQUENCE_1:
			tim.prescale = timerPrescale1;
			break;
		case TIMER_FREQUENCE_2:
			tim.prescale = timerPrescale2;
			break;
		case TIMER_FREQUENCE_3:
			tim.prescale = timerPrescale4;
			break;
		case TIMER_FREQUENCE_4:
			tim.prescale = timerPrescale8;
			break;
		case TIMER_FREQUENCE_5:
			tim.prescale = timerPrescale16;
			break;
		case TIMER_FREQUENCE_6:
			tim.prescale = timerPrescale32;
			break;
		case TIMER_FREQUENCE_7:
			tim.prescale = timerPrescale64;
			break;
		case TIMER_FREQUENCE_8:
			tim.prescale = timerPrescale128;
			break;
		case TIMER_FREQUENCE_9:
			tim.prescale = timerPrescale256;
			break;
		case TIMER_FREQUENCE_A:
			tim.prescale = timerPrescale512;
			break;
		default:
			return RT_EINVAL;
		}
		TIMER_Init(regs, &tim);
		return RT_EOK;
	}
	else
	{
		return RT_EINVAL;
	}
}

static rt_uint32_t timer_counter_get(rt_hwtimer_t *timer)
{
	return TIMER0->CNT;
}

static const struct rt_hwtimer_info tim_info =
	{
		48000000,
		93750,
		0xFFFF,
		HWTIMER_CNTMODE_UP,
};

static const struct rt_hwtimer_ops tim_ops =
	{
		timer_init,
		timer_start,
		timer_stop,
		timer_counter_get,
		timer_ctrl,
};

void rt_hw_timer_init(void)
{

#ifdef RT_USING_TIMER0
	timer0.info = &tim_info;
	timer0.ops = &tim_ops;
	timer0.mode = HWTIMER_MODE_ONESHOT;
	htimer0.init.enable = false;
	htimer0.init.prescale = timerPrescale32;
	htimer0.init.oneShot = false;
	rt_device_hwtimer_register(&timer0, TIMER0_NAME, &htimer0);
#endif
#ifdef RT_USING_TIMER1
	timer1.info = &tim_info;
	timer1.ops = &tim_ops;
	timer1.mode = HWTIMER_MODE_ONESHOT;
	htimer1.init.enable = false;
	htimer1.init.prescale = timerPrescale32;
	htimer1.init.oneShot = false;
	rt_device_hwtimer_register(&timer1, TIMER1_NAME, &htimer1);
#endif
#ifdef RT_USING_TIMER2
	timer2.info = &tim_info;
	timer2.ops = &tim_ops;
	timer2.mode = HWTIMER_MODE_ONESHOT;
	htimer2.init.enable = false;
	htimer2.init.prescale = timerPrescale32;
	htimer2.init.oneShot = false;
	rt_device_hwtimer_register(&timer2, TIMER2_NAME, &htimer2);
#endif
}