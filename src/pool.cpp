#include <Arduino.h>
#include "config.h"
#include <SPI.h>
#include <WiFi101.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            5         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

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
void setupDHT();
void readDHT();

char ssid[] = WIFI_SSID;                     // your network SSID (name)
char pass[] = WIFI_PASS;       // your network key
int status = WL_IDLE_STATUS;                     // the Wifi radio's status


void setup() {
    WiFi.setPins(8,7,4,2); //correct pins for feather
    Serial.begin(9600);     // serial output for debug
    pinMode(sensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA network, SSID: ");
      Serial.println(ssid);
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(10000);
    }
    setupDHT();
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
    readDHT();
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
  Serial.println("\n ****************************** ");
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

void setupDHT() {
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void readDHT() {
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }
}
