#include "dev_sram.h"
#include "board.h"

#ifdef RT_USING_EXT_SRAM

void rt_hw_ext_sram_init(void)
{
    /* initialize the built-in SRAM as a memory heap */
    rt_memheap_init(&ext_heap, "ext_sram", (void *)EFM32_EXT_HEAP_BEGIN, 384 * 1024);
}

void *ext_sram_alloc(size_t size)
{
    return rt_memheap_alloc(&ext_heap, size);
}

void ext_sram_free(void *ptr)
{
    rt_memheap_free(ptr);
}

void *ext_sram_realloc(void *ptr, size_t new_size)
{
    return rt_memheap_realloc(&ext_heap, ptr, new_size);
}

#endif