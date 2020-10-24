#pragma once

#include "Sensor.hpp"

namespace ArduinoDataLogger {
	class AnalogSensor :public Sensor<int> {
	protected:
		uint8_t pin;
	public:
		AnalogSensor(const String& name, uint8_t pin) :Sensor(name) {
			this->pin = pin;
			this->onSetup = nullptr;
			this->onUpdate = nullptr;
		}

		virtual void setup() override {
			pinMode(pin, INPUT);
		}

		virtual void update() override {
			this->current = analogRead(this->pin);
		}
	};
}
