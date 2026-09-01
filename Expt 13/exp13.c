#include <reg51.h> 

void main(void) 
{ 
 unsigned int i; 
 // Fix the line below:
 unsigned int array[5] = {0x1111, 0x2222, 0x8888, 0x4444, 0xABCD}; 
 unsigned long sum = 0; 

 for (i = 0; i < 5; i++) 
 { 
  sum = sum + array[i]; 
 } 

 P0 = (unsigned char)(sum & 0xFF);         // Lower 8 bits 
 P1 = (unsigned char)((sum >> 8) & 0xFF);  // Next 8 bits 
 P2 = (unsigned char)((sum >> 16) & 0xFF); // Upper 8 bits 
  
 while(1); // keep output stable 
}