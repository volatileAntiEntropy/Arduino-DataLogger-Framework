#pragma once
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

void(*reboot)() = 0;

class ILogger {
public:
	RTC_DS1307& rtc;
	SDClass& storage;
	ILogger(SDClass& Sd, RTC_DS1307& RTC) :storage(Sd), rtc(RTC) {	}

	//if you want to set data update event under other conditions, here is an example
	//sensor.handle = app.onAvalable(&Serial, sensor.update);
	//put them in initialize(); and also setInterval in initialize();
	virtual void initialize() {
		if (!rtc.begin()) {
			Serial.println(F("RTC not found, rebooting..."));
			reboot();
		}
		if (!rtc.isrunning()) {
			Serial.println(F("RTC not running..."));
		}
		DateTime now = rtc.now();
		if (!storage.begin()) {
			Serial.println(F("Card Initialization Failed."));
			while (true);
		}
	}

	//DataType for a Sensor need to be native type or derived class of Printable
	template<class DataType>
	void log(String name, const DataType& data) const {
		DateTime now = rtc.now();
		File log = storage.open(name, FILE_WRITE);
		if (!log) {
			Serial.println(F("File IO Error."));
			return;
		}
		log.print(now.timestamp());
		log.print(F("$ "));
		log.println(data);
		log.close();
	}
};

namespace ArduinoDataLogger {
	RTC_DS1307 RTC;
	ILogger Logger(SD, RTC);
}