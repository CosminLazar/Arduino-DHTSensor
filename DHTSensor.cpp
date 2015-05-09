#include "DHTSensor.h"
#include "Arduino.h"

DHTSensor::DHTSensor(uint8_t dataPin){
	_dataPin = dataPin;

	pinMode(_dataPin, INPUT_PULLUP);
}

DHTSensor::DHTSensor(uint8_t dataPin, uint8_t powerPin) :DHTSensor(dataPin){
	//power up powerPin	
	pinMode(powerPin, OUTPUT);
	digitalWrite(powerPin, HIGH);
}

DHTSensorMeasurement DHTSensor::Read(){
	if (!BeginSensorRead()){
		//the sensor did not replied
		return DHTSensorMeasurement::WithError();
	}

	uint8_t sensorData[5] = {};

	if (!ReadSensorData(sensorData))
	{
		return DHTSensorMeasurement::WithError();
	}

	//verify checksum
	if (!IsChecksumValid(sensorData))
	{
		return DHTSensorMeasurement::WithError();
	}

	//end of measurement
	return ProduceResult(sensorData);
}


DHTSensorMeasurement DHTSensor::ProduceResult(uint8_t* sensorData){
	float humidity = ((((uint16_t)sensorData[0]) << 8) | sensorData[1]) * 0.1;

	float temperature = sensorData[2] >> 7 ? -1 : 1;//check negative temperature
	temperature *= ((((uint16_t)(sensorData[2] & 0x7f)) << 8) | sensorData[3]) * 0.1;

	return DHTSensorMeasurement::WithReadings(humidity, temperature);
}

bool DHTSensor::ReadSensorData(uint8_t* sensorData){
	for (int i = 0; i < 40; i++)
	{
		if (MeasureLineLowTime() >= _samplingTimeout)
		{
			return false;
		}

		unsigned long bit = MeasureLineHighTime();

		if (bit >= _samplingTimeout)
		{
			//measurement timed out			
			return false;
		}

		uint8_t* targetByte = &sensorData[i / 8];
		uint8_t bitValue = bit < 36 ? 0 : 1;
		*targetByte = ((*targetByte << 1) | bitValue);
	}

	return true;
}

bool DHTSensor::IsChecksumValid(uint8_t* dataArray){
	uint8_t expectedValue = dataArray[4];
	uint8_t actualValue = 0;

	for (uint8_t i = 0; i < 4; i++)
	{
		actualValue += dataArray[i];
	}
	//only the last 8 bits should be taken into consideration
	actualValue &= 0xff;

	return expectedValue == actualValue;
}

bool DHTSensor::BeginSensorRead(){
	pinMode(_dataPin, OUTPUT);

	//1. Request sensor reading
	//1.0 pull low min 1 ms
	digitalWrite(_dataPin, LOW);
	delay(18);

	//1.1 pull high 20-40 us
	pinMode(_dataPin, INPUT_PULLUP);


	//2. The sensor acknowledges the request
	//2.0 wait for the sensor to begin acknowledging the request by pulling low
	if (MeasureLineHighTime() > _samplingTimeout){
		return false;
	}

	//2.1 the sensor pulls the line low for 80us
	if (MeasureLineLowTime() > _samplingTimeout){
		return false;
	}

	//2.2 the sensor pulls the line high for 80us
	if (MeasureLineHighTime() > _samplingTimeout){
		return false;
	}

	//3. Acknowledgment phase finished successfully
	return true;
}

unsigned long DHTSensor::MeasureLineHighTime(){
	unsigned long samplingStart = micros();
	while (LineIsHigh() && (HasTimedOut(samplingStart) == false));

	return micros() - samplingStart;
}

unsigned long DHTSensor::MeasureLineLowTime(){
	unsigned long samplingStart = micros();
	while (LineIsLow() && (HasTimedOut(samplingStart) == false));

	return micros() - samplingStart;
}

bool DHTSensor::HasTimedOut(unsigned long samplingStart){
	//since we are working with unsigned long we will not be affected by micros() overflowing
	return (micros() - samplingStart) > _samplingTimeout;
}

bool DHTSensor::LineIsHigh(){
	return digitalRead(_dataPin) == HIGH;
}

bool DHTSensor::LineIsLow(){
	return LineIsHigh() == false;
}



DHTSensorMeasurement DHTSensorMeasurement::WithError(){
	DHTSensorMeasurement result;
	result._hasError = true;
	return result;
}

DHTSensorMeasurement DHTSensorMeasurement::WithReadings(float humidity, float temperatureCelsius){
	DHTSensorMeasurement result;
	result._hasError = false;
	result._humidity = humidity;
	result._temperatureCelsius = temperatureCelsius;
	return result;
}

bool DHTSensorMeasurement::HasError(){
	return _hasError;
}

float DHTSensorMeasurement::Humidity(){
	return _humidity;
}

float DHTSensorMeasurement::TemperatureInCelsius(){
	return _temperatureCelsius;
}

float DHTSensorMeasurement::TemperatureInFahrenheit(){
	return _temperatureCelsius * 1.8 + 32;
}

float DHTSensorMeasurement::TemperatureInKelvin(){
	return _temperatureCelsius + 273.15;
}