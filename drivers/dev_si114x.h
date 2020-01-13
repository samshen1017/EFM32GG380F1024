/*
 * @Author: ShenQuan 
 * @Date: 2019-07-02 13:11:44 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-07-02 14:37:26
 */

#ifndef __DEV_SI114X_H__
#define __DEV_SI114X_H__

#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_SI114X)
uint32_t Si1141_Write_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t data);
uint32_t Si1141_Write_Block_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t length, uint8_t const *data);
uint32_t Si1141_Read_Block_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data);
uint32_t Si1141_Read_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t *data);

/**
 * @brief Read IR light value from the given Si114x device, returned in lx
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  IR light in lx
 */
uint16_t si114x_read_ir(struct rt_i2c_bus_device *dev);

/**
 * @brief Read visible light value from the given Si114x device, returned in lx
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  Visible light in lx
 */
uint16_t si114x_read_visible(struct rt_i2c_bus_device *dev);

/**
 * @brief Read distance measure from the given Si114x device, returned in ADC
 *        counts.
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  Distance in ADC counts
 */
uint16_t si114x_read_distance(struct rt_i2c_bus_device *dev);

/**
 * @brief Read the response register.
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  Device response register contents
 */
uint8_t si114x_read_response(struct rt_i2c_bus_device *dev);

#endif

int rt_hw_si114x_init(void);

void si114x_init(void);
uint16_t si114x_measure(void);
#endif //__DEV_SI114X_H__
