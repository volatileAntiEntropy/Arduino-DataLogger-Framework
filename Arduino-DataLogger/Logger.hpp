#pragma once
#include <Arduino.h>
#include "StringDataPacket.hpp"
#include <SD.h>
#include <RTClib.h>

namespace ArduinoDataLogger {

	typedef enum {
		TIMESTAMP,
		MILLISECONDS,
		TIMESPAN
	}TimeLogOption;

	class ILogger {
	public:
		DateTime beginning;
		RTC_DS1307& rtc;
		SDClass& storage;
		ILogger(SDClass& Sd, RTC_DS1307& RTC) :storage(Sd), rtc(RTC), beginning() {	}

		//if you want to set data update event under other conditions, here is an example
		//sensor.handle = app.onAvailable(&Serial, sensor.update);
		//put them in initialize(); and also setInterval in initialize();
		virtual boolean initialize() {
			if (!rtc.begin()) {
				Serial.println(F("RTC not found."));
				while (true);
			}
			if (!storage.begin()) {
				Serial.println(F("Card Initialization Failed."));
				while (true);
			}
			this->beginning = rtc.now();
		}

		void log(const StringDataPacket& data, TimeLogOption opt = MILLISECONDS) const {
			File log = storage.open(data.fileName, FILE_WRITE);
			if (!log) {
				Serial.println(F("File IO Error."));
				return;
			}
			switch (opt)
			{
			case TIMESTAMP:
				if (!rtc.isrunning()) {
					Serial.println(F("RTC not running..."));
					while (true);
				}
				log.print(rtc.now().timestamp());
				break;
      case MILLISECONDS:
        log.print(millis());
        break;
			case TIMESPAN:
				if (!rtc.isrunning()) {
					Serial.println(F("RTC not running..."));
					while (true);
				}
				TimeSpan interval = rtc.now() - this->beginning;
				log.print(interval.days());
				log.print(F(":"));
				log.print(interval.hours());
				log.print(F(":"));
				log.print(interval.minutes());
				log.print(F(":"));
				log.print(interval.seconds());
				break;
			default:
        Serial.println(F("No Such Option."));
				break;
			}
			log.print(F(","));
			log.println(data.measuredValue);
			log.close();
		}
	};

	namespace Default {
		RTC_DS1307 RTC;
		ILogger Logger(SD, RTC);
	}
}
