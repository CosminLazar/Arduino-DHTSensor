#include "Arduino.h"

class DHTSensorMeasurement
{
public:
	static DHTSensorMeasurement WithError();
	static DHTSensorMeasurement WithReadings(float humidity, float temperatureCelsius);
	bool HasError();
	float Humidity();
	float TemperatureInCelsius();
	float TemperatureInFahrenheit();
	float TemperatureInKelvin();
private:
	float _humidity;
	float _temperatureCelsius;
	bool _hasError;
};

class DHTSensor
{
public:
	DHTSensor(uint8_t dataPin);
	DHTSensor(uint8_t dataPin, uint8_t powerPin);
	DHTSensorMeasurement Read();

private:
	uint8_t _dataPin;
	unsigned long _samplingTimeout = 1 * 1000000;//one second

	bool BeginSensorRead();
	bool ReadSensorData(uint8_t* sensorData);
	DHTSensorMeasurement ProduceResult(uint8_t* sensorData);
	unsigned long MeasureLineHighTime();
	unsigned long MeasureLineLowTime();
	bool LineIsHigh();
	bool LineIsLow();
	bool HasTimedOut(unsigned long samplingStart);
	bool IsChecksumValid(uint8_t* dataArray);
};
