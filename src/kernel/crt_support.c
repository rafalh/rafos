#include "main.h"
#include "conio.h"
#include "keyb.h"
#include "string.h"
#include "crt_support.h"

unsigned console_pos = 0x0;
unsigned char console_attr = 0x7;

void _scroll_console()
{
    memcpy((void*)0xb8000, (void*)(0xb8000 + 160), 24 * 160);
}

int putchar(int c)
{
    switch(c)
    {
    case '\n':
        console_pos = console_pos + 80 - (console_pos % 80);
        break;
    case '\r':
        console_pos -= console_pos % 80;
        break;
    case '\t':
        do
        {
            putchar(' ');
        }
        while(console_pos % 4);
        break;
    case '\b':
        if(console_pos)
        {
            --console_pos;
            *(char*)(0xb8000 + (console_pos << 1)) = ' ';
        }
        break;
    default:
        *(char*)(0xb8000 + (console_pos << 1)) = c;
        *(char*)(0xb8001 + (console_pos << 1)) = console_attr; /* kolor szary */
        ++console_pos;
    }
    
    if(console_pos >= 80 * 25)
    {
        _scroll_console();
        console_pos = 80 * 24;
        clreol();
    }
    __asm__ __volatile__("outb %%al,%%dx"::"a"(14), "d"(0x3D4));
    __asm__ __volatile__("outb %%al,%%dx"::"a"((unsigned char)(console_pos>>8)), "d"(0x3D5));
    __asm__ __volatile__("outb %%al,%%dx"::"a"(15), "d"(0x3D4));
    __asm__ __volatile__("outb %%al,%%dx"::"a"((unsigned char)console_pos), "d"(0x3D5));
    return c;
}
