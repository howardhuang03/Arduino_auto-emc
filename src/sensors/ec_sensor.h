#ifndef EC_SENSOR_H
#define EC_SENSOR_H

#include <arduino.h>
#include "sensor_base.h"

#define EC1Addr 2 // Flash position for EC1Raw
#define ECSAddr 6 // Flash position for ECSRaw
#define EC1Reading 335.0f // CON reading in the 1.413 ms/cm solution
#define ECSReading 401.5f // CON reading in the sea water
#define EC1Buffer 1.41f // CON value for the 1.413 ms/cm solution
#define ECSBuffer 56.0f // CON value in the sea
#define ECAverageSize 10
#define ECReadDelay 10

class ECSensor : public SensorBase {
public:
    ECSensor(byte pin);

    float EC1Address;
    float ECSAddress;

    float getValue();
    void setTemperature(float t);
    void setEC1Raw(float v);
    void setECSRaw(float v);

private:

    byte pin;
    float currentTemperature;
    float EC1Raw;
    float ECSRaw;

};
#endif
