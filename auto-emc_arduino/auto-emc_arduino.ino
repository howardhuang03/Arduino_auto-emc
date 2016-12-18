#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SC16IS750.h>
#include <espduino.h>
#include <mqtt.h>

#define pHSensorPin  1    // pH meter Analog output to Arduino Analog Input 0

#define windowSize  10    // Sampling window size for sensor data
#define centerSize   6    // Center window size for sensor data

#define debugPort Serial

SC16IS750 i2cuart = SC16IS750(SC16IS750_PROTOCOL_I2C, SC16IS750_ADDRESS_AA);
ESP esp(&i2cuart, &debugPort, 4);
MQTT mqtt(&esp);
boolean wifiConnected = false;
boolean mqttLinked = false;

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if (res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if (status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
      debugPort.println("ARDUINO: mqtt connect to broker");
      delay(500);
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
  debugPort.println("MQTT connected");
  mqttLinked = true;
  //mqtt.subscribe("/topic/0"); //or mqtt.subscribe("topic"); /*with qos = 0*/
  //mqtt.subscribe("/topic/1");
  //mqtt.subscribe("/topic/2");
  mqtt.subscribe("topic999");
  mqtt.publish("topic99", "MQTT connected");
}

void mqttDisconnected(void* response) {
  debugPort.println("MQTT disconnected");
  mqttLinked = false;
}

void mqttData(void* response)
{
  RESPONSE res(response);

  debugPort.print("Received: topic=");
  String topic = res.popString();
  debugPort.println(topic);

  debugPort.print("data=");
  String data = res.popString();
  debugPort.println(data);

}

void mqttPublished(void* response) {
  debugPort.print("Data published");
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
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
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

void setup() {
  debugPort.begin(115200);
  i2cuart.begin(9600);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  debugPort.println("Waiting for ESP");
  while (!esp.ready());

  debugPort.println("ARDUINO: setup mqtt client");
  if (!mqtt.begin("arduino", "", "", 120, false)) {
    debugPort.println("ARDUINO: fail to setup mqtt");
    while (1);
  }

  /* setup wifi */
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect("", "");

  /*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  //debugPort.println("ARDUINO: setup mqtt lwt");
  //mqtt.lwt("lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

  debugPort.println("ARDUINO: system started");
}

void loop() {
  float phData = pHSensorRead();
  //pHDataPrint(phData);
  esp.process();
  if (wifiConnected && mqttLinked) {
    mqtt.publish("topic99", "123");
  }
  delay(5000);
}
