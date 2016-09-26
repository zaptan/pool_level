#include <Arduino.h>
#include "config.h"
#include <SPI.h>
#include <WiFi101.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define TMPPIN            A2        // Analog pin 2 for water temp
#define WATPIN            A1        // Analog pin 1 for water level
#define DHTPIN            5         // digital pin 5 for DHT
#define NEOPIN            11        // Digital pin for Neopixils
#define SILPIN            10        // Digital pin for silinoid

#define DHTTYPE           DHT22     // DHT 22 (AM2302)

#define THERMISTORNOMINAL 10000
#define SERIESRESISTOR    9980
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

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
void waterStatus();
void publishio();

char ssid[] = WIFI_SSID;                     // your network SSID (name)
char pass[] = WIFI_PASS;       // your network key
int status = WL_IDLE_STATUS;                     // the Wifi radio's status


AdafruitIO_Feed *pooltemp = io.feed("pooltemp");
AdafruitIO_Feed *poolrssi = io.feed("poolrssi");
AdafruitIO_Feed *poollevel = io.feed("poollevel");
AdafruitIO_Feed *airtemp = io.feed("airtemp");
AdafruitIO_Feed *airrh = io.feed("airrh");
AdafruitIO_Feed *poolpump = io.feed("poolpump");
float ptm, atm, arh;
long prss, plv;
bool ppm;

void setup() {
    WiFi.setPins(8,7,4,2); //correct pins for feather
    Serial.begin(9600);     // serial output for debug
    pinMode(WATPIN, INPUT);
    pinMode(TMPPIN, INPUT);
    pinMode(NEOPIN, OUTPUT);
    tapStop();
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA network, SSID: ");
      Serial.println(ssid);
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(10000);
    }
    setupDHT();
      Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

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
    io.run();
    printWifiData();
    readDHT();
    waterStatus();
    publishio();
    }
}

void tapStart() {
    // tun on water
    ppm = true;
    digitalWrite(NEOPIN, HIGH);
}

void tapStop() {
    // tun off water
    ppm = false;
    digitalWrite(NEOPIN, LOW);
}


    int waterLevel() {
        return 30; //TODO: implement measurement
    }

    int waterRaw() {
        plv = analogRead(WATPIN);
        return plv;
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
  prss = rssi;
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
    float atmf;
    atm = event.temperature;
    atmf = 9 * atm;
    atmf /= 5;
    atmf += 32;

    Serial.print("Temperature: ");
    Serial.print(atm);
    Serial.print(" *C  ");
    Serial.print(atmf);
    Serial.println(" *F");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    arh = event.relative_humidity;
    Serial.print("Humidity: ");
    Serial.print(arh);
    Serial.println("%");
  }
}

void waterStatus() {

        float reading;
        reading = analogRead(TMPPIN);
        reading = (1023 / reading)  - 1;
        reading = SERIESRESISTOR / reading;
        float steinhart;
        steinhart = reading / THERMISTORNOMINAL;
        steinhart = log(steinhart);
        steinhart /= BCOEFFICIENT;
        steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
        steinhart = 1.0 / steinhart;
        steinhart -= 273.15; 
        float steinhartf = 9 * steinhart;
        steinhartf /= 5;
        steinhartf += 32;
        ptm = steinhart;
        
        Serial.println(" ___***___ ");
        Serial.print("water level: ");
        Serial.println(waterRaw());
        Serial.print("water temp: ");
        Serial.print(steinhart);
        Serial.print(" *C  ");
        Serial.print(steinhartf);
        Serial.println(" *F");
        if (waterRaw() < limit) {
            tapStart();
        } else {
            tapStop();
        }
}

void publishio() {
  pooltemp->save(ptm);
  poolrssi->save(prss);
  poollevel->save(plv);
  airtemp->save(atm);
  airrh->save(arh);
  poolpump->save(ppm);

}
