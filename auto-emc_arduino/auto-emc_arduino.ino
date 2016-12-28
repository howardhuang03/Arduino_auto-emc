#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Timer.h>
#include <SC16IS750.h>
#include <espduino.h>
#include <mqtt.h>

struct SensorInfo {
  float T;
  float PH;
  float DO;
  float CON;
};

#define pHSensorPin  1    // pH meter Analog output to Arduino Analog Input 0

#define windowSize  10    // Sampling window size for sensor data
#define centerSize   6    // Center window size for sensor data

#define debugPort Serial
#define deviceName "EMC00"
#define dataTopic "channels/local/data"
#define cmdTopic "channels/local/cmd"

Timer t;
SC16IS750 i2cuart = SC16IS750(SC16IS750_PROTOCOL_I2C, SC16IS750_ADDRESS_AA);
ESP esp(&i2cuart, &debugPort, 4);
MQTT mqtt(&esp);

boolean wifiConnected = false;
boolean mqttLinked = false;

const unsigned long wifiDelay = 15 * 1000; // Delay the wifi setup time for stability
const unsigned long updateDelay = 30 * 1000; // Update sensor data per updataTime

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if (res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if (status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED & mqtt connect to broker");
      mqtt.lwt("lwt", "offline", 0, 0);
      //mqtt.connect("10.0.1.7", 1883, false);
      wifiConnected = true;
    } else {
      wifiConnected = false;
      mqtt.disconnect();
    }
  }
}

void mqttConnected(void* response) {
  char topic[32];
  debugPort.println("MQTT connected");
  sprintf(topic, "%s/%s", cmdTopic, deviceName);
  mqttLinked = true;
  mqtt.subscribe(topic);
}

void mqttDisconnected(void* response) {
  debugPort.println("MQTT disconnected");
  mqttLinked = false;
}

void mqttData(void* response) {
  RESPONSE res(response);

  debugPort.print("Received: topic = ");
  String topic = res.popString();
  debugPort.println(topic);

  debugPort.print("data = ");
  String data = res.popString();
  debugPort.println(data);
}

void mqttPublished(void* response) {
  debugPort.println("Data published");
}

float pHTransfer (float data) {
  float phValue = (float)((720 + (400 / 3) - data) * 3 * 0.01);
  return data;
}

float pHSensorRead() {
  unsigned long int avgValue = 0;  // Store the average value of the sensor feedback
  int buf[windowSize], temp;

  // Get sample value from the sensor for smooth the value
  for (int i = 0; i < windowSize; i++) {
    buf[i] = analogRead(pHSensorPin);
    delay(10);
  }

  // Sort the analog data
  for (int i = 0; i < windowSize - 1; i++) {
    for (int j = i + 1; j < windowSize; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  // take the average value of centerSize sample
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }

  float phValue = (float)avgValue / 6;
  phValue = pHTransfer(phValue);

  return phValue;
}

void pHDataPrint(float data) {
  Serial.print("pH:");
  Serial.println(data, 2);
}

void updateSensorInfo() {
  char buf[100];
  SensorInfo info;

  info.T = 0;
  info.PH = pHSensorRead();
  info.DO = 0;
  info.CON = 0;

  setData2String(&info, buf);

  if (wifiConnected && mqttLinked) {
    mqtt.publish(dataTopic, buf);
  }
}

void setData2String(struct SensorInfo *info, char *buf) {
  int len = sizeof(struct SensorInfo) / sizeof(float);
  char str_temp[8];
  float *ptr = (float *)info;

  sprintf(buf, "%s", deviceName);
  for (int i = 0; i < len; i++) {
    dtostrf(*(ptr + i), 4, 2, str_temp);
    sprintf(buf, "%s,%s", buf, str_temp);
  }
}

void setupMqtt() {
  /* Setup mqtt & callback  */
  debugPort.println("Setup mqtt client");
  if (!mqtt.begin("arduino", "", "", 120, false)) {
    debugPort.println("Fail to setup mqtt");
    while (1);
  }
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);
}

void setupWifi() {
  /* Setup wifi */
  esp.enable();
  esp.reset();
  debugPort.println("Setup wifi");
  while (!esp.ready());
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect("", "");
}

void setup() {
  debugPort.begin(115200);
  i2cuart.begin(9600);
  // Set timer
  t.after(0, setupMqtt);
  t.after(wifiDelay, setupWifi);
  t.every(updateDelay, updateSensorInfo);
}

void loop() {
  esp.process();
  t.update();
}
