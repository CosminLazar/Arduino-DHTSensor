# Arduino-DHTSensor
Arduino library for communicating with the DHT22/AM2302 Digital Temperature And Humidity Sensor without using a pull up resistor. In theory, this should also work for the DHT11 sensor, but I haven't tried it yet.

_You can support the author by buying the sensor on [ebay](https://rover.ebay.com/rover/1/711-53200-19255-0/1?icep_id=114&ipn=icep&toolid=20004&campid=5338101636&mpre=http%3A%2F%2Fwww.ebay.com%2Fitm%2FDHT22-AM2302-Digital-Temperature-And-Humidity-Sensor-Replace-SHT11-SHT15-%2F381605063274%3Fhash%3Ditem58d96f2a6a%3Ag%3AEYwAAOSwajVUNpxD)._

## Setup
Because there is no requirement for a pull up resistor setting up the sensor is very easy and it only requires connecting the sensor to your board via 3 jumper cables.

Remember
* pin1 = VDD
* pin2 = data
* pin3 = not used
* pin4 =  GND

## Usage

1. Copy the contents of this repository to your arduino libraries directory in a subdirectory named DHTSensor
2. Restart the IDE
3. Profit

```c
#include <DHTSensor.h>
DHTSensor sensor(3);//Will be using pin3 for collecting the data
void loop()
{
		DHTSensorMeasurement re = sensor.Read();
		if (!re.HasError())
		{
			Serial.print("H: ");
			Serial.print(re.Humidity());
			Serial.print(" T: ");
			Serial.print(re.TemperatureInCelsius());
			Serial.print(" T(F): ");
			Serial.print(re.TemperatureInFahrenheit());
			Serial.print(" T(K): ");			
			Serial.println(re.TemperatureInKelvin());
		}
}
```
If for some reason you want to power the sensor via a digital pin, you can just use another constructor
```c
DHTSensor sensor(3, 7);//Will be using pin3 for collecting the data and pin7 for powering up the sensor
```

## Disclaimer
There are several implementation of the DHT22 protocol out there but I decided to write my own so that I wipe the dust from my long forgotten C++ skills. I hope you will find this useful/educational.

## Literature
In the literature directory you can find the full sensor specification sheet.
