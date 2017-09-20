#ifndef _KERNEL_H_
#define	_KERNEL_H_

#include "sysdef.h"

#define DMA_MODE_SINGLE 0x40
#define DMA_MODE_READ 0x4
#define DMA_MODE_WRITE 0x8
#define DMA_MODE_ADDRINC 0x10

#define APIENTRY

void reboot();
void krnl_panic( char *msg );
void krnl_start( void );
void mem_dump( void *ptr, unsigned len );
void open_dma_channel( unsigned channel, PVOID address, unsigned count, unsigned mode );
BYTE get_cmos_byte( unsigned i );
void mem_dump( void *ptr, unsigned len );

#endif /* _KERNEL_H_ */
