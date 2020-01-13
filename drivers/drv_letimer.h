#ifndef DRV_LETIMER_H
#define DRV_LETIMER_H

#include "board.h"
#include <rtthread.h>
#include <rtdevice.h>

#define LETIMER_FREQ_32768 32768 // DIV1
#define LETIMER_FREQ_16384 16384 // DIV2
#define LETIMER_FREQ_8192 8192   // DIV4
#define LETIMER_FREQ_4096 4096   // DIV8
#define LETIMER_FREQ_2048 2048   // DIV16
#define LETIMER_FREQ_1024 1024   // DIV32
#define LETIMER_FREQ_512 512     // DIV64
#define LETIMER_FREQ_256 256     // DIV128
#define LETIMER_FREQ_128 128     // DIV256
#define LETIMER_FREQ_64 64       // DIV512
#define LETIMER_FREQ_32 32       // DIV1024
#define LETIMER_FREQ_16 16       // DIV2048
#define LETIMER_FREQ_8 8         // DIV4096
#define LETIMER_FREQ_4 4         // DIV8192
#define LETIMER_FREQ_2 2         // DIV16384
#define LETIMER_FREQ_1 1         // DIV32768

void rt_hw_letimer_init(void);

#endif /* __DRV_PMTIMER_H__ */
