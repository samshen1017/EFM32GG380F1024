#ifndef __DEV_RX8025_H__
#define __DEV_RX8025_H__

/*
 * File      : drv_rx8025.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2008 - 2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author          Notes
 * 2012-09-26     heyuanjie87     the first version
 */


/* RX8025T register definition */
#define REG_Secnods           0x00
#define REG_Minutes           0x10
#define REG_Hours             0x20
#define REG_Weekdays          0x30
#define REG_Days              0x40
#define REG_Months            0x50
#define REG_Years             0x60
#define REG_RAM               0x70
#define REG_AlarmW_Min        0x80
#define REG_AlarmW_Hour       0x90
#define REG_AlarmW_Weekday    0xA0
#define REG_AlarmD_Min        0xB0
#define REG_AlarmD_Hour       0xC0
#define REG_Reserved          0xD0
#define REG_Control1          0xE0
#define REG_Control2          0xF0

struct i2c_slave
{
    struct rt_device device;
    char *busname;
};

void rt_hw_rx8025_init(void);

#endif /* __DRV_RX8025_H__ */
