#include <Arduino.h>

const int sensorPin = 0;
const int ledPin = 12;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 1000;
const int limit = 300;

void turnOnWater();
void turnOffWater();


void setup() {
    Serial.begin(9600);
    pinMode(sensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
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
            turnOnWater();
        } else {
            turnOffWater();
        }
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
