/*
 * 已函数库的方式调用不太好，不符合驱动规范，后续需要改进（以rt_device_t实现）
 */

#ifndef DEV_BLUEMOD_SR_H
#define DEV_BLUEMOD_SR_H

#include <rtthread.h>
#include <rtdevice.h>

rt_err_t bluemod_sr_open(void);                              //open
void bluemod_sr_close(void);                                 //close
rt_size_t bluemod_sr_write(const char *data, rt_size_t len); //write

void bluemod_sr_set_connect_callback(void (*cb)(void));
void bluemod_sr_set_disconnect_callback(void (*cb)(void));
void bluemod_sr_set_receive_callback(void (*cb)(const char *data, rt_size_t size));
rt_err_t bluemod_sr_set_bname(const char *bname); // io_ctrl

rt_err_t rt_hw_bluemod_sr_init(void);

#endif