#include <Wire.h>
#include <EEPROM.h>
#include <UnoWiFiDevEd.h>

#include "sensors/utils.h"
#include "sensors/sensor_base.h"
#include "sensors/ph_sensor.h"
#include "sensors/t_sensor.h"
#include "sensors/con_sensor.h"
#include "sensors/do_sensor.h"

#define version   "1.1.0"
// Analog pin definition
#define PH_Rx A0   // PH sensor analog rx
#define EC_Rx A1   // Conductivity sensor analog rx
// Digital pin definition
#define T_Rx  2    // Temperature sensor digital rx
#define DO_Rx 3    // DO sensor digital rx using uart protoc
#define DO_Tx 4    // DO sensor digital tx using uart protoc

#define debugPort Serial
#define dataTopic "channels/local/data"
// Andy's channel
//#define dataTopic "channels/225785/publish/4T8RYMGPVKKGT3BL"
// Howard's channel
//#define dataTopic "channels/207041/publish/Y03Y5CTLEOQL1RQO"
#define cmdTopic "channels/local/cmd"
#define testTopic "channels/local/test"
#define MQTT "mqtt"

PHSensor PH(PH_Rx);
TSensor T(T_Rx);
CONSensor EC(EC_Rx);
DOSensor DO(DO_Tx, DO_Rx);

int delayTime = 5 * 1000;  // 5s
int update = 12 * 5;   // Update to cloud each delay * update
String devName = "EMC";

void readMqttCmd() {
  CiaoData data = Ciao.read(MQTT, cmdTopic);
  String msg = "";

  while (!data.isEmpty()) {
    /*
    debugPort.print("id=");
    debugPort.println(data.get(0));
    debugPort.print("channel=");
    debugPort.println(data.get(1));
    debugPort.print("payload=");
    debugPort.println(data.get(2));
    */
    msg = data.get(2);
    data = Ciao.read(MQTT, cmdTopic);
  }

  if (msg.length() == 0) return;

  // FIXME hard coded DO calibration
  DO.Calibration(msg.toInt());
}

float readPH() {
    return PH.getValue();
}

float readEC(float temp) {
    EC.setTemperature(temp);
    return EC.getValue();
}

float readT() {
  return T.getValue();
}

float readDO(float temp) {
    DO.setTemperature(temp);
    return DO.getValue();
}

void updateSensorInfo() {
  String msg;
  float T  = readT();;

  static int count = 0;
  static float T_Sum = 0;
  static float PH_Sum = 0;
  static float DO_Sum = 0;
  static float EC_Sum = 0;

  count++;
  T_Sum += T;
  PH_Sum += readPH();
  DO_Sum += readDO(T);
  EC_Sum += readEC(T);

  if (count == update) {
    // Process average data & send data to cloud
    // msg = String("field1=") + String(T_Sum / count, 2);
    // msg += String("&field2=") + String(PH_Sum / count, 2);
    // msg += String("&field3=") + String(DO_Sum / count, 2);
    // msg += String("&field4=") + String(EC_Sum / count, 2);
    // Process average data & send data to local service
    msg = devName;
    msg += String(",") + String(T_Sum / count, 2);
    msg += String(",") + String(PH_Sum / count, 2);
    msg += String(",") + String(DO_Sum / count, 2);
    msg += String(",") + String(EC_Sum / count, 2);
    debugPort.println(msg);
    Ciao.write(MQTT, dataTopic, msg);
    T_Sum = PH_Sum = DO_Sum = EC_Sum = count = 0;
  }
}

// Write flash only
/*void writeDevNum() {
  int addr = 0;
  // Clear EEPROM
  for (addr = EEPROM.length(); addr >= 0 ; addr--) {
    EEPROM.write(addr, 0);
  }
  // Write EEPROM
  EEPROM.write(addr, 0);
  EEPROM.write(addr + 1, 0);
}*/

String getDevName() {
  byte num = 0;
  String name = devName;
  for (int addr = 0; addr < 2; addr++) {
    num = EEPROM.read(addr);
    name += String(num, 10);
  }
  return name;
}

void setup() {
  debugPort.begin(115200);
  devName = getDevName();
  debugPort.println(devName + " " + version);

  PH.setDebugStream(&debugPort);
  T.setDebugStream(&debugPort);
  EC.setDebugStream(&debugPort);
  DO.setDebugStream(&debugPort);

  Ciao.begin();
  Ciao.write(MQTT, testTopic, devName + "," + version);
}

void loop() {
  updateSensorInfo();
  readMqttCmd();
  delay(delayTime);
}
