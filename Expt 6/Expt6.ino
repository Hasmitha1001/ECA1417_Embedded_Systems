#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() 
{
  lcd.begin(16, 2);      // Initialize 16x2 LCD
  lcd.print("Hello world!");
}

void loop() 
{
  // Nothing needed here
}
