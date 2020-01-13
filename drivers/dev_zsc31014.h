#ifndef __DEV_ZSC31014_H__
#define __DEV_ZSC31014_H__

#include <rtthread.h>
#include <rtdevice.h>

rt_err_t zsc31014_read(rt_uint8_t *data, rt_size_t size);
rt_err_t rt_hw_zsc31014_init(void);

#endif /* __DEV_ZSC31014_H__ */
