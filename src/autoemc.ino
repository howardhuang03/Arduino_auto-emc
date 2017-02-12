#include <Wire.h>
#include <UnoWiFiDevEd.h>

#include "sensors/utils.h"
#include "sensors/sensor_base.h"
#include "sensors/ph_sensor.h"
#include "sensors/t_sensor.h"
#include "sensors/con_sensor.h"
#include "sensors/do_sensor.h"

#define emcVersion   1.1.0
// Analog pin definition
#define pHSensorRx    A0    // PH sensor analog rx
#define conSensorRx   A1    // Conductivity sensor analog rx
// Digital pin definition
#define tempSensorRx  2    // Temperature sensor digital rx
#define doSensorRx    3    // DO sensor digital rx using uart protoc
#define doSensorTx    4    // DO sensor digital tx using uart protoc

#define debugPort Serial
#define deviceName "EMC00"
//#define dataTopic "channels/local/data"
#define dataTopic "channels/225785/publish/4T8RYMGPVKKGT3BL"
#define cmdTopic "channels/local/cmd"
#define MQTT "mqtt"


PHSensor phSensor(pHSensorRx);
TSensor tSensor(tempSensorRx);
CONSensor conSensor(conSensorRx);
DOSensor doSensor(doSensorTx, doSensorRx);

void readMqtt() {
    
  CiaoData data = Ciao.read(MQTT, cmdTopic);
  while (!data.isEmpty()){
    debugPort.print("id=");
    debugPort.println(data.get(0));
    debugPort.print("sender=");
    debugPort.println(data.get(1));
    debugPort.print("msg=");
    debugPort.println(data.get(2));
    data = Ciao.read(MQTT, cmdTopic);
  }
}

float pHSensorRead() {
    return phSensor.getValue();
}

float conSensorRead(float temp) {
    conSensor.setTemperature(temp);
    return conSensor.getValue();
}

float tempSensorRead() {
  return tSensor.getValue();
}

float doSensorRead(float temp) {
    doSensor.setTemperature(temp);
    return doSensor.getValue();
}

void updateSensorInfo() {
  float T = tempSensorRead();
  float PH = pHSensorRead();
  float DO = doSensorRead(T);
  float CON = conSensorRead(T);
  
  String mqttMsg;
  mqttMsg = String("field1=") + String(T, 2);
  mqttMsg += String("&field2=") + String(PH, 2);
  mqttMsg += String("&field3=") + String(DO, 2);
  mqttMsg += String("&field4=") + String(CON, 2);
  
  debugPort.println(mqttMsg);
  Ciao.write(MQTT, dataTopic, mqttMsg);
}

void setup() {
  debugPort.begin(115200);
  debugPort.println("Setup sensor");
  
  phSensor.setDebugStream(&debugPort);
  tSensor.setDebugStream(&debugPort);
  conSensor.setDebugStream(&debugPort);
  doSensor.setDebugStream(&debugPort);
  
  Ciao.begin();
}

void loop() { 
  updateSensorInfo();
  delay(15 * 1000);
  readMqtt();
  delay(15 * 1000);
}
