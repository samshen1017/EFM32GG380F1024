#include "drv_ebi.h"
#include "board.h"
#include "efm32.h"
#include "em_gpio.h"
#include "em_ebi.h"
#include "em_cmu.h"

#ifdef RT_USING_EBI

void rt_hw_ebi_init(void)
{
    //EBI_Init_TypeDef ebiConfig = EBI_INIT_DEFAULT;

    /* Enable clocks*/
    CMU_ClockEnable(cmuClock_EBI, true);

    /* EBI GPIO config. */
    {
        /* [AD00:AD07] */
        GPIO_PinModeSet(gpioPortE, 8, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 9, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 11, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 14, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortE, 15, gpioModePushPull, 0);
        /* [AD08] */
        GPIO_PinModeSet(gpioPortA, 15, gpioModePushPull, 0);
        /* [AD9:AD15] */
        GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortA, 4, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 0);
        GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);

        /* EBI_ALE */
        GPIO_PinModeSet(gpioPortC, 11, gpioModePushPull, 0);

        /* EBI_WE and EBI_RE */
        GPIO_PinModeSet(gpioPortF, 8, gpioModePushPull, 1);
        GPIO_PinModeSet(gpioPortF, 9, gpioModePushPull, 1);

        /* EBI_CS0 and EBI_CS1 */
        GPIO_PinModeSet(gpioPortD, 9, gpioModePushPull, 1);
        GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 1);
    }
    /* EBI bank config. */
    {
        EBI_Init_TypeDef ebiConfig =
            {
                ebiModeD8A8,   /* 8 bit address, 8 bit data */
                ebiActiveLow,  /* ARDY polarity */
                ebiActiveHigh, /* ALE polarity */
                ebiActiveLow,  /* WE polarity */
                ebiActiveLow,  /* RE polarity */
                ebiActiveLow,  /* CS polarity */
                ebiActiveLow,  /* BL polarity */
                false,         /* disble BL */
                false,         /* enable NOIDLE */
                false,         /* disable ARDY */
                false,         /* disable ARDY timeout */
                EBI_BANK0,     /* enable bank 0 */
                EBI_CS0,       /* no chip select */
                0,             /* addr setup cycles */
                1,             /* addr hold cycles */
                false,         /* disable half cycle ALE strobe */
                0,             /* read setup cycles */
                0,             /* read strobe cycles */
                0,             /* read hold cycles */
                false,         /* disable page mode */
                false,         /* disable prefetch */
                false,         /* disable half cycle REn strobe */
                0,             /* write setup cycles */
                0,             /* write strobe cycles */
                1,             /* write hold cycles */
                false,         /* enable the write buffer */
                false,         /* disable half cycle WEn strobe */
                ebiALowA0,     /* ALB - Low bound, address lines */
                ebiAHighA0,    /* APEN - High bound, address lines */
                ebiLocation1,  /* Use Location 1 */
                true,          /* enable EBI */
            };

        /* config BANK1 for SRAM. */
        {
            ebiConfig.mode = ebiModeD8A24ALE;
            ebiConfig.banks = EBI_BANK1;
            ebiConfig.csLines = EBI_CS1;

            /* AddressTiming */
            ebiConfig.addrSetupCycles = 1; /* 0~3 */
            ebiConfig.addrHoldCycles = 1;  /* 0~3 */

            /* ReadTiming */
            ebiConfig.readSetupCycles = 1;  /* 0~3 */
            ebiConfig.readStrobeCycles = 3; /* 0~63 */
            ebiConfig.readHoldCycles = 1;   /* 0~3 */

            /* WriteTiming */
            ebiConfig.writeSetupCycles = 1;  /* 0~3 */
            ebiConfig.writeStrobeCycles = 2; /* 0~63 */
            ebiConfig.writeHoldCycles = 1;   /* 0~3 */

            EBI_Init(&ebiConfig);
        }
    }
}

#endif
