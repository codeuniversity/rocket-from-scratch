#include <Servo.h>

Servo innerServo;
Servo outerServo;

//int INNER_MIN = 0;
//int INNER_MAX = 45;
//int OUTER_MIN = 80;
//int OUTER_MAX = 180;

int INNER_MID = 22;
int OUTER_MID = 130;

int INNER_MIN = INNER_MID - 20;
int INNER_MAX = INNER_MID + 20;
int OUTER_MIN = OUTER_MID - 20;
int OUTER_MAX = OUTER_MID + 20;

void setup() {
  innerServo.attach(3);
  outerServo.attach(9);

  setCoordinates(50, 50);
  delay(1000);
}

void loop() { 
  setCoordinates(0, 0);
  delay(150);
  setCoordinates(100, 0);
  delay(150);
  setCoordinates(100, 100);
  delay(150);
  setCoordinates(0, 100);
  delay(150);
}

void setCoordinates(int inner, int outer){
  float percentInner = (float)inner / 100.0;
  float percentOuter = (float)outer / 100.0;

  innerServo.write(getAngleInner(percentInner));
  outerServo.write(getAngleOuter(percentOuter));
}

int getAngleInner(float percent) {
  return getAngle(percent, INNER_MIN, INNER_MAX);
}

int getAngleOuter(float percent) {
  return getAngle(percent, OUTER_MIN, OUTER_MAX);
}
   

int getAngle(float percent, int minAngle, int maxAngle) {
  int span = maxAngle - minAngle;
  float adding = percent * (float)span;
  return minAngle + (int)adding;
}
