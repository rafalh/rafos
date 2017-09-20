#ifndef _DMA_H_
#define _DMA_H_

enum DMA_MODE
{
    DMA_MODE_SINGLE  = 0x40,
    DMA_MODE_READ    = 0x04,
    DMA_MODE_WRITE   = 0x08,
    DMA_MODE_ADDRINC = 0x10,
};

void open_dma_channel(unsigned char channel, void *address, unsigned count, unsigned mode);

#endif // _DMA_H
