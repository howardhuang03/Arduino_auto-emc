#ifndef T_SENSOR_H
#define T_SENSOR_H

#include <arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensor_base.h"

class TSensor : public SensorBase {
public:
    
    TSensor(byte pin);
    
    float getValue();

private:
    
    OneWire oneWire;
    DallasTemperature tempSensor;

};

#endif