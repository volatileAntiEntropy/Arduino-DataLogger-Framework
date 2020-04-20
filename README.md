# Arduino-DataLogger-Framework
A Data Logger Coding Framework (Interface) for Arduino and the Data Logger Shield. Proposed for UM-SJTU JI IPP Project.
## Dependencies
- <SD.h>
- <TimerOne.h> available at: https://github.com/PaulStoffregen/TimerOne
- <RTClib.h> available at: https://github.com/adafruit/RTClib
## Notice
- The name of sensor is its correspond file name, it must be less or equal to 8 characters since FAT follows 8.3 standard.
- The correspond log file is sensor's_name.log.
- SensorManager.setUpdateClock(t), t is in microseconds.
- Don't set a clock shorter then 10ms.
