#include "board.h"
#include "dev_zsc31014.h"

#include <drv_pin.h>

#if defined(RT_USING_ZSC31014)

#define ZSC31014_ADDR 0x28
static struct rt_i2c_bus_device *i2c_bus = RT_NULL; /* I2C总线设备句柄 */

rt_err_t zsc31014_read(rt_uint8_t *data, rt_size_t size)
{
    struct rt_i2c_msg msgs[1];
    rt_size_t ret;
    if (i2c_bus == RT_NULL)
    {
        return RT_ERROR;
    }
    msgs[0].addr = ZSC31014_ADDR;
    msgs[0].flags = RT_I2C_RD; /* Read from slave */
    msgs[0].buf = data;
    msgs[0].len = size;
    ret = rt_i2c_transfer(i2c_bus, msgs, 1);
    if (ret == 0)
    {
        rt_kprintf("zsc31014 read failed.\n");
        return RT_EIO;
    }
    return RT_EOK;
}

rt_err_t rt_hw_zsc31014_init(void)
{
    // rt_uint16_t pwr_pin = get_PinNumber(GPIO_B_PORT, 1);
    rt_uint16_t pwr_pin = get_PinNumber(GPIO_D_PORT, 12);
    rt_pin_mode(pwr_pin, PIN_MODE_OUTPUT);
    rt_pin_write(pwr_pin, PIN_LOW);
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(ZSC31014_USING_I2C_NAME);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", ZSC31014_USING_I2C_NAME);
        return RT_ERROR;
    }
    return RT_EOK;
}

static void test_zsc31014(void)
{
    rt_uint8_t temp[4] = {0, 0, 0, 0};
    zsc31014_read(temp, 4);
    rt_kprintf("__%x__\n", *((int *)temp));
}
FINSH_FUNCTION_EXPORT(test_zsc31014, zsc31014 test);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(test_zsc31014, zsc31014 test);
#endif
//FINSH_FUNCTION_EXPORT(set_led, turn led (0 - 3) on (1) or off (0).)

#endif