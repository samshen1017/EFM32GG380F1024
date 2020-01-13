/*
 * @Author: ShenQuan 
 * @Date: 2019-05-14 14:35:21 
 * @Last Modified by:   ShenQuan 
 * @Last Modified time: 2019-05-14 14:35:21 
 */

#include "drv_i2c.h"
#include "i2cspm.h"
#include "i2cspmconfig.h"
#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_I2C)

typedef struct _efm_i2c_bus
{
	struct rt_i2c_bus_device bus;
	I2CSPM_Init_TypeDef *base;
	char *device_name;
	/* Lock */
	struct rt_semaphore *lock;
} efm_i2c_bus;

#ifdef RT_I2C_DEBUG
#define iic_debug(format, args...) rt_kprintf(format, ##args)
#else
#define iic_debug(format, args...)
#endif

#if defined(RT_USING_I2C0)
static efm_i2c_bus i2c_bus0 = {0};
static I2CSPM_Init_TypeDef i2c0Init = {
	I2C0,							 /* Use I2C instance 0 */
	AF_I2C0_SCL_PORT(I2C0_LOCATION), /* SCL port */
	AF_I2C0_SCL_PIN(I2C0_LOCATION),  /* SCL pin */
	AF_I2C0_SDA_PORT(I2C0_LOCATION), /* SDA port */
	AF_I2C0_SDA_PIN(I2C0_LOCATION),  /* SDA pin */
	I2C0_LOCATION,					 /* Location */
	0,								 /* Use currently configured reference clock */
	I2C_FREQ_FAST_MAX,			 /* Set to standard rate  */
	i2cClockHLRFast,			 /* Set to use 4:4 low/high duty cycle */
};
#endif

#if defined(RT_USING_I2C1)
static efm_i2c_bus i2c_bus1 = {0};
static I2CSPM_Init_TypeDef i2c1Init = {
	I2C1,							 /* Use I2C instance 0 */
	AF_I2C1_SCL_PORT(I2C1_LOCATION), /* SCL port */
	AF_I2C1_SCL_PIN(I2C1_LOCATION),  /* SCL pin */
	AF_I2C1_SDA_PORT(I2C1_LOCATION), /* SDA port */
	AF_I2C1_SDA_PIN(I2C1_LOCATION),  /* SDA pin */
	I2C1_LOCATION,					 /* Location */
	0,								 /* Use currently configured reference clock */
	I2C_FREQ_STANDARD_MAX,			 /* Set to standard rate  */
	i2cClockHLRStandard,			 /* Set to use 4:4 low/high duty cycle */
};
#endif

static rt_size_t i2c_read(efm_i2c_bus *i2c_bus, rt_off_t pos, void *buffer, rt_size_t size)
{
	if (!size)
	{
		return 0;
	}
	rt_err_t err_code;
	rt_size_t read_size;
	I2C_TransferSeq_TypeDef seq;
	I2C_TransferReturn_TypeDef ret;
	err_code = RT_EOK;
	read_size = 0;
	err_code = rt_sem_take(i2c_bus->lock, RT_WAITING_FOREVER);
	if (err_code != RT_EOK)
	{
		return err_code;
	}
	seq.addr = (rt_uint16_t)(pos << 1) | 0x1; //可能有问题
	seq.flags = I2C_FLAG_READ;
	seq.buf[0].data = (rt_uint8_t *)buffer;
	seq.buf[0].len = size;
	ret = I2CSPM_Transfer(i2c_bus->base->port, &seq);
	if (ret != i2cTransferDone)
	{
		iic_debug("%s: read error %d\n", i2c_bus->device_name, ret);
		if (ret == i2cTransferInProgress)
		{
			err_code = RT_ETIMEOUT;
		}
		else
		{
			err_code = RT_ERROR;
		}
	}
	else
	{
		read_size = size;
		iic_debug("%s: read size %d\n", i2c_bus->device_name, read_size);
	}
	rt_sem_release(i2c_bus->lock);
	/* set error code */
	rt_set_errno(err_code);
	return read_size;
}

static rt_size_t i2c_write(efm_i2c_bus *i2c_bus, rt_off_t pos, const void *buffer, rt_size_t size)
{
	rt_err_t err_code;
	rt_size_t write_size;
	I2C_TransferSeq_TypeDef seq;
	I2C_TransferReturn_TypeDef ret;
	if (!size)
	{
		return 0;
	}
	err_code = RT_EOK;
	write_size = 0;
	err_code = rt_sem_take(i2c_bus->lock, RT_WAITING_FOREVER);
	if (err_code != RT_EOK)
	{
		return err_code;
	}
	seq.addr = (rt_uint16_t)(pos << 1);
	seq.flags = I2C_FLAG_WRITE;
	seq.buf[0].data = (rt_uint8_t *)buffer;
	seq.buf[0].len = size;
	ret = I2CSPM_Transfer(i2c_bus->base->port, &seq);
	if (ret != i2cTransferDone)
	{
		iic_debug("%s: write error %d\n", i2c_bus->device_name, ret);
		if (ret == i2cTransferInProgress)
		{
			err_code = RT_ETIMEOUT;
		}
		else
		{
			err_code = RT_ERROR;
		}
	}
	else
	{
		write_size = size;
		iic_debug("%s: write size %d\n", i2c_bus->device_name, write_size);
	}
	rt_sem_release(i2c_bus->lock);
	/* set error code */
	rt_set_errno(err_code);
	return write_size;
}

static rt_size_t i2c_writeread(efm_i2c_bus *i2c_bus, rt_off_t pos, const void *txbuf, rt_size_t tx_size, const void *rxbuf, rt_size_t rx_size)
{
	rt_err_t err_code;
	rt_size_t size;
	I2C_TransferSeq_TypeDef seq;
	I2C_TransferReturn_TypeDef ret;
	if (!tx_size || !rx_size)
	{
		return 0;
	}
	err_code = RT_EOK;
	err_code = rt_sem_take(i2c_bus->lock, RT_WAITING_FOREVER);
	if (err_code != RT_EOK)
	{
		return err_code;
	}
	seq.addr = (rt_uint16_t)(pos << 1) | 0x1;
	seq.flags = I2C_FLAG_WRITE_READ;
	seq.buf[0].data = (rt_uint8_t *)txbuf;
	seq.buf[0].len = tx_size;
	seq.buf[1].data = (rt_uint8_t *)rxbuf;
	seq.buf[1].len = rx_size;
	ret = I2CSPM_Transfer(i2c_bus->base->port, &seq);
	if (ret != i2cTransferDone)
	{
		iic_debug("%s: WriteRead error %d\n", i2c_bus->device_name, ret);
		if (ret == i2cTransferInProgress)
		{
			err_code = RT_ETIMEOUT;
		}
		else
		{
			err_code = RT_ERROR;
		}
	}
	else
	{
		size = rx_size;
		iic_debug("%s: w_size %d, r_size %d\n", i2c_bus->device_name, tx_size, rx_size);
	}
	rt_sem_release(i2c_bus->lock);
	/* set error code */
	rt_set_errno(err_code);
	return size;
}

static rt_size_t master_xfer(struct rt_i2c_bus_device *bus, struct rt_i2c_msg msgs[], rt_uint32_t num)
{
	rt_size_t ret = (0);
	rt_uint32_t index = 0;
	efm_i2c_bus *i2c = RT_NULL;
	struct rt_i2c_msg *msg = RT_NULL;
	RT_ASSERT(bus != RT_NULL);
	i2c = (efm_i2c_bus *)bus;
	for (int i = 0; i < num; i++)
	{
		msg = &msgs[i];
		if (msg->flags & RT_I2C_RD)
		{
			ret = i2c_read(i2c, msg->addr, msg->buf, msg->len);
		}
		else if (msg->flags == RT_I2C_WR)
		{
			ret = i2c_write(i2c, msg->addr, msg->buf, msg->len);
		}
		else if (msg->flags & RT_I2C_WRITEREAD)
		{
			ret = i2c_writeread(i2c, msg->addr, msg->buf, msg->len, msg->buf2, msg->len2);
		}
		if (ret > 0)
		{
			index++;
		}
		else
		{
			return index;
		}
	}
	return index;
}

static rt_size_t slave_xfer(struct rt_i2c_bus_device *bus, struct rt_i2c_msg msgs[], rt_uint32_t num)
{
	return 0;
}

static rt_err_t i2c_bus_control(struct rt_i2c_bus_device *bus, rt_uint32_t cmd, rt_uint32_t arg)
{
	return RT_EOK;
}

static const struct rt_i2c_bus_device_ops ops =
	{
		master_xfer,
		slave_xfer,
		i2c_bus_control,
};

void rt_hw_i2c_bus_init(void)
{
#if defined(RT_USING_I2C0)
	I2CSPM_Init(&i2c0Init);
	i2c_bus0.base = &i2c0Init;
	i2c_bus0.device_name = RT_I2C0_NAME;
	i2c_bus0.bus.ops = &ops;
	i2c_bus0.lock = (struct rt_semaphore *)rt_malloc(sizeof(struct rt_semaphore));
	if (rt_sem_init(i2c_bus0.lock, "s_i2c0", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("rt sem init failed!\n");
		while (1)
			;
	}
	rt_i2c_bus_device_register(&i2c_bus0.bus, i2c_bus0.device_name);
#endif

#if defined(RT_USING_I2C1)
	I2CSPM_Init(&i2c1Init);
	i2c_bus1.base = &i2c1Init;
	i2c_bus1.device_name = RT_I2C1_NAME;
	i2c_bus1.bus.ops = &ops;
	i2c_bus1.lock = (struct rt_semaphore *)rt_malloc(sizeof(struct rt_semaphore));
	if (rt_sem_init(i2c_bus1.lock, "s_i2c1", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("rt sem init failed!\n");
		while (1)
			;
	}
	rt_i2c_bus_device_register(&i2c_bus1.bus, i2c_bus1.device_name);
#endif
}

#endif
