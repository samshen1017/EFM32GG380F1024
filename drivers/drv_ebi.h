#ifndef DRV_EBI_H
#define DRV_EBI_H

#include <rtthread.h>
#include <rtdevice.h>

#define EBI_BANK0_ADD    (0x80000000)
#define EBI_BANK1_ADD    (0x84000000)

void rt_hw_ebi_init(void);

#endif