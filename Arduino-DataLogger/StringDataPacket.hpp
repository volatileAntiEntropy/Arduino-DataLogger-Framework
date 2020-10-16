#pragma once

#include <Arduino.h>

namespace ArduinoDataLogger {
	class StringDataPacket {
	public:
		String fileName;
		String measuredValue;
	};
}