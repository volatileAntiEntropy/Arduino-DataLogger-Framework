#pragma once

#include "SensorManager.hpp"

namespace ArduinoDataLogger {
	template<uint8_t N>
	class SerialConsole {
	private:
		SensorManagerClass<N>& Manager;
	public:
		SerialConsole(SensorManagerClass<N>& manager) :Manager(manager) {  }

		void turnOn(const String& name) {
			ISensor* pSensor = this->Manager.getSensor(name);
			if (pSensor) {
				pSensor->turnOn();
				Serial.print(F("Sensor "));
				Serial.print(name);
				Serial.println(F(" turned on."));
			}
			else {
				Serial.println(F("Sensor not found!"));
			}
		}

		void turnOff(const String& name) {
			ISensor* pSensor = this->Manager.getSensor(name);
			if (pSensor) {
				pSensor->turnOff();
				Serial.print(F("Sensor "));
				Serial.print(name);
				Serial.println(F(" turned off."));
			}
			else {
				Serial.println(F("Sensor not found!"));
			}
		}

		void stop() {
			this->Manager.stopUpdateClock();
			Serial.println(F("Sensors stopped."));
		}

		void start() {
			this->Manager.startUpdateClock();
			Serial.println(F("Sensors started."));
		}

		void processCommand(const String& command) {
			if (command == F("stop")) {
				this->stop();
			}
			else if (command == F("start")) {
				this->start();
			}
			else if (command.startsWith(F("turnOn "))) {
				this->turnOn(command.substring(7));
			}
			else if (command.startsWith(F("turnOff"))) {
				this->turnOff(command.substring(7));
			}
			else {
				Serial.println(F("Command not found!"));
			}
		}
	};
}