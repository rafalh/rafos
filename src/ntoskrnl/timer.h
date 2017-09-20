#ifndef _TIMER_H_
#define _TIMER_H_

void timer_init();
void set_pit_freq(unsigned long hz);
unsigned KeTickCount(void);
void Sleep(unsigned dwMilliseconds);

#ifdef __cplusplus
extern "C" {
#endif

void timer_irq();

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H_ */
