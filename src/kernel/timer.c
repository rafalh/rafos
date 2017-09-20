#include "main.h"
#include "timer.h"
#include "irq.h"
#include "intr.h"
#include "io.h"
#include "time.h"
#include "stdio.h"
#include "asm.h"
#include "debug.h"
#include "cmos.h"

#define BCDtoBINb(n) (((n)>>4)*10+((n)&0xf))

#ifdef __cplusplus
extern "C" {
#endif

void irq0();

#ifdef __cplusplus
}
#endif

time_t sys_start_time = 0u;

unsigned long g_PitCurrentFreq; // Actual frequency of PIT
unsigned long long g_PitTickLength; // ms between IRQs in 32.32 fixed point
unsigned long long g_SysTicks = 0; // ms from system start in 32.32 fixed point

void GetRTCTime(struct tm *ptm)
{
    while(get_cmos_byte(10) & 0x80); // Is update in progress?
    
    BYTE rtc_second = get_cmos_byte(0); // Get seconds (00 to 59)
    BYTE rtc_minute = get_cmos_byte(2); // Get minutes (00 to 59)
    BYTE rtc_hour = get_cmos_byte(4); // Get hours (see notes)
    BYTE rtc_day = get_cmos_byte(7); // Get day of month (01 to 31)
    BYTE rtc_month = get_cmos_byte(0x08); // Get month (01 to 12)
    BYTE rtc_year = get_cmos_byte(0x09); // Get year (00 to 99)
    
    ptm->tm_sec = BCDtoBINb(rtc_second);
    ptm->tm_min = BCDtoBINb(rtc_minute);
    ptm->tm_hour = BCDtoBINb(rtc_hour);
    ptm->tm_mday = BCDtoBINb(rtc_day);
    ptm->tm_mon = BCDtoBINb(rtc_month) - 1;
    ptm->tm_year = (BCDtoBINb(rtc_year) % 100) + 100;
}

unsigned PitReloadValFromFreq(unsigned long hz)
{
    unsigned reloadVal, tmp;
    
    // Do some checking
    if(hz <= 18) //Is the requested frequency too low?
    {
        // yes, use slowest possible frequency
        reloadVal = 0x10000; // reload value for slowest possible frequency (65536)
    }
    else if(hz >= 1193181) // is the requested frequency too high?
    {
        // yes, use fastest possible frequency
        reloadVal = 0x10000; // reload value for fastest possible frequency (1)
    }
    else
    {
        // Calculate the reload value
        reloadVal = 3579545 / hz;
        if(3579545 % hz > 3579545 / 2) //Is the remainder more than half?
            ++reloadVal; // yes, round up
            
        tmp = reloadVal % 3;
        reloadVal /= 3;
        if(tmp > 1)
            ++reloadVal;
    }
    
    return reloadVal;
}

unsigned PitFreqFromReloadVal(unsigned reloadVal)
{
    unsigned hz, tmp;
    
    // Calculate the actual frequency
    hz = 3579545 / reloadVal;
    if(3579545 % reloadVal > 3579545 / 2) // Is the remainder more than half?
        ++hz; // yes, round up
        
    tmp = hz % 3;
    hz /= 3;
    if(tmp > 1) //Is the remainder more than half?
        ++hz;
        
    return hz;
}

void set_pit_freq(unsigned long hz)
{
    unsigned reloadVal;
    
    TRACE("set_pit_freq\n");
    
    reloadVal = PitReloadValFromFreq(hz);
    g_PitCurrentFreq = PitFreqFromReloadVal(reloadVal); // Store the actual frequency for displaying later
    
    // Calculate the amount of time between IRQs in 32.32 fixed point
    //
    // Note: The basic formula is:
    //           time in ms = reload_value / (3579545 / 3) * 1000
    
    g_PitTickLength = reloadVal * 0xDBB3A062ull; // 3000 * (2^42) / 3579545
    ASSERT(sizeof(g_PitTickLength) == 8);
    g_PitTickLength >>= 10; // ms between IRQs in 32.32 fixed point
    
    // Program the PIT channel
    _cli(); //Disabled interrupts (just in case)
    _out(0x43, 0x34); //channel 0, lobyte/hibyte, rate generator
    _out(0x40, reloadVal & 0xFF);  //Set low byte of PIT reload value
    _out(0x40, reloadVal >> 8);  //Set high byte of PIT reload value
    _sti();
}

void timer_init()
{
    struct tm curTime;
    
    TRACE("timer_init\n");
    
    /* Obliczanie czasu startu systemu */
    GetRTCTime(&curTime);
    sys_start_time = mktime(&curTime);
    
    /* Przerwanie zegraowe */
    set_intr_gate(0x20, irq0);
    set_pit_freq(100);
    
    ASSERT(PitReloadValFromFreq(100) == 0x2E9C);
    ASSERT(g_PitCurrentFreq == 0x64);
    ASSERT(g_PitTickLength == 0xA000A11D5ull);
    
    enable_irq(0);
}

unsigned KeTickCount()
{
    return g_SysTicks >> 32; // return ms from system start
}

void timer_irq()
{
    g_SysTicks += g_PitTickLength;
    
    /*static unsigned t = 0;
    if((GetTickCount()-t)>1000)
    {
        t += 1000;
        GetRTCTime();
        printf("RTC: %u.%u.%u %u:%u:%u\n", BCDtoBINb(rtc_day), BCDtoBINb(rtc_month), BCDtoBINb(rtc_year), BCDtoBINb(rtc_hour), BCDtoBINb(rtc_minute), BCDtoBINb(rtc_second));
    }*/
}


