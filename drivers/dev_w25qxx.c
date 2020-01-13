/*
 * @Author: ShenQuan 
 * @Date: 2019-05-17 15:12:18 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-05-17 15:44:42
 */
#include "dev_w25qxx.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <drv_pin.h>
#include <drv_spi.h>
#include <spi_flash_sfud.h>
#include <sfud/inc/sfud.h>
#include <dfs.h>
#include <dfs_fs.h>
// #include <dfs_elm.h>
#include <fal.h>

#if defined(RT_USING_W25QXX)

static rt_spi_flash_device_t w25q256 = RT_NULL;

static void w25qxx_mount(void)
{
    /* mount root part */
    if (dfs_mount(FAL_PART_NAME_1, "/", "lfs", 0, 0) == 0)
    {
        rt_kprintf("%s mounted on '/'.\n", FAL_PART_NAME_1);
    }
    else
    {
        /* 格式化文件系统 */
        rt_kprintf("%s mount fault, auto mkfs start...\n", FAL_PART_NAME_1);
        dfs_mkfs("lfs", FAL_PART_NAME_1);
        /* 挂载 littlefs */
        if (dfs_mount(FAL_PART_NAME_1, "/", "lfs", 0, 0) == 0)
        {
            rt_kprintf("%s mounted on '/'.\n", FAL_PART_NAME_1);
        }
        else
        {
            rt_kprintf("Failed to mount %s filesystem!\n", FAL_PART_NAME_1);
            return;
        }
    }

    if (mkdir("/usr", 0x777) == 0)
        rt_kprintf("make a directory: '/usr'.\n");

    /* mount data part */
    if (dfs_mount(FAL_PART_NAME_2, "/usr", "lfs", 0, 0) == 0)
    {
        rt_kprintf("%s mounted on '/usr'.\n", FAL_PART_NAME_2);
    }
    else
    {
        /* 格式化文件系统 */
        rt_kprintf("%s mount fault, auto mkfs start...\n", FAL_PART_NAME_2);
        dfs_mkfs("lfs", FAL_PART_NAME_2);
        /* 挂载 littlefs */
        if (dfs_mount(FAL_PART_NAME_2, "/usr", "lfs", 0, 0) == 0)
        {
            rt_kprintf("%s mounted on '/usr'.\n", FAL_PART_NAME_2);
        }
        else
        {
            rt_kprintf("Failed to mount %s filesystem!\n", FAL_PART_NAME_2);
        }
    }
}
FINSH_FUNCTION_EXPORT(w25qxx_mount, w25qxx mount);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(w25qxx_mount, w25qxx mount);
#endif

static void w25qxx_unmount()
{
    if (dfs_unmount("/usr") == 0)
        rt_kprintf("/usr unmounted.\n");
    if (dfs_unmount("/") == 0)
        rt_kprintf("/ unmounted.\n");
}
FINSH_FUNCTION_EXPORT(w25qxx_unmount, w25qxx ummount);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(w25qxx_unmount, w25qxx ummount);
#endif

static int wipe(int argc, char **argv)
{
    if (argc != 2)
    {
        goto _NOTHING;
    }
    if (rt_strncmp(argv[1], "--", 2) != 0)
    {
        goto _NOTHING;
    }
    switch (argv[1][2])
    {
    case 'd':
        /* make a littlefs format filesystem */
        if (dfs_mkfs("lfs", FAL_PART_NAME_2) == 0)
            rt_kprintf("%s make littlefs filesystem success.\n", FAL_PART_NAME_2);
        break;
    case 'r':
        /* make a littlefs format filesystem */
        if (dfs_mkfs("lfs", FAL_PART_NAME_1) == 0)
            rt_kprintf("%s make littlefs filesystem success.\n", FAL_PART_NAME_1);
        break;
    case 'a':
        if (dfs_mkfs("lfs", FAL_PART_NAME_2) == 0)
            rt_kprintf("%s make littlefs filesystem success.\n", FAL_PART_NAME_2);
        if (dfs_mkfs("lfs", FAL_PART_NAME_1) == 0)
            rt_kprintf("%s make littlefs filesystem success.\n", FAL_PART_NAME_1);
        break;
    default:
        goto _NOTHING;
        break;
    }
    return 0;
_NOTHING:
    rt_kprintf("wipe [option --all --data --root]\n");
    return -1;
}
FINSH_FUNCTION_EXPORT(wipe, wipe filesystem like android);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(wipe, boot wipe filesystem like android);
#endif

int rt_hw_w25qxx_init(void)
{
    rt_err_t ret;
    //struct fal_blk_device *blk_dev_boot, *blk_dev_data;
    rt_device_t mtd_dev_root = RT_NULL;
    rt_device_t mtd_dev_data = RT_NULL;
    rt_uint16_t cs_pin = get_PinNumber(GPIO_E_PORT, 4);
    rt_hw_us_delay(150 * 1024); //上电后等待w25qxx 100ms，否则操作失败
    ret = efm32_spi_bus_attach_device(cs_pin, W25QXX_USING_SPI_NAME, "w25qxx");
    w25q256 = rt_sfud_flash_probe("flash0", "w25qxx");
    if (w25q256 == RT_NULL)
    {
        rt_kprintf("rt_sfud_flash_probe failed!\n");
        ret = -1;
        goto _ERROR;
    }

    /* fal init */
    ret = fal_init();
    if (ret <= 0)
    {
        rt_kprintf("fal_init failed!\n");
        goto _ERROR;
    }
    /* dfs init */
    ret = dfs_init();
    if (ret != 0)
    {
        rt_kprintf("dfs_init failed!\n");
        goto _ERROR;
    }

    /* littlefs init*/
    ret = dfs_lfs_init();
    if (ret != 0)
    {
        rt_kprintf("dfs_lfs_init failed!\n");
        goto _ERROR;
    }

    mtd_dev_root = fal_mtd_nor_device_create(FAL_PART_NAME_1);
    if (mtd_dev_root == RT_NULL)
    {
        rt_kprintf("Can't create a mtd device on '%s' partition.\n", FAL_PART_NAME_1);
        ret = -1;
        goto _ERROR;
    }
    mtd_dev_data = fal_mtd_nor_device_create(FAL_PART_NAME_2);
    if (mtd_dev_data == RT_NULL)
    {
        rt_kprintf("Can't create a mtd device on '%s' partition.\n", FAL_PART_NAME_2);
        ret = -1;
        goto _ERROR;
    }

#if defined(W25QXX_DFS_AUTO_MOUNT)
    w25qxx_mount();
#endif

_ERROR:
    return ret;
}

#endif