/*
 * @Author: ShenQuan 
 * @Date: 2019-07-02 13:13:31 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-07-02 15:45:03
 */

#include "dev_si114x.h"
#include <board.h>
#include "si114x_internals.h"

#if defined(RT_USING_SI114X)

#ifdef SI114X_DEBUG
#define si114x_debug(format, args...) rt_kprintf(format, ##args)
#else
#define si114x_debug(format, args...)
#endif

#define SI114X_NAME "si114x"
static struct rt_i2c_bus_device *i2c_bus = RT_NULL; /* I2C总线设备句柄 */

/* Internal function prototypes */
//static void _reset(struct rt_i2c_bus_device *dev);
static void Si114xInitialize(struct rt_i2c_bus_device *dev);
static int16_t Si114xParamSet(struct rt_i2c_bus_device *dev, uint8_t address, uint8_t value);

uint32_t Si1141_Read_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t *data)
{
    struct rt_i2c_msg msgs[1];
    RT_ASSERT(i2c_bus != RT_NULL);
    uint8_t i2c_write_data[1];
    uint8_t i2c_read_data[1];
    msgs[0].addr = addr;
    msgs[0].flags = RT_I2C_WRITEREAD;
    i2c_write_data[0] = reg;
    msgs[0].buf = i2c_write_data;
    msgs[0].len = 1;
    msgs[0].buf2 = i2c_read_data;
    msgs[0].len2 = 1;
    rt_size_t ret = rt_i2c_transfer(i2c_bus, msgs, 1);
    *data = i2c_read_data[0];
    return ret;
}

uint32_t Si1141_Write_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t data)
{
    struct rt_i2c_msg msgs[1];
    uint8_t i2c_write_data[2];
    RT_ASSERT(i2c_bus != RT_NULL);
    msgs[0].addr = addr;
    msgs[0].flags = RT_I2C_WR;
    /* Select register and data to write */
    i2c_write_data[0] = reg;
    i2c_write_data[1] = data;
    msgs[0].buf = i2c_write_data;
    msgs[0].len = 2;
    return rt_i2c_transfer(i2c_bus, msgs, 1);
}

uint32_t Si1141_Write_Block_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t length, uint8_t const *data)
{
    struct rt_i2c_msg msgs[1];
    uint8_t i2c_write_data[10];
    RT_ASSERT(i2c_bus != RT_NULL);
    msgs[0].addr = addr;
    msgs[0].flags = RT_I2C_WR;
    i2c_write_data[0] = reg;
    for (int i = 0; i < length; i++)
    {
        i2c_write_data[i + 1] = data[i];
    }
    msgs[0].buf = i2c_write_data;
    msgs[0].len = 1 + length;
    return rt_i2c_transfer(i2c_bus, msgs, 1);
}

uint32_t Si1141_Read_Block_Register(struct rt_i2c_bus_device *i2c_bus, uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data)
{
    struct rt_i2c_msg msgs[1];
    uint8_t i2c_write_data[1];
    RT_ASSERT(i2c_bus != RT_NULL);
    msgs[0].addr = addr;
    msgs[0].flags = RT_I2C_WRITEREAD;
    /* Select register to start reading from */
    i2c_write_data[0] = reg;
    msgs[0].buf = i2c_write_data;
    msgs[0].len = 1;
    /* Select length of data to be read */
    msgs[0].buf2 = data;
    msgs[0].len2 = length;
    return rt_i2c_transfer(i2c_bus, msgs, 1);
}

int16_t Si114xWriteToRegister(struct rt_i2c_bus_device *i2c_bus, uint8_t reg, uint8_t data)
{
    return Si1141_Write_Register(i2c_bus, SI114X_ADDR, reg, data);
}

int16_t Si114xReadFromRegister(struct rt_i2c_bus_device *i2c_bus, uint8_t reg)
{
    uint8_t data = 0;
    Si1141_Read_Register(i2c_bus, SI114X_ADDR, reg, &data);
    return data;
}

int16_t Si114xBlockWrite(struct rt_i2c_bus_device *i2c_bus, uint8_t reg, uint8_t length, uint8_t *values)
{
    return Si1141_Write_Block_Register(i2c_bus, SI114X_ADDR, reg, length, values);
}

int16_t Si114xBlockRead(struct rt_i2c_bus_device *i2c_bus, uint8_t reg, uint8_t length, uint8_t *values)
{
    return Si1141_Read_Block_Register(i2c_bus, SI114X_ADDR, reg, length, values);
}

void delay_10ms(void)
{
    rt_hw_us_delay(1000 * 10);
}

void delay_1ms(void)
{
    rt_hw_us_delay(1000);
}

int rt_hw_si114x_init(void)
{
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(SI114X_USING_I2C_NAME);
    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", SI114X_USING_I2C_NAME);
        return RT_EEMPTY;
    }

    return RT_EOK;
}

void si114x_init(void)
{
    // rt_uint16_t pwr_pin = get_PinNumber(GPIO_F_PORT, 8);
    // rt_pin_mode(pwr_pin, PIN_MODE_OUTPUT);
    // rt_pin_write(pwr_pin, PIN_LOW);
    /* check sensor ID */
    char checkid = Si114xReadFromRegister(i2c_bus, 0x00);
    if (checkid != 0x41)
    {
        si114x_debug("The given i2c is not enabled.\n");
    }
    else
    {
        si114x_debug("checkid:0x%x\n", checkid);
    }
    /* reset sensor */
    //_reset(i2c_bus);

    /* initialize internals registers */
    Si114xInitialize(i2c_bus);
}
FINSH_FUNCTION_EXPORT(si114x_init, si114x_init test);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(si114x_init, si114x_init test);
#endif

static void si114x_test(void)
{
    uint16_t response;
    Si114xWriteToRegister(i2c_bus, REG_COMMAND, CMD_PS_ALS_FORCE);
    // Wait for command to finish
    response = Si114xReadFromRegister(i2c_bus, REG_RESPONSE);
    si114x_debug("REG_RESPONSE %x\n", response);
    rt_kprintf("IR light: %d\n", si114x_read_ir(i2c_bus));
    rt_kprintf("Visible light: %d\n", si114x_read_visible(i2c_bus));
    rt_kprintf("Distance: %d\n", si114x_read_distance(i2c_bus));
    rt_kprintf("Response: 0x%x\n", si114x_read_response(i2c_bus));
}
FINSH_FUNCTION_EXPORT(si114x_test, si114x test);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(si114x_test, si114x test);
#endif

static void si114x_reset(void)
{
    uint16_t response;
    Si114xWriteToRegister(i2c_bus, REG_COMMAND, CMD_RESET);
    // Wait for command to finish
    response = Si114xReadFromRegister(i2c_bus, REG_RESPONSE);
    si114x_debug("REG_RESPONSE %x\n", response);
}
FINSH_FUNCTION_EXPORT(si114x_reset, si114x reset);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(si114x_reset, si114x reset);
#endif

uint16_t si114x_read_ir(struct rt_i2c_bus_device *dev)
{
    uint8_t buffer[2] = {0, 0};
    Si114xBlockRead(dev, REG_ALS_IR_DATA0, 2, buffer);
    return (uint16_t)((buffer[1] << 8) | buffer[0]);
}

uint16_t si114x_read_visible(struct rt_i2c_bus_device *dev)
{
    uint8_t buffer[2] = {0, 0};
    Si114xBlockRead(dev, REG_ALS_VIS_DATA0, 2, buffer);
    return (uint16_t)((buffer[1] << 8) | buffer[0]);
}

uint16_t si114x_read_distance(struct rt_i2c_bus_device *dev)
{
    uint8_t buffer[2] = {0, 0};
    Si114xBlockRead(dev, REG_PS1_DATA0, 2, buffer);
    si114x_debug("PS1:%d\n", (uint16_t)((buffer[1] << 8) | buffer[0]));

    uint8_t buffer2[2] = {0, 0};
    Si114xBlockRead(dev, REG_PS2_DATA0, 2, buffer2);
    si114x_debug("PS2:%d\n", (uint16_t)((buffer2[1] << 8) | buffer2[0]));

    uint8_t buffer3[2] = {0, 0};
    Si114xBlockRead(dev, REG_PS3_DATA0, 2, buffer3);
    si114x_debug("PS3:%d\n", (uint16_t)((buffer3[1] << 8) | buffer3[0]));

    return (uint16_t)((buffer[1] << 8) | buffer[0]);
    //return (Si114xReadFromRegister(dev, REG_PS1_DATA0) + (256 * Si114xReadFromRegister(dev, REG_PS1_DATA1)));
}

uint8_t si114x_read_response(struct rt_i2c_bus_device *dev)
{
    return Si114xReadFromRegister(dev, REG_RESPONSE);
}
/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

// void _reset(struct rt_i2c_bus_device *dev)
// {
//     si114x_debug("Resetting si114x.\n");
//     /* write configuration values */
//     Si114xWriteToRegister(dev, REG_MEAS_RATE, 0);
//     Si114xWriteToRegister(dev, REG_ALS_RATE, 0);
//     Si114xWriteToRegister(dev, REG_IRQ_ENABLE, 0);
//     Si114xWriteToRegister(dev, REG_IRQ_MODE1, 0);
//     Si114xWriteToRegister(dev, REG_IRQ_MODE2, 0);
//     Si114xWriteToRegister(dev, REG_INT_CFG, 0);
//     Si114xWriteToRegister(dev, REG_IRQ_STATUS, 0xFF);

//     /* perform RESET command */
//     Si114xWriteToRegister(dev, REG_COMMAND, CMD_RESET);
//     delay_10ms();

//     /* write HW_KEY for proper operation */
//     Si114xWriteToRegister(dev, REG_HW_KEY, SI114X_HW_KEY_VAL0);
//     delay_10ms();
// }

void Si114xInitialize(struct rt_i2c_bus_device *dev)
{
    si114x_debug("Initializing si114x.\n");
    /* write HW_KEY for proper operation */
    Si114xWriteToRegister(dev, REG_HW_KEY, SI114X_HW_KEY_VAL0);

    /* active LED current */
    Si114xWriteToRegister(dev, REG_PS_LED21, 0x07);
    // Si114xWriteToRegister(dev, REG_PS_LED3, 0x00);

    /* enable interrupt on every sample */
    // Si114xWriteToRegister(dev, REG_INT_CFG, SI114X_INTCFG_INTOE);
    // Si114xWriteToRegister(dev, REG_IRQ_ENABLE, SI114X_EN_ALS_IE | SI114X_EN_PS1_IE);

    // _set_param(dev, PARAM_PS1_ADC_MUX, SI114X_PARAM_ADCMUX_LARGEIR);

    /* proximity sensor uses LED1 */
    //_set_param(dev, SI114X_PARAM_PSLED12SEL, SI114X_PARAM_PSLED12SEL_PS1LED1);

    /* ADC gain */
    //_set_param(dev, SI114X_PARAM_PSADCGAIN, 0);
    //_set_param(dev, SI114X_PARAM_PSADCOUNTER, SI114X_ADC_REC_CLK);

    /* proximity range */
    // _set_param(dev, PARAM_PSADCMISC,
    //            SI114X_PARAM_PSADCMISC_RANGE | SI114X_PARAM_PSADCMISC_PSMODE);
    // _set_param(dev, SI114X_PARAM_ALSIRADCMUX, SI114X_PARAM_ADCMUX_SMALLIR);

    /* clocks configuration */
    // _set_param(dev, SI114X_PARAM_ALSIRADCGAIN, 0);
    // _set_param(dev, SI114X_PARAM_ALSIRADCOUNTER, SI114X_ADC_REC_CLK);

    /* high range mode */
    // _set_param(dev, SI114X_PARAM_ALSIRADCMISC, SI114X_PARAM_ALSIRADCMISC_RANGE);

    /* clocks config */
    // _set_param(dev, SI114X_PARAM_ALSVISADCGAIN, 0);
    // _set_param(dev, SI114X_PARAM_ALSVISADCOUNTER, SI114X_ADC_REC_CLK);

    // _set_param(dev, SI114X_PARAM_ALSVISADCMISC,
    //            SI114X_PARAM_ALSVISADCMISC_VISRANGE);
    Si114xWriteToRegister(dev, REG_MEAS_RATE, 0xDF);
    Si114xWriteToRegister(dev, REG_ALS_RATE, 0x32);
    Si114xWriteToRegister(dev, REG_PS_RATE, 0x32);

    /* auto-run */
    Si114xWriteToRegister(dev, REG_COMMAND, CMD_PS_ALS_AUTO);

    /* enable measures */
    Si114xParamSet(dev, PARAM_CH_LIST, SI114X_EN_AUX | SI114X_EN_ALS_IR | SI114X_EN_ALS_VIS | SI114X_EN_PS1 | SI114X_EN_PS2 | SI114X_EN_PS3);
}

static int16_t Si114xParamSet(struct rt_i2c_bus_device *dev, uint8_t address, uint8_t value)
{
    uint8_t buffer[2];
    int16_t response;
    Si114xWriteToRegister(dev, REG_COMMAND, CMD_NOP);
    response = Si114xReadFromRegister(dev, REG_RESPONSE);
    if (response != 0x00)
    {
        si114x_debug("REG_RESPONSE not ready. %d\n", response);
    }
    buffer[0] = value;
    buffer[1] = CMD_PARAM_SET | (address & 0x1F);
    Si114xBlockWrite(dev, REG_PARAM_WR, 2, (uint8_t *)buffer);

    // Wait for command to finish
    response = Si114xReadFromRegister(dev, REG_RESPONSE);
    si114x_debug("REG_RESPONSE %x\n", response);
    return response;
}

uint16_t si114x_measure(void)
{
    return si114x_read_distance(i2c_bus);
}
#endif