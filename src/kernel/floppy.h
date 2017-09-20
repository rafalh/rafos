#ifndef _FLOPPY_H_
#define _FLOPPY_H_

#include "main.h"
#include "device.h"

/* FDC base address */
#define FDC0_BASE 0x3f0
#define FDC1_BASE 0x370

#ifdef __cplusplus
extern "C" {
#endif

void floppy_irq();
void irq6();

#ifdef __cplusplus
}
#endif

void floppy_init();
int floppy_reset();
void floppy_specify(unsigned flp);
void floppy_send_byte(BYTE command);
BYTE floppy_get_byte();
int floppy_calibrate(unsigned flp);
int floppy_wait();
void floppy_check_interrupt(unsigned *st0, unsigned *cyl);
void floppy_motor_on(unsigned flp);
void floppy_motor_off(unsigned flp);
int floppy_seek(unsigned flp, unsigned head, unsigned cyl);
int floppy_read(unsigned flp, unsigned track, char *buf);
int floppy_write(unsigned flp, unsigned track, char *buf);
int floppy_rw(unsigned flp, unsigned track, BYTE cmd);

/* Storage functions */
int flp_dev_open(io_dev_t *dev);
int flp_dev_close(io_dev_t *dev);
int flp_dev_read(io_dev_t *dev, BYTE *buf, UINT64 offset, UINT64 count);
int flp_dev_write(io_dev_t *dev, const BYTE *buf, UINT64 offset, UINT64 count);

#endif /* _FLOPPY_H_ */
