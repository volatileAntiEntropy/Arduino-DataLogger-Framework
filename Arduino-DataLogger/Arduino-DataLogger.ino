/*
 Name:		Arduino_DataLogger.ino
 Created:	2020/4/19 17:56:51
 Author:	qinbi
*/
#include "Sensor.hpp"
#include <math.h>
using namespace ArduinoDataLogger;
SensorManagerSingleton<1> SensorManager(Logger);
Sensor<float>* Thermistor = nullptr;
constexpr float ref_temp_inverse = 1 / (25 + 273.15);
// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    SensorManager.initialize();
    //Name should be less than 8 characters
    Thermistor = SensorManager.build<float>(F("Therm"));
    Thermistor->setup = []() {
        pinMode(A0, INPUT);
    };
    Thermistor->onUpdate = []() {
        double result = analogRead(A0) / (float)(AnalogInputMax);
        result = (result / (1 - result)) * 5110;
        result = (log(result / 10000) / 3434) + ref_temp_inverse;
        result = 1 / result;
        result -= 273.15;
        return (float)result;
    };
    SensorManager.setup();
    //In Microseconds
    SensorManager.setUpdateClock(1000000);
    Serial.println(F("Initialization Finished."));
}

// the loop function runs over and over again until power down or reset
void loop() {
    SensorManager.updateOnClockInterrupt(SerialOnly);
    yield();
}

void serialEvent() {
    String command = Serial.readString();
    if (command == F("stop")) {
        SensorManager.stopUpdateClock();
        Serial.println(F("Sensors Stopped."));
    }
    else if (command == F("start")) {
        SensorManager.startUpdateClock();
        Serial.println(F("Sensors Started."));
    }
}