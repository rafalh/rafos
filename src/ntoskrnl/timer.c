#include "main.h"
#include "timer.h"
#include "irq.h"
#include "intr.h"
#include "io.h"
#include "time.h"
#include "stdio.h"

#define BCDtoBINb(n) (((n)>>4)*10+((n)&0xf))

void irq0();
extern BYTE rtc_second, rtc_minute, rtc_hour, rtc_day, rtc_month, rtc_year;
void GetRTCTime();
time_t sys_start_time = 0u;

void timer_init()
{
	struct tm time;

	/* Obliczanie czasu startu systemu */
	GetRTCTime( );
	time.tm_sec = BCDtoBINb( rtc_second );
	time.tm_min = BCDtoBINb( rtc_minute );
	time.tm_hour = BCDtoBINb( rtc_hour );
	time.tm_mday = BCDtoBINb( rtc_day );
	time.tm_mon = BCDtoBINb( rtc_month )-1;
	time.tm_year = (BCDtoBINb( rtc_year )%100)+100;

	sys_start_time = mktime(&time);

	/* Przerwanie zegraowe */
	set_intr_gate( 0x20, irq0 );
	set_pit_freq( 100 );
	enable_irq( 0 );
}

void timer_irq()
{
	//printf("lol\n");
	/*static unsigned t = 0;
	if((GetTickCount()-t)>1000)
	{
		t += 1000;
		GetRTCTime();
		printf("RTC: %u.%u.%u %u:%u:%u\n", BCDtoBINb(rtc_day), BCDtoBINb(rtc_month), BCDtoBINb(rtc_year), BCDtoBINb(rtc_hour), BCDtoBINb(rtc_minute), BCDtoBINb(rtc_second));
	}*/
}
