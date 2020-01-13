#include "board.h"
#include "drv_spi.h"
#include <em_usart.h>
#include <em_dma.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <stdio.h>
#include <string.h>
//#include <drivers/pin.h>

#if defined(RT_USING_SPI)
#ifdef RT_SPI_DEBUG
#define spi_debug(format, args...) rt_kprintf(format, ##args)
#else
#define spi_debug(format, args...)
#endif

#define SPI0 (USART0)
#define SPI1 (USART1)
#define SPI2 (USART2)

#ifdef SPI_USING_DMA
#define SPI0_DMA_CHANNEL_TX 0
#define SPI0_DMA_CHANNEL_RX 1
#define SPI1_DMA_CHANNEL_TX 2
#define SPI1_DMA_CHANNEL_RX 3
#define SPI2_DMA_CHANNEL_TX 4
#define SPI2_DMA_CHANNEL_RX 5

static void txFinished(unsigned int channel, bool primary, void *user)
{
    (void)channel;
    (void)primary;
    struct efm32_spi *hspi = (struct efm32_spi *)user;
    if (channel == hspi->txDmaChannel)
    {
        /* Clear flag to indicate complete transfer */
        hspi->txActive = false;
    }
}

static void rxComplete(unsigned int channel, bool primary, void *user)
{
    (void)channel;
    (void)primary;
    struct efm32_spi *hspi = (struct efm32_spi *)user;
    if (channel == hspi->RxDmaChannel)
    {
        hspi->RxActive = false;
    }
}

static void DMA_Configuration(struct efm32_spi *hspi)
{
    spi_debug("DMA_Configuration: rtChn:%d, txChn:%d\n", hspi->RxDmaChannel, hspi->txDmaChannel);
    DMA_CfgChannel_TypeDef rxChnlCfg;
    DMA_CfgDescr_TypeDef rxDescrCfg;
    DMA_CfgChannel_TypeDef txChnlCfg;
    DMA_CfgDescr_TypeDef txDescrCfg;
    hspi->txCallback.cbFunc = txFinished;
    hspi->txCallback.userPtr = hspi;
    hspi->RxCallback.cbFunc = rxComplete;
    hspi->RxCallback.userPtr = hspi;
    /*** Setting up RX DMA ***/
    /* Setting up DMA channel */
    rxChnlCfg.highPri = false;
    rxChnlCfg.enableInt = true;
    if (hspi->Instance == SPI0)
    {
        hspi->RxDmaChannel = SPI0_DMA_CHANNEL_RX;
        rxChnlCfg.select = DMAREQ_USART0_RXDATAV;
    }
    else if (hspi->Instance == SPI1)
    {
        hspi->RxDmaChannel = SPI1_DMA_CHANNEL_RX;
        rxChnlCfg.select = DMAREQ_USART1_RXDATAV;
    }
    else if (hspi->Instance == SPI2)
    {
        hspi->RxDmaChannel = SPI2_DMA_CHANNEL_RX;
        rxChnlCfg.select = DMAREQ_USART2_RXDATAV;
    }
    rxChnlCfg.cb = &hspi->RxCallback;
    DMA_CfgChannel(hspi->RxDmaChannel, &rxChnlCfg);
    /* Setting up channel descriptor */
    rxDescrCfg.dstInc = dmaDataInc1;
    rxDescrCfg.srcInc = dmaDataIncNone;
    rxDescrCfg.size = dmaDataSize1;
    rxDescrCfg.arbRate = dmaArbitrate1;
    rxDescrCfg.hprot = 0;
    DMA_CfgDescr(hspi->RxDmaChannel, true, &rxDescrCfg);

    /*** Setting up TX DMA ***/
    /* Setting up channel */
    txChnlCfg.highPri = false;
    txChnlCfg.enableInt = true;
    if (hspi->Instance == SPI0)
    {
        hspi->txDmaChannel = SPI0_DMA_CHANNEL_TX;
        txChnlCfg.select = DMAREQ_USART0_TXBL;
    }
    else if (hspi->Instance == SPI1)
    {
        hspi->txDmaChannel = SPI1_DMA_CHANNEL_TX;
        txChnlCfg.select = DMAREQ_USART1_TXBL;
    }
    else if (hspi->Instance == SPI2)
    {
        hspi->txDmaChannel = SPI2_DMA_CHANNEL_TX;
        txChnlCfg.select = DMAREQ_USART2_TXBL;
    }
    txChnlCfg.cb = &hspi->txCallback;
    DMA_CfgChannel(hspi->txDmaChannel, &txChnlCfg);
    /* Setting up channel descriptor */
    txDescrCfg.dstInc = dmaDataIncNone;
    txDescrCfg.srcInc = dmaDataInc1;
    txDescrCfg.size = dmaDataSize1;
    txDescrCfg.arbRate = dmaArbitrate1;
    txDescrCfg.hprot = 0;
    DMA_CfgDescr(hspi->txDmaChannel, true, &txDescrCfg);
    hspi->txActive = false;
    hspi->RxActive = false;
}

static rt_uint32_t _dmaxfer(struct efm32_spi *hspi, const rt_uint8_t *sndb, const rt_uint8_t *rcvb, rt_int32_t length)
{
    /* ---------------------------------- */
    if (rcvb != NULL)
    {
        hspi->RxActive = true;
        hspi->Instance->CMD = USART_CMD_CLEARRX;
        DMA_ActivateBasic(hspi->RxDmaChannel, true, false, (void *)rcvb, (void *)&(hspi->Instance->RXDATA), length - 1);
    }
    hspi->txActive = true;
    hspi->Instance->CMD = USART_CMD_CLEARTX;
    DMA_ActivateBasic(hspi->txDmaChannel, true, false, (void *)&(hspi->Instance->TXDATA), (void *)sndb, length - 1);
    while (hspi->txActive || hspi->RxActive)
        ;
    /* ---------------------------------- */
    return length;
}

static rt_uint32_t spiDmaXfer(struct rt_spi_device *device, struct rt_spi_message *message)
{
    rt_err_t err_code;
    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->bus != RT_NULL);
    RT_ASSERT(device->bus->parent.user_data != RT_NULL);
    struct efm32_spi *hspi = (struct efm32_spi *)(device->bus->parent.user_data);
    struct efm32_hw_spi_cs *cs = device->parent.user_data;
    if (message->cs_take)
    {
        rt_pin_write(cs->pin, cs->select_by_high);
    }
    const rt_uint8_t *sndb = message->send_buf;
    rt_uint8_t *rcvb = message->recv_buf;
    rt_int32_t length = message->length;
    err_code = rt_sem_take(hspi->lock, RT_WAITING_FOREVER);
    if (err_code != RT_EOK)
    {
        spi_debug("rt_sem_take failed.\n");
        return err_code;
    }
//将数据块切开，分段发送适合DMA大小的段
#define SWAP_SIZE 128
    if (length < SWAP_SIZE)
    {
        _dmaxfer(hspi, sndb, rcvb, length);
    }
    else
    {
        for (int i = 0; i < length; i += SWAP_SIZE)
        {
            if ((i + SWAP_SIZE > length))
            {
                _dmaxfer(hspi, sndb + i, rcvb + i, (length % SWAP_SIZE));
            }
            else
            {
                _dmaxfer(hspi, sndb + i, rcvb + i, SWAP_SIZE);
            }
        }
    }
    rt_sem_release(hspi->lock);
    rt_set_errno(err_code);
    if (message->cs_release)
    {
        rt_pin_write(cs->pin, !cs->select_by_high);
    }
    spi_debug("spi xfer:%d\n", message->length);
    return message->length;
}
#endif

static void efm32_spi_unit_init(SPI_TypeDef *spix, rt_uint8_t location)
{
    GPIO_Port_TypeDef port_tx, port_rx, port_clk;
    rt_uint32_t pin_tx, pin_rx, pin_clk;
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    if (spix == SPI0)
    {
        CMU_ClockEnable(cmuClock_USART0, true);
        port_tx = AF_USART0_TX_PORT(location);
        pin_tx = AF_USART0_TX_PIN(location);
        port_rx = AF_USART0_RX_PORT(location);
        pin_rx = AF_USART0_RX_PIN(location);
        port_clk = AF_USART0_CLK_PORT(location);
        pin_clk = AF_USART0_CLK_PIN(location);
    }
    else if (spix == SPI1)
    {
        CMU_ClockEnable(cmuClock_USART1, true);
        port_tx = AF_USART1_TX_PORT(location);
        pin_tx = AF_USART1_TX_PIN(location);
        port_rx = AF_USART1_RX_PORT(location);
        pin_rx = AF_USART1_RX_PIN(location);
        port_clk = AF_USART1_CLK_PORT(location);
        pin_clk = AF_USART1_CLK_PIN(location);
    }
    else if (spix == SPI2)
    {
        CMU_ClockEnable(cmuClock_USART2, true);
        port_tx = AF_USART2_TX_PORT(location);
        pin_tx = AF_USART2_TX_PIN(location);
        port_rx = AF_USART2_RX_PORT(location);
        pin_rx = AF_USART2_RX_PIN(location);
        port_clk = AF_USART2_CLK_PORT(location);
        pin_clk = AF_USART2_CLK_PIN(location);
    }

    /* Config GPIO */
    GPIO_PinModeSet(port_tx, pin_tx, gpioModePushPull, 0);
    GPIO_PinModeSet(port_rx, pin_rx, gpioModeInputPull, 0);
    GPIO_PinModeSet(port_clk, pin_clk, gpioModePushPull, 0);
    spi_debug("spi io init: tx: %x %x \n rx: %x %x \n clk: %x %x \n",
              port_tx, pin_tx,
              port_rx, pin_rx,
              port_clk, pin_clk);
}

//static rt_err_t efm32_spi_init(struct efm32_spi *spi, rt_uint8_t location, struct rt_spi_configuration *cfg)
static rt_err_t efm32_spi_init(struct rt_spi_device *device)
{
    RT_ASSERT(device != RT_NULL);
    struct efm32_spi *hspi = (struct efm32_spi *)(device->bus->parent.user_data);
    struct rt_spi_configuration *cfg = hspi->cfg;
    SPI_TypeDef *spix = hspi->Instance;
    rt_uint8_t location = hspi->location;
    struct efm32_hw_spi_cs *cs = device->parent.user_data;
    USART_InitSync_TypeDef spi_init_t = USART_INITSYNC_DEFAULT;
    /** master or slave mode */
    if (cfg->mode & RT_SPI_SLAVE)
    {
        spi_init_t.master = false;
    }
    else
    {
        spi_init_t.master = true;
    }

    /** Number of databits in frame */
    if (cfg->data_width == 8)
    {
        spi_init_t.databits = usartDatabits8;
    }
    else if (cfg->data_width == 16)
    {
        spi_init_t.databits = usartDatabits16;
    }
    else
    {
        spi_debug("unsupported data bits.\n");
        return RT_EIO;
    }
    /**
     * At CPOL=0 the base value of the clock is zero
     *  - For CPHA=0, data are captured on the clock's rising edge (low->high transition)
     *    and data are propagated on a falling edge (high->low clock transition).
     *  - For CPHA=1, data are captured on the clock's falling edge and data are
     *    propagated on a rising edge.
     * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)
     *  - For CPHA=0, data are captured on clock's falling edge and data are propagated
     *    on a rising edge.
     *  - For CPHA=1, data are captured on clock's rising edge and data are propagated
     *    on a falling edge.
     */
    if ((cfg->mode & RT_SPI_CPOL) == 0)
    {
        if ((cfg->mode & RT_SPI_CPHA) == 0)
        {
            /** Clock idle low, sample on rising edge. */
            spi_init_t.clockMode = usartClockMode0;
        }
        else
        {
            /** Clock idle low, sample on falling edge. */
            spi_init_t.clockMode = usartClockMode1;
        }
    }
    else
    {
        if ((cfg->mode & RT_SPI_CPHA) == 0)
        {
            /** Clock idle high, sample on falling edge. */
            spi_init_t.clockMode = usartClockMode2;
        }
        else
        {
            /** Clock idle high, sample on rising edge. */
            spi_init_t.clockMode = usartClockMode3;
        }
    }

    /** it to 0 if currently configurated reference clock shall be used */
    spi_init_t.refFreq = 0;

    /** Desired baudrate */
    //HFPERCLK/2
    rt_uint32_t spi_maxHZ = CMU_ClockFreqGet(cmuClock_HFPER) / 2 - 1;
    if (cfg->max_hz > spi_maxHZ)
    {
        spi_init_t.baudrate = spi_maxHZ;
    }
    else
    {
        spi_init_t.baudrate = cfg->max_hz;
    }
    /** msb or lsb */
    if (cfg->mode & RT_SPI_MSB)
    {
        spi_init_t.msbf = true;
    }
    else
    {
        spi_init_t.msbf = false;
    }
    if (cfg->mode & RT_SPI_CS_HIGH)
    {
        cs->select_by_high = true;
        rt_pin_write(cs->pin, 0); //cs高为选中，所以默认为低
    }
    else
    {
        cs->select_by_high = false;
        rt_pin_write(cs->pin, 1);
    }
    /* 开启ext SRAM后, 由于SRAM速度比较慢, 造成spi flash 挂死, 所以强制将SPI速度设置为8M.*/
    //TODO 以后优化，提高SRAM时序速度
    spi_init_t.baudrate = 10000000;
#if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_TINY_FAMILY)
    spi_init_t.prsRxEnable = false;
    spi_init_t.prsRxCh = usartPrsRxCh0;
    spi_init_t.autoTx = false;
#endif
    efm32_spi_unit_init(spix, location);
    USART_InitSync(spix, &spi_init_t);
#ifdef SPI_USING_DMA
    DMA_Configuration(hspi);
#endif
    spix->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_CLKPEN | (location << _USART_ROUTE_LOCATION_SHIFT);
    spix->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
    spi_debug("spi addr: %p \n", spix);
    spi_debug("enable: 0x%x \n", spi_init_t.enable);
    spi_debug("baudrate: %d \n", spi_init_t.baudrate);
    spi_debug("databits: %d \n", spi_init_t.databits);
    spi_debug("master: 0x%x \n", spi_init_t.master);
    spi_debug("msbf: 0x%x \n", spi_init_t.msbf);
    spi_debug("clockMode: 0x%x \n", spi_init_t.clockMode);
    spi_debug("spix->ROUTE: 0x%x \n", spix->ROUTE);
    spi_debug("cs select by high: %d\n", cs->select_by_high);
    return RT_EOK;
}

static rt_uint32_t spiXfer(struct rt_spi_device *device, struct rt_spi_message *message)
{

    rt_err_t err_code;
    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->bus != RT_NULL);
    RT_ASSERT(device->bus->parent.user_data != RT_NULL);
    struct efm32_spi *hspi = (struct efm32_spi *)(device->bus->parent.user_data);
    struct efm32_hw_spi_cs *cs = device->parent.user_data;
    if (message->cs_take)
    {
        rt_pin_write(cs->pin, cs->select_by_high);
    }
    const rt_uint8_t *sndb = message->send_buf;
    rt_uint8_t *rcvb = message->recv_buf;
    rt_int32_t length = message->length;
    err_code = rt_sem_take(hspi->lock, RT_WAITING_FOREVER);
    if (err_code != RT_EOK)
    {
        spi_debug("rt_sem_take failed.\n");
        return 0;
    }
    while (length)
    {

        rt_uint8_t txbuf = 0xff;
        int index = message->length - length;
        if (sndb != RT_NULL)
        {
            txbuf = sndb[index];
        }
        USART_Tx(hspi->Instance, txbuf);
        while (!(hspi->Instance->STATUS & USART_STATUS_TXC))
            ;
        rcvb[index] = USART_Rx(hspi->Instance);
        length--;
    }
    rt_sem_release(hspi->lock);
    rt_set_errno(err_code);
    if (message->cs_release)
    {

        rt_pin_write(cs->pin, !cs->select_by_high);
    }
    return message->length - length;
}

rt_err_t spi_configure(struct rt_spi_device *device, struct rt_spi_configuration *configuration)
{
    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(configuration != RT_NULL);

    struct efm32_spi *hspi = (struct efm32_spi *)(device->bus->parent.user_data);
    hspi->cfg = configuration;
    return efm32_spi_init(device);
    //return efm32_spi_init(hspi, hspi->location, configuration);
}

const struct rt_spi_ops efm32_spi_ops =
    {
        .configure = spi_configure,
#ifdef SPI_USING_DMA
        .xfer = spiDmaXfer,
#else
        .xfer = spiXfer,
#endif
};

rt_err_t efm32_spi_register_bus(SPI_TypeDef *SPIx, const char *name, int location)
{
    struct rt_spi_bus *spi_bus = (struct rt_spi_bus *)rt_malloc(sizeof(struct rt_spi_bus));
    RT_ASSERT(spi_bus != RT_NULL);

    struct efm32_spi *hspi = (struct efm32_spi *)rt_malloc(sizeof(struct efm32_spi));
    RT_ASSERT(hspi != RT_NULL);

    hspi->location = location;
    hspi->Instance = SPIx;
    char sem_name[12] = {0};
    sprintf(sem_name, "s_%s", name);

    hspi->lock = (struct rt_semaphore *)rt_malloc(sizeof(struct rt_semaphore));
    RT_ASSERT(hspi->lock != RT_NULL);

    if (rt_sem_init(hspi->lock, sem_name, 1, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("rt sem init failed!\n");
        while (1)
            ;
    }
    spi_bus->parent.user_data = hspi;
    return rt_spi_bus_register(spi_bus, name, &efm32_spi_ops);
}

rt_err_t efm32_spi_bus_attach_device(rt_uint32_t pin, const char *bus_name, const char *device_name)
{
    struct rt_spi_device *spi_device = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device));
    RT_ASSERT(spi_device != RT_NULL);

    struct efm32_hw_spi_cs *cs_pin = (struct efm32_hw_spi_cs *)rt_malloc(sizeof(struct efm32_hw_spi_cs));
    RT_ASSERT(cs_pin != RT_NULL);

    cs_pin->pin = pin;
    rt_pin_mode(pin, PIN_MODE_OUTPUT);
    return rt_spi_bus_attach_device(spi_device, device_name, bus_name, (void *)cs_pin);
}

void efm32_hw_spi_init(void)
{
    rt_err_t ret;
#ifdef RT_USING_SPI0
    ret = efm32_spi_register_bus(SPI0, RT_SPI0_NAME, SPI0_LOCATION);
    if (ret != RT_EOK)
    {
        spi_debug("spi0 register failed %d\n", ret);
    }
#endif

#ifdef RT_USING_SPI1
    ret = efm32_spi_register_bus(SPI1, RT_SPI1_NAME, SPI1_LOCATION);
    if (ret != RT_EOK)
    {
        spi_debug("spi1 register failed %d\n", ret);
    }
#endif

#ifdef RT_USING_SPI2
    ret = efm32_spi_register_bus(SPI2, RT_SPI2_NAME, SPI2_LOCATION);
    if (ret != RT_EOK)
    {
        spi_debug("spi2 register failed %d\n", ret);
    }
#endif
}

#endif