#include "dev_bluemod_sr.h"
#include "drv_pin.h"
#include "string.h"
#include <at.h>

#if defined(RT_USING_BLUEMOD_SR)

#ifdef BLUEMOD_SR_DEBUG
#define ble_debug(format, args...) rt_kprintf(format, ##args)
#else
#define ble_debug(format, args...)
#endif

typedef struct BlueMod_SR *BlueMod_SR_t;
struct BlueMod_SR
{
    rt_uint16_t rst_pin;
    rt_uint16_t pwr_pin;
    rt_uint16_t rts_pin;
    rt_uint16_t cts_pin;
    rt_uint16_t iur_out_pin;
    rt_uint16_t iur_in_pin;
    void (*connect_cb)(void);
    void (*disconnect_cb)(void);
    void (*receive_cb)(const char *data, rt_size_t size);
};
static struct BlueMod_SR ble_obj;

#define BLUEMOD_POWER_ON 1
#define BLUEMOD_POWER_OFF 0

// PA 7     IUR_OUT
#define IUROUT_LOW() rt_pin_write(ble_obj.iur_out_pin, PIN_LOW);
#define IUROUT_HIGH() rt_pin_write(ble_obj.iur_out_pin, PIN_HIGH);

// PA 8     CTS
#define CTS_LOW() rt_pin_write(ble_obj.cts_pin, PIN_LOW);
#define CTS_HIGH() rt_pin_write(ble_obj.cts_pin, PIN_HIGH);

// PA 10    IUR_IN/IUC_IN
#define IURIN_LOW() rt_pin_write(ble_obj.iur_in_pin, PIN_LOW);
#define IURIN_HIGH() rt_pin_write(ble_obj.iur_in_pin, PIN_HIGH);

// PA 9     RTS/IUC_OUT
#define RTS_LOW() rt_pin_write(ble_obj.rts_pin, PIN_LOW);
#define RTS_HIGH() rt_pin_write(ble_obj.rts_pin, PIN_HIGH);

void ble_urc_connected(struct at_client *client, const char *data, rt_size_t size)
{
    ble_obj.connect_cb();
}

void ble_urc_disconnect(struct at_client *client, const char *data, rt_size_t size)
{
    ble_obj.disconnect_cb();
}

void ble_urc_transfer(struct at_client *client, const char *data, rt_size_t size)
{
    ble_obj.receive_cb(data, size);
}

void bluemod_sr_set_connect_callback(void (*cb)(void))
{
    ble_obj.connect_cb = cb;
}

void bluemod_sr_set_disconnect_callback(void (*cb)(void))
{
    ble_obj.disconnect_cb = cb;
}

void bluemod_sr_set_receive_callback(void (*cb)(const char *data, rt_size_t size))
{
    ble_obj.receive_cb = cb;
}

static const char p_head[] = {0xFE};
static const char p_tail[] = {0x0D, 0x0A};
// static struct at_urc ble_urc_table[] = {
//     {"CONNECT", "\r\n", ble_urc_connected},
//     {"NO CARRIER", "\r\n", ble_urc_disconnect},
//     {p_head, p_tail, ble_urc_transfer},
// };

static struct at_urc ble_urc_table[] = {
    {"{connected}", "\r\n", ble_urc_connected},
    {"{disconnected}", "\r\n", ble_urc_disconnect},
    {p_head, "\r\n", ble_urc_transfer},
};

// static void CTS_handler(void *args)
// {
//     rt_uint32_t val;
//     val = rt_pin_read(ble_obj.cts_pin);
//     if (val)
//     {
//         rt_kprintf("H、n");
//     }
//     else
//     {
//         rt_kprintf("L\n");
//     }
// }

static void IURIN_handler(void *args)
{
    rt_uint32_t val;
    val = rt_pin_read(ble_obj.iur_in_pin);
    if (val)
    {
        RTS_HIGH();
    }
    else
    {
        RTS_LOW();
    }
}

static void bluemod_sr_poweron(BlueMod_SR_t instance, rt_uint8_t onoff)
{
    switch (onoff)
    {
    case 1:
        //POWER ON
        rt_pin_write(instance->pwr_pin, PIN_LOW);  //低使能
        rt_pin_write(instance->rst_pin, PIN_HIGH); //默认高电平
        break;
    case 0:
        //POWER OFF
        rt_pin_write(instance->pwr_pin, PIN_HIGH);
        rt_pin_write(instance->rst_pin, PIN_LOW);
        break;
    }
}

rt_err_t rt_hw_bluemod_sr_init(void)
{
    rt_err_t err_code;
    // ble_obj.pwr_pin = get_PinNumber(GPIO_F_PORT, 9); //电源
    // rt_pin_mode(ble_obj.pwr_pin, PIN_MODE_OUTPUT);

    // ble_obj.rst_pin = get_PinNumber(GPIO_D_PORT, 1); //reset
    // rt_pin_mode(ble_obj.rst_pin, PIN_MODE_OUTPUT);

    // ble_obj.iur_out_pin = get_PinNumber(GPIO_A_PORT, 7); //IUR OUT
    // rt_pin_mode(ble_obj.iur_out_pin, PIN_MODE_OUTPUT);

    // ble_obj.iur_in_pin = get_PinNumber(GPIO_A_PORT, 10); //IUR IN
    // rt_pin_mode(ble_obj.iur_in_pin, PIN_MODE_INPUT_PULLUP);

    // ble_obj.rts_pin = get_PinNumber(GPIO_A_PORT, 9); //RTS
    // rt_pin_mode(ble_obj.rts_pin, PIN_MODE_OUTPUT);

    // ble_obj.cts_pin = get_PinNumber(GPIO_A_PORT, 8); //CTS
    // rt_pin_mode(ble_obj.cts_pin, PIN_MODE_INPUT_PULLUP);

    ble_obj.iur_out_pin = get_PinNumber(GPIO_E_PORT, 3); //IUR OUT
    rt_pin_mode(ble_obj.iur_out_pin, PIN_MODE_OUTPUT);

    ble_obj.iur_in_pin = get_PinNumber(GPIO_E_PORT, 0); //IUR IN
    rt_pin_mode(ble_obj.iur_in_pin, PIN_MODE_INPUT_PULLUP);

    ble_obj.rts_pin = get_PinNumber(GPIO_E_PORT, 2); //RTS
    rt_pin_mode(ble_obj.rts_pin, PIN_MODE_OUTPUT);

    ble_obj.cts_pin = get_PinNumber(GPIO_E_PORT, 1); //CTS
    rt_pin_mode(ble_obj.cts_pin, PIN_MODE_INPUT_PULLUP);

    rt_pin_attach_irq(ble_obj.iur_in_pin, PIN_IRQ_MODE_RISING_FALLING, IURIN_handler, RT_NULL);
    rt_pin_irq_enable(ble_obj.iur_in_pin, PIN_IRQ_ENABLE);

    // rt_pin_attach_irq(ble_obj.cts_pin, PIN_IRQ_MODE_RISING_FALLING, CTS_handler, RT_NULL);
    // rt_pin_irq_enable(ble_obj.cts_pin, PIN_IRQ_ENABLE);
    // IUROUT_HIGH();
    // RTS_HIGH();

    err_code = at_client_init(BLUEMOD_SR_USING_SERIAL_NAME, 512);
    if (err_code != RT_EOK)
    {
        ble_debug("at_client_init failed. %d\n", err_code);
        return err_code;
    }
    /* urc数据列表初始化 */
    err_code = at_set_urc_table(ble_urc_table, sizeof(ble_urc_table) / sizeof(ble_urc_table[0]));
    if (err_code != RT_EOK)
    {
        ble_debug("at_set_urc_table failed. %d\n", err_code);
        return err_code;
    }
    return err_code;
}

rt_err_t bluemod_sr_open(void)
{
    rt_err_t err_code = RT_EOK;
    bluemod_sr_poweron(&ble_obj, BLUEMOD_POWER_ON);
    rt_hw_us_delay(5000);
    at_response_t resp = RT_NULL;
    resp = at_create_resp(256, 0, 1000);
    err_code = at_exec_cmd(resp, "AT&F");
    if (err_code != 0)
    {
        rt_kprintf("%d, AT&F\n", err_code);
    }
    err_code = at_exec_cmd(resp, "ATE0");
    if (err_code != 0)
    {
        rt_kprintf("%d, ATE0\n", err_code);
    }
    err_code = at_exec_cmd(resp, "AT+UICP=1");
    if (err_code != 0)
    {
        rt_kprintf("%d, AT+UICP=1\n", err_code);
    }
    RTS_LOW();
    IUROUT_LOW();
    rt_thread_mdelay(100);
    at_delete_resp(resp);
    return err_code;
}

void bluemod_sr_close(void)
{
    bluemod_sr_poweron(&ble_obj, BLUEMOD_POWER_OFF);
}

rt_err_t bluemod_sr_set_bname(const char *bname)
{
    rt_err_t err_code;
    at_response_t resp = RT_NULL;
    resp = at_create_resp(256, 0, 1000);
    IUROUT_LOW();
    while (rt_pin_read(ble_obj.cts_pin))
        ;
    rt_kprintf("BNAME SET.\n");
    err_code = at_exec_cmd(resp, "AT+BNAME=%s", bname);
    if (err_code != 0)
    {
        rt_kprintf("%d, AT+BNAME\n", err_code);
    }
    rt_hw_us_delay(110000);
    IUROUT_HIGH();
    while (rt_pin_read(ble_obj.cts_pin) == 0)
        ;
    rt_kprintf("BNAME SET FINISH.\n");
    at_delete_resp(resp);
    return err_code;
}

rt_size_t bluemod_sr_write(const char *data, rt_size_t len)
{
    rt_size_t size;
    IUROUT_LOW();
    while (rt_pin_read(ble_obj.cts_pin))
        ;
    size = at_client_send(data, len);
    rt_hw_us_delay(110000);
    IUROUT_HIGH();
    while (rt_pin_read(ble_obj.cts_pin) == 0)
        ;
    return size;
}

#endif