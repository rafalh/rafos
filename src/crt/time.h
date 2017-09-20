#ifndef _TIME_H_
#define _TIME_H_

#include <stddef.h>
#include "sysdef.h"

#define CLOCKS_PER_SEC 1000

//typedef unsigned long time_t;
typedef unsigned long clock_t;

struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

char *asctime(const struct tm *timeptr);
clock_t clock(void);
char *ctime(const time_t *timer);
double difftime(time_t time1, time_t time2);
time_t time(time_t *timer);
time_t mktime(struct tm *timeptr);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t * timer);
size_t strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr);

#endif /* _TIME_H_ */
