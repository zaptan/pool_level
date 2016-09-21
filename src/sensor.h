/*
 * File:   sensor.h
 * Author: Zaptan
 *
 * Created on September 21, 2016, 12:34 PM
 *
 *  Handle all the sensors here
 *  * Water level
 *  * Water Temp
 *  * ambient Temp
 *  * ambient Humidity
 *  * Solar voltage
 */

#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
    int sensorPin;
    int rate;

public:
    Sensor(int, int);
    Sensor();
    int getLevel();
    int getRaw();
    void setRate(int);
};


#endif /* SENSOR_H */

