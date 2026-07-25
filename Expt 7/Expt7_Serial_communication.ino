
int number = 0;

void setup() 
{
  Serial.begin(9600); // Initialize serial communication at 9600 baud
}

void loop()
{
  Serial.print("Number is "); 
  Serial.println(number); // Print the number
  delay(500);             // Wait for 0.5 seconds
  number++;               // Increment the number
}
