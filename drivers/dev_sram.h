#ifndef DEV_SRAM_H
#define DEV_SRAM_H

#include <rtthread.h>

void rt_hw_ext_sram_init(void);

void *ext_sram_alloc(size_t size);
void ext_sram_free(void *ptr);
void *ext_sram_realloc(void *ptr, size_t new_size);

#endif //DEV_SRAM_H