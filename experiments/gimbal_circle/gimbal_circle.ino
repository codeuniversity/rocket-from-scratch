#include <Servo.h>
#include <math.h>

Servo innerServo;
Servo outerServo;

//calibration
int INNER_MID = 40;
int OUTER_MID = 130;

//int INNER_MIN = INNER_MID - 20;
//int INNER_MAX = INNER_MID + 20;
//int OUTER_MIN = OUTER_MID - 20;
//int OUTER_MAX = OUTER_MID + 20;

int MIN_COORD = -20;
int MAX_COORD = 20;

void setup() {
  innerServo.attach(3);
  outerServo.attach(9);

  setCoordinates(0, 0);
  delay(1000);
}

void loop() { 
  for(int i=MIN_COORD; i<MAX_COORD; i++){
    int p = i - MIN_COORD;
    int q = MAX_COORD - i;

    int h = round((1.1*sqrt(p*q)));

    setCoordinates(i, h);
    delay(10);
  }
  for(int i=MAX_COORD; i>MIN_COORD; i--){
    int p = i - MIN_COORD;
    int q = MAX_COORD - i;

    int h = -round((1.1*sqrt(p*q)));

    setCoordinates(i, h);
    delay(10);
  }
}

void setCoordinates(int inner, int outer){
  innerServo.write(INNER_MID + inner);
  outerServo.write(OUTER_MID + outer);
}

//int getAngleInner(float percent) {
//  return getAngle(percent, INNER_MIN, INNER_MAX);
//}
//
//int getAngleOuter(float percent) {
//  return getAngle(percent, OUTER_MIN, OUTER_MAX);
//}
   

int getAngle(float percent, int minAngle, int maxAngle) {
  int span = maxAngle - minAngle;
  float adding = percent * (float)span;
  return minAngle + (int)adding;
}
