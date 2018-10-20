#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <OneWire.h>

#define ONE_WIRE_BUS_PIN 2  // Any pin 2 to 12 (not 13) and A0 to A5
#define CURRENT_SENSOR_PIN 0

Adafruit_ADS1115 ads1(0x48);
Adafruit_ADS1115 ads2(0x49);

OneWire  oneWire(ONE_WIRE_BUS_PIN);  // Create a 1-wire object
DallasTemperature sensors(&oneWire);

DeviceAddress Battery1TemperatureProbe = {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25};
DeviceAddress Battery2TemperatureProbe = {0x28, 0xDC, 0x6D, 0x45, 0x92, 0x0B, 0x02, 0xC3};
DeviceAddress Battery3TemperatureProbe = {0x28, 0xAA, 0xD6, 0xB8, 0x13, 0x13, 0x02, 0x69};
DeviceAddress Battery4TemperatureProbe = {0x28, 0xAA, 0x39, 0xDF, 0x12, 0x13, 0x02, 0x61};
DeviceAddress Battery5TemperatureProbe = {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE};
DeviceAddress Battery6TemperatureProbe = {0x28, 0x5B, 0x3D, 0xDD, 0x1B, 0x13, 0x01, 0x50};

const double CELL_MULTIPLIERS[] = {
  1.95643,
  1.90253,
  0.0,
  0.0,
  0.0,
  0.0
};

void setup()
{
  Serial.begin(115200);

  sensors.begin();

  // set the resolution to 12 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Battery1TemperatureProbe, 12);
  sensors.setResolution(Battery2TemperatureProbe, 12);
  sensors.setResolution(Battery3TemperatureProbe, 12);
  sensors.setResolution(Battery4TemperatureProbe, 12);
  sensors.setResolution(Battery5TemperatureProbe, 12);
  sensors.setResolution(Battery6TemperatureProbe, 12);


  ads1.begin();
  ads2.begin();

}

void loop()
{
  Serial.print(GetCurrentDraw());
  Serial.print("\t");

  Serial.print(GetCellVoltage(0));
  Serial.print("\t");

  Serial.println(GetCellVoltage(1));
}

double GetCellVoltage(uint8_t Index) {
  double CellVoltage = 0;
  if (Index == 0) {
    CellVoltage = GetBankVoltageAtIndex(Index);
  }
  else if (Index > 0 && Index <= 5) {
    CellVoltage = GetBankVoltageAtIndex(Index) - GetBankVoltageAtIndex(Index - 1);
  }
  else {
    CellVoltage = -1.0;
  }

  return CellVoltage;
}

double GetBankVoltageAtIndex(uint8_t Index) {
  double BankVoltage = 0;

  int ads_index = Index;
  if (Index >= 0 && Index <= 3) {
    BankVoltage = GetBankVoltageAtIndex(&ads1, Index, CELL_MULTIPLIERS[Index]);
  }
  else if (Index > 3 && Index <= 5) {
    BankVoltage = GetBankVoltageAtIndex(&ads2, Index - 4, CELL_MULTIPLIERS[Index]);
  }
  else {
    BankVoltage = -1.0;
  }

  return BankVoltage;
}

double GetBankVoltageAtIndex(Adafruit_ADS1115 ads, uint8_t Index, double Multiplier) {
  int16_t AnalogValue = ads.readADC_SingleEnded(Index);
  double AnalogVoltage = (AnalogValue * 0.1875) / 1000;
  return AnalogValue * Multiplier;
}

double GetCurrentDraw() {
  uint32_t analog = analogRead(CURRENT_SENSOR_PIN);
  double current = map(analog, 0, 1023, 0, 1000);
  return current;
}
