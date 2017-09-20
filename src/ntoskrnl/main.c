#include "../version.h"
#include "main.h"
#include "stdio.h"
#include "io.h"
#include "memory.h"
#include "vfs.h"
#include "conio.h"
#include "keyb.h"
#include "asm.h"
#include "multiboot.h"
#include "segments.h"
#include "intr.h"
#include "floppy.h"
#include "fat.h"
#include "shell.h"

void reboot()
{
    BYTE b;
    
    _cli(); /* disable all interrupts */
    /* Clear all keyboard buffers (output and command buffers) */
    do
    {
        inportb(0x64, b); /* empty user data */
        if(b & 1)
            __asm__ __volatile__("inb $0x60, %al");
    }
    while(b & 2);
    
    outportb(0x64, 0xFE); /* pulse CPU reset line */
    _hlt(); /* if that didn't work, halt the CPU */
}

void mem_dump(const void *ptr, unsigned len)
{
    unsigned i;
    printf("MEM_DUMP 0x%X - 0x%X\n-------------------------------", ptr, ((unsigned)ptr) + len);
    for(i = 0u; i < len; ++i)
    {
        if(!(i & 15))
            printf("\n");
        printf("%02X ", ((BYTE*)ptr)[i]);
    }
    printf("\n\n");
}

void krnl_panic(const char *msg)
{
    _cli();
    puts(msg);
    _hlt();
    for(;;);
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
    shell_run();
}
