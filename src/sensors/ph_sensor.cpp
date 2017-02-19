#include "utils.h"
#include "ph_sensor.h"

PHSensor::PHSensor(byte p)
    :pin(p)
{}
    
float PHSensor::getValue() {
    float v = Utils::analogReadAverage(this->pin, PHAverageSize, PHReadDelay);
    float ph = Utils::map(v, PH7Reading, PH4Reading, PH7Buffer, PH4Buffer);
    this->log(String("PH Reading: ") + String(v, 1) + String(" PH: ") + String(ph, 2));
    return ph;
}