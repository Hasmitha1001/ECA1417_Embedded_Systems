#include <reg51.h>

void serial_ISR(void) interrupt 4
{
    char ch;

    if (RI == 1)
    {
        ch = SBUF;
        RI = 0;

        SBUF = ch;

        while (TI == 0);
        TI = 0;
    }
}

void main()
{
    TMOD = 0x20;
    TH1 = 0xFD;
    SCON = 0x50;
    TR1 = 1;

    ES = 1;
    EA = 1;

    while(1);
}