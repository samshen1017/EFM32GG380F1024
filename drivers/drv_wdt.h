#ifndef __DRV_WDT_H__
#define __DRV_WDT_H__

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "em_wdog.h"
#include <rtconfig.h>

void rt_hw_wdt_init(void);

#endif