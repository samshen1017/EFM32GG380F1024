#include "dev_ls027b7dh.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <drv_pin.h>
#include <drv_spi.h>

#if defined(RT_USING_LS027B7DH)

#ifdef LS027B7DH_DEBUG
#define lcd_debug(format, args...) rt_kprintf(format, ##args)
#else
#define lcd_debug(format, args...)
#endif

#define LS027B7DH_NAME "ls027b7"
#define LCD_DEVICE_NAME "memlcd"
#define REVERSE_SCREEN 1
// Commands
#define MLCD_WR 0x80 //MLCD write line command
#define MLCD_CM 0x20 //MLCD clear memory command
#define MLCD_NO 0x00 //MLCD NOP command (used to switch VCOM)
//LCD resolution
#define MLCD_X 400                   //pixels per horizontal line
#define MLCD_Y 240                   //pixels per vertical line
#define MLCD_BYTES_LINE (MLCD_X / 8) //number of bytes in a line
#define MLCD_BUF_SIZE (MLCD_Y * MLCD_BYTES_LINE)
//defines the VCOM bit in the command word that goes to the LCD
#define VCOM_MASK 0x40
static uint8_t vcom = 0;

struct _memlcd_t
{
    struct rt_device parent;
    struct rt_device_graphic_info lcd_info;
    struct rt_spi_device *spi_dev;
    rt_uint8_t *buf;
    rt_uint16_t cs_pin;
    rt_uint16_t pwr_pin;
};

typedef struct _memlcd_t memlcd_t;
static memlcd_t mlcd_instance;

//置1为白底，置0为黑点
static void memlcd_clear(memlcd_t *mlcd)
{
    rt_size_t ret;
    vcom ^= VCOM_MASK;
    ret = rt_spi_transfer(mlcd->spi_dev, (rt_uint8_t[]){MLCD_CM | vcom, MLCD_NO}, RT_NULL, 2);
    if (ret != 2)
    {
        lcd_debug("memlcd clear failed.\n");
    }
    rt_memset(mlcd->buf, 0xff, MLCD_BUF_SIZE);
}

/** 
 *          Gate Line Address Setup
 *  Line    AG0 AG1 AG2 AG3 AG4 AG5 AG6 AG7
 *  L1      H   L   L   L   L   L   L   L
 *  L2      L   H   L   L   L   L   L   L
 *  L3      H   H   L   L   L   L   L   L
 *  .
 *  .
 *  .
 *  L238    L   H   H   H   L   H   H   H
 *  L239    H   H   H   H   L   H   H   H
 *  L240    L   L   L   L   H   H   H   H
 * 
*/
/**
 * @line 0-239
*/
static inline rt_uint8_t lineToAddr(rt_uint8_t line)
{
    rt_uint8_t addr = 0;
    line++;
    if (line & 0x80)
        addr |= 0x01;
    if (line & 0x40)
        addr |= 0x02;
    if (line & 0x20)
        addr |= 0x04;
    if (line & 0x10)
        addr |= 0x08;
    if (line & 0x08)
        addr |= 0x10;
    if (line & 0x04)
        addr |= 0x20;
    if (line & 0x02)
        addr |= 0x40;
    if (line & 0x01)
        addr |= 0x80;
    return addr;
}

#if REVERSE_SCREEN
static rt_size_t memlcd_refresh(memlcd_t *mlcd, rt_uint8_t line)
{
    rt_size_t ret;
    vcom ^= VCOM_MASK;
    line = MLCD_Y - line - 1;
    rt_uint8_t buf[MLCD_BYTES_LINE + 4] = {0};
    buf[0] = MLCD_WR | vcom;   //command
    buf[1] = lineToAddr(line); //line number
    for (int i = 2; i < (MLCD_BYTES_LINE + 2); i++)
    {
        buf[i] = mlcd->buf[(i - 2) + (line * MLCD_BYTES_LINE)];
    }
    //数组元素最后两个为0x0，具体查看memlcd通信协议
    ret = rt_spi_transfer(mlcd->spi_dev, &buf, RT_NULL, (sizeof(buf) / sizeof(rt_uint8_t)));
    return ret;
}

static void _drawpoint(memlcd_t *mlcd, uint16_t x, uint16_t y, bool bDraw)
{
    uint16_t pos, bx, tmp = 0;
    pos = ((MLCD_X - x - 1) / 8) + ((MLCD_Y - y - 1) * MLCD_BYTES_LINE);
    bx = (MLCD_X - x - 1) % 8;
    tmp |= 1 << (7 - bx);
    if (bDraw)
        (mlcd->buf)[pos] &= ~tmp;
    else
        (mlcd->buf)[pos] |= tmp;
}
#else
static rt_size_t memlcd_refresh(memlcd_t *mlcd, rt_uint8_t line)
{
    rt_size_t ret;
    vcom ^= VCOM_MASK;
    rt_uint8_t buf[MLCD_BYTES_LINE + 4] = {0};
    buf[0] = MLCD_WR | vcom;   //command
    buf[1] = lineToAddr(line); //line number
    for (int i = 2; i < (MLCD_BYTES_LINE + 2); i++)
    {
        buf[i] = mlcd->buf[(i - 2) + (line * MLCD_BYTES_LINE)];
    }
    //数组元素最后两个为0x0，具体查看memlcd通信协议
    ret = rt_spi_transfer(mlcd->spi_dev, &buf, RT_NULL, (sizeof(buf) / sizeof(rt_uint8_t)));
    return ret;
}

static void _drawpoint(memlcd_t *mlcd, uint16_t x, uint16_t y, bool bDraw)
{
    uint16_t pos, bx, tmp = 0;
    pos = (x / 8) + (y * MLCD_BYTES_LINE);
    bx = x % 8;
    tmp |= 1 << (7 - bx);
    if (bDraw)
        (mlcd->buf)[pos] &= ~tmp;
    else
        (mlcd->buf)[pos] |= tmp;
}
#endif

static void memlcd_drawpoint(memlcd_t *mlcd, uint16_t x, uint16_t y, bool bDraw)
{
    _drawpoint(mlcd, x, y, bDraw);
    memlcd_refresh(mlcd, y);
}

static void memlcd_fillRect(memlcd_t *mlcd, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool bDraw)
{
    uint16_t w, h;
    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            _drawpoint(mlcd, x + w, y + h, bDraw);
        }
        memlcd_refresh(mlcd, y + h);
    }
}

#define LCD_POWER_ON() rt_pin_write(mlcd->pwr_pin, PIN_HIGH)
#define LCD_POWER_OFF() rt_pin_write(mlcd->pwr_pin, PIN_LOW)
static rt_err_t memlcd_unit_init(memlcd_t *mlcd)
{
    lcd_debug("memlcd init.\n");
    rt_err_t ret;
    // mlcd->cs_pin = get_PinNumber(GPIO_E_PORT, 14);
    // mlcd->pwr_pin = get_PinNumber(GPIO_A_PORT, 5);
    mlcd->cs_pin = get_PinNumber(GPIO_C_PORT, 9);
    mlcd->pwr_pin = get_PinNumber(GPIO_C_PORT, 8);
    rt_pin_mode(mlcd->pwr_pin, PIN_MODE_OUTPUT);
    ret = efm32_spi_bus_attach_device(mlcd->cs_pin, LS027B7DH_USING_SPI_NAME, LS027B7DH_NAME);
    if (ret != RT_EOK)
    {
        lcd_debug("efm32_spi_bus_attach_device failed: %d\n", ret);
        return ret;
    }
    mlcd->spi_dev = (struct rt_spi_device *)rt_device_find(LS027B7DH_NAME);
    RT_ASSERT(mlcd->spi_dev != RT_NULL);
    mlcd->buf = (rt_uint8_t *)rt_malloc(MLCD_BUF_SIZE);
    struct rt_spi_configuration spi_cfg = {
        .mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB | RT_SPI_CS_HIGH,
        .data_width = 8,
        .max_hz = 2000000,
    };
    ret = rt_spi_configure(mlcd->spi_dev, &spi_cfg);
    if (ret != RT_EOK)
    {
        lcd_debug("rt_spi_configure failed: %d\n", ret);
        return ret;
    }
    LCD_POWER_ON();
    memlcd_clear(mlcd);
    return ret;
}

#ifdef PKG_USING_GUIENGINE
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/widget.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/box.h>
#include <rtgui/image.h>
static rt_err_t memlcd_init(rt_device_t dev)
{
    rt_err_t ret;
    memlcd_t *mlcd = (memlcd_t *)dev;
    ret = memlcd_unit_init(mlcd);
    if (ret != RT_EOK)
    {
        lcd_debug("memlcd_init failed.\n");
        return ret;
    }
    mlcd->lcd_info.width = MLCD_X;
    mlcd->lcd_info.height = MLCD_Y;
    return RT_EOK;
}

static rt_err_t memlcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    lcd_debug("memlcd_open success.\n");
    return RT_EOK;
}

static rt_err_t memlcd_close(rt_device_t dev)
{
    lcd_debug("memlcd_close success.\n");
    return RT_EOK;
}

static rt_err_t memlcd_control(rt_device_t dev, int cmd, void *args)
{
    memlcd_t *mlcd = (memlcd_t *)dev;
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;
        RT_ASSERT(info != RT_NULL);
        //this needs to be replaced by the customer
        info->pixel_format = mlcd->lcd_info.pixel_format;
        info->bits_per_pixel = mlcd->lcd_info.bits_per_pixel;
        info->width = mlcd->lcd_info.width;
        info->height = mlcd->lcd_info.height;
        info->framebuffer = RT_NULL;
        break;
    }
    case RTGRAPHIC_CTRL_RECT_UPDATE:
        /* nothong to be done */
        break;
    default:
        break;
    }
    return RT_EOK;
}

static void set_pixel(const char *pixel, int x, int y)
{
    if (*pixel)
    {
        memlcd_drawpoint(&mlcd_instance, x, y, false);
    }
    else
    {
        memlcd_drawpoint(&mlcd_instance, x, y, true);
    }
}
//置1为白底，置0为黑点
static void get_pixel(char *pixel, int x, int y)
{
    uint16_t pos, bx, tmp = 0;
    rt_uint8_t *buf = mlcd_instance.buf;
    pos = (x / 8) + (y * MLCD_BYTES_LINE);
    bx = x % 8;
    tmp |= 1 << (7 - bx);
    if ((buf[pos] & tmp) == 0)
    {
        *pixel = rtgui_color_to_mono(BLACK);
    }
    else
    {
        *pixel = rtgui_color_to_mono(WHITE);
    }
}

static void draw_hline(const char *pixel, int x1, int x2, int y)
{
    if (*pixel)
    {
        memlcd_fillRect(&mlcd_instance, x1, y, x2 - x1 + 1, 1, false);
    }
    else
    {
        memlcd_fillRect(&mlcd_instance, x1, y, x2 - x1 + 1, 1, true);
    }
}

static void draw_vline(const char *pixel, int x, int y1, int y2)
{
    if (*pixel)
    {
        memlcd_fillRect(&mlcd_instance, x, y1, 1, y2 - y1 + 1, false);
    }
    else
    {
        memlcd_fillRect(&mlcd_instance, x, y1, 1, y2 - y1 + 1, true);
    }
}

static void blit_line(const char *pixel, int x, int y, rt_size_t size)
{
    for (int i = 0; i < size; i++)
    {
        if (pixel[i])
        {
            _drawpoint(&mlcd_instance, x + i, y, false);
        }
        else
        {
            _drawpoint(&mlcd_instance, x + i, y, true);
        }
    }
    memlcd_refresh(&mlcd_instance, y);
}

static struct rt_device_graphic_ops mlcd_ops =
    {
        set_pixel,
        get_pixel,
        draw_hline,
        draw_vline,
        blit_line};
#ifdef RT_USING_DEVICE_OPS
static struct rt_device_ops mlcd_dev_ops =
    {
        .init = memlcd_init,
        .control = memlcd_control};
#endif

int rt_hw_memlcd_init(void)
{
    rt_err_t ret;
    rt_device_t device = &mlcd_instance.parent;
    mlcd_instance.lcd_info.bits_per_pixel = 8;
    mlcd_instance.lcd_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_MONO;
    device->type = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops = &mlcd_dev_ops;
#else
    device->init = memlcd_init;
    device->open = memlcd_open;
    device->close = memlcd_close;
    device->control = memlcd_control;
    device->read = RT_NULL;
    device->write = RT_NULL;
#endif
    device->user_data = &mlcd_ops;
    /* register graphic device driver */
    rt_device_register(device, LCD_DEVICE_NAME, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
    return ret;
}

void lcd_test(void)
{
    /* find lcd device */
    rt_device_t lcd = rt_device_find(LCD_DEVICE_NAME);
    if (lcd == RT_NULL)
    {
        lcd_debug("Can't find LCD\n");
        return;
    }
    rtgui_graphic_set_device(lcd);
    /* init rtgui system server */
    rtgui_system_server_init();
    /* read LCD info */
    struct rt_device_graphic_info lcd_info;
    lcd->control(lcd, RTGRAPHIC_CTRL_GET_INFO, (void *)&lcd_info);
    lcd_debug("LCD size: %dX%d\n", lcd_info.width, lcd_info.height);

    /* create application */
    struct rtgui_app *app;
    app = rtgui_app_create("gui_app");
    if (app == RT_NULL)
    {
        lcd_debug("Create application \"gui_app\" failed!\n");
        return;
    }
    struct rtgui_rect rect1;
    struct rtgui_win *win_info;
    struct rtgui_container *container;
    struct rtgui_graphic_driver *gui_driver = rtgui_graphic_driver_get_default();
    if (gui_driver == NULL)
    {
        lcd_debug("rtgui_graphic_driver_get_default failed.\n");
        return;
    }
    rtgui_graphic_driver_get_rect(gui_driver, &rect1);
    lcd_debug("%d %d %d %d\n", rect1.x1, rect1.x2, rect1.y1, rect1.y2);
}
FINSH_FUNCTION_EXPORT(lcd_test, sharp memory lcd test);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(lcd_test, sharp memory lcd test);
#endif
#endif

void lcd_test2(void)
{
    memlcd_unit_init(&mlcd_instance);
    // for (int y = 0; y < 240; y++)
    // {
    //     for (int x = 0; x < 400; x++)
    //     {
    //         memlcd_drawpoint(&mlcd_instance, x, y, true);
    //         //rt_thread_mdelay(5);
    //     }
    // }
    // rt_kprintf("lcd test finished.\n");
    memlcd_fillRect(&mlcd_instance, 0, 0, 400, 240, true);
    memlcd_fillRect(&mlcd_instance, 0, 0, 10, 10, false);
    memlcd_fillRect(&mlcd_instance, 0, 0, 100, 5, false);
    memlcd_drawpoint(&mlcd_instance, 0, 0, true);
    memlcd_drawpoint(&mlcd_instance, 1, 0, true);
    memlcd_drawpoint(&mlcd_instance, 0, 1, true);
    memlcd_drawpoint(&mlcd_instance, 1, 1, true);
}
FINSH_FUNCTION_EXPORT(lcd_test2, sharp memory lcd test);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(lcd_test2, sharp memory lcd test);
#endif
#endif
