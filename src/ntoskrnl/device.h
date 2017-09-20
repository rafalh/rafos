#ifndef _DEVICE_H_
#define	_DEVICE_H_

#include "main.h"
#include "string.h"

typedef struct io_dev_t
{
	void *param;
	char name[16];
	int (*open)(struct io_dev_t *dev);
	int (*close)(struct io_dev_t *dev);
	int (*read)(struct io_dev_t *dev, BYTE *buf, UINT64 offset, UINT64 count);
	int (*write)(struct io_dev_t *dev, const BYTE *buf, UINT64 offset, UINT64 count);
} io_dev_t;

extern io_dev_t lpIoDevices[24];
extern unsigned cIoDevices;

int InstallStorage(io_dev_t *dev);

#endif /* _DEVICE_H_ */
