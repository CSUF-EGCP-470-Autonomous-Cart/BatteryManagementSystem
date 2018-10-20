#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <OneWire.h>

#define ONE_WIRE_BUS_PIN 2  // Any pin 2 to 12 (not 13) and A0 to A5

Adafruit_ADS1115 ADC1(0x48);  // construct an ads1115 at address 0x48
//Adafruit_ADS1115 ADC2(0x49);  // construct an ads1115 at address 0x49

OneWire  oneWire(ONE_WIRE_BUS_PIN);  // Create a 1-wire object
DallasTemperature sensors(&oneWire);

DeviceAddress Battery1TemperatureProbe = {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE};
DeviceAddress Battery2TemperatureProbe = {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25};
//DeviceAddress Battery3TemperatureProbe = {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25};
//DeviceAddress Battery4TemperatureProbe = {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25};
//DeviceAddress Battery5TemperatureProbe = {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25};
//DeviceAddress Battery6TemperatureProbe = {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25};


void setup()
{
  Serial.begin(9600);

  sensors.begin();

  // set the resolution to 12 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Battery1TemperatureProbe, 12);
  sensors.setResolution(Battery2TemperatureProbe, 12);
  //  sensors.setResolution(Battery3TemperatureProbe, 12);
  //  sensors.setResolution(Battery4TemperatureProbe, 12);
  //  sensors.setResolution(Battery5TemperatureProbe, 12);
  //  sensors.setResolution(Battery6TemperatureProbe, 12);

  ADC1.begin();
  //ADC2.begin();


}

void loop()
{
  // Nothing happening here
}
