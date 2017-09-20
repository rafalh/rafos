#include "dma.h"
#include "asm.h"
#include <stdint.h>

enum DMA_REGISTERS
{
    DMA_SINGLE_CH_MASK_REG   = 0x0A,
    DMA_FLIP_FLOP_RESET_REG  = 0x0C,
    DMA_START_ADDR_REG       = 0x04,
    DMA_COUNT_REG            = 0x05,
    DMA_PAGE_ADDR_REG        = 0x81,
    DMA_MODE_REG             = 0x0B,
};

void open_dma_channel(unsigned char channel, void *address, unsigned count, unsigned mode)
{
    uint32_t addr = (uint32_t)address;
    
    _out(DMA_SINGLE_CH_MASK_REG, channel | 0x04); // mask DMA channel
    _out(DMA_FLIP_FLOP_RESET_REG, 0xFF); // reset flip-flop
    _out(DMA_START_ADDR_REG, addr & 0xFF); // address (low byte)
    _out(DMA_START_ADDR_REG, (addr >> 8) & 0xFF); // address (high byte)
    _out(DMA_PAGE_ADDR_REG, addr >> 16); // external page register
    _out(DMA_FLIP_FLOP_RESET_REG, 0xFF); // reset flip-flop again
    _out(DMA_COUNT_REG, count & 0xFF); // count (low byte)
    _out(DMA_COUNT_REG, count >> 8); // count (high byte)
    _out(DMA_MODE_REG, mode | channel); // mode
    _out(DMA_SINGLE_CH_MASK_REG, channel); // unmask DMA channel
}
