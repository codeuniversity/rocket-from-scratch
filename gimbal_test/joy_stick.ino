#include <Servo.h>
#include <math.h>

const int switchPin = A1;  // Joystick button
const int pinX = A3; // Joystick X axis
const int pinY = A2; // Joystick Y axis

//calibration
int INNER_MID = 40;
int OUTER_MID = 130;

int MIN_COORD = -20;
int MAX_COORD = 20;

Servo innerServo;
Servo outerServo;

void setup() {
  innerServo.attach(3);
  outerServo.attach(9);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH); // Connect the built-in wind-up resistor
  Serial.begin(9600);

  setCoordinates(0, 0);
  delay(1000);
}
 
void loop() {
  int ledState = digitalRead(switchPin); // Read the button's state
  int X = analogRead(pinX); // Read the X axis analog value
  int Y = analogRead(pinY); // Read the Y axis analog value
  int converted_x;
  int converted_y;
  convert_x = (X-512)/20;
  convert_y = (Y-512)/20;

//  converted_x = -round((((float(X)/1023)*40)-22));
//  converted_y = round((((float(Y)/1023)*40)-22));


  setCoordinates(converted_x, converted_y);
  Serial.print(X);  // Input the values into Serial Monitor
  Serial.print("\t"); // Tabulation
  Serial.println(Y);


  Serial.print("converted x:");  
  Serial.print("\t"); 
  Serial.println(converted_x);  
  Serial.print("converted y:");  
  Serial.print("\t"); 
  Serial.println(converted_y); 
}

void setCoordinates(int inner, int outer){
  innerServo.write(INNER_MID + inner);
  outerServo.write(OUTER_MID + outer);
}
