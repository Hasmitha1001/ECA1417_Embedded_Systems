#include <reg51.h>

void main()
{
    unsigned char hexa = 0xFF;
    unsigned char hundreds, tens, units;

    hundreds = hexa / 100;
    tens = (hexa % 100) / 10;
    units = hexa % 10;

    P0 = units;
    P1 = tens;
    P2 = hundreds;

    while(1);
}