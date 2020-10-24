#pragma once
#include <Arduino.h>
#include "StringDataPacket.hpp"

namespace ArduinoDataLogger
{

	class ISensor : public Printable
	{
	protected:
		String name;

	public:
		ISensor(String Name)
		{
			if (Name.length() > 8)
			{
				Name = Name.substring(0, 8);
			}
			name = Name + F(".log");
		}

		inline String nameNoExtension() const
		{
			return name.substring(0, name.lastIndexOf('.'));
		}

		inline String fileName() const
		{
			return name;
		}

		virtual void setup() = 0;

		virtual void update() = 0;

		virtual bool isTurnedOn() const = 0;

		virtual void turnOn() = 0;

		virtual void turnOff() = 0;

		virtual StringDataPacket toStringPacket() const = 0;
	};

	template <class DataType>
	class Sensor : public ISensor
	{
	protected:
		DataType current;
		bool isWorking;

	public:
		using voidCallback = void (*)(void);
		using updateCallback = DataType (*)(void);

		voidCallback onSetup;
		updateCallback onUpdate;

		Sensor(const String &Name) : ISensor(Name), isWorking(true) {}

		Sensor(const String &Name, voidCallback setupFunction, updateCallback updateFunction) : ISensor(Name), onSetup(setupFunction), onUpdate(updateFunction), isWorking(true) {}

		inline DataType getCurrentData() const
		{
			return current;
		}

		virtual void setup() override
		{
			(this->onSetup)();
		}

		virtual void update() override
		{
			this->current = (this->onUpdate)();
		}

		virtual bool isTurnedOn() const override
		{
			return this->isWorking;
		}

		virtual void turnOn() override
		{
			this->isWorking = true;
		}

		virtual void turnOff() override
		{
			this->isWorking = false;
		}

		virtual StringDataPacket toStringPacket() const override
		{
			return StringDataPacket{this->fileName(), String(this->current)};
		}

		virtual size_t printTo(Print &p) const override
		{
			return p.print(this->nameNoExtension()) + p.print(F(": ")) + p.print(this->current);
		}
	};
} // namespace ArduinoDataLogger