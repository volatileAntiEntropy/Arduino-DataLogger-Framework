/*
 Name:		Arduino_DataLogger.ino
 Created:	2020/4/19 17:56:51
 Author:	qinbi
*/
#include "ArduinoDataLoggerUtils.hpp"

using namespace ArduinoDataLogger;
using namespace ArduinoDataLogger::Default;
using namespace ArduinoDataLogger::Utils;

constexpr uint8_t MAX_SENSOR_NUM = 3U;
SensorManagerClass<MAX_SENSOR_NUM> SensorManager(Logger);
SerialConsole<MAX_SENSOR_NUM> Console(SensorManager);

//Name should be less than 8 characters
//Thermistor thermistor("Therm", 3434, 25, 10000, 5110, A0);
DHTSensor sensorDHT("DHT_11", 2, DHT11);
AnalogSensor groundMoisture("GND_H", A1);
LoadCell loadCell("Scale", 6, 5, 128, -100000);

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);
  //SensorManager.push(thermistor);
  SensorManager.push(sensorDHT);
  SensorManager.push(groundMoisture);
  SensorManager.push(loadCell);
  SensorManager.initialize();
  SensorManager.setup();
  //In Microseconds
  SensorManager.setUpdateClock(2000000);
  Serial.println(F("Initialization finished."));
  SensorManager.startUpdateClock();
}

// the loop function runs over and over again until power down or reset
void loop()
{
	SensorManager.updateOnClockInterrupt(LogAndSerial,MILLISECONDS);
	yield();
}

void serialEvent()
{
  Console.processCommand(Serial.readString());
}
