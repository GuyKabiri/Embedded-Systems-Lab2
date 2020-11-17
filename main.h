#ifndef __MAIN_H__
#define __MAIN_H__

#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF
#pragma config FNOSC =	FRCPLL
#pragma config FSOSCEN =	OFF
#pragma config POSCMOD =	EC
#pragma config OSCIOFNC =	ON
#pragma config FPBDIV =     DIV_1
#pragma config FPLLIDIV =	DIV_2
#pragma config FPLLMUL =	MUL_20
#pragma config FPLLODIV =	DIV_1

void main();
void busy(void);
void screenWrite(char* str, int upOrDown);
void turnOnScreen(int mode);
void init();
void reset_LEDs();
void reset_LCD();
void counter();
void delay();
void shift();
void fan();
void make_sound();


#endif  // __MAIN_H__