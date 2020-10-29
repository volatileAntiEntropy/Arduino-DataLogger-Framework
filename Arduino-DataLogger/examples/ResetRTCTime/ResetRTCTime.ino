#include <ArduinoDataLoggerUtils.hpp>

using namespace ArduinoDataLogger;
using namespace ArduinoDataLogger::Default;

void setup(){
    Serial.begin(115200);
    if (!RTC.begin()) {
		Serial.println(F("RTC not found."));
		while (true);
	}
    RTC.adjust(DateTime(__DATE__,__NAME__));//Use this when no user.
    Serial.println(F("Calibrate RTC Time."));
    Serial.println(F("Input a ISO-8601 Format date string."));
    RTC.adjust(DateTime(Serial.readString().c_str()));
}

void loop(){

}