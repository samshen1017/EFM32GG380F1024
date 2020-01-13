/*
 * @Author: majiangfeng 
 * @Date: 2019-7-23 09:22:56 
 * @Last Modified by: majiangfeng
 * @Last Modified time: 2019-7-23 09:22:56
 */
#ifndef __DRV_TIMER_H__
#define __DRV_TIMER_H__

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <hdl_interrupt.h>
#include "em_timer.h"

#define TIMER_FREQUENCE_1       48000000
#define TIMER_FREQUENCE_2       24000000
#define TIMER_FREQUENCE_3       12000000
#define TIMER_FREQUENCE_4       6000000
#define TIMER_FREQUENCE_5       3000000
#define TIMER_FREQUENCE_6       1500000
#define TIMER_FREQUENCE_7       750000
#define TIMER_FREQUENCE_8       375000
#define TIMER_FREQUENCE_9       187500
#define TIMER_FREQUENCE_A       93750

void rt_hw_timer_init(void);
// void TIMER0_isr(void *usrdata);
// void TIMER1_isr(void *usrdata);
// void TIMER2_isr(void *usrdata);

#endif