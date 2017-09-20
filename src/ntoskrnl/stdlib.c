#include "stdlib.h"
#include "ctype.h"
#include "main.h"
#include "string.h"

/*int abs(int n)
{
    return (n<0)?(-n):(n);
}*/

long int labs(long int n)
{
    return (n < 0l) ? (-n) : (n);
}

char *itoa(int value, char *str, int base)
{
    unsigned i = 0u, j = 0u, t;
    if(base > 36 || base < 2)
        return str;
    if(base == 10 && value < 0)
    {
        str[0] = '-';
        i = 1;
        j = 1;
        value = -value;
    }
    do
    {
        t = value % base;
        if(t < 10) str[i] = (char)t + '0';
        else str[i] = (char)t + 'a' - 10;
        ++i;
        value /= base;
    }
    while(value);
    str[i] = '\0';
    for(--i; i > j; --i, ++j)
    {
        value = str[i];
        str[i] = str[j];
        str[j] = value;
    }
    return str;
}

errno_t _itoa_s(int value, char *buffer, size_t sizeInCharacters, int radix)
{
    unsigned i = 0u, j = 0u, t;
    if(radix > 36 || radix < 2 || sizeInCharacters < 2)
        return 1;
    if(radix == 10 && value < 0)
    {
        buffer[0] = '-';
        i = 1;
        j = 1;
        value = -value;
    }
    do
    {
        if(i >= sizeInCharacters - 1)
            break;
        t = value % radix;
        if(t < 10) buffer[i] = (char)t + '0';
        else buffer[i] = (char)t + 'a' - 10;
        ++i;
        value /= radix;
    }
    while(value);
    buffer[i] = '\0';
    for(--i; i > j; --i, ++j)
    {
        value = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = value;
    }
    return 0;
}

int atoi(const char * str)
{
    int r = 0;
    unsigned i = 0u;
    if(str[0] == '-')
        i = 1u;
    for(; str[i] >= '0' && str[i] <= '9'; ++i)
    {
        r = r * 10 + (int)(str[i] - '0');
    }
    if(str[i] != '\0')
        return 0;
    if(str[0] == '-')
        return -r;
    return r;
}

long int atol(const char * str)
{
    long int r = 0;
    unsigned i = 0u;
    if(str[0] == '-')
        i = 1u;
    for(; str[i] >= '0' && str[i] <= '9'; ++i)
    {
        r = r * 10l + (long int)(str[i] - '0');
    }
    if(str[i] != '\0')
        return 0l;
    if(str[0] == '-')
        return -r;
    return r;
}

long int strtol(const char *str, const char **endptr, int base)
{
    unsigned sig = 0;
    for(; isspace(*str); ++str);
    if(str[0] == '-')
        ++sig;
    else if(str[0] == '+')
        ++str;
    return sig ? (-strtoul(str, endptr, base)) : strtoul(str, endptr, base);
}

unsigned long int strtoul(const char *str, const char **endptr, int base)
{
    long int r = 0;
    unsigned i = 0;
    char c_max_dig = min(9, base - 1) + '0';
    char c_max_letter1 = (base > 10) ? 'a' + base - 11 : '\0';
    char c_max_letter2 = (base > 10) ? 'A' + base - 11 : '\0';
    if(base && (base < 2 || base > 36))
        return 0;
    for(; isspace(*str); ++str);
    if((!base || base == 16) && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        base = 16;
        str += 2;
    }
    if(!base && *str == '0')
    {
        base = 8;
        ++str;
    }
    
    for(; str[i]; ++i)
    {
        if(str[i] >= '0' && str[i] <= c_max_dig)
            r = r * base + (long int)(str[i] - '0');
        else if(str[i] >= 'a' && str[i] <= c_max_letter1)
            r = r * base + (long int)(str[i] - 'a' + 10);
        else if(str[i] >= 'A' && str[i] <= c_max_letter2)
            r = r * base + (long int)(str[i] - 'A' + 10);
        else break;
    }
    if(endptr)
        *endptr = (char*)str + i;
    if(str[0] == '-')
        return -r;
    return r;
}

void *bsearch(const void *key, const void *base, size_t num, size_t size, int (*comparator)(const void *, const void *))
{
    unsigned i;
    int t;
    
    while(num)   /* dla num = 1 tez dziala */
    {
        i = num / 2; /* indeks elementu srodkowego */
        t = memcmp(((BYTE*)base) + i, key, size); /* porownanie */
        if(!t)
            return ((BYTE*)base) + i;
        if(t > 0) /* else if( base[i]>key ) */
            num -= (num + 1) / 2;
        else     /* else if( base[i]<key ) */
        {
            base = ((BYTE*)base) + i + 1;
            num -= i + 1;
        }
    }
    return NULL;
}
