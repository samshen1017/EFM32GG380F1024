/*
 * @Author: ShenQuan 
 * @Date: 2019-06-17 16:35:21 
 * @Last Modified by: ShenQuan
 * @Last Modified time: 2019-06-17 17:23:31
 */

#include "drv_serial.h"
#include "hdl_interrupt.h"
//#include <efm32gg380f512.h>

#ifdef RT_USING_SERIAL
#include <drivers/serial.h>

#ifdef RT_SEIRAL_DEBUG
#define serial_debug(format, args...) rt_kprintf(format, ##args)
#else
#define serial_debug(format, args...)
#endif

/* Using UART0 */
#ifdef RT_USING_UART0
static struct rt_serial_device serial0;
static struct rt_serial_rx_fifo uart0_rx_fifo;
static struct efm32_uart huart0 = {
    UART0,
    UART0_RX_IRQn,
    UART0_TX_IRQn,
    UART0_LOCATION,
};
#endif

/* Using UART1 */
#ifdef RT_USING_UART1
static struct rt_serial_device serial1;
static struct rt_serial_rx_fifo uart1_rx_fifo;
static struct efm32_uart huart1 = {
    UART1,
    UART1_RX_IRQn,
    UART1_TX_IRQn,
    UART1_LOCATION,
};
#endif

/* Using USART0 */
#ifdef RT_USING_USART0
static struct rt_serial_device serial2;
static struct rt_serial_rx_fifo usart0_rx_fifo;
static struct efm32_uart husart0 = {
    USART0,
    USART0_RX_IRQn,
    USART0_TX_IRQn,
    USART0_LOCATION,
};
#endif

/* Using USART1 */
#ifdef RT_USING_USART1
static struct rt_serial_device serial3;
static struct rt_serial_rx_fifo usart1_rx_fifo;
static struct efm32_uart husart1 = {
    USART1,
    USART1_RX_IRQn,
    USART1_TX_IRQn,
    USART1_LOCATION,
};
#endif

/* Using USART2 */
#ifdef RT_USING_USART2
static struct rt_serial_device serial4;
static struct rt_serial_rx_fifo usart2_rx_fifo;
static struct efm32_uart husart2 = {
    USART2,
    USART2_RX_IRQn,
    USART2_TX_IRQn,
    USART2_LOCATION,
};
#endif

void usart_rx_isr(struct efm32_uart *uart)
{
    switch ((rt_size_t)uart->uart_regs)
    {
#ifdef RT_USING_UART0
    case (rt_size_t)UART0:
        rt_hw_serial_isr(&serial0, RT_SERIAL_EVENT_RX_IND);
        break;
#endif
#ifdef RT_USING_UART1
    case (rt_size_t)UART1:
        rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
        break;
#endif
#ifdef RT_USING_USART0
    case (rt_size_t)USART0:
        rt_hw_serial_isr(&serial2, RT_SERIAL_EVENT_RX_IND);
        break;
#endif
#ifdef RT_USING_USART1
    case (rt_size_t)USART1:
        rt_hw_serial_isr(&serial3, RT_SERIAL_EVENT_RX_IND);
        break;
#endif
#ifdef RT_USING_USART2
    case (rt_size_t)USART2:
        rt_hw_serial_isr(&serial4, RT_SERIAL_EVENT_RX_IND);
        break;
#endif
    default:
        serial_debug("usart_rx_isr error.\n");
        while (1)
            ;
    }
}

static rt_err_t uart_enable(struct efm32_uart *uart);
static rt_err_t uart_disable(struct efm32_uart *uart);

/**
 * The following is UART operations
 */
static rt_err_t usart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct efm32_uart *uart;
    USART_TypeDef *regs;
    RT_ASSERT(cfg != RT_NULL);

    uart = serial->parent.user_data;
    regs = uart->uart_regs;
    /* stop usart first */
    USART_IntDisable(regs, USART_IEN_RXDATAV);
    USART_Enable(regs, usartDisable);

    serial->config = *cfg;
    /* set baudrate */
    USART_BaudrateAsyncSet(regs, 0, cfg->baud_rate, usartOVS16);
    /* enable usart */
    USART_IntEnable(regs, USART_IEN_RXDATAV);
    USART_Enable(regs, usartEnable);

    return (RT_EOK);
}

static rt_err_t usart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct efm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        NVIC_DisableIRQ(uart->rx_irq);
        uart_disable(uart);
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        NVIC_ClearPendingIRQ(uart->rx_irq);
        NVIC_SetPriority(uart->rx_irq, EFM32_IRQ_PRI_DEFAULT);
        NVIC_EnableIRQ(uart->rx_irq);
        uart_enable(uart);
        break;
    }

    return (RT_EOK);
}

static int usart_putc(struct rt_serial_device *serial, char c)
{
    struct efm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = serial->parent.user_data;

    USART_Tx(uart->uart_regs, c);

    return (1);
}

static int usart_getc(struct rt_serial_device *serial)
{
    struct efm32_uart *uart;
    USART_TypeDef *regs;

    RT_ASSERT(serial != RT_NULL);
    uart = serial->parent.user_data;
    regs = uart->uart_regs;

    /* Some error occured */
    if (regs->IF & (USART_IF_PERR | USART_IF_RXOF))
    {
        regs->IFC = (regs->IF & (USART_IF_PERR | USART_IF_RXOF));

        return (-1);
    }

    if (regs->IF & USART_IF_RXDATAV)
    {
        return (regs->RXDATA);
    }
    else
    {
        return (-1);
    }
}

static struct rt_uart_ops usart_ops = {
    usart_configure,
    usart_control,
    usart_putc,
    usart_getc,
};

static void efm32gg_usart_init(struct efm32_uart *uart)
{
    USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;
    USART_TypeDef *regs;
    CMU_Clock_TypeDef clock;
    efm32_irq_hook_init_t hook;

    switch ((rt_uint32_t)uart->uart_regs)
    {
    case (rt_uint32_t)UART0:
        clock = cmuClock_UART0;
        hook.type = efm32_irq_type_uart; // uart
        hook.unit = 0 * 2 + 1;
        //hook.cbFunc = (void *)UART0_RX_isr;
        break;
    case (rt_uint32_t)UART1:
        clock = cmuClock_UART1;
        hook.type = efm32_irq_type_uart;
        hook.unit = 1 * 2 + 1;
        //hook.cbFunc = (void *)UART1_RX_isr;
        break;
    case (rt_uint32_t)USART0:
        clock = cmuClock_USART0;
        hook.type = efm32_irq_type_usart; // usart
        hook.unit = 0 * 2 + 1;
        //hook.cbFunc = (void *)USART0_RX_isr;
        break;
    case (rt_uint32_t)USART1:
        clock = cmuClock_USART1;
        hook.type = efm32_irq_type_usart;
        hook.unit = 1 * 2 + 1;
        //hook.cbFunc = (void *)USART1_RX_isr;
        break;
    case (rt_uint32_t)USART2:
        clock = cmuClock_USART2;
        hook.type = efm32_irq_type_usart;
        hook.unit = 2 * 2 + 1;
        //hook.cbFunc = (void *)USART2_RX_isr;
        break;
    default:
        return;
    }
    hook.userPtr = uart;
    hook.cbFunc = (void *)usart_rx_isr;
    efm32_irq_hook_register(&hook);
    regs = (USART_TypeDef *)uart->uart_regs;
    CMU_ClockEnable(clock, true);
    USART_InitAsync(regs, &uartInit);
    USART_IntEnable(regs, USART_IEN_RXDATAV);
}

/*
 * uart enable/disable used to reduce the energy consumption of the UART0
 */
static rt_err_t uart_enable(struct efm32_uart *uart)
{
    rt_err_t ret = RT_EOK;
    rt_uint8_t port;
    rt_uint8_t pin_tx, pin_rx;

    switch ((rt_uint32_t)uart->uart_regs)
    {
#ifdef RT_USING_UART0
    case (rt_uint32_t)UART0:
        port = AF_UART0_TX_PORT(uart->location);
        pin_tx = AF_UART0_TX_PIN(uart->location);
        pin_rx = AF_UART0_RX_PIN(uart->location);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_tx, gpioModePushPull, 1);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_rx, gpioModeInput, 1);
        CMU_ClockEnable(cmuClock_UART0, RT_TRUE);

        /* Enable RX and TX pins and set location */
        UART0->ROUTE = (1 << 1) | (1 << 0) | (uart->location << 8);
        break;
#endif

#ifdef RT_USING_UART1
    case (rt_uint32_t)UART1:
        port = AF_UART1_TX_PORT(uart->location);
        pin_tx = AF_UART1_TX_PIN(uart->location);
        pin_rx = AF_UART1_RX_PIN(uart->location);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_tx, gpioModePushPull, 1);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_rx, gpioModeInput, 1);
        CMU_ClockEnable(cmuClock_UART1, RT_TRUE);

        /* Enable RX and TX pins and set location */
        UART1->ROUTE = (1 << 1) | (1 << 0) | (uart->location << 8);
        break;
#endif

#ifdef RT_USING_USART0
    case (rt_uint32_t)USART0:
        port = AF_USART0_TX_PORT(uart->location);
        pin_tx = AF_USART0_TX_PIN(uart->location);
        pin_rx = AF_USART0_RX_PIN(uart->location);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_tx, gpioModePushPull, 1);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_rx, gpioModeInput, 1);
        CMU_ClockEnable(cmuClock_USART0, RT_TRUE);

        /* Enable RX and TX pins and set location */
        USART0->ROUTE = (1 << 1) | (1 << 0) | (uart->location << 8);
        break;
#endif

#ifdef RT_USING_USART1
    case (rt_uint32_t)USART1:
        port = AF_USART1_TX_PORT(uart->location);
        pin_tx = AF_USART1_TX_PIN(uart->location);
        pin_rx = AF_USART1_RX_PIN(uart->location);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_tx, gpioModePushPull, 1);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_rx, gpioModeInput, 1);
        CMU_ClockEnable(cmuClock_USART1, RT_TRUE);

        /* Enable RX and TX pins and set location */
        USART1->ROUTE = (1 << 1) | (1 << 0) | (uart->location << 8);
        break;
#endif

#ifdef RT_USING_USART2
    case (rt_uint32_t)USART2:
        port = AF_USART2_TX_PORT(uart->location);
        pin_tx = AF_USART2_TX_PIN(uart->location);
        pin_rx = AF_USART2_RX_PIN(uart->location);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_tx, gpioModePushPull, 1);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_rx, gpioModeInput, 1);
        CMU_ClockEnable(cmuClock_USART2, RT_TRUE);

        /* Enable RX and TX pins and set location */
        USART2->ROUTE = (1 << 1) | (1 << 0) | (uart->location << 8);
        break;
#endif

    default:
        ret = -RT_ERROR;
        break;
    }

    return (ret);
}

static rt_err_t uart_disable(struct efm32_uart *uart)
{
    rt_err_t ret = RT_EOK;
    CMU_Clock_TypeDef clock;
    rt_uint8_t port;
    rt_uint8_t pin_tx, pin_rx;

    switch ((rt_uint32_t)uart->uart_regs)
    {
#ifdef RT_USING_UART0
    case (rt_uint32_t)UART0:
        port = AF_UART0_TX_PORT(uart->location);
        pin_tx = AF_UART0_TX_PIN(uart->location);
        pin_rx = AF_UART0_RX_PIN(uart->location);
        clock = cmuClock_UART0;

        /* disable TX RX pin */
        UART0->ROUTE &= ~3;
        break;
#endif

#ifdef RT_USING_UART1
    case (rt_uint32_t)UART1:
        port = AF_UART1_TX_PORT(uart->location);
        pin_tx = AF_UART1_TX_PIN(uart->location);
        pin_rx = AF_UART1_RX_PIN(uart->location);
        clock = cmuClock_UART1;

        /* disable TX RX pin */
        UART1->ROUTE &= ~3;
        break;
#endif

#ifdef RT_USING_USART0
    case (rt_uint32_t)USART0:
        port = AF_USART0_TX_PORT(uart->location);
        pin_tx = AF_USART0_TX_PIN(uart->location);
        pin_rx = AF_USART0_RX_PIN(uart->location);
        clock = cmuClock_USART0;

        /* disable TX RX pin */
        USART0->ROUTE &= ~3;
        break;
#endif

#ifdef RT_USING_USART1
    case (rt_uint32_t)USART1:
        port = AF_USART1_TX_PORT(uart->location);
        pin_tx = AF_USART1_TX_PIN(uart->location);
        pin_rx = AF_USART1_RX_PIN(uart->location);
        clock = cmuClock_USART1;

        /* disable TX RX pin */
        USART1->ROUTE &= ~3;
        break;
#endif

#ifdef RT_USING_USART2
    case (rt_uint32_t)USART2:
        port = AF_USART2_TX_PORT(uart->location);
        pin_tx = AF_USART2_TX_PIN(uart->location);
        pin_rx = AF_USART2_RX_PIN(uart->location);
        clock = cmuClock_USART2;

        /* disable TX RX pin */
        USART2->ROUTE &= ~3;
        break;
#endif

    default:
        ret = -RT_ERROR;
        break;
    }

    if (ret == RT_EOK)
    {
        /* Disnable UART clock */
        CMU_ClockEnable(clock, RT_FALSE);
        /* Disable GPIO of UART */
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_tx, gpioModeDisabled, 0);
        GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin_rx, gpioModeDisabled, 0);
    }

    return (ret);
}

void rt_hw_serial_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#ifdef RT_USING_UART0
    /* Initialize uart0 controller */
    efm32gg_usart_init(&huart0);
    serial0.ops = &usart_ops;
    serial0.serial_rx = &uart0_rx_fifo;
    serial0.config = config;
    /* Register UART0 device */
    rt_hw_serial_register(&serial0, "uart0", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, &huart0);
#endif /* RT_USING_UART0 */

#ifdef RT_USING_UART1
    /* Initialize uart1 controller */
    efm32gg_usart_init(&huart1);
    serial1.ops = &usart_ops;
    serial1.serial_rx = &uart1_rx_fifo;
    serial1.config = config;
    /* Register UART1 device */
    rt_hw_serial_register(&serial1, "uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, &huart1);
#endif /* RT_USING_UART1 */

#ifdef RT_USING_USART0
    /* Initialize usart0 controller */
    efm32gg_usart_init(&husart0);
    serial2.ops = &usart_ops;
    serial2.serial_rx = &usart0_rx_fifo;
    serial2.config = config;
    /* Register UART0 device */
    rt_hw_serial_register(&serial2, "usart0", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, &husart0);
#endif /* RT_USING_USART0 */

#ifdef RT_USING_USART1
    /* Initialize usart1 controller */
    efm32gg_usart_init(&husart1);
    serial3.ops = &usart_ops;
    serial3.serial_rx = &usart1_rx_fifo;
    serial3.config = config;
    /* Register USART1 device */
    rt_hw_serial_register(&serial3, "usart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, &husart1);
#endif /* RT_USING_USART1 */

#ifdef RT_USING_USART2
    /* Initialize usart1 controller */
    efm32gg_usart_init(&husart2);
    serial4.ops = &usart_ops;
    serial4.serial_rx = &usart2_rx_fifo;
    serial4.config = config;
    /* Register USART1 device */
    rt_hw_serial_register(&serial4, "usart2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, &husart2);
#endif /* RT_USING_USART2 */
}

#endif
