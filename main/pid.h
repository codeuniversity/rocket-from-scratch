#pragma once

#define PID_MODE_MANUAL 0
#define PID_MODE_AUTOMATIC 1

#define PID_DIRECTION_DIRECT 0
#define PID_DIRECTION_REVERSE 1

class PID {
  public:
    float Run(float input); // run with new input
    void setTunings(float newKP, float newKI, float newKD); // change gains
    void setIterationDelay(int newTimeMillis); // change delay between iterations
    void setOutputLimits(float min, float max); // set boundaries for the output values of the PID
    //void setMode(int _mode);
    void setTarget(float _target); // set performance messure ("set point")
    void setControllerDirection(int _direction); // option to reverse output values

    explicit PID(float _target, float _kp, float _ki, float _kd, int _delay, float _min, float _max, int _direction) {
      setTarget(_target);
      setTunings(_kp, _ki, _kd);
      setIterationDelay(_delay);
      setOutputLimits(_min, _max);
      setControllerDirection(_direction);
    }

  private:
    unsigned long lastTimeMillis;
    float output, target;
    float integralTerm, lastInput;
    float kp, ki, kd;
    int iterationDelay;
    float outMin, outMax;
    int controllerDirection = PID_DIRECTION_DIRECT;
    bool isAuto = true;

    //void initialize();
};

float PID::Run(float input) {
  if (!isAuto) {
    return 0.0;
  }

  unsigned long nowMillis = millis();
  int timeChange = (nowMillis - lastTimeMillis);

  // check if configured iteration delay passed since the last run
  if (timeChange >= iterationDelay) {
    float error = target - input;
    integralTerm += (ki * error);

    // integral term is bound to the output maximum as well
    if (integralTerm > outMax) integralTerm = outMax;
    if (integralTerm < outMin) integralTerm = outMin;

    float inputDelta = (input - lastInput); //derivitive

    output = kp * error + integralTerm - kd * inputDelta; //calculate output
    // check that output is inbetween boundaries
    if (output > outMax) output = outMax;
    if (output < outMin) output = outMin;

    lastInput = input;
    lastTimeMillis = nowMillis;
  }

  return output;
}

void PID::setTunings(float newKP, float newKI, float newKD) {
  if (newKP < 0 || newKI < 0 || newKD < 0) return;

  kp = newKP;
  ki = newKI;
  kd = newKD;

  if (controllerDirection == PID_DIRECTION_REVERSE) {
    kp = (0 - kp);
    ki = (0 - ki);
    kd = (0 - kd);
  }
}

void PID::setIterationDelay(int newDelayMillis) {
  if (newDelayMillis > 0) {
    // float ratio  = (float)newDelayMillis / (float) iterationDelay;
    // ki *= ratio;
    // kd /= ratio;
    iterationDelay = (unsigned long) newDelayMillis;
  }
}

void PID::setOutputLimits(float min, float max) {
  if (min > max) return;
  outMin = min;
  outMax = max;

  if (output > outMax) output = outMax;
  if (output < outMin) output = outMin;

  if (integralTerm > outMax) integralTerm = outMax;
  if (integralTerm < outMin) integralTerm = outMin;
}

//void PID::setMode(int _mode){
//    bool modeBool = (_mode == PID_MODE_AUTOMATIC);
//    if(modeBool == !isAuto) initialize(); //going from manual to auto
//    isAuto = modeBool;
//}

//void PID::initialize(){
//    //lastInput = input;
//    integralTerm = output;
//    if(integralTerm > outMax) integralTerm = outMax;
//    if(integralTerm < outMin) integralTerm = outMin;
//}

void PID::setTarget(float _target) {
  target = _target;
}

void PID::setControllerDirection(int _direction) {
  controllerDirection = _direction;
}
