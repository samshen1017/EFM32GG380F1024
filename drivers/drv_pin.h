/*
 * @Author: ShenQuan 
 * @Date: 2019-05-13 16:41:09 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-05-14 14:39:16
 */

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/pin.h>

typedef enum
{
    GPIO_A_PORT = 1U,
    GPIO_B_PORT = 2U,
    GPIO_C_PORT = 3U,
    GPIO_D_PORT = 4U,
    GPIO_E_PORT = 5U,
    GPIO_F_PORT = 6U
} GPIO_PIN_PORT;

extern rt_uint16_t get_PinNumber(GPIO_PIN_PORT port, rt_uint8_t pin);
extern int rt_hw_pin_init(void);
#endif // __DRV_GPIO_H__
