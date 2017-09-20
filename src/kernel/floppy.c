#include "floppy.h"
#include "main.h"
#include "string.h"
#include "io.h"
#include "irq.h"
#include "device.h"
#include "memory.h"
#include "dma.h"
#include "cmos.h"
#include "timer.h"

/* Registers */
#define STATUS_REGISTER_A       0 // read-only
#define STATUS_REGISTER_B       1 // read-only
#define DIGITAL_OUTPUT_REG      2
#define TAPE_DRIVE_REG          3
#define MAIN_STATUS_REG         4 // read-only
#define DATA_RATE_SELECT_REG    4 // write-only
#define DATA_FIFO               5
#define DIGITAL_INPUT_REG       7 // read-only
#define CONFIG_CONTROL_REG      7 // write-only

/* Commands */
#define CMD_SPECIFY             0x03
#define CMD_WRITE_DATA          0xC5
#define CMD_READ_DATA           0xC6
#define CMD_RECALIBRATE         0x07
#define CMD_SENSE_INTERRUPT     0x08
#define CMD_SEEK                0x0F

#define FLOPPY_DMA_CHANNEL      2
#define FLOPPY_DMA_LEN          0x4800

#define MSR_MRQ 0x80
#define MSR_DIO 0x40

#define DOR_DRV0 0x0
#define DOR_DRV1 0x1
#define DOR_NOTRESET 0x4
#define DOR_DMA 0x8
#define DOR_MOT0 0x10
#define DOR_MOT1 0x20

typedef struct floppy_geometry_t
{
    unsigned heads;
    unsigned cylinders;
    unsigned sectors;
    unsigned speed; /* 0 - 500 kbits/s, 1 - 300 kbits/s, 2 - 250 kbits/s */
} floppy_geometry_t;

const struct floppy_geometry_t floppy_geometry[8] =
{
    { 0, 0,  0,  0 },    // no drive
    { 2, 40, 9,  2 },    // 360KB 5.25"
    { 2, 80, 15, 0 },    // 1.2MB 5.25"
    { 2, 80, 9,  1 },    // 720KB 3.5"
    { 2, 80, 18, 0 },    // 1.44MB 3.5"
    { 2, 80, 36, 0 },    // 2.88MB 3.5"
    { 0, 0,  0,  0 },    // unknown
    { 0, 0,  0,  0 },    // unknown
};

const unsigned floppy_speed[4] = { 500000, 300000, 250000, 1000000 }; /* predkosci transferu w bit/s */

typedef struct floppy_t
{
    unsigned uMotor;
    const struct floppy_geometry_t *geom;
} floppy_t;

floppy_t floppy[2] = {{0, NULL}, {0, NULL}};
volatile unsigned bFloppyIrq = 0;
unsigned uFlpInBuf = 0xffffffff, uTrackInBuf = 0xffffffff;
PVOID lpFlpDmaBuf;

void floppy_irq()
{
    bFloppyIrq = 1;
}

void floppy_init()
{
    BYTE i;
    struct io_dev_t io_dev;
    
    i = get_cmos_byte(0x10);
    
    if(!i) /* No floppies */
        return;
        
    if(!(lpFlpDmaBuf = MmAllocateContiguousMemory(FLOPPY_DMA_LEN, 0xffffff)))     /* Alokacja bufora */
        return;
        
    set_intr_gate(0x26, irq6);
    enable_irq(0x6);
    
    io_dev.open = flp_dev_open;
    io_dev.close = flp_dev_close;
    io_dev.read = flp_dev_read;
    io_dev.write = flp_dev_write;
    
    if(i >> 4)  // Floppy A
    {
        floppy[0].geom = &floppy_geometry[i >> 4];
        strcpy(io_dev.name, "Floppy A");
        io_dev.param = (void*)0;
        InstallStorage(&io_dev);
    }
    if(i & 0xf) // Floppy B
    {
        floppy[1].geom = &floppy_geometry[i & 0xf];
        strcpy(io_dev.name, "Floppy B");
        io_dev.param = (void*)1;
        InstallStorage(&io_dev);
    }
    
    if(floppy_reset())
        return;
}

int floppy_reset()
{
    unsigned i;
    unsigned st0, cyl;
    
    bFloppyIrq = 0;
    
    outportb((FDC0_BASE + DIGITAL_OUTPUT_REG), 0x00); /* disable controller */
    outportb((FDC0_BASE + DIGITAL_OUTPUT_REG), DOR_NOTRESET | DOR_DMA); /* enable controller */
    
    if(floppy_wait())
        return -2;
        
    for(i = 0; i < 4; ++i)
        floppy_check_interrupt(&st0, &cyl);
        
    outportb(FDC0_BASE + CONFIG_CONTROL_REG, floppy[0].geom->speed); //floppy[(base==FDC0_BASE)?0:1].geom->speed ); /* data rate */
    
    floppy_specify(0);
    
    return floppy_calibrate(0);
}

void floppy_specify(unsigned flp)
{
    unsigned data_rate;
    
    data_rate = floppy_speed[floppy[flp].geom->speed];
    
    floppy_send_byte(CMD_SPECIFY);
    floppy_send_byte((data_rate / 33333) | ((data_rate / 62500) << 4));   //floppy_parameters.steprate_headunload 0xdf  HUT 0.24*bs/8000 | SRT (16-0.008*bs/500)
    floppy_send_byte((data_rate / 50000));   //floppy_parameters.headload_ndma(0x02)   0.01 * bs / 1000 <<1
    return;
}

int floppy_wait()
{
    unsigned timeout = 300;
    while((--timeout) && (!bFloppyIrq))
        Sleep(10);
    if((!bFloppyIrq))
        return -1;
        
    bFloppyIrq = 0;
    return 0;
}

void floppy_check_interrupt(unsigned *st0, unsigned *cyl)
{
    floppy_send_byte(CMD_SENSE_INTERRUPT);
    *st0 = floppy_get_byte();
    *cyl = floppy_get_byte();
}

int floppy_calibrate(unsigned flp)
{
    unsigned st0, cyl;
    unsigned i;
    
    bFloppyIrq = 0;
    floppy_motor_on(flp);
    for(i = 0; i < 5; ++i)
    {
        floppy_send_byte(CMD_RECALIBRATE);
        floppy_send_byte(flp);
        
        if(floppy_wait())
        {
            floppy_motor_off(flp);
            return -3;
        }
        floppy_check_interrupt(&st0, &cyl);
        
        if(st0 & 0xC0)   /* B³¹d */
            continue;
        if(!cyl)
        {
            floppy_motor_off(flp);
            return 0;
        }
        
    }
    
    floppy_motor_off(flp);
    
    return -1;
}

void floppy_send_byte(BYTE byte)
{
    unsigned timeout = 128;
    BYTE b;
    while(--timeout)
    {
        inportb(FDC0_BASE + MAIN_STATUS_REG, b);
        if((b & 0xc0) == 0x80)
        {
            outportb(FDC0_BASE + DATA_FIFO, byte);
            return;
        }
        inportb(0x80, b);
    }
}

BYTE floppy_get_byte()
{
    unsigned timeout = 128;
    BYTE b;
    while(--timeout)
    {
        inportb(FDC0_BASE + MAIN_STATUS_REG, b);
        if((b & 0xc0) == 0xc0)
        {
            inportb(FDC0_BASE + DATA_FIFO, b);
            return b;
        }
        inportb(0x80, b);
    }
    return 0xff;
}

void floppy_motor_on(unsigned flp)
{
    if(!(floppy[flp].uMotor))
    {
        // need to turn on
        outportb(FDC0_BASE + DIGITAL_OUTPUT_REG, ((flp) ? (DOR_DRV1 | DOR_MOT1) : (DOR_DRV0 | DOR_MOT0)) | DOR_NOTRESET | DOR_DMA);
        Sleep(500);   //floppy_parameters.motor_start_time<<3
    }
    ++floppy[flp].uMotor;
}

void floppy_motor_off(unsigned flp)
{
    if(!(floppy[flp].uMotor))   /* Motor turned off */
        return;
    --floppy[flp].uMotor;
    if(!floppy[flp].uMotor)
        outportb(FDC0_BASE + DIGITAL_OUTPUT_REG, DOR_NOTRESET | DOR_DMA);
}

int floppy_seek(unsigned flp, unsigned head, unsigned cyl)
{
    unsigned i, st0, cyl2 = 0xffffffff; // set to bogus cylinder
    
    bFloppyIrq = 0;
    floppy_motor_on(flp);
    
    for(i = 0; i < 10; i++)
    {
        // Attempt to position to given cylinder
        floppy_send_byte(CMD_SEEK);
        floppy_send_byte((head << 2) | flp);
        floppy_send_byte(cyl);
        
        if(floppy_wait())
        {
            floppy_motor_off(flp);
            return -1;
        }
        floppy_check_interrupt(&st0, &cyl2);
        
        /*if(st0 & 0xC0)
        {
            static const char * status[] =
            { 0, "error", "invalid command", "drive not ready" };
            printf("floppy_seek ERROR: status = %s\n", status[st0 >> 6]);
            continue;
        }*/
        
        if(cyl2 == cyl)
        {
            floppy_motor_off(flp);
            return 0;
        }
        //printf("cyl (%u) != %u\n", cyl2, cyl);
    }
    floppy_motor_off(flp);
    //printf("cyl2: %u\n", cyl2);
    return -1;
}

int floppy_read(unsigned flp, unsigned track, char *buf)
{
    int r;
    r = floppy_rw(flp, track, CMD_READ_DATA);
    if(!r)
        memcpy(buf, (void*)lpFlpDmaBuf, FLOPPY_DMA_LEN);
    return r;
}

int floppy_write(unsigned flp, unsigned track, char *buf)
{
    memcpy((void*)lpFlpDmaBuf, buf, FLOPPY_DMA_LEN);
    return floppy_rw(flp, track, CMD_WRITE_DATA);
}

int floppy_rw(unsigned flp, unsigned track, BYTE cmd)
{
    // transfer command, set below
    
    // Read is MT:MF:SK:0:0:1:1:0, write MT:MF:0:0:1:0:1
    // where MT = multitrack, MF = MFM mode, SK = skip deleted
    //
    // Specify multitrack and MFM mode
    
    unsigned i;
    
    floppy_motor_on(flp);
    outportb(FDC0_BASE + CONFIG_CONTROL_REG, floppy[flp].geom->speed);
    
    for(i = 0; i < 10; ++i)
    {
        enum DMA_MODE dmaReadWrite = (cmd == CMD_READ_DATA) ? DMA_MODE_READ : DMA_MODE_WRITE;
        open_dma_channel(FLOPPY_DMA_CHANNEL, lpFlpDmaBuf, FLOPPY_DMA_LEN - 1, DMA_MODE_SINGLE | dmaReadWrite);
        
        if(floppy_seek(flp, 0, track))
        {
            floppy_motor_off(flp);
            return -1;
        }
        
        if(floppy_seek(flp, 1, track))
        {
            floppy_motor_off(flp);
            return -1;
        }
        
        Sleep(100);   // give some time (100ms) to settle after the seeks
        bFloppyIrq = 0;
        
        floppy_send_byte(cmd);    // set above for current direction
        floppy_send_byte(flp);      // 0:0:0:0:0:HD:US1:US0 = head and drive (head<<2) |
        floppy_send_byte(track);    // cylinder
        floppy_send_byte(0);   // first head (should match with above)
        floppy_send_byte(1);      // first sector, strangely counts from 1
        floppy_send_byte(2);      // bytes/sector, 128*2^x (x=2 -> 512)
        floppy_send_byte(floppy[flp].geom->sectors);   // number of tracks to operate on
        floppy_send_byte(0x1b);   // GAP3 length, 27 is default for 3.5"
        floppy_send_byte(0xff);   // data length (0xff if B/S != 0)
        
        if(floppy_wait())   // don't SENSE_INTERRUPT here!
        {
            floppy_reset();
            return -1;
        }
        
        // first read status information
        unsigned st0, st1, st2;
        st0 = floppy_get_byte();
        st1 = floppy_get_byte();
        st2 = floppy_get_byte();
        /* These are cylinder/head/sector values, updated with some
        * rather bizarre logic, that I would like to understand. */
        floppy_get_byte(); // rcy
        floppy_get_byte(); // rhe
        floppy_get_byte(); // rse
        // bytes per sector, should be what we programmed in
        floppy_get_byte(); // bps
        
        if((st0 & 0xC8) || (st1 & 0xB5) || (st2 & 0x77))
        {
            floppy_calibrate(flp);
            continue;
        }
        
        if(st1 & 0x02)   /* not writable */
        {
            floppy_motor_off(flp);
            return -2;
        }
        floppy_motor_off(flp);
        return 0;
    }
    
    floppy_motor_off(flp);
    return -1;
}

int flp_dev_open(io_dev_t *dev)
{
    if(!floppy[(unsigned)dev->param].geom)
        return -1; //no device
    return 0;
}

int flp_dev_close(io_dev_t *dev)
{
    if(!floppy[(unsigned)dev->param].geom)
        return -1; //no device
    return 0;
}

int flp_dev_read(io_dev_t *dev, BYTE *buf, UINT64 offset, UINT64 count)
{
    int r;
    unsigned track, track_size, t;
    
    if(!floppy[(unsigned)dev->param].geom)
        return -1; //no device
        
    track_size = 512 * floppy[(unsigned)dev->param].geom->sectors * floppy[(unsigned)dev->param].geom->heads;
    track = ((unsigned)offset) / track_size;
    
    if(uFlpInBuf != ((unsigned)dev->param) || track != uTrackInBuf)
    {
        //open_dma_channel( FLOPPY_DMA_CHANNEL, FLOPPY_DMA_ADDR, FLOPPY_DMA_LEN-1, DMA_MODE_SINGLE|DMA_MODE_READ );
        r = floppy_rw((unsigned)dev->param, track, CMD_READ_DATA);
        if(r)
            return r;
    }
    t = min(((unsigned)count), track_size - (((unsigned)offset) % track_size));
    memcpy(buf, ((BYTE*)lpFlpDmaBuf) + ((unsigned)offset) % track_size, t);
    count -= t;
    for(++track; ((unsigned)count); ++track)
    {
        //open_dma_channel( FLOPPY_DMA_CHANNEL, FLOPPY_DMA_ADDR, FLOPPY_DMA_LEN-1, DMA_MODE_SINGLE|DMA_MODE_READ );
        r = floppy_rw((unsigned)dev->param, track, CMD_READ_DATA);
        if(r)
            return r;
        t = min(((unsigned)count), track_size - (((unsigned)offset) % track_size));
        memcpy(buf, ((BYTE*)lpFlpDmaBuf) + ((unsigned)offset) % track_size, t);
        *((unsigned*)&count) -= t;
    }
    uTrackInBuf = track - 1;
    uFlpInBuf = (unsigned)dev->param;
    
    return 0;
}

int flp_dev_write(io_dev_t *dev, const BYTE *buf, UINT64 offset, UINT64 count)
{
    int r;
    unsigned track, track_size, t;
    
    if(!floppy[(unsigned)dev->param].geom)
        return -1; //no device
        
    track_size = 512 * floppy[(unsigned)dev->param].geom->sectors * floppy[(unsigned)dev->param].geom->heads;
    for(track = ((unsigned)offset) / track_size; ((unsigned)count); ++track)
    {
        //open_dma_channel( FLOPPY_DMA_CHANNEL, FLOPPY_DMA_ADDR, FLOPPY_DMA_LEN-1, DMA_MODE_SINGLE|DMA_MODE_WRITE );
        r = floppy_rw((unsigned)dev->param, track, CMD_WRITE_DATA);
        if(r)
            return r;
        t = min(((unsigned)count), track_size - (((unsigned)offset) % track_size));
        memcpy(((BYTE*)lpFlpDmaBuf) + ((unsigned)offset) % track_size, buf, t);
        *((unsigned*)&count) -= t;
    }
    
    return 0;
}
