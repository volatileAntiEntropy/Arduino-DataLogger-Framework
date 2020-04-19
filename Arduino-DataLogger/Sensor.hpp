#pragma once
#include <Arduino.h>
#include "Logger.hpp"
#include <TimerOne.h>

#ifndef SENSOR_COUNT
#define SENSOR_COUNT 3
#endif

constexpr int AnalogInputMax = 1024;
constexpr int AnalogOutputMax = 256;

class ISensor {
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

	virtual void update() = 0;

	virtual void printLogTo(const ILogger&) const = 0;
};

template<class DataType>
class Sensor :public ISensor {
protected:
	volatile DataType current;
public:
	using update_callback = DataType(*)(void);
	update_callback onUpdate;

	Sensor(String Name) :ISensor(Name) {  }

	inline volatile DataType getCurrentData() const {
		return current;
	}

	virtual void update() override {
		this->current = (this->onUpdate)();
	}

	virtual void printLogTo(const ILogger& logger) const override {
		logger.log(this->name, this->current);
	}
};

extern ILogger Logger;

template<uint8_t SIZE>
class SensorManagerSingleton {
protected:
	static uint8_t InstanceCounter;
	static ISensor* SensorCollection[SIZE];
public:
	static volatile boolean shouldUpdate;

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

	void setup() const {
		for (uint8_t i = 0; i < SIZE && SensorCollection[i] != nullptr; i++) {
			SensorCollection[i]->setup();
		}
	}

	void setUpdateClock(unsigned long interval) const {
		Timer1.initialize(interval);
	}

	void updateOnClockInterrupt() const {
		if (shouldUpdate) {
			this->update();
			this->log();
			shouldUpdate = false;
		}
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
uint8_t SensorManagerSingleton<SIZE>::InstanceCounter = 0;
template<uint8_t SIZE>
ISensor* SensorManagerSingleton<SIZE>::SensorCollection[SIZE];
template<uint8_t SIZE>
volatile boolean SensorManagerSingleton<SIZE>::shouldUpdate = false;

SensorManagerSingleton<SENSOR_COUNT> SensorManager;

class DataTypeBase :protected Printable {
public:
	virtual DataTypeBase& operator=(DataTypeBase&) = 0;
};