#include <ArduinoDataLoggerUtils.hpp>

using namespace ArduinoDataLogger::Utils;

float calibration_factor = 1.0f;
LoadCell loadCell("Scale", 6, 5, 128, calibration_factor);

void setup(){
    Serial.begin(115200);
    loadCell.setup();
    Serial.println(F("HX711 Calibration"));
    Serial.println(F("Remove all weight from scale"));
    Serial.println(F("After readings begin, place known weight on scale"));
    Serial.println(F("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively"));
    Serial.println(F("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively"));
    Serial.println(F("Press t for tare"));
}

void loop(){
    loadCell.LoadCellObject.set_scale(calibration_factor); //Adjust to this calibration factor
    Serial.print(F("Reading: "));
    Serial.print(scale.get_units(), 3);
    Serial.print(F(" kg")); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    Serial.print(F(" calibration_factor: "));
    Serial.print(calibration_factor);
    Serial.println();
}

void serialEvent(){
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == 's')
      calibration_factor += 100;  
    else if(temp == 'x')
      calibration_factor -= 100;  
    else if(temp == 'd')
      calibration_factor += 1000;  
    else if(temp == 'c')
      calibration_factor -= 1000;
    else if(temp == 'f')
      calibration_factor += 10000;  
    else if(temp == 'v')
      calibration_factor -= 10000;  
    else if(temp == 't')
      scale.tare();  //Reset the scale to zero
}