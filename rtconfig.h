#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 256
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512
#define RT_DEBUG

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_MEMHEAP_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "leuart0"
#define RT_VER_NUM 0x40002

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 16

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_HWTIMER
#define RT_USING_I2C
#define RT_USING_PIN
#define RT_USING_MTD_NOR
#define RT_USING_PM
#define RT_USING_RTC
#define RT_USING_SPI
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE

/* Using Hardware Crypto drivers */


/* Using WiFi */


/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC

/* Network */

/* Socket abstraction layer */


/* Network interface device */


/* light weight TCP/IP stack */


/* Modbus master and slave stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */

#define RT_USING_RYM

/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */

#define PKG_USING_GUIENGINE
#define PKG_USING_GUIENGINE_LATEST_VERSION
#define PKG_USING_RGB888_PIXEL_BITS_32
#define PKG_USING_RGB888_PIXEL_BITS 32
#define GUIENGINE_CMD_STRING_MAX 16
#define GUIENGINE_USING_FONT16
#define GUIENGINE_USING_FONT12
#define GUIENGINE_NAME_MAX 16
#define PKG_USING_FAL
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define FAL_USING_SFUD_PORT
#define FAL_USING_NOR_FLASH_DEV_NAME "flash0"
#define PKG_USING_FAL_LATEST_VERSION
#define PKG_FAL_VER_NUM 0x99999
#define PKG_USING_LITTLEFS
#define PKG_USING_LITTLEFS_LATEST_VERSION
#define LFS_READ_SIZE 256
#define LFS_PROG_SIZE 256
#define LFS_BLOCK_SIZE 4096
#define LFS_CACHE_SIZE 256
#define LFS_BLOCK_CYCLES 100
#define LFS_LOOKAHEAD_MAX 128

/* peripheral libraries and drivers */


/* miscellaneous packages */


/* samples: kernel and components samples */


/* EFM32 EBI Driver */

#define RT_USING_EBI
#define RT_USING_EXT_SRAM
#define RT_USING_EXT_MALLOC

/* Override rt_malloc with extended sram */

/* please modify rtconfig.h that remove " " in #define */

#define RT_KERNEL_MALLOC ext_sram_alloc
#define RT_KERNEL_FREE ext_sram_free
#define RT_KERNEL_REALLOC ext_sram_realloc

/* EFM32 Serial Driver */

#define RT_USING_LEUART0
#define LEUART0_LOCATION 0x0
#define RT_LEUART0_NAME "leuart0"
#define RT_USING_UART0
#define UART0_LOCATION 0x0
#define RT_UART0_NAME "uart0"
#define RT_USING_USART2
#define USART2_LOCATION 0x0
#define RT_USART2_NAME "usart2"

/* EFM32 I2C Driver */

#define RT_USING_I2C0
#define I2C0_LOCATION 0x2
#define RT_I2C0_NAME "i2c0"
#define RT_USING_I2C1
#define I2C1_LOCATION 0x1
#define RT_I2C1_NAME "i2c1"

/* EFM32 SPI Driver */

#define SPI_USING_DMA
#define RT_USING_SPI0
#define SPI0_LOCATION 0x1
#define RT_SPI0_NAME "spi0"
#define RT_USING_SPI1
#define SPI1_LOCATION 0x1
#define RT_SPI1_NAME "spi1"

/* EFM32 WDT */

/* EFM32 TIMER */

#define RT_USING_LETIMER0
#define RT_LETIMER0_NAME "letimer0"

/* EFM32 W25QXX Device support */

#define RT_USING_W25QXX
#define W25QXX_USING_SPI_NAME "spi0"

/* EFM32 LS027B7DH Device support */

#define RT_USING_LS027B7DH
#define LS027B7DH_USING_SPI_NAME "spi1"

/* EFM32 ZSC31014 Device support */

#define RT_USING_ZSC31014
#define ZSC31014_USING_I2C_NAME "i2c0"

/* EFM32 SI114X Device support */

#define RT_USING_SI114X
#define SI114X_USING_I2C_NAME "i2c1"

/* EFM32 BlueMod_SR Device support */


/* EFM32 RX8025 Device support */

#define RX8025_RTC_NAME "rtc"
#define RX8025_USING_I2C_NAME "i2c0"

#endif
