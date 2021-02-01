#include <Servo.h>

Servo inner;
Servo outer;

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
  inner.attach(3);
  outer.attach(9);

  outer.write(getAngle(0.5, OUTER_MIN, OUTER_MAX));
  inner.write(getAngle(0.5, INNER_MIN, INNER_MAX));
}

void loop() {
  inner.write(getAngle(0.0, INNER_MIN, INNER_MAX));
  delay(1000);
  outer.write(getAngle(0.0, OUTER_MIN, OUTER_MAX));
  delay(1000);
  inner.write(getAngle(1.0, INNER_MIN, INNER_MAX));
  delay(1000);
  outer.write(getAngle(1.0, OUTER_MIN, OUTER_MAX));
  delay(1000);
}

int getAngle(float percent, int minAngle, int maxAngle) {
  int span = maxAngle - minAngle;
  float adding = percent * (float)span;
  return minAngle + (int)adding;
}
