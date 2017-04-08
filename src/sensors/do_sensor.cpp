#include "do_sensor.h"

DOSensor::DOSensor(byte pT, byte pR)
    :sensor(pT, pR), currentTemperature(20.0f)
{
    sensor.begin(9600);
    this->setValue("RESPONSE,0"); // Disable response code
}

int DOSensor::setValue(String s) {
    String result = s + String("\r");
    sensor.print(s); // Write value
    return 0;
}

float DOSensor::getValue() {
    float v = 0.0f;
    this->setValue("R");
    String result;

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
        this->log("DO read empty value");
    }

    return v;
}

void DOSensor::setTemperature(float t) {
    if (t >= 0.0f && t <= 100.0f && this->currentTemperature != t) {
        String msg = String("T,") + String(t, 2);
        this->setValue(msg);
        this->currentTemperature = t;
    }
}

void DOSensor::setCalibration(float t) {
  String Calibration = "Cal";
  this->setTemperature(t);
  delay(100);
  this->setValue(Calibration);
}
