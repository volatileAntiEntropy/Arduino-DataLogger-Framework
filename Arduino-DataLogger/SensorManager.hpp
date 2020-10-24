#pragma once

#include "Sensor.hpp"
#include "Logger.hpp"
#include <TimerOne.h>

namespace ArduinoDataLogger 
{
	typedef enum {
		LogOnly,
		SerialOnly,
		LogAndSerial,
		DoNothing
	}UpdateOption;

	template<uint8_t SIZE>
	class SensorManagerClass {
	protected:
		ILogger& Logger;
		uint8_t InstanceCounter;
		ISensor* SensorCollection[SIZE];
	public:
		static volatile boolean shouldUpdate;

		SensorManagerClass(ILogger& logger) :Logger(logger) {
			InstanceCounter = 0;
		};

		ISensor* getSensor(const String& name) {
			for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
				if (SensorCollection[i]->nameNoExtension() == name) {
					return SensorCollection[i];
				}
				return nullptr;
			}
		}

		template<class T>
		Sensor<T>* build(const String& name) {
			if (InstanceCounter >= SIZE) {
				return nullptr;
			}
			else {
				Sensor<T>* ptr = new Sensor<T>(name);
				SensorCollection[InstanceCounter] = static_cast<ISensor*>(ptr);
				InstanceCounter++;
				return (ptr);
			}
		}

		template<class T>
		Sensor<T>* build(const __FlashStringHelper* name) {
			if (InstanceCounter >= SIZE) {
				return nullptr;
			}
			else {
				Sensor<T>* ptr = new Sensor<T>(name);
				SensorCollection[InstanceCounter] = static_cast<ISensor*>(ptr);
				InstanceCounter++;
				return (ptr);
			}
		}

		template<class T>
		Sensor<T>* build(const String& name, void(*setupFunction)(void), T(*updateFunction)(void)) {
			if (InstanceCounter >= SIZE) {
				return nullptr;
			}
			else {
				Sensor<T>* ptr = new Sensor<T>(name, setupFunction, updateFunction);
				SensorCollection[InstanceCounter] = static_cast<ISensor*>(ptr);
				InstanceCounter++;
				return (ptr);
			}
		}

		template<class T>
		Sensor<T>* build(const __FlashStringHelper* name, void(*setupFunction)(void), T(*updateFunction)(void))
		{
			if (InstanceCounter >= SIZE) {
				return nullptr;
			}
			else {
				Sensor<T>* ptr = new Sensor<T>(name, setupFunction, updateFunction);
				SensorCollection[InstanceCounter] = static_cast<ISensor*>(ptr);
				InstanceCounter++;
				return (ptr);
			}
		}

		template<class T>
		void push(Sensor<T>& sensor) {
			if (InstanceCounter < SIZE) {
				SensorCollection[InstanceCounter] = static_cast<ISensor*>(&sensor);
				InstanceCounter++;
			}
		}

		void initialize() {
			Logger.initialize();
			for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
				if (Logger.storage.exists(SensorCollection[i]->fileName())) {
					Logger.storage.remove(SensorCollection[i]->fileName());
				}
			}
			Serial.println(F("time value"));
		}

		void setup() const {
			for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
				SensorCollection[i]->setup();
			}
		}

		void setUpdateClock(unsigned long interval) const {
			Timer1.initialize(interval);
		}

		void update() const {
			for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
				if (SensorCollection[i]->isTurnedOn()) {
					SensorCollection[i]->update();
				}
			}
		}

		void log() const {
			for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
				if (SensorCollection[i]->isTurnedOn()) {
					Logger.log(SensorCollection[i]->toStringPacket());
				}
			}
		}

		void updateOnClockInterrupt(UpdateOption opt = LogOnly) const {
			if (shouldUpdate) {
				this->update();
				switch (opt) {
				case LogOnly:
					this->log();
					break;
				case SerialOnly:
					for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
						if (SensorCollection[i]->isTurnedOn()) {
							Serial.print(*(SensorCollection[i]));
							Serial.println();
						}
					}
					Serial.println(F("=========="));
					break;
				case LogAndSerial:
					this->log();
					for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
						if (SensorCollection[i]->isTurnedOn()) {
							Serial.print(*(SensorCollection[i]));
							Serial.println();
						}
					}
					Serial.println(F("=========="));
					break;
				case DoNothing:
					break;
				}
				shouldUpdate = false;
			}
		}

		void stopUpdateClock() const {
			Timer1.stop();
			shouldUpdate = false;
		}

		void startUpdateClock() const {
			Timer1.attachInterrupt([]() {
				shouldUpdate = true;
				});
			Timer1.resume();
		}

		void resumeUpdateClock() const {
			Timer1.resume();
		}

		constexpr uint8_t capacity() const { return SIZE; }

		inline uint8_t length() const { return InstanceCounter; }
	};

	template<uint8_t SIZE>
	volatile boolean SensorManagerClass<SIZE>::shouldUpdate = false;
}
