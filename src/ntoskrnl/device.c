#include "device.h"
#include "string.h"

io_dev_t lpIoDevices[24];
unsigned cIoDevices = 0;

int InstallStorage(io_dev_t *dev)
{
    if(cIoDevices >= 24)
        return -1;
    memcpy(&lpIoDevices[cIoDevices], dev, sizeof(*dev));
    ++cIoDevices;
    return 0;
}
