#include "do_sensor.h"

DOSensor::DOSensor(byte pT, byte pR)
    :sensor(pT, pR), currentTemperature(0.0f)
{
    sensor.begin(9600);
    sensor.print("RESPONSE,0\r"); // Disable response code
    this->setTemperature(20.0f);
}

float DOSensor::getValue() {
    float v = 0.0f;
    sensor.print("R\r");
    String result;
    delay(10);
    
    // read value
    while (sensor.available()) {
        char inchar = (char) sensor.read();
        if (inchar == '\r') {
          break;
        }
        result += inchar;
    }
    if (result.length() > 0) {
        v = result.toFloat();
        this->log(String("DO: ") + String(v, 2));
    } else {
        this->log("DO read error");
    }
    
    return v;
}

void DOSensor::setTemperature(float t) {
    if (t >= 0.0f && t <= 100.0f && this->currentTemperature != t) {
        String msg = String("T,") + String(t, 2) + String("\r");
        this->sensor.print(msg);
        this->currentTemperature = t;
    }
}