#pragma once

#include <Servo.h>

#define INNER_MID 22
#define OUTER_MID 130

const int INNER_MIN = INNER_MID - 20;
const int INNER_MAX = INNER_MID + 20;
const int OUTER_MIN = OUTER_MID - 20;
const int OUTER_MAX = OUTER_MID + 20;

class Gimbal{
    public:
        void setCoordinates(int inner, int outer);

        Gimbal(int innerPin, int outerPin){
            innerServo.attach(innerPin);
            outerServo.attach(outerPin);
        }

    private:
        Servo innerServo;
        Servo outerServo;

        int getAngleInner(float percent);
        int getAngleOuter(float percent);
        int getAngle(float percent, int minAngle, int maxAngle);
};

void Gimbal::setCoordinates(int inner, int outer){
  float percentInner = (float)inner / 100.0;
  float percentOuter = (float)outer / 100.0;

  innerServo.write(getAngleInner(percentInner));
  outerServo.write(getAngleOuter(percentOuter));
}

int Gimbal::getAngleInner(float percent) {
  return getAngle(percent, INNER_MIN, INNER_MAX);
}

int Gimbal::getAngleOuter(float percent) {
  return getAngle(percent, OUTER_MIN, OUTER_MAX);
}

int Gimbal::getAngle(float percent, int minAngle, int maxAngle) {
  int span = maxAngle - minAngle;
  float adding = percent * (float)span;
  return minAngle + (int)adding;
}
