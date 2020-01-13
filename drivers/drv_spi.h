#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <em_usart.h>
#include <em_dma.h>
#include <hdl_interrupt.h>

struct efm32_hw_spi_cs
{
    rt_uint32_t pin;
    bool select_by_high;
};

typedef USART_TypeDef SPI_TypeDef;
struct efm32_spi
{
    SPI_TypeDef *Instance;
    rt_uint8_t location;
    struct rt_spi_configuration *cfg;
    struct rt_semaphore *lock;
#ifdef SPI_USING_DMA
    rt_bool_t txActive;
    rt_uint8_t txDmaChannel;
    DMA_CB_TypeDef txCallback;
    rt_bool_t RxActive;
    rt_uint8_t RxDmaChannel;
    DMA_CB_TypeDef RxCallback;
#endif
};

//cannot be used before completion init
extern rt_err_t efm32_spi_bus_attach_device(rt_uint32_t pin, const char *bus_name, const char *device_name);
extern void efm32_hw_spi_init(void);

#endif
