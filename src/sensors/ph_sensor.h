#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include "arduino.h"
#include "sensor_base.h"

#define PH7Reading 645.0f // PH7 Buffer Solution Reading
#define PH4Reading 760.0f // PH4 Buffer Solution Reading
#define PH7Buffer 7.0f
#define PH4Buffer 4.0f
#define PHAverageSize 10
#define PHReadDelay 10

class PHSensor : public SensorBase
{
public:
    
    PHSensor(byte pin);
    
    float getValue();
    
private:

    byte pin;
    
};

#endif