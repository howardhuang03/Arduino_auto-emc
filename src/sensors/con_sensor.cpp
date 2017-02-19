#include "utils.h"
#include "con_sensor.h"

CONSensor::CONSensor(byte p)
    :pin(p), currentTemperature(20.0f)
{}

float CONSensor::getValue() {
    float v = Utils::analogReadAverage(this->pin, CONAverageSize, CONReadDelay);
    float con = Utils::map(v, CONSReading, CON1Reading, CONSBuffer, CON1Buffer);
    
    // TODO
    // float ec = 0;
    // float averageVoltage = v * 5000.0f / 1024.0f; // mapping to 5000 mv
    // float tempCoefficient = 1.0 + 0.0185 * (this->currentTemperature - 25.0);
    // 
    // float coefficientVolatge=(float)averageVoltage/tempCoefficient;   
    // if(coefficientVolatge < 150) {
    //     //25^C 1413us/cm<-->about 216mv  if the voltage(compensate)<150,that is <1ms/cm,out of the range
    //     this->log("No solution!");   
    // } else if (coefficientVolatge > 3300) {
    //     this->log("Out of the range!");  //>20ms/cm,out of the range
    // } else { 
    //     if(coefficientVolatge<=448) {
    //         ec = 6.84*coefficientVolatge-64.32;   //1ms/cm<EC<=3ms/cm
    //     } else if(coefficientVolatge<=1457) {
    //         ec =6.98*coefficientVolatge-127;  //3ms/cm<EC<=10ms/cm
    //     } else {
    //         ec = 5.3*coefficientVolatge+2278;                           //10ms/cm<EC<20ms/cm
    //     }
    //     ec /= 1000;    //convert us/cm to ms/cm
    // }
    // this->log(String("CoefficientVolatge: ") + String(coefficientVolatge, 1));
    
    this->log(String("CON Reading: ") + String(v, 1) + String(" CON: ") + String(con, 2));
    return con;
}

void CONSensor::setTemperature(float t) {
    if (t >= 0.0f && t <= 100.0f) {
        this->currentTemperature = t;
    }
}