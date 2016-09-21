#include <Arduino.h>
//#include "sensor.h"

class Sensor {
    int sensorPin;
    int rate;

public:

    Sensor(int pin, int nrate) {
        sensorPin = pin;
        rate = nrate;
    }
        Sensor() {
        sensorPin = 0;
        rate = 40;
    }

    int getLevel() {
        return 0; //TODO: implement measurement
    }

    int getRaw() {
        return analogRead(sensorPin);
    }

    void setRate(int nrate) {
        rate = nrate;
    }
};
