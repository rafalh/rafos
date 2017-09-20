#ifndef _CONIO_H_
#define _CONIO_H_

void clreol(void);
void clrscr(void);
void gotoxy(int x, int y);
void textattr(int _attr);
void textbackground(int color);
void textcolor(int color);
int wherex(void);
int wherey(void);

#endif /* _CONIO_H_ */
