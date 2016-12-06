#include <SoftwareSerial.h>

#define pHSensorPin  1    // pH meter Analog output to Arduino Analog Input 0

#define windowSize  10    // Sampling window size for sensor data
#define centerSize   6    // Center window size for sensor data

float pHTransfer (float data) {
  float phValue = (float)((720 + (400 / 3) - data) * 3 * 0.01);
  return data;
}

float pHSensorRead() {
  unsigned long int avgValue = 0;  // Store the average value of the sensor feedback
  int buf[windowSize], temp;
  
  // Get sample value from the sensor for smooth the value
  for(int i = 0; i < windowSize; i++) {
    buf[i] = analogRead(pHSensorPin);
    delay(10);
  }

  // Sort the analog data
  for(int i = 0; i < 9; i++) {
    for(int j = i + 1; j < 10; j++) {
      if(buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  // take the average value of centerSize sample
  for(int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }

  float phValue = (float)avgValue / 6;
  phValue = pHTransfer(phValue);
  
  return phValue;
}

void pHDataPrint(float data) {
  Serial.print(" pH:");  
  Serial.print(data, 2);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Auto-EMC Ready");
}

void loop() {
  float phData = pHSensorRead();
  pHDataPrint(phData);
}

