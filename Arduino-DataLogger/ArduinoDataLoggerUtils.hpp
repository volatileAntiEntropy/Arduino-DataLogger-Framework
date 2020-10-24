#pragma once

#include "SensorManager.hpp"
#include "SerialConsole.hpp"
#include "AnalogSensor.hpp"
#include <DHT.h>
#include <HX711.h>

namespace ArduinoDataLogger
{
	namespace Utils
	{
		void (*reboot)() = 0;
		constexpr int AnalogInputMax = 1023;
		constexpr int AnalogOutputMax = 255;

		class DataPacket : public Printable
		{
		public:
			virtual size_t printTo(Print &p) const
			{
				return p.print(String(*this));
			}

			virtual operator String() const = 0;
		};

		template <class T, class U>
		class Pair : public DataPacket
		{
		public:
			T first;
			U second;

			Pair(T firstElement, U secondElement) : first(firstElement), second(secondElement)
			{

			}

			Pair() = default;

			operator String() const
			{
				String result;
				result.concat(this->first);
				result.concat(F(", "));
				result.concat(this->second);
				return result;
			}
		};

		class Thermistor : public Sensor<float>
		{
		protected:
			double BConstant;
			double ReferenceTemperature;
			double ReferenceResistance;
			float LoadResistance;
			uint8_t Pin;

		public:
			Thermistor(const String &name, double B_constant, double referenceTemperature, double referenceResistance, double loadResistance, uint8_t pin)
				: Sensor<float>(name)
			{
				this->BConstant = B_constant;
				this->ReferenceTemperature = referenceTemperature;
				this->ReferenceResistance = referenceResistance;
				this->LoadResistance = loadResistance;
				this->Pin = pin;
				this->onSetup = nullptr;
				this->onUpdate = nullptr;
			}

			float getTemperatureCached() const
			{
				return this->current;
			}

			virtual void setup() override
			{
				pinMode(this->Pin, INPUT);
			}

			virtual void update() override
			{
				this->current = analogRead(this->Pin) / (float)(AnalogInputMax);
				this->current = (this->current / (1 - this->current)) * (this->LoadResistance);
				this->current = (log(this->current / this->ReferenceResistance) / this->BConstant) + 1 / (this->ReferenceTemperature + 273.15);
				this->current = 1 / this->current;
				this->current -= 273.15;
			}
		};

		class DHTSensor : public Sensor<Pair<float, float>>
		{
		public:
			DHT DHTObject;

			DHTSensor(const String &name, uint8_t pin, int type) : DHTObject(pin, type), Sensor<Pair<float, float>>(name)
			{
				this->onSetup = nullptr;
				this->onUpdate = nullptr;
			}

			virtual void setup() override
			{
				this->DHTObject.begin();
			}

			virtual void update() override
			{
				this->current.first = this->DHTObject.readTemperature();
				this->current.second = this->DHTObject.readHumidity();
			}

			Pair<float, float> getTemperatureAndHumidityCached() const
			{
				return this->current;
			}
		};

		class LoadCell : public Sensor<float> {
		protected:
			uint8_t signalPin;
			uint8_t SCKPin;
			uint8_t gain;
		public:
			HX711 LoadCellObject;

			LoadCell(const String& name, uint8_t pinSIG, uint8_t pinSCK, uint8_t chipGain = 128) :Sensor<float>(name), LoadCellObject() {
				this->signalPin = pinSIG;
				this->SCKPin = pinSCK;
				this->gain = chipGain;
				this->onSetup = nullptr;
				this->onUpdate = nullptr;
			}

			virtual void setup() override {
				this->LoadCellObject.begin(this->signalPin, this->SCKPin, this->gain);
				this->LoadCellObject.tare();
			}

			virtual void update() override {
				if (this->LoadCellObject.is_ready()) {
					this->current = this->LoadCellObject.get_value();
				}
			}

			float getScaleReadingCached() const {
				return this->current;
			}
		};

	} // namespace Utils
} // namespace ArduinoDataLogger
