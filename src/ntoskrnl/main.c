#include "../version.h"
#include "main.h"
#include "exceptions.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"
#include "conio.h"
#include "ctype.h"
#include "io.h"
#include "memory.h"
#include "ctype.h"
#include "vfs.h"
#include "conio.h"
#include "keyb.h"
#include "asm.h"

char cmdbuf[256];
char buf[256];
char lpCurrDir[MAX_PATH] = "A:\\";

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

BOOL is_cmd(const char *buf, const char *cmd)
{
    for(; *cmd && *cmd != ' ' && tolower(*cmd) == tolower(*buf); ++buf, ++cmd);
    if(*cmd == ' ' || *cmd == '\0')
        return TRUE;
    return FALSE;
}

void fix_path(char *path)
{
    int i = 0, j = 0;
    for(; path[i] == '\\' || path[i] == '/'; ++i);
    while(path[i])
    {
        if(path[i] == '.')
        {
            if(path[i + 1] == '\0' || path[i + 1] == '\\' || path[i + 1] == '/') /* /./ */
                ++i;
            else if(path[i + 1] == '.' && (path[i + 2] == '\0' || path[i + 2] == '\\' || path[i + 2] == '/')) /* /../ */
            {
                i += 2;
                if(j)
                    for(--j; j > 0 && path[j] != '\\'; --j);
                else
                {
                    path[j++] = '.';
                    path[j++] = '.';
                }
            }
            for(; path[i] == '\\' || path[i] == '/'; ++i); /* Usuniecie powtarzajacych sie slashow */
        }
        else     /* Kopiowanie */
        {
            if(j)
                path[j++] = '\\';
            for(; path[i] && path[i] != '\\' && path[i] != '/'; ++i, ++j)
                path[j] = path[i];
            for(; path[i] == '\\' || path[i] == '/'; ++i); /* Usuniecie powtarzajacych sie slashow */
        }
    }
    path[j] = '\0';
}

void ProcessCmdLine(void)
{
    unsigned i, j;
    int t;
    dir_t *current_dir = FS_ROOT_DIR;
    
    for(;;)
    {
        printf("\n%s>", lpCurrDir);
        gets(cmdbuf);
        if(!cmdbuf[0])   // Brak komendy
            continue;
        if(cmdbuf[1] == ':' && cmdbuf[2] == '\0')
        {
            if(!lpVolumes[toupper(cmdbuf[0]) - 'A'].dev)
                puts("Cannot find volume!");
            else
            {
                lpCurrDir[0] = toupper(cmdbuf[0]);
                strcpy(lpCurrDir + 1, ":\\");
                current_dir = FS_ROOT_DIR;
            }
        }
        else if(is_cmd(cmdbuf, "cd "))
        {
            if(cmdbuf[2] && cmdbuf[3] && lpVolumes[lpCurrDir[0] - 'A'].fs)
            {
                dir_t *dir;
                unsigned i;
                
                fix_path(cmdbuf + 3);
                if(!lpVolumes[lpCurrDir[0] - 'A'].fs->opendir(&lpVolumes[lpCurrDir[0] - 'A'], current_dir, cmdbuf + 3, &dir))
                {
                    lpVolumes[lpCurrDir[0] - 'A'].fs->closedir(&lpVolumes[lpCurrDir[0] - 'A'], current_dir);
                    current_dir = dir;
                    if(!strcmp(cmdbuf + 3, ".."))
                    {
                        unsigned j = 0xffffffff;
                        for(i = 0; lpCurrDir[i]; ++i)
                            if(lpCurrDir[i] == '\\')
                                j = i;
                        if(j != 0xffffffff)
                            lpCurrDir[(j != 2) ? j : 3] = '\0';
                    }
                    else
                    {
                        unsigned len = strlen(lpCurrDir);
                        if(lpCurrDir[len - 1] != '\\' && cmdbuf[3])
                        {
                            lpCurrDir[len] = '\\';
                            ++len;
                        }
                        strcpy(lpCurrDir + len, cmdbuf + 3);
                    }
                }
                else printf("Cannot open directory!\n");
            }
            else printf("%s\n", lpCurrDir);
        }
        else if(is_cmd(cmdbuf, "cls"))
            clrscr();
        else if(is_cmd(cmdbuf, "date"))
        {
            time_t t = time(NULL);
            struct tm *ptm = gmtime(&t);
            printf("Date: %u.%02u.%02u\n", ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900);
        }
        else if(is_cmd(cmdbuf, "devlst"))
        {
            unsigned i;
            for(i = 0; i < cIoDevices; ++i)
                printf("%s\n", lpIoDevices[i].name);
        }
        else if(is_cmd(cmdbuf, "dir") || is_cmd(cmdbuf, "lst"))
        {
            if(lpVolumes[lpCurrDir[0] - 'A'].fs)
            {
                dir_t *dir;
                fileinfo_t fi;
                char buf[64];
                struct tm *tm;
                unsigned cFiles = 0, cDirs = 0;
                unsigned size = 0;
                if(cmdbuf[3])
                    fix_path(cmdbuf + 4);
                if(!lpVolumes[lpCurrDir[0] - 'A'].fs->opendir(&lpVolumes[lpCurrDir[0] - 'A'], current_dir, (cmdbuf[3]) ? (cmdbuf + 4) : "", &dir))
                {
                    char label[24];
                    UINT32 serialnr;
                    lpVolumes[lpCurrDir[0] - 'A'].fs->getinfo(&lpVolumes[lpCurrDir[0] - 'A'], &serialnr, label, sizeof(label), NULL);
                    printf(" Volume in drive %c is %s\n", lpCurrDir[0], label);
                    printf(" Volume Serial Number is %X-%X\n\n", HIWORD(serialnr), LOWORD(serialnr));
                    printf(" Directory of %s%s\n\n", lpCurrDir, (cmdbuf[3]) ? (cmdbuf + 4) : "");
                    while(!lpVolumes[lpCurrDir[0] - 'A'].fs->readdir(&lpVolumes[lpCurrDir[0] - 'A'], dir, &fi)) // lista plików
                    {
                        if(fi.name[0] == '.' && (fi.name[1] == '\0') || (fi.name[1] == '.' && fi.name[2] == '\0')) // nie wyswietlam . i ..
                            continue;
                        strftime(buf, sizeof(buf), "%Y-%m-%d  %H:%M", gmtime(&(fi.modify_time)));
                        if(fi.attributes & FS_DIR)
                        {
                            printf("%s    <DIR>          %s\n", buf, fi.name);
                            ++cDirs;
                        }
                        else
                        {
                            printf("%s       %3.0u %3.0u %3.0u %s\n", buf, fi.size / 1000000, (fi.size / 1000) % 1000, fi.size % 1000, fi.name);
                            ++cFiles;
                            size += fi.size;
                        }
                    }
                    lpVolumes[lpCurrDir[0] - 'A'].fs->closedir(&lpVolumes[lpCurrDir[0] - 'A'], dir);
                    printf("              %u file(s)   %3.u %3.0u %3.0u %3.0u bytes\n              %u directories(s)\n", cFiles, (size / 1000000000), (size / 1000000) % 1000, (size / 1000) % 1000, size % 1000, cDirs);
                }
                else printf("Cannot open directory!\n");
            }
        }
        else if(is_cmd(cmdbuf, "echo"))
            printf("%s\n", cmdbuf + 5);
        else if(is_cmd(cmdbuf, "pause"))
        {
            printf("Press any key to continue . . . ");
            for(; !input_get(););
            printf("\n");
        }
        else if(is_cmd(cmdbuf, "rem"))
        {} // Do nothing
        else if(is_cmd(cmdbuf, "reboot"))
            reboot();
        else if(is_cmd(cmdbuf, "time"))
        {
            time_t t = time(NULL);
            struct tm *ptm = gmtime(&t);
            printf("Time: %u:%02u:%02u\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        }
        else if(is_cmd(cmdbuf, "vol"))
        {
            char label[24];
            UINT32 serialnr;
            if(lpVolumes[lpCurrDir[0] - 'A'].fs)
            {
                lpVolumes[lpCurrDir[0] - 'A'].fs->getinfo(&lpVolumes[lpCurrDir[0] - 'A'], &serialnr, label, sizeof(label), NULL);
                printf(" Volume in drive %c is %s\n", lpCurrDir[0], label);
                printf(" Volume Serial Number is %X-%X\n", HIWORD(serialnr), LOWORD(serialnr));
            }
            else
                puts("No FS loaded for this volume.");
                
        }
        else if(is_cmd(cmdbuf, "ver"))
        {
            printf("\n"SYS_NAME" ["SYS_VER_STR"]\n");
        }
        else if(is_cmd(cmdbuf, "help"))
        {
            printf(
                "Commands list:\n"
                "CD\t\t\tChanges current directory\n"
                "CLS\t\t\tClears screen\n"
                "DATE\t\tShows date\n"
                "DEVLST\t\tShows list of installed devices\n"
                "DIR\t\t\tShows directory\n"
                "ECHO\t\tPrints text\n"
                "HELP\t\tShows help\n"
                "PAUSE\t\tWaits for any key\n"
                "REM\t\t\tRecords comments (remarks)\n"
                "REBOOT\t\tReboots computer\n"
                "TIME\t\tShows time\n"
                "VOL\t\t\tShows volume label and serial number\n"
                "VER\t\t\tShows system version\n"
            );
        }
        else printf("Unknown command: \"%s\"!\n", cmdbuf);
    }
}

void krnl_panic(const char *msg)
{
    _cli();
    puts(msg);
    _hlt();
    for(;;);
}
