#include <SPI.h>
#include <EthernetENC.h>
#include <EthernetUdp.h>
#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <Adafruit_HTU21DF.h>

#include "NetworkConfig.h"

// 3. Instantiate the UDP Class
EthernetUDP Udp;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

unsigned long lastSent = 0;

const int tempSensorPin = A0;

unsigned long int humidityReadId = 1;
unsigned long int temperatureReadId = 1;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB boards)
  }

  Serial.println(F("Initializing Ethernet..."));

  // Try to configure using DHCP (dynamic IP allocation)
  // if (Ethernet.begin(mac) == 0) {
  //   Serial.println("Failed to configure Ethernet using DHCP, falling back to static IP.");
  //   Ethernet.begin(mac, ip);
  // }

  //
  Ethernet.init(10);
  Ethernet.begin(mac, ip, dnsServer, gateway, subnet);

  // Start the UDP socket
  Udp.begin(localPort);

  Serial.print(F("Arduino IP address: "));
  Serial.println(Ethernet.localIP());

  if (!htu.begin()) {
    Serial.println(F("HTU21D not found!"));
    while (1);
  }

  Serial.println(F("HTU21D initialized."));
}

void loop() {
  // Service networking continuously
  Ethernet.maintain();

  // Send UDP every five seconds
  if (millis() - lastSent >= 5000) {
    lastSent = millis();  // Reset the five-second timer

    char messageBuffer[50];

    // V1|node-<nodeIpLastOctet>|1042|READING|temp_f|72.45

    // float fTemp_TMP36 = getTempF_TMP36();

    // Serial.println(F("TMP36 Temp F:"));
    // Serial.println(fTemp_TMP36);

    float cTemp_HTU21D = htu.readTemperature();
    float humidity_HTU21D = htu.readHumidity();

    Serial.println(F("HTU21D Temp C:"));
    Serial.println(cTemp_HTU21D);
    Serial.println(F("HTU21D Humidity:"));
    Serial.println(humidity_HTU21D);

    float fTemp_HTU21D = convertCelsiusToFarenheit(cTemp_HTU21D);

    Serial.println(F("HTU21D Temp F:"));
    Serial.println(fTemp_HTU21D);

    Serial.print(F("Sending packets to "));
    Serial.print(targetIP);
    Serial.print(F(" on port "));
    Serial.println(targetPort);

    // TMP36 temperature message
    // snprintf(messageBuffer, sizeof(messageBuffer), "V1|node-%u|1042|TMP36_READ|temp_f|", nodeIpLastOctet);

    // dtostrf(fTemp_TMP36, 0, 2,
    //     messageBuffer + strlen(messageBuffer));

    // Serial.println(messageBuffer);

    // Udp.beginPacket(targetIP, targetPort);
    // Udp.write(messageBuffer);
    // if (Udp.endPacket() == 1) {
    //   Serial.println(F("TMP36 Temp F - packet sent successfully."));
    // } else {
    //   Serial.println(F("TMP36 Temp F - error sending packet."));
    // }

    // HTU21D temperature message
    snprintf(messageBuffer, sizeof(messageBuffer), "V1|node-%u|%lu|HTU21D_READ|temp_f|", nodeIpLastOctet, temperatureReadId);

    dtostrf(fTemp_HTU21D, 0, 2,
        messageBuffer + strlen(messageBuffer));

    Serial.println(messageBuffer);

    Udp.beginPacket(targetIP, targetPort);
    Udp.write(messageBuffer);
    if (Udp.endPacket() == 1) {
      Serial.println(F("HTU21D Temp C - packet sent successfully."));
    } else {
      Serial.println(F("HTU21D Temp C - error sending packet."));
    }
    temperatureReadId++;

    // HTU21D humidity message
    snprintf(messageBuffer, sizeof(messageBuffer), "V1|node-%u|%lu|HTU21D_READ|humidity|", nodeIpLastOctet, humidityReadId);

    dtostrf(humidity_HTU21D, 0, 2,
        messageBuffer + strlen(messageBuffer));

    Serial.println(messageBuffer);

    Udp.beginPacket(targetIP, targetPort);
    Udp.write(messageBuffer);
    if (Udp.endPacket() == 1) {
      Serial.println(F("HTU21D Humidity - packet sent successfully."));
    } else {
      Serial.println(F("HTU21D Humidity - error sending packet."));
    }
    humidityReadId++;
  }

  delay(5);
}


float getTempF_TMP36() {
  // 1. Get the raw analog reading (0 to 1023)
  int reading = analogRead(tempSensorPin);  

  // 2. Convert the raw reading to voltage (assuming a 5V Arduino power supply)
  // For 3.3V Arduinos (like the Due or ESP32), change 5.0 to 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 

  // 3. Convert voltage to Celsius
  // The TMP36 has a 500mV (0.5V) offset and changes by 10mV per degree Celsius
  float temperatureC = (voltage - 0.5) * 100.0; 

  // 4. Convert Celsius to Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  // 5. Print the results to the Serial Monitor
  Serial.print(F("Voltage: "));
  Serial.print(voltage);
  Serial.print(F(" V | "));

  Serial.print(F("Temp: "));
  Serial.print(temperatureC);
  Serial.print(F(" °C | "));

  Serial.print(temperatureF);
  Serial.println(F(" °F"));

  return temperatureF;
}

float convertCelsiusToFarenheit(float tempC) {
  return tempC * 9/5 + 32;
}