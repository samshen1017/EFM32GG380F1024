#include "delay.h"

struct timer_set
{
	rt_device_t dev;
	rt_sem_t sem;
	rt_uint32_t period;
};
#ifdef RT_USING_TIMER0
static struct timer_set timer0_set;
#endif
#ifdef RT_USING_TIMER1
static struct timer_set timer1_set;
#endif
#ifdef RT_USING_TIMER2
static struct timer_set timer2_set;
#endif
#ifdef RT_USING_TIMER0
rt_err_t timer0_callback(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(timer0_set.sem);
	return RT_EOK;
}
#endif
#ifdef RT_USING_TIMER1
rt_err_t timer1_callback(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(timer1_set.sem);
	return RT_EOK;
}
#endif
#ifdef RT_USING_TIMER2
rt_err_t timer2_callback(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(timer2_set.sem);
	return RT_EOK;
}
#endif
void DLF_hw_delay_init(void)
{
#ifdef RT_USING_TIMER0
	timer0_set.dev = rt_device_find(TIMER0_NAME);
	timer0_set.period = TIMER_FREQUENCE_6;
	if (timer0_set.dev != RT_NULL)
	{
		timer0_set.sem = rt_sem_create("timer0sem", 0, RT_IPC_FLAG_FIFO);
		rt_device_set_rx_indicate(timer0_set.dev, timer0_callback);
		rt_device_open(timer0_set.dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_control(timer0_set.dev, HWTIMER_CTRL_FREQ_SET, &timer0_set.period);
		rt_device_control(timer0_set.dev, HWTIMER_CTRL_MODE_SET, HWTIMER_MODE_ONESHOT);
	}
#endif
#ifdef RT_USING_TIMER1
	timer1_set.dev = rt_device_find(TIMER1_NAME);
	timer1_set.period = TIMER_FREQUENCE_6;	
	if (timer1_set.dev != RT_NULL)
	{
		timer1_set.sem = rt_sem_create("timer1sem", 0, RT_IPC_FLAG_FIFO);
		rt_device_set_rx_indicate(timer1_set.dev, timer0_callback);
		rt_device_open(timer1_set.dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_control(timer1_set.dev, HWTIMER_CTRL_FREQ_SET, &timer1_set.period);
		rt_device_control(timer1_set.dev, HWTIMER_CTRL_MODE_SET, HWTIMER_MODE_ONESHOT);
	}
#endif
#ifdef RT_USING_TIMER2
	timer2_set.dev = rt_device_find(TIMER2_NAME);
	timer2_set.period = TIMER_FREQUENCE_6;
	if (timer2_set.dev != RT_NULL)
	{
		timer2_set.sem = rt_sem_create("timer2sem", 0, RT_IPC_FLAG_FIFO);
		rt_device_set_rx_indicate(timer2_set.dev, timer0_callback);
		rt_device_open(timer2_set.dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_control(timer2_set.dev, HWTIMER_CTRL_FREQ_SET, &timer2_set.period);
		rt_device_control(timer2_set.dev, HWTIMER_CTRL_MODE_SET, HWTIMER_MODE_ONESHOT);
	}
#endif
}
#ifdef RT_USING_TIMER0
void Delay_us(uint32_t time)
{
	rt_hwtimerval_t timeout_s;
	timeout_s.sec = 0;
	timeout_s.usec = time;
	rt_device_write(timer0_set.dev, 0, &timeout_s, sizeof(timeout_s));
	while (1)
	{
		if (rt_sem_take(timer0_set.sem, RT_WAITING_FOREVER) == RT_EOK)
		{
			break;
		}
	}
}
void Delay_ms(uint32_t time)
{
	rt_hwtimerval_t timeout_s;
	timeout_s.sec = 0;
	timeout_s.usec = time*1000;
	rt_device_write(timer0_set.dev, 0, &timeout_s, sizeof(timeout_s));
	while (1)
	{
		if (rt_sem_take(timer0_set.sem, RT_WAITING_FOREVER) == RT_EOK)
		{
			break;
		}
	}
}
void Delay_sec(uint32_t time)
{
	rt_hwtimerval_t timeout_s;
	timeout_s.sec = time;
	timeout_s.usec = 0;
	rt_device_write(timer0_set.dev, 0, &timeout_s, sizeof(timeout_s));
	while (1)
	{
		if (rt_sem_take(timer0_set.sem, RT_WAITING_FOREVER) == RT_EOK)
		{
			break;
		}
	}
}
#endif
