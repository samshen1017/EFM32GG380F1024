#include "rtthread.h"
#include "rtdevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(RT_USING_RYM) && defined(RT_USING_DFS)
#include "ymodem.h"
#include <dirent.h>
#define DEFAULT_DOWNLOAD_PART "/download"

rt_device_t serial_get_device()
{
    return rt_device_find(RT_CONSOLE_DEVICE_NAME);
}

static char *recv_path = RT_NULL;
static rt_uint32_t update_file_total_size, update_file_cur_size;
//static int fd;
static FILE *cur_f;

static enum rym_code ymodem_on_begin(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    DIR *d;
    char *file_name, *file_size;
    char file_str[100] = {0};
    /* calculate and store file size */
    file_name = (char *)&buf[0];
    file_size = (char *)&buf[rt_strlen(file_name) + 1];
    update_file_total_size = atol(file_size);
    update_file_cur_size = 0;
    d = opendir(recv_path);
    if (d == NULL)
    {
        mkdir(recv_path, 0x777);
    }
    closedir(d);
    sprintf(file_str, "%s/%s", recv_path, file_name);
    cur_f = fopen(file_str, "w");
    if (cur_f == NULL)
    {
        /* if fail then end session */
        return RYM_CODE_CAN;
    }
    return RYM_CODE_ACK;
}

static enum rym_code ymodem_on_data(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    int ret = fwrite(buf, 1, len, cur_f);
    if (ret != len)
    {
        /* if fail then end session */
        return RYM_CODE_CAN;
    }
    return RYM_CODE_ACK;
}

static enum rym_code ymodem_on_end(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    int ret = fwrite(buf, 1, len, cur_f);
    if (ret != len)
    {
        /* if fail then end session */
        return RYM_CODE_CAN;
    }
    fflush(cur_f);
    fclose(cur_f);
    cur_f = NULL;
    return RYM_CODE_ACK;
}

void ymodem_ota(rt_uint8_t argc, void **argv)
{
#ifdef RT_USING_PM
    rt_pm_run_enter(PM_RUN_MODE_HIGH_SPEED);
    rt_pm_release(PM_SLEEP_MODE_DEEP);
#endif
    struct rym_ctx rctx;
    if (argc < 2)
    {
        recv_path = DEFAULT_DOWNLOAD_PART;
        rt_kprintf("Default save firmware on /download.\n");
    }
    else
    {
        const char *operator= argv[1];
        if (!strcmp(operator, "-p"))
        {
            if (argc < 3)
            {
                rt_kprintf("Usage: ymodem_ota -p <partiton name>.\n");
                return;
            }
            else
            {
                /* change default partition to save firmware */
                recv_path = argv[2];
            }
        }
        else
        {
            rt_kprintf("Usage: ymodem_ota -p <partiton name>.\n");
            return;
        }
    }
    rt_kprintf("Please select the file and use Ymodem to send.\n");
    if (!rym_recv_on_device(&rctx, rt_console_get_device(), RT_DEVICE_OFLAG_RDWR,
                            ymodem_on_begin, ymodem_on_data, ymodem_on_end, RT_TICK_PER_SECOND))
    {
        // fflush(cur_f);
        // fclose(cur_f);
        rt_kprintf("Download file to flash success.\n");
    }
    else
    {
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("Update file failed.\n");
    }
#ifdef RT_USING_PM
    rt_pm_run_enter(PM_RUN_MODE_LOW_SPEED);
    rt_pm_request(PM_SLEEP_MODE_DEEP);
#endif
    return;
}
MSH_CMD_EXPORT(ymodem_ota, Ymodem download file to flash chip);

#endif