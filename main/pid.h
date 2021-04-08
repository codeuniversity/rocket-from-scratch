#pragma once

#define PID_MODE_MANUAL 0
#define PID_MODE_AUTOMATIC 1
 
#define PID_DIRECTION_DIRECT 0
#define PID_DIRECTION_REVERSE 1

class PID {
    public:
        void Run(float input);
        void setTunings(float newKP, float newKI, float newKD);
        void setIterationDelay(int newTimeMillis);
        void setOutputLimits(float min, float max);
        void setMode(int _mode);
        void setTarget(float _target);
        void setControllerDirection(int _direction);

        explicit PID(float _target, float _kp, float _ki, float _kd, int _delay, float _min, float _max, int _direction){
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
        bool isAuto = false;

        void initialize();
};

void PID::Run(input) {
    if(!isAuto) return;
    
    unsigned long nowMillis = millis();
    int timeChange = (nowMillis - lastTimeMillis);

    if(timeChange >= iterationDelay){
        float error = target - input;
        integralTerm += (ki * error);

        if(integralTerm > outMax) integralTerm = outMax;
        if(integralTerm < outMin) integralTerm = outMin;

        float inputDelta = (input - lastInput);

        output = kp * error + integralTerm - kd * inputDelta;
        if(output > outMax) output = outMax;
        if(output < outMin) output = outMin;

        lastInput = input;
        lastTimeMillis = nowMillis;
    }
}

void PID::setTunings(float newKP, float newKI, float newKD){
    if (newKP < 0 || newKI < 0|| newKD < 0) return;
 
    float iterationDelaySeconds = ((float)iterationDelay)/1000;
    kp = newKP;
    ki = newKI * iterationDelaySeconds;
    kd = newKD / iterationDelaySeconds;
    
    if(controllerDirection == PID_DIRECTION_REVERSE){
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
}

void PID::setIterationDelay(int newDelayMillis){
    if (newDelayMillis > 0){
        float ratio  = (float)newDelayMillis / (float) iterationDelay;
        ki *= ratio;
        kd /= ratio;
        iterationDelay = (unsigned long) newDelayMillis;
    }
}
 
void PID::setOutputLimits(float min, float max){
    if(min > max) return;
    outMin = min;
    outMax = max;
 
    if(output > outMax) output = outMax;
    if(output < outMin) output = outMin;
 
    if(integralTerm > outMax) integralTerm = outMax;
    if(integralTerm < outMin) integralTerm = outMin;
}

void PID::setMode(int _mode){
    bool modeBool = (_mode == PID_MODE_AUTOMATIC);
    if(modeBool == !isAuto) initialize(); //going from manual to auto
    isAuto = modeBool;
}
 
void PID::initialize(){
    lastInput = input;
    integralTerm = output;
    if(integralTerm > outMax) integralTerm = outMax;
    if(integralTerm < outMin) integralTerm = outMin;
}

void PID::setTarget(float _target){
    target = _target;
}

void PID::setControllerDirection(int _direction){
    controllerDirection = _direction;
}