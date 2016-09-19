#include <Arduino.h>

const int sensorPin= 0;
const int ledPin= 12;
int liquid_level;
int s, ticks;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  liquid_level=0;
  s=0;
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
  ticks++;
  s = analogRead(sensorPin);
  liquid_level = map(s, 258, 415, 0, 40);
  if (liquid_level < 0) { liquid_level = 0; }

  delay(1000);

  if (liquid_level > 30) {
    digitalWrite(ledPin, HIGH);
    Serial.print("Hi : ");
    Serial.print(liquid_level);
    Serial.print(" | ");
    Serial.println(s);
  }
  else {
    digitalWrite(ledPin, LOW);
    Serial.print("Low: ");
    Serial.print(liquid_level);
    Serial.print(" | ");
    Serial.println(s);
  }
}
