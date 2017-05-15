#include <Wire.h>
#include <EEPROM.h>
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
#define defaultName "EMC"
#define dataTopic "channels/local/data"
// Andy's channel
//#define dataTopic "channels/225785/publish/4T8RYMGPVKKGT3BL"
// Howard's channel
//#define dataTopic "channels/207041/publish/Y03Y5CTLEOQL1RQO"
#define cmdTopic "channels/local/cmd"
#define testTopic "channels/local/test"
#define MQTT "mqtt"

PHSensor phSensor(pHSensorRx);
TSensor tSensor(tempSensorRx);
CONSensor conSensor(conSensorRx);
DOSensor doSensor(doSensorTx, doSensorRx);

int delayTime = 5 * 1000; // 15s
int updateCount = 12 * 5;   // Update to cloud each delay * updateCount
int count = 0;
float T_Sum = 0, PH_Sum = 0, DO_Sum = 0, EC_Sum = 0;
String deviceName = "";

void readMqttCmd() {
  CiaoData data = Ciao.read(MQTT, cmdTopic);
  String msg = "";

  while (!data.isEmpty()){
    debugPort.print("id=");
    debugPort.println(data.get(0));
    debugPort.print("channel=");
    debugPort.println(data.get(1));
    debugPort.print("payload=");
    msg = data.get(2);
    debugPort.println(data.get(2));
    data = Ciao.read(MQTT, cmdTopic);
  }

  if (msg.length() == 0) return;

  // FIXME hard coded DO calibration
  doSensor.setCalibration(msg.toInt());
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
  float EC = conSensorRead(T);

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

void writeDeviceNum() {
  int addr = 0;
  // Clear EEPROM
  for (addr = EEPROM.length(); addr >= 0 ; addr--) {
    EEPROM.write(addr, 0);
  }
  // Write EEPROM
  EEPROM.write(addr, 0);
  EEPROM.write(addr + 1, 0);
}

String getDeviceName() {
  byte num = 0;
  String name = defaultName;
  for (int addr = 0; addr < 2; addr++) {
    num = EEPROM.read(addr);
    name += String(num, 10);
  }
  return name;
}

void setup() {
  debugPort.begin(115200);
  deviceName = getDeviceName();
  debugPort.println("Start " + deviceName);

  phSensor.setDebugStream(&debugPort);
  tSensor.setDebugStream(&debugPort);
  conSensor.setDebugStream(&debugPort);
  doSensor.setDebugStream(&debugPort);

  Ciao.begin();
  Ciao.write(MQTT, testTopic, deviceName);
}

void loop() {
  updateSensorInfo();
  readMqttCmd();
  delay(delayTime);
}
