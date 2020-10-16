/*
 Name:		Arduino_DataLogger.ino
 Created:	2020/4/19 17:56:51
 Author:	qinbi
*/
#include "SensorManager.hpp"
#include "SerialConsole.hpp"
#include "ArduinoDataLoggerUtils.hpp"

using namespace ArduinoDataLogger;
using namespace ArduinoDataLogger::Default;
using namespace ArduinoDataLogger::Utils;

SensorManagerClass<1> SensorManager(Logger);
SerialConsole<1> Console(SensorManager);
Sensor<float>* Thermistor = nullptr;
constexpr float ref_temp_inverse = 1 / (25 + 273.15);

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    //Name should be less than 8 characters
    Thermistor = SensorManager.build<float>(F("Therm"));
    Thermistor->onSetup = []() {
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
    SensorManager.initialize();
    SensorManager.setup();
    //In Microseconds
    SensorManager.setUpdateClock(100000);
    Serial.println(F("Initialization finished."));
    SensorManager.startUpdateClock();
}

// the loop function runs over and over again until power down or reset
void loop() {
    SensorManager.updateOnClockInterrupt(LogOnly);
    yield();
}

void serialEvent() {
    Console.processCommand(Serial.readString());
}