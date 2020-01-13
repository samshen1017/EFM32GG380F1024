/*
 * @Author: ShenQuan 
 * @Date: 2019-06-17 16:35:21 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-06-17 17:30:35
 */

#ifndef __DRV_SERIAL_H__
#define __DRV_SERIAL_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "board.h"

struct efm32_uart
{
    void *uart_regs;
    /* irq number */
    IRQn_Type rx_irq;
    IRQn_Type tx_irq;
    rt_uint32_t location;
    //struct rt_serial_rx_fifo rx_fifo;
};

void rt_hw_serial_init(void);

#endif //__DRV_SERIAL_H__