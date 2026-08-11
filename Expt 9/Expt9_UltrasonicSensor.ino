int trig = 10;
int echo = 9;
long duration;
int cm;
void setup() {
// put your setup code here, to run once:
pinMode(trig, OUTPUT);
pinMode(echo,INPUT);

Serial.begin(9600);
}
void loop() { // Sketch to calculate ultrasonic distance
digitalWrite(trig, LOW);
delayMicroseconds(10);
digitalWrite(trig, HIGH);
delayMicroseconds(10);
digitalWrite(trig, LOW);
delayMicroseconds(10);
duration = pulseIn(echo, HIGH);
cm = (duration/2) * 0.034;
// Print Value on Serial Monitor
Serial.print(" Distance = ");
Serial.print(cm);
Serial.println(" cm");
}
