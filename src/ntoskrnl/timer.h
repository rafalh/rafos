#ifndef _TIMER_H_
#define	_TIMER_H_

void timer_init();
void timer_irq();
void set_pit_freq(unsigned long hz);

#endif /* _TIMER_H_ */
