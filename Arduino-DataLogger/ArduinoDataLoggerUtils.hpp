#pragma once

namespace ArduinoDataLogger {
	namespace Utils {
		void(*reboot)() = 0;
		constexpr int AnalogInputMax = 1024;
		constexpr int AnalogOutputMax = 256;
	}
}