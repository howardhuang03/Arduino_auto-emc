#ifndef DO_SENSOR_H
#define DO_SENSOR_H

#include <arduino.h>
#include <SoftwareSerial.h>
#include "sensor_base.h"

#define DefaultTemperature 20

class DOSensor : public SensorBase{
public:

    DOSensor(byte pinTx, byte pinRx);
    int setValue(String s);
    float getValue();

    void setTemperature(float t);
    void setCalibration(float t);

private:

    SoftwareSerial sensor;
    float currentTemperature;

};

#endif
