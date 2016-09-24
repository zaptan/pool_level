#include <Arduino.h>
#include "config.h"
#include <SPI.h>
#include <WiFi101.h>



const int sensorPin = 0;
const int ledPin = 12;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 10000;
const int limit = 300;

void tapStart();
void tapStop();
int waterLevel();
int waterRaw();
int waterTemp();
int airTemp();
int airHumidity();
void printWifiData();


char ssid[] = WIFI_SSID;                     // your network SSID (name)
char pass[] = WIFI_PASS;       // your network key
int status = WL_IDLE_STATUS;                     // the Wifi radio's status


void setup() {
    WiFi.setPins(8,7,4,2); //correct pins for feather
    Serial.begin(9600);     // serial output for debug
    pinMode(sensorPin, INPUT_PULLDOWN);
    pinMode(ledPin, OUTPUT);
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA network, SSID: ");
      Serial.println(ssid);
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(10000);
    }
}

/*
the sensorPin returns a number based on how conductive whatever medium is
touching the sensor. tapwater ranged from 200 to 320 pool water seems to go from
258 to 415. there is a graph on the back of the sensor with 4cm marled out so I
mapped the numbers to that. the sensor is slow it can take up to a min to settle
on a value, it will always jump really high and then slowly come back down to
settle on the actual value

 */
void loop() {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        Serial.println(waterRaw());
        if (waterRaw() < limit) {
            tapStart();
        } else {
            tapStop();
        }
    printWifiData();
    }
}

void tapStart() {
    // tun on water
    digitalWrite(ledPin, HIGH);
}

void tapStop() {
    // tun off water
    digitalWrite(ledPin, LOW);
}


    int waterLevel() {
        return 30; //TODO: implement measurement
    }

    int waterRaw() {
        return analogRead(sensorPin);
    }

    int waterTemp() {
        return 0;
    }
    int airTemp() {
        return 0;
    }
    int airHumidity() {
        return 0;
    }

void printWifiData() {
  Serial.println("\n ****************************** "
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
