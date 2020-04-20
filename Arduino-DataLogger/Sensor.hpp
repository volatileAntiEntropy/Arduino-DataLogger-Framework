#pragma once
#include <Arduino.h>
#include "Logger.hpp"
#include <TimerOne.h>

constexpr int AnalogInputMax = 1024;
constexpr int AnalogOutputMax = 256;

class ISensor: public Printable {
protected:
	String name;
public:
	using callback = void(*)(void);
	callback setup;

	ISensor(String Name) {
		if (Name.length() > 8) {
			Name = Name.substring(0, 8);
		}
		name = Name + F(".log");
	}

	inline String getName() const {
		return name.substring(0, name.lastIndexOf('.'));
	}

	inline String getFileName() const {
		return name;
	}

	virtual void update() = 0;

	virtual void printLogTo(const ILogger&) const = 0;
};

template<class DataType>
class Sensor :public ISensor {
protected:
	DataType current;
public:
	using update_callback = DataType(*)(void);
	update_callback onUpdate;

	Sensor(String Name) :ISensor(Name) {  }

	inline DataType getCurrentData() const {
		return current;
	}

	virtual void update() override {
		this->current = (this->onUpdate)();
	}

	virtual void printLogTo(const ILogger& logger) const override {
		logger.log(this->name, this->current);
	}

	virtual size_t printTo(Print& p) const override {
		return p.print(this->current);
	}
};

typedef enum {
	LogOnly,
	SerialOnly,
	LogAndSerial,
	DoNothing
}UpdateOption;

template<uint8_t SIZE>
class SensorManagerSingleton {
protected:
	ILogger& Logger;
	uint8_t InstanceCounter;
	ISensor* SensorCollection[SIZE];
public:
	static volatile boolean shouldUpdate;

	SensorManagerSingleton(ILogger& logger) :Logger(logger) {
		InstanceCounter = 0;
	};

	template<class T>
	Sensor<T>* build(String name) {
		if (InstanceCounter >= SIZE) {
			return static_cast<Sensor<T>*>(SensorCollection[SIZE - 1]);
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
			return static_cast<Sensor<T>*>(SensorCollection[SIZE - 1]);
		}
		else {
			Sensor<T>* ptr = new Sensor<T>(name);
			SensorCollection[InstanceCounter] = static_cast<ISensor*>(ptr);
			InstanceCounter++;
			return (ptr);
		}
	}

	void initialize() {
		Logger.initialize();
		for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
			if (Logger.storage.exists(SensorCollection[i]->getFileName())) {
				Logger.storage.remove(SensorCollection[i]->getFileName());
			}
		}
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
			SensorCollection[i]->update();
		}
	}

	void log() const {
		for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
			SensorCollection[i]->printLogTo(Logger);
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
					Serial.print(*SensorCollection[i]);
					Serial.print(F(" "));
				}
				Serial.println();
				break;
			case LogAndSerial:
				this->log();
				for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
					Serial.print(*SensorCollection[i]);
					Serial.print(F(" "));
				}
				Serial.println();
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
volatile boolean SensorManagerSingleton<SIZE>::shouldUpdate = false;