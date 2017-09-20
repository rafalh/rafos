#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "sysdef.h"
#include "crt_support.h"

int getchar()
{
    char c;
    for(; !(c = input_get()););
    if(c != '\b')
        putchar(c);
    return c;
}

char *gets(char *str)
{
    unsigned i = 0;
    for(;;)
    {
        for(; !(str[i] = input_get()););
        if(str[i] == '\n')
            break;
        if(str[i] == '\b')
        {
            if(i)
            {
                --i;
                putchar('\b');
            }
        }
        /*else if(str[i] == '\t')
        {
            do
            {
                putchar('\t');
                ++i;
            }
            while(console_pos % 4);
        }*/
        else
        {
            putchar(str[i]);
            ++i;
        }
    }
    putchar('\n');
    str[i] = '\0';
    return str;
}

int puts(const char *s)
{
    for(; *s; ++s)
        putchar(*s);
        
    putchar('\n');
    return 0;
}

int printf(const char *format, ...)
{
    int r;
    va_list pArgList;
    va_start(pArgList, format);
    r = vprintf(format, pArgList);
    va_end(pArgList);
    return r;
}

int sprintf(char *str, const char *format, ...)
{
    int r;
    va_list pArgList;
    va_start(pArgList, format);
    r = vsprintf(str, format, pArgList);
    va_end(pArgList);
    return r;
}

int vprintf(const char *format, va_list arg)
{
    unsigned j;
    for(; *format; ++format)
    {
        if(*format == '%')
        {
            unsigned n;
            char pad = ' ';
            unsigned width, precision = 1, lng = 0;
            ++format;
            if(*format == '0')
            {
                ++format;
                pad = '0';
            }
            if(*format == '*')
            {
                width = va_arg(arg, unsigned);
                ++format;
            }
            else width = strtoul(format, &format, 10);
            if(*format == '.')
                precision = strtoul(format + 1, &format, 10);
                
lswitch:
            switch(*format)
            {
            case 'l':
            case 'L':
                ++lng;
                ++format;
                goto lswitch;
            case 'h':
                goto lswitch;
            case 'd':
            {
                int n2 = va_arg(arg, int);
                if(lng > 1) /* long long */
                {
                    printf("%.0d", va_arg(arg, int));
                    n = abs(n2);
                }
                else if(n2 < 0)
                {
                    putchar('-');
                    n = -n2;
                }
                else n = n2;
            }
            case 'u':
            {
                char buf[12];
                int i = 0;
                if(*format == 'u')
                {
                    n = va_arg(arg, unsigned);
                    if(lng > 1) /* long long */
                        printf("%.0u", va_arg(arg, unsigned));
                }
                while(n)
                {
                    buf[i] = (char)(n % 10) + '0';
                    ++i;
                    n /= 10;
                }
                if(i < precision)
                    for(j = 0; j < (precision - i); ++j)
                        putchar('0');
                if(max(precision, i) < width)
                    for(j = 0; j < (width - max(precision, i)); ++j)
                        putchar(pad);
                for(--i; i >= 0; --i)
                    putchar(buf[i]);
                break;
            }
            case 'x':
            case 'X':
            case 'p':
            {
                char buf[12];
                int i = 0;
                n = va_arg(arg, unsigned);
                while(n)
                {
                    buf[i] = (char)(n % 16);
                    if(buf[i] < 10) buf[i] += '0';
                    else buf[i] += (*format == 'X' ? 'A' : 'a') - 10;
                    ++i;
                    n /= 16;
                }
                if(i < precision)
                    for(j = 0; j < (precision - i); ++j)
                        putchar('0');
                if(max(precision, i) < width)
                    for(j = 0; j < (width - max(precision, i)); ++j)
                        putchar(pad);
                for(--i; i >= 0; --i)
                    putchar(buf[i]);
                break;
            }
            case 's':
            {
                char *s;
                for(s = va_arg(arg, char*); *s; ++s)
                    putchar(*s);
                break;
            }
            case 'c':
                putchar((char)va_arg(arg, int));
                break;
            case '%':
                putchar('%');
                break;
            }
        }
        else putchar(*format);
    }
    return 0; /**************************/
}

int vsprintf(char *str, const char *format, va_list arg)
{
    unsigned c = 0, j;
    for(; *format; ++format)
    {
        if(*format == '%')
        {
            unsigned n;
            char pad = ' ';
            unsigned width, precision = 1;
            ++format;
            if(*format == '0')
            {
                ++format;
                pad = '0';
            }
            if(*format == '*')
            {
                width = va_arg(arg, unsigned);
                ++format;
            }
            else width = strtoul(format, &format, 10);
            if(*format == '.')
                precision = strtoul(format + 1, &format, 10);
                
            switch(*format)
            {
            case 'd':
            {
                int n2 = va_arg(arg, int);
                if(n2 < 0)
                {
                    str[c++] = '-';
                    n = -n2;
                }
                else n = n2;
            }
            case 'u':
            {
                char buf[12];
                int i = 0;
                if(*format == 'u')
                    n = va_arg(arg, unsigned);
                while(n)
                {
                    buf[i] = (char)(n % 10) + '0';
                    ++i;
                    n /= 10;
                }
                if(i < precision)
                    for(j = 0; j < (precision - i); ++j)
                        str[c++] = '0';
                if(max(precision, i) < width)
                    for(j = 0; j < (width - max(precision, i)); ++j)
                        str[c++] = pad;
                for(--i; i >= 0; --i)
                    str[c++] = buf[i];
                break;
            }
            case 'x':
            case 'X':
            case 'p':
            {
                char buf[12];
                unsigned n = va_arg(arg, unsigned);
                int i = 0;
                while(n)
                {
                    buf[i] = (char)(n % 16);
                    if(buf[i] < 10) buf[i] += '0';
                    else buf[i] += (*format == 'X' ? 'A' : 'a') - 10;
                    ++i;
                    n /= 16;
                }
                if(i < precision)
                    for(j = 0; j < (precision - i); ++j)
                        str[c++] = '0';
                if(max(precision, i) < width)
                    for(j = 0; j < (width - max(precision, i)); ++j)
                        str[c++] = pad;
                for(--i; i >= 0; --i)
                    str[c++] = buf[i];
                for(--i; i >= 0; --i)
                    str[c++] = buf[i];
                break;
            }
            case 's':
            {
                char *s;
                for(s = va_arg(arg, char*); *s; ++s)
                    str[c++] = *s;
                break;
            }
            case 'c':
                str[c++] = (char)va_arg(arg, int);
                break;
            case '%':
                str[c++] = '%';
                break;
            }
        }
        else str[c++] = *format;
    }
    str[c] = '\0';
    return c;
}

int _snprintf(char *buffer, size_t count, const char *format, ...)
{
    int r;
    va_list pArgList;
    va_start(pArgList, format);
    r = _vsnprintf(buffer, count, format, pArgList);
    va_end(pArgList);
    return r;
}

int _vsnprintf(char *buffer, size_t count, const char *format, va_list arg)
{
    unsigned c = 0, j;
    for(; *format && c < count - 1; ++format)
    {
        if(*format == '%')
        {
            unsigned n;
            char pad = ' ';
            unsigned width, precision = 1;
            ++format;
            if(*format == '0')
            {
                ++format;
                pad = '0';
            }
            if(*format == '*')
            {
                width = va_arg(arg, unsigned);
                ++format;
            }
            else width = strtoul(format, &format, 10);
            if(*format == '.')
                precision = strtoul(format + 1, &format, 10);
                
            switch(*format)
            {
            case 'd':
            {
                int n2 = va_arg(arg, int);
                if(n2 < 0)
                {
                    buffer[c++] = '-';
                    n = -n2;
                }
                else n = n2;
            }
            case 'u':
            {
                char buf[12];
                int i = 0;
                if(*format == 'u')
                    n = va_arg(arg, unsigned);
                while(n)
                {
                    buf[i] = (char)(n % 10) + '0';
                    ++i;
                    n /= 10;
                }
                if(i < precision)
                    for(j = 0; j < (precision - i) && c < count - 1; ++j)
                        buffer[c++] = '0';
                if(max(precision, i) < width)
                    for(j = 0; j < (width - max(precision, i)) && c < count - 1; ++j)
                        buffer[c++] = pad;
                for(--i; i >= 0 && c < count - 1; --i)
                    buffer[c++] = buf[i];
                break;
            }
            case 'x':
            case 'X':
            case 'p':
            {
                char buf[12];
                unsigned n = va_arg(arg, unsigned);
                int i = 0;
                while(n)
                {
                    buf[i] = (char)(n % 16);
                    if(buf[i] < 10) buf[i] += '0';
                    else buf[i] += (*format == 'X' ? 'A' : 'a') - 10;
                    ++i;
                    n /= 16;
                }
                if(i < precision)
                    for(j = 0; j < (precision - i) && c < count - 1; ++j)
                        buffer[c++] = '0';
                if(max(precision, i) < width)
                    for(j = 0; j < (width - max(precision, i)) && c < count - 1; ++j)
                        buffer[c++] = pad;
                for(--i; i >= 0 && c < count - 1; --i)
                    buffer[c++] = buf[i];
                break;
            }
            case 's':
            {
                char *s;
                for(s = va_arg(arg, char*); *s && c < count - 1; ++s)
                    buffer[c++] = *s;
                break;
            }
            case 'c':
                buffer[c++] = (char)va_arg(arg, int);
                break;
            case '%':
                buffer[c++] = '%';
                break;
            }
        }
        else buffer[c++] = *format;
    }
    buffer[c] = '\0';
    return c;
}
