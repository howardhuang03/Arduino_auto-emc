#include "t_sensor.h"

TSensor::TSensor(byte pin)
    :oneWire(pin), tempSensor(&oneWire)
{
    tempSensor.begin();
}

float TSensor::getValue() {
    tempSensor.requestTemperatures();
    float v = tempSensor.getTempCByIndex(0);
    if (v < 0) v = 0;
    this->log(String("Temperature: ") + String(v, 2));
    return v;
}
