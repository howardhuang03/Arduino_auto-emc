#ifndef CON_SENSOR_H
#define CON_SENSOR_H

#include <arduino.h>
#include "sensor_base.h"

#define CON1Reading 390.8f // CON reading in the 1.413 ms/cm solution
#define CONSReading 405.5f // CON reading in the sea water
#define CON1Buffer 1.41f // CON value for the 1.413 ms/cm solution
#define CONSBuffer 56.0f // CON value in the sea
#define CONAverageSize 10
#define CONReadDelay 10

class CONSensor : public SensorBase {
public:

    CONSensor(byte pin);

    float getValue();
    void setTemperature(float t);

private:

    byte pin;
    float currentTemperature;

};

#endif
