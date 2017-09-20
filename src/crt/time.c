#include "time.h"
#include "stdio.h"
#include "crt_support.h"

struct tm tm_global;
unsigned lpDaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char *lpWeekDays[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char *lpMonths[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
const char *lpAmPm[2] = { "AM", "PM" };

char *asctime(const struct tm *timeptr)
{
    static char buf[256];
    strftime(buf, sizeof(buf), "%c", timeptr);
    return buf;
}

clock_t clock(void)
{
    return KeTickCount();
}

char *ctime(const time_t *timer)
{
    return asctime(gmtime(timer));
}

double difftime(time_t time1, time_t time2)
{
    return (double)(time1 - time2);
}

time_t time(time_t *timer)
{
    time_t t = sys_start_time + KeTickCount() / 1000;
    if(timer)
        *timer = t;
    return t;
}

time_t mktime(struct tm *timeptr)
{
    unsigned i;
    time_t t = 0;
    if(!timeptr)
        return 0;
    for(i = 1970; i < ((timeptr->tm_year) + 1900); ++i) /* Rok */
        t += ((!(i & 3) && (i % 100)) || !(i % 400)) ? (366 * 24 * 3600) : (365 * 24 * 3600);
    timeptr->tm_yday = 0;
    for(i = 0; i < (timeptr->tm_mon); ++i) /* Miesi¹c */
        timeptr->tm_yday += lpDaysInMonth[i]; /* Dzieñ roku */
    if(((!((timeptr->tm_year) & 3) && ((timeptr->tm_year) % 100)) || !((timeptr->tm_year) % 400)) && (timeptr->tm_mon) > 1)
        ++timeptr->tm_yday;
    t += timeptr->tm_yday * 24 * 3600;
    t += (timeptr->tm_mday - 1) * 24 * 3600 + timeptr->tm_hour * 3600 + timeptr->tm_min * 60 + timeptr->tm_sec; /* Reszta */
    timeptr->tm_wday = (t / (3600 * 24) + 4) % 7;
    return t;
}

struct tm *gmtime(const time_t *timer)
{
    unsigned i, tmp;
    time_t t = *timer;
    tm_global.tm_sec = (*timer) % 60;
    tm_global.tm_min = ((*timer) / 60) % 60;
    tm_global.tm_hour = ((*timer) / 3600) % 24;
    for(i = 1970; t >= (tmp = ((!(i & 3) && (i % 100)) || !(i % 400)) ? (366 * 24 * 3600) : (365 * 24 * 3600)); ++i)
        t -= tmp;
    tm_global.tm_year = i - 1900;
    tm_global.tm_yday = t / (3600 * 24);
    i = ((!(i & 3) && (i % 100)) || !(i % 400)) ? 1 : 0;
    for(tm_global.tm_mon = 0; t >= (tmp = (i && tm_global.tm_mon == 1) ? (29 * 24 * 3600) : (lpDaysInMonth[tm_global.tm_mon] * 24 * 3600)); ++tm_global.tm_mon) /* time_t jest unsigned */
        t -= tmp;
    tm_global.tm_mday = t / (3600 * 24) + 1;
    tm_global.tm_wday = ((*timer) / (3600 * 24) + 4) % 7;
    return &tm_global;
}

struct tm *localtime(const time_t * timer)
{
    return gmtime(timer);
}

size_t strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr)
{
    unsigned i, j;
    for(i = 0; i < maxsize - 1 && *format ; ++format)
    {
        if(*format == '%')
        {
            ++format;
            if(!(*format))
                break;
            switch(*format)
            {
            case 'a': /* 3 litery nazwy dnia tygodnia */
                if(timeptr->tm_wday >= 0 && timeptr->tm_wday <= 6)
                    for(j = 0u; i < maxsize - 1 && j < 3 && lpWeekDays[timeptr->tm_wday][j]; ++i, ++j)
                        ptr[i] = lpWeekDays[timeptr->tm_wday][j];
                break;
            case 'A': /* nazwa dnia tygodnia */
                if(timeptr->tm_wday >= 0 && timeptr->tm_wday <= 6)
                    for(j = 0u; i < maxsize - 1 && lpWeekDays[timeptr->tm_wday][j]; ++i, ++j)
                        ptr[i] = lpWeekDays[timeptr->tm_wday][j];
                break;
            case 'b': /* 3 litery nazwy miesi¹ca */
                if(timeptr->tm_mon >= 0 && timeptr->tm_mon <= 12)
                    for(j = 0u; i < maxsize - 1 && j < 3 && lpMonths[timeptr->tm_wday][j]; ++i, ++j)
                        ptr[i] = lpMonths[timeptr->tm_mon][j];
                break;
            case 'B': /* nazwa miesi¹ca */
                if(timeptr->tm_mon >= 0 && timeptr->tm_mon <= 11)
                    for(j = 0u; i < maxsize - 1 && lpMonths[timeptr->tm_wday][j]; ++i, ++j)
                        ptr[i] = lpMonths[timeptr->tm_mon][j];
                break;
            case 'c': /* data i godzina */
                i += strftime(ptr + i, maxsize - i, "%a %b %d %H:%M:%S %Y", timeptr);
                break;
            case 'd': /* dzieñ miesi¹ca */
                i += _snprintf(ptr + i, maxsize - i, "%02d", timeptr->tm_mday);
                break;
            case 'H': /* godzina w formacie 24-godzinnym */
                i += _snprintf(ptr + i, maxsize - i, "%02u", timeptr->tm_hour);
                break;
            case 'I': /* godzina w formacie 12-godzinnym */
                i += _snprintf(ptr + i, maxsize - i, "%02u", (timeptr->tm_hour + 11) % 12 + 1);
                break;
            case 'j': /* dzieñ roku */
                i += _snprintf(ptr + i, maxsize - i, "%02u", timeptr->tm_yday);
                break;
            case 'm': /* miesi¹c */
                i += _snprintf(ptr + i, maxsize - i, "%02u", timeptr->tm_mon + 1);
                break;
            case 'M': /* minuty */
                i += _snprintf(ptr + i, maxsize - i, "%02u", timeptr->tm_min);
                break;
            case 'p':
                if(timeptr->tm_hour >= 0 && timeptr->tm_hour <= 23)
                    for(j = 0u; i < maxsize - 1 && lpAmPm[timeptr->tm_hour / 12][j]; ++i, ++j)
                        ptr[i] = lpAmPm[timeptr->tm_hour / 12][j];
                break;
            case 'S': /* sekundy */
                i += _snprintf(ptr + i, maxsize - i, "%02u", timeptr->tm_sec);
                break;
            case 'U': /* numer tygodnia (pierwsza niedziela jako pierwszy dzieñ 1. tygodnia */
                i += _snprintf(ptr + i, maxsize - i, "%02u", (timeptr->tm_yday - timeptr->tm_wday) / 7);
                break;
            case 'w': /* dzieñ tygodnia */
                ptr[i++] = timeptr->tm_wday + '0';
                break;
            case 'W': /* numer tygodnia (pierwszy poniedzia³ek jako pierwszy dzieñ 1. tygodnia */
                i += _snprintf(ptr + i, maxsize - i, "%02u", (timeptr->tm_yday - timeptr->tm_wday - 1) / 7);
                break;
            case 'x': /* data */
                i += strftime(ptr + i, maxsize - i, "%d/%m/%y", timeptr);
                break;
            case 'X': /* czas */
                i += strftime(ptr + i, maxsize - i, "%H:%M:%S", timeptr);
                break;
            case 'y': /* 2 cyfry roku */
                i += _snprintf(ptr + i, maxsize - i, "%02u", timeptr->tm_year % 100);
                break;
            case 'Y': /* rok */
                i += _snprintf(ptr + i, maxsize - i, "%u", timeptr->tm_year + 1900);
                break;
            case 'Z': /* ? */
                break;
            case '%':
                ptr[i++] = '%';
                break;
            }
        }
        else ptr[i++] = *format;
    }
    ptr[i] = '\0';
    return i;
}

void Sleep(unsigned dwMilliseconds)
{
    unsigned t;
    for(t = KeTickCount(); (KeTickCount() - t) < dwMilliseconds;);
}
