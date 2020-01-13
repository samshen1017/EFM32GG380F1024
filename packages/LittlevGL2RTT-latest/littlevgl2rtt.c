#include "littlevgl2rtt.h"
#include "lvgl.h"

static rt_device_t device;
static struct rt_device_graphic_info info;
static struct rt_messagequeue *input_mq;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if USE_LV_FILESYSTEM
#include <dfs.h>
typedef FILE *pc_file_t;
/*Interface functions to standard C file functions (only the required ones to image handling)*/
static lv_fs_res_t pcfs_open(void *file_p, const char *fn, lv_fs_mode_t mode);
static lv_fs_res_t pcfs_close(void *file_p);
static lv_fs_res_t pcfs_read(void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t pcfs_seek(void *file_p, uint32_t pos);
static lv_fs_res_t pcfs_tell(void *file_p, uint32_t *pos_p);
#endif

/* Todo: add gpu */
static void lcd_fb_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > info.width - 1)
        return;
    if (y1 > info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width - 1 ? info.width - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;
    long int location = 0;

    /* 8 bit per pixel */
    if (info.bits_per_pixel == 8)
    {
        uint8_t *fbp8 = (uint8_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp8[location] = color.full;
            }
        }
    }

    /* 16 bit per pixel */
    else if (info.bits_per_pixel == 16)
    {
        uint16_t *fbp16 = (uint16_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp16[location] = color.full;
            }
        }
    }

    /* 24 or 32 bit per pixel */
    else if (info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
    {
        uint32_t *fbp32 = (uint32_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp32[location] = color.full;
            }
        }
    }

    struct rt_device_rect_info rect_info;

    rect_info.x = x1;
    rect_info.y = y1;
    rect_info.width = x2 - x1;
    rect_info.height = y2 - y1;
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);
}

static void lcd_fb_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > info.width - 1)
        return;
    if (y1 > info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width - 1 ? info.width - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;
    long int location = 0;

    /* 8 bit per pixel */
    if (info.bits_per_pixel == 8)
    {
        uint8_t *fbp8 = (uint8_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }

    /* 16 bit per pixel */
    else if (info.bits_per_pixel == 16)
    {
        uint16_t *fbp16 = (uint16_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }

    /* 24 or 32 bit per pixel */
    else if (info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
    {
        uint32_t *fbp32 = (uint32_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }

    struct rt_device_rect_info rect_info;

    rect_info.x = x1;
    rect_info.y = y1;
    rect_info.width = x2 - x1;
    rect_info.height = y2 - y1;
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);
}

static void lcd_fb_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > info.width - 1)
        return;
    if (y1 > info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width - 1 ? info.width - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;
    long int location = 0;

    /* 8 bit per pixel */
    if (info.bits_per_pixel == 8)
    {
        uint8_t *fbp8 = (uint8_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }

    /* 16 bit per pixel */
    else if (info.bits_per_pixel == 16)
    {
        uint16_t *fbp16 = (uint16_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }

    /* 24 or 32 bit per pixel */
    else if (info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
    {
        uint32_t *fbp32 = (uint32_t *)info.framebuffer;

        for (y = act_y1; y <= act_y2; y++)
        {
            for (x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y)*info.width;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }

    struct rt_device_rect_info rect_info;

    rect_info.x = x1;
    rect_info.y = y1;
    rect_info.width = x2 - x1;
    rect_info.height = y2 - y1;
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);

    lv_flush_ready();
}

static void lcd_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > info.width - 1)
        return;
    if (y1 > info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width - 1 ? info.width - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;

    for (y = act_y1; y <= act_y2; y++)
    {
        char pixel[1] = {color.full};
        rt_graphix_ops(device)->draw_hline(pixel, act_x1, act_x2, y);
        //rt_graphix_ops(device)->draw_hline(&(color.full), act_x1, act_x2, y);
    }
}

static void lcd_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > info.width - 1)
        return;
    if (y1 > info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width - 1 ? info.width - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;

    for (y = act_y1; y <= act_y2; y++)
    {
        char pixel[1] = {color_p->full};
        rt_graphix_ops(device)->blit_line(pixel, act_x1, y, act_x2 - act_x1 + 1);
        //rt_graphix_ops(device)->blit_line(&color_p->full , act_x1, y, act_x2 - act_x1 + 1);
        color_p += (x2 - x1 + 1);
    }
}

static void lcd_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > info.width - 1)
        return;
    if (y1 > info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width - 1 ? info.width - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;

    for (y = act_y1; y <= act_y2; y++)
    {
        rt_graphix_ops(device)->blit_line(color_p, act_x1, y, act_x2 - act_x1 + 1);
        color_p += (x2 - x1 + 1);
    }

    lv_flush_ready();
}

static rt_bool_t touch_down = RT_FALSE;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;

// static bool input_read(lv_indev_data_t *data)
// {
//     data->point.x = last_x;
//     data->point.y = last_y;
//     data->state = (touch_down == RT_TRUE) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

//     return false;
// }

#include <drv_pin.h>
static rt_uint16_t btn0_pin, btn1_pin, btn2_pin;
//static const lv_point_t points_array[] = {{40, 200}, {180, 200}, {330, 200}};
static const lv_point_t points_array[] = {{330, 200}, {180, 200}, {40, 200}};
static void hw_input_device_init(void)
{
    btn0_pin = get_PinNumber(GPIO_F_PORT, 2);
    btn1_pin = get_PinNumber(GPIO_B_PORT, 4);
    btn2_pin = get_PinNumber(GPIO_C_PORT, 1);
    rt_pin_mode(btn0_pin, PIN_MODE_INPUT);
    rt_pin_mode(btn1_pin, PIN_MODE_INPUT);
    rt_pin_mode(btn2_pin, PIN_MODE_INPUT);
    rt_kprintf("hw_input_device_init\n");
}

static int hw_btn_read(void)
{
    if (rt_pin_read(btn0_pin) == PIN_LOW)
    {
        return 0;
    }
    else if (rt_pin_read(btn1_pin) == PIN_LOW)
    {
        return 1;
    }
    else if (rt_pin_read(btn2_pin) == PIN_LOW)
    {
        return 2;
    }
    else
    {
        return -1;
    }
}

static bool button_read(lv_indev_data_t *data)
{
    static uint32_t last_btn = 0; /* Store the last pressed button */
    int btn_pr = hw_btn_read();
    if (btn_pr >= 0)
    {
        last_btn = btn_pr;
        data->state = LV_INDEV_STATE_PR; /* Set the pressed state */
        rt_kprintf("btn: %d\n", btn_pr);
    }
    else
    {
        data->state = LV_INDEV_STATE_REL; /* Set the released state */
    }
    data->btn = last_btn;
    return false;
}

//samshen 2019/07/01
static void lvgl_tick_run(void *p)
{
    while (1)
    {
        // lv_tick_inc(1);
        // rt_thread_delay(1); //delay过短导，导致在进入pm模式时，finsh线程卡死
        lv_tick_inc(10);
        rt_thread_delay(10);
    }
}

void littlevgl2rtt_send_input_event(rt_int16_t x, rt_int16_t y, rt_uint8_t state)
{
    switch (state)
    {
    case LITTLEVGL2RTT_INPUT_UP:
        touch_down = RT_FALSE;
        break;
    case LITTLEVGL2RTT_INPUT_DOWN:
        last_x = x;
        last_y = y;
        touch_down = RT_TRUE;
        break;
    case LITTLEVGL2RTT_INPUT_MOVE:
        last_x = x;
        last_y = y;
        break;
    }
}

#if USE_LV_LOG
void littlevgl2rtt_log_register(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{
    if (level >= LV_LOG_LEVEL)
    {
        //Show the log level if you want
        if (level == LV_LOG_LEVEL_TRACE)
        {
            rt_kprintf("Trace:");
        }

        rt_kprintf("%s\n", dsc);
        //You can write 'file' and 'line' too similary if required.
    }
}
#endif

rt_err_t littlevgl2rtt_init(const char *name)
{
    RT_ASSERT(name != RT_NULL);

    /* LCD Device Init */
    device = rt_device_find(name);
    RT_ASSERT(device != RT_NULL);
    rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info);
    // if (rt_device_open(device, RT_DEVICE_OFLAG_RDWR) == RT_EOK)
    // {
    //     rt_kprintf("rt_device_open\n");
    //     rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info);
    // }
    // rt_kprintf("-- %d --\n", info.bits_per_pixel);
    RT_ASSERT(info.bits_per_pixel == 8 || info.bits_per_pixel == 16 ||
              info.bits_per_pixel == 24 || info.bits_per_pixel == 32);

    if ((info.bits_per_pixel != LV_COLOR_DEPTH) && (info.bits_per_pixel != 32 && LV_COLOR_DEPTH != 24))
    {
        rt_kprintf("Error: framebuffer color depth mismatch! (Should be %d to match with LV_COLOR_DEPTH)",
                   info.bits_per_pixel);
        return RT_ERROR;
    }

    /* littlevgl Init */
    lv_init();

#if USE_LV_LOG
    /* littlevgl Log Init */
    lv_log_register_print(littlevgl2rtt_log_register);
#endif

    /* littlevGL Display device interface */
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    if (info.framebuffer == RT_NULL)
    {
        /* If the display device is a non-framebuffer device,hook the display driver interface according to the specific lcd ic. */
        disp_drv.disp_fill = lcd_fill;
        disp_drv.disp_map = lcd_map;
        disp_drv.disp_flush = lcd_flush;
    }
    else
    {
        disp_drv.disp_fill = lcd_fb_fill;
        disp_drv.disp_map = lcd_fb_map;
        disp_drv.disp_flush = lcd_fb_flush;
    }

    lv_disp_drv_register(&disp_drv);

    /* littlevGL Input device interface */
    hw_input_device_init();
    input_mq = rt_mq_create("lv_input", sizeof(lv_indev_data_t), 256, RT_IPC_FLAG_FIFO);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);

    //indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read = button_read;

    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    lv_indev_set_button_points(indev, points_array);

    /* littlevgl File System device interface */
#ifdef USE_LV_FILESYSTEM
    /**************************
     * IMAGE FROM BINARY FILE
     **************************/

    /* Add a simple drive to open images from PC*/
    lv_fs_drv_t pcfs_drv;                      /*A driver descriptor*/
    memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t)); /*Initialization*/

    pcfs_drv.file_size = sizeof(pc_file_t); /*Set up fields...*/
    pcfs_drv.letter = 'P';
    pcfs_drv.open = pcfs_open;
    pcfs_drv.close = pcfs_close;
    pcfs_drv.read = pcfs_read;
    pcfs_drv.seek = pcfs_seek;
    pcfs_drv.tell = pcfs_tell;
    lv_fs_add_drv(&pcfs_drv);
#endif

    /* littlevGL Tick thread */
    rt_thread_t thread = RT_NULL;

    thread = rt_thread_create("lv_tick", lvgl_tick_run, RT_NULL, 512, 6, 10);
    if (thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    /* Info Print */
    rt_kprintf("[littlevgl2rtt] Welcome to the littlevgl2rtt.\n");
    rt_kprintf("[littlevgl2rtt] You can find latest ver from https://github.com/liu2guang/LittlevGL2RTT.\n");

    return RT_EOK;
}

#if USE_LV_FILESYSTEM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * Open a file from the PC
 * @param file_p pointer to a FILE* variable
 * @param fn name of the file.
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return LV_FS_RES_OK: no error, the file is opened
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t pcfs_open(void *file_p, const char *fn, lv_fs_mode_t mode)
{
    errno = 0;

    const char *flags = "";

    if (mode == LV_FS_MODE_WR)
        flags = "wb";
    else if (mode == LV_FS_MODE_RD)
        flags = "rb";
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        flags = "a+";

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256] = {0};
    sprintf(buf, "/%s", fn);
    //rt_kprintf("%s", buf);
    pc_file_t f = fopen(buf, flags);
    if (f == RT_NULL)
    {
        rt_kprintf("-- pcfs_open FAULT PATH: %s\n", buf);
        return LV_FS_RES_UNKNOWN;
    }
    else
    {
        fseek(f, 0, SEEK_SET);

        /* 'file_p' is pointer to a file descriptor and
         * we need to store our file descriptor here*/
        pc_file_t *fp = file_p; /*Just avoid the confusing casings*/
        *fp = f;
    }
    LV_LOG_INFO("-- pcfs_open SUCCESS\n");
    return LV_FS_RES_OK;
}

/**
 * Close an opened file
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_close(void *file_p)
{
    pc_file_t *fp = file_p; /*Just avoid the confusing casings*/
    int rst = fclose(*fp);
    if(rst != 0){
        return LV_FS_RES_FS_ERR;
    }
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_read(void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    pc_file_t *fp = file_p; /*Just avoid the confusing casings*/
    *br = fread(buf, 1, btr, *fp);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_seek(void *file_p, uint32_t pos)
{
    pc_file_t *fp = file_p; /*Just avoid the confusing casings*/
    fseek(*fp, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FILE* variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_tell(void *file_p, uint32_t *pos_p)
{
    pc_file_t *fp = file_p; /*Just avoid the confusing casings*/
    *pos_p = ftell(*fp);
    return LV_FS_RES_OK;
}

#endif
