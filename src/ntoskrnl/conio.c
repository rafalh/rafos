#include "conio.h"

extern unsigned console_pos;
extern unsigned char console_attr;

void clreol(void)
{
    unsigned i;
    for(i = console_pos; (i % 80) != 79; ++i)
    {
        ((char*)0xb8000)[i << 1] = ' ';
        ((char*)0xb8001)[i << 1] = console_attr;
    }
}

void clrscr(void)
{
    unsigned i;
    for(i = 0; i < 80 * 25; ++i)
    {
        ((char*)0xb8000)[i << 1] = ' ';
        ((char*)0xb8001)[i << 1] = console_attr;
    }
    gotoxy(0, 0);
}

void gotoxy(int x, int y)
{
    console_pos = y * 80u + x;
    __asm__ __volatile__("outb %%al,%%dx"::"a"(14), "d"(0x3D4));
    __asm__ __volatile__("outb %%al,%%dx"::"a"((unsigned char)(console_pos>>8)), "d"(0x3D5));
    __asm__ __volatile__("outb %%al,%%dx"::"a"(15), "d"(0x3D4));
    __asm__ __volatile__("outb %%al,%%dx"::"a"((unsigned char)console_pos), "d"(0x3D5));
}

void textattr(int _attr)
{
    console_attr = _attr;
}

void textbackground(int color)
{
    console_attr = (console_attr & 0x0f) | ((color & 0xf) << 4);
}

void textcolor(int color)
{
    console_attr = (console_attr & 0xf0) | (color & 0xf);
}

int wherex(void)
{
    return console_pos % 80;
}

int wherey(void)
{
    return console_pos / 80;
}
