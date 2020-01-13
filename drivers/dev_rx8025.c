/*
 * File      : drv_rx8025.c
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
 * 2013-11-05     aozima          fix issues: the mon register is 0-11.
 */

#include <time.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "dev_rx8025.h"
#include "board.h"

#ifdef RT_USING_RTC

#define RX8025_ADDR 0x32

#ifndef RT_USING_I2C
#error "This RTC device need i2c bus(please enable RT_USING_I2C in rtconfig.h)"
#endif

static struct i2c_slave _rtc;

/* convert DEC to BCD */
static rt_uint8_t DECtoBCD(rt_uint8_t x)
{
    return (((x / 10) << 4) + (x % 10));
}

/* convert BCD to DEC */
static rt_uint8_t BCDtoDEC(rt_uint8_t x)
{
    return (((x >> 4) * 10) + (x & 0x0F));
}

static rt_size_t rx8025sa_read(rt_uint8_t reg,
                               rt_uint8_t *data,
                               rt_size_t size)
{
    rt_size_t ret = 0;
    struct rt_i2c_msg msgs[1];
    uint8_t i2c_write_data[1];
    if (_rtc.device.user_data == RT_NULL)
        return (0);
    msgs[0].addr = RX8025_ADDR;
    msgs[0].flags = RT_I2C_WRITEREAD;
    /* Select register to start reading from */
    i2c_write_data[0] = reg;
    msgs[0].buf = i2c_write_data;
    msgs[0].len = 1;
    /* Select length of data to be read */
    msgs[0].buf2 = data;
    msgs[0].len2 = size;
    ret = rt_i2c_transfer((struct rt_i2c_bus_device *)_rtc.device.user_data, msgs, 1);
    if (ret == 1)
    {
        return size;
    }
    else
        return 0;
}

static rt_size_t rx8025sa_write(rt_uint8_t reg,
                                rt_uint8_t *data,
                                rt_size_t size)
{
    rt_size_t ret = 0;
    struct rt_i2c_msg msgs[1];
    rt_uint8_t data_t[8] = {0};

    if (_rtc.device.user_data == RT_NULL)
        return (0);
    data_t[0] = reg;
    memcpy(&data_t[1], data, size);
    msgs[0].addr = RX8025_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = data_t;
    msgs[0].len = size + 1;
    ret = rt_i2c_transfer((struct rt_i2c_bus_device *)_rtc.device.user_data, msgs, 1);
    if (ret == 1)
    {
        return size;
    }
    else
        return 0;
}
/**
 * The following is rt-thread device operating interface
 */
static rt_err_t rt_rtc_init(rt_device_t dev)
{
    return (RT_EOK);
}

static rt_err_t rt_rx8025_init(void)
{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t data[2];
    rt_err_t ret = -RT_ERROR;

    if (_rtc.device.user_data == RT_NULL)
        _rtc.device.user_data = rt_device_find(RX8025_USING_I2C_NAME);

    /* repeated mode,enable timer interrupt */

    if (_rtc.device.user_data == RT_NULL)
        return (0);

    data[0] = 0xE0;
    data[1] = 0x20;
    msgs[0].addr = RX8025_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = data;
    msgs[0].len = 2;

    if (rt_i2c_transfer((struct rt_i2c_bus_device *)_rtc.device.user_data, msgs, 1) == 1)
        ret = RT_EOK;

    //ret = RT_EOK;

    //_exit:

    return (ret);
}

static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    rt_err_t ret = RT_EOK;

    struct i2c_slave *slave;

    RT_ASSERT(dev != RT_NULL);

    if (!dev->user_data)
    {
        slave = (struct i2c_slave *)dev;

        dev->user_data = rt_device_find(slave->busname);
        if (dev->user_data == RT_NULL)
            ret = RT_ERROR;

        if (RT_EOK == rt_rx8025_init())
        {
            rt_kprintf("8025 set OK\r\n");
        }
        else
        {
            rt_kprintf("8025 set  ERROR\r\n");
        }
    }

    return (ret);
}

static rt_err_t rt_rtc_close(rt_device_t dev)
{
    return (RT_EOK);
}

static rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    int ret = 0;
    rt_uint8_t data[7];
    struct tm t, *p_tm;
    switch (cmd)
    {
    case RT_DEVICE_CTRL_RTC_GET_TIME:
    {
        ret = rx8025sa_read(0, data, 7);
        if (ret != 7)
        {
            return -RT_ERROR;
        }
        t.tm_sec = BCDtoDEC(data[0] & 0x7F);
        t.tm_min = BCDtoDEC(data[1]);
        t.tm_hour = BCDtoDEC(data[2]);
        t.tm_wday = BCDtoDEC(data[3]);
        t.tm_mday = BCDtoDEC(data[4]);
        t.tm_mon = BCDtoDEC(data[5] & 0x1F) - 1; /* tm_mon: 0-11 */
        t.tm_year = 100 + BCDtoDEC(data[6]);     /* 2000 - 2099 */
        *(time_t *)args = mktime(&t);
        break;
    }
    case RT_DEVICE_CTRL_RTC_SET_TIME:
    {
        p_tm = localtime_r((time_t *)args, &t);
        if (p_tm->tm_year < 100)
        {
            return -RT_ERROR;
        }
        data[0] = DECtoBCD(p_tm->tm_sec);
        data[1] = DECtoBCD(p_tm->tm_min);
        data[2] = DECtoBCD(p_tm->tm_hour);
        data[3] = DECtoBCD(p_tm->tm_wday);
        data[4] = DECtoBCD(p_tm->tm_mday);
        data[5] = DECtoBCD((p_tm->tm_mon + 1));  /* data[mon]: 1-12 */
        data[6] = DECtoBCD(p_tm->tm_year - 100); /* 2000 - 2099 */
        ret = rx8025sa_write(0, data, 7);
        if (ret != 7)
        {
            return -RT_ERROR;
        }
        break;
    }
    default:
        rt_kprintf("unknow cmd.\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}

void rt_hw_rx8025_init(void)
{
    rt_device_t device;
    _rtc.busname = (char *)RX8025_USING_I2C_NAME;
    // _rtc.busname = "i2c0";

    device = (rt_device_t)&_rtc;

    /* Register RTC device */
    device->type = RT_Device_Class_RTC;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;
    device->init = rt_rtc_init;
    device->open = rt_rtc_open;
    device->close = rt_rtc_close;
    device->read = RT_NULL;
    device->write = RT_NULL;
    device->control = rt_rtc_control;
    device->user_data = RT_NULL;

    rt_device_register(device, RX8025_RTC_NAME, RT_DEVICE_FLAG_RDWR);
}

/**
 * Used for test
 */

static void rtcread(void)
{
    int i;
    rt_uint8_t data[10] = {0};
    rt_memset(data, 0, 7);
    _rtc.device.user_data = rt_device_find(_rtc.busname);
    i = rx8025sa_read(0, data, 7);

    for (i = 0; i < 7; i++)
        rt_kprintf("<%x>", data[i]);
    rt_kprintf("\n");
}

static void rtcwrite(void)
{
    rt_uint8_t data[7] = {DECtoBCD(55), DECtoBCD(59), DECtoBCD(23), DECtoBCD(1), DECtoBCD(31), DECtoBCD(12), DECtoBCD(19)};
    _rtc.device.user_data = rt_device_find(_rtc.busname);
    rx8025sa_write(0, data, 7);
}

static void setdate(int argc, void *argv[])
{
    if (argc < 2)
    {
        return;
    }
    char *arg = argv[1];
    int min = atoi(arg);
    rt_kprintf("min:%d\n", min);
    set_date(2019, 8, 23);
    set_time(14, min, 22);
}

#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT(setdate, set date);
FINSH_FUNCTION_EXPORT(rtcread, read rtc);
FINSH_FUNCTION_EXPORT(rtcwrite, write rtc);
#endif /* RT_USING_FINSH */
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(setdate, set date);
MSH_CMD_EXPORT(rtcread, read rtc);
MSH_CMD_EXPORT(rtcwrite, write rtc);
#endif
// #endif /* DRV_FUNCTION_EXPORT */

#endif /* RT_USING_RTC */
