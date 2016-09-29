#include <Arduino.h>
#include "config.h"
#include <SPI.h>
#include <WiFi101.h>
#include <Time.h>  
#include <NTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFiUdp.h>

#define TMPPIN            A2        // Analog pin 2 for water temp
#define WATPIN            A1        // Analog pin 1 for water level
#define DHTPIN            5         // digital pin 5 for DHT
#define NEOPIN            11        // Digital pin for Neopixils
#define SILPIN            10        // Digital pin for silinoid

#define DHTTYPE           DHT22     // DHT 22 (AM2302)

#define THERMISTORNOMINAL 10000
#define SERIESRESISTOR     9980
#define TEMPERATURENOMINAL   25
#define BCOEFFICIENT       3950

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
unsigned long stupid;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org", -25200, 60000);

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const unsigned long interval = 5000;     // delay in mils for next loop
const unsigned long uploadRate = 300000; // delay before next upload
const int limit = 300;                   // water level limit
unsigned long uploadclk = 0;

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
void runServer();
void connectWiFi();
void connectIO();

char ssid[] = WIFI_SSID;       // your network SSID (name)
char pass[] = WIFI_PASS;       // your network key
int status = WL_IDLE_STATUS;   // the Wifi radio's status
WiFiServer server(80);


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
    stupid = 0;
    WiFi.setPins(8,7,4,2); //correct pins for feather
    Serial.begin(9600);     // serial output for debug
    pinMode(WATPIN, INPUT);
    pinMode(TMPPIN, INPUT);
    pinMode(NEOPIN, OUTPUT);
    pinMode(SILPIN, OUTPUT);
    tapStop();
    setupDHT();
    connectWiFi();
    //connectIO();
    server.begin();
    WiFi.maxLowPowerMode();
    timeClient.begin();
}

void connectWiFi() {
  Serial.print("Attempting to connect to WPA network, SSID: ");
  Serial.println(ssid);
  while ( status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(ssid, pass);
    delay(250);
  }
  Serial.println("connected!");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP : ");
  Serial.println(ip);
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
}

void connectIO() {
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

void loop() {
    currentMillis = millis();
    io.run();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      readDHT();
      waterStatus();
      Serial.println(stupid++);
      Serial.println(timeClient.getFormattedTime());
    }
    if (currentMillis >= uploadclk) {
      timeClient.update();
      uploadclk = currentMillis + uploadRate;
      //publishio();
    }
    runServer();
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
    atm = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    arh = event.relative_humidity;
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
  ptm = steinhart;
  plv = analogRead(WATPIN);
}

void publishio() {
  Serial.println(io.statusText());
  if (io.status() < AIO_CONNECTED) 
  {
    io.connect();
    uploadclk = currentMillis + 1000;
  }
  else {
    Serial.println("Sending Data");
    pooltemp->save(ptm);
    poolrssi->save(WiFi.RSSI());
    poollevel->save(plv);
    airtemp->save(atm);
    airrh->save(arh);
    poolpump->save(ppm);
    uploadclk = currentMillis + uploadRate;
  }
  
  // when sussfull: 
}
void runServer() {
    // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
            client.print("plv ");
            client.print(plv);
            client.println("<br />");
            client.print("ptm ");
            client.print(ptm);
            client.println("<br />");
            client.print("atm ");
            client.print(atm);
            client.println("<br />");
            client.print("arh ");
            client.print(arh);
            client.println("<br />");
            client.print("ppm ");
            client.print(ppm);
            client.println("<br />");
            client.print("prss ");
            client.print(prss);
            client.println("<br />");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    //delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
