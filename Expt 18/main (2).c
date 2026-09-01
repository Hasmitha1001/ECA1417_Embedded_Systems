#include <reg51.h>

sbit PWM = P1^0;

unsigned char count = 0;

void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFF;
    TL0 = 0xA4;

    count++;

    if(count >= 10)
        count = 0;

    if(count < 5)
        PWM = 1;
    else
        PWM = 0;
}

void main()
{
    TMOD = 0x01;

    TH0 = 0xFF;
    TL0 = 0xA4;

    ET0 = 1;
    EA = 1;
    TR0 = 1;

    while(1);
}