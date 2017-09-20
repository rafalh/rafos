#include "multiboot.h"
#include "../version.h"
#include "device.h"
#include "vfs.h"
#include "fat.h"
#include "time.h"
#include "intr.h"
#include "conio.h"
#include "stdio.h"
#include "segments.h"
#include "memory.h"
#include "keyb.h"
#include "floppy.h"
#include "asm.h"
#include "time.h"

void kmain(struct multiboot_info *info, multiboot_uint32_t magic);

int puts2(const char *s)
{
    for(; *s; ++s)
        putchar(*s);
        
    putchar('\n');
    return 0;
}

void kmain(struct multiboot_info *info, multiboot_uint32_t magic)
{
    unsigned i, j;
    int t;
    volume_t vol;
    
    _cli();
    clrscr();
    textcolor(14);
    puts(SYS_NAME " " SYS_VER_STR);
    textcolor(7);
    
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Boot failed! Invalid magic value: %x\n", magic);
        return;
    }
    
    InitSegments();
    InitInterrupts();
    
    init_memory();
    timer_init();
    keyb_init();
    floppy_init();
    
    vol.start_sector = 0;
    LoadFS(&fat_fs);
    
    for(i = 0; i < cIoDevices; ++i)
    {
        vol.dev = &lpIoDevices[i];
        for(j = 0; j < cFilesys; ++j)
        {
            vol.fs = &lpFilesys[j];
            if(!lpFilesys[j].init(&vol))
            {
                t = MountVolume(0, &vol);
                printf("Device %u filesystem detected - ", i);
                if(t < 0) printf("mounting failed!\n");
                else printf("mounted as %c.\n", t + 'A');
                break;
            }
        }
    }
    
    time_t tNow = time(NULL);
    printf("Today is %s.\n", ctime(&tNow));
    
    puts("Ready!");
    ProcessCmdLine();
}
