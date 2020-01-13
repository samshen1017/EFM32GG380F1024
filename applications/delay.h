/*
 * @Author: majiangfeng 
 * @Date: 2019-7-25 16:18:41 
 * @Last Modified by: majiangfeng
 * @Last Modified time: 2019-7-25 16:18:41
 */
#ifndef __DELAY_H__
#define __DELAY_H__

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_timer.h"
#include "drv_pin.h"

void DLF_hw_delay_init(void);
void Delay_us(uint32_t time);
void Delay_ms(uint32_t time);
void Delay_sec(uint32_t time);

#endif