#include <Wire.h>
#include <UnoWiFiDevEd.h>

#include "sensors/utils.h"
#include "sensors/sensor_base.h"
#include "sensors/ph_sensor.h"
#include "sensors/t_sensor.h"
#include "sensors/con_sensor.h"
#include "sensors/do_sensor.h"

#define emcVersion   1.1.0.beta
// Analog pin definition
#define pHSensorRx    A0    // PH sensor analog rx
#define conSensorRx   A1    // Conductivity sensor analog rx
// Digital pin definition
#define tempSensorRx  2    // Temperature sensor digital rx
#define doSensorRx    3    // DO sensor digital rx using uart protoc
#define doSensorTx    4    // DO sensor digital tx using uart protoc

#define debugPort Serial
#define deviceName "EMC00"
#define dataTopic "channels/local/data"
// Andy's channel
//#define dataTopic "channels/225785/publish/4T8RYMGPVKKGT3BL"
// Howard's channel
//#define dataTopic "channels/207041/publish/Y03Y5CTLEOQL1RQO"
#define cmdTopic "channels/local/cmd"
#define MQTT "mqtt"

PHSensor phSensor(pHSensorRx);
TSensor tSensor(tempSensorRx);
CONSensor conSensor(conSensorRx);
DOSensor doSensor(doSensorTx, doSensorRx);

int delayTime = 15 * 1000; // 15s
int updateCount = 4 * 5;   // Update to cloud each delay * updateCount
int count = 0;
float T_Sum = 0, PH_Sum = 0, DO_Sum = 0, EC_Sum = 0;

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
  String mqttMsg;
  float T = tempSensorRead();
  float PH = pHSensorRead();
  float DO = doSensorRead(T);
  float EC = 0; //conSensorRead(T);

  T_Sum += T;
  PH_Sum += PH;
  DO_Sum += DO;
  EC_Sum += EC;
  count++;

  if (count == updateCount) {
    // Process average data & send data to cloud
    // mqttMsg = String("field1=") + String(T_Sum / count, 2);
    // mqttMsg += String("&field2=") + String(PH_Sum / count, 2);
    // mqttMsg += String("&field3=") + String(DO_Sum / count, 2);
    // mqttMsg += String("&field4=") + String(EC_Sum / count, 2);
    // Process average data & send data to local service
    mqttMsg = String(deviceName);
    mqttMsg += String(",") + String(T_Sum / count, 2);
    mqttMsg += String(",") + String(PH_Sum / count, 2);
    mqttMsg += String(",") + String(DO_Sum / count, 2);
    mqttMsg += String(",") + String(EC_Sum / count, 2);
    debugPort.println(mqttMsg);
    Ciao.write(MQTT, dataTopic, mqttMsg);
    resetData();
  }
}

void resetData() {
  T_Sum = 0;
  PH_Sum = 0;
  DO_Sum = 0;
  EC_Sum = 0;
  count = 0;
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
  delay(delayTime);
  //readMqtt();
  //delay(15 * 1000);
}
