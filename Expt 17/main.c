#include <reg51.h>

void serial_ISR(void) interrupt 4
{
    if (RI == 1)
    {
        P1 = SBUF;       // Display received data on Port 1
        RI = 0;          // Clear receive flag
    }

    if (TI == 1)
    {
        TI = 0;          // Clear transmit flag
    }
}

void main()
{
    TMOD = 0x20;         // Timer 1, Mode 2
    TH1 = 0xFD;          // 9600 baud rate
    SCON = 0x50;         // UART Mode 1, receive enabled
    TR1 = 1;             // Start Timer 1

    ES = 1;              // Enable serial interrupt
    EA = 1;              // Enable global interrupt

    while(1);
}