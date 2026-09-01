#include <reg51.h>
#include <stdio.h> 
void main (void){ 
SCON = 0x50; /* SCON: mode 1, 8-bit UART, enable rcvr */ TMOD = 0x20; /* TMOD: timer 1, mode 2, 8-bit reload */ TH1 = 0xFD; /* TH1: reload  value for 9600 baudrate */ 
TR1 = 1; /* TR1: timer 1 run */ 
TI = 1; /* TI: set TI to send first char of UART */ while (1) 
{ 
printf ("Hello World ! \n "); 
} 
} 
