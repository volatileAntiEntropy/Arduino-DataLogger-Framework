#pragma once

#include "Sensor.hpp"

namespace ArduinoDataLogger {
	class DigitalSensor :public Sensor<int> {
	protected:
		uint8_t pin;
		int8_t mode;
	public:
		DigitalSensor(const String& name, uint8_t pin, int8_t mode = INPUT) :Sensor(name) {
			this->pin = pin;
			if (mode != INPUT && mode != INPUT_PULLUP) {
				mode = INPUT;
			}
			this->mode = mode;
			this->onSetup = nullptr;
			this->onUpdate = nullptr;
		}

		virtual void setup() override {
			pinMode(pin, this->mode);
		}

		virtual void update() override {
			this->current = digitalRead(this->pin);
		}

		StringDataPacket toStringPacket() const override {
			String value = (this->current) ? (F("HIGH")) : (F("LOW"));
			return StringDataPacket{ this->fileName(), value };
		}
	};
}
