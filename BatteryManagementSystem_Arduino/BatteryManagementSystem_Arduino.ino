/*

  Cell 0    Black     {0x28, 0xAA, 0x39, 0xDF, 0x12, 0x13, 0x02, 0x61}
  Cell 1    White     {0x28, 0xAA, 0xD6, 0xB8, 0x13, 0x13, 0x02, 0x69}
  Cell 2    Blue      {0x28, 0xDC, 0x6D, 0x45, 0x92, 0x0B, 0x02, 0xC3}
  Cell 3    Green     {0x28, 0x5B, 0x3D, 0xDD, 0x1B, 0x13, 0x01, 0x50}
  Cell 4    Red       {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE}
  Cell 5    Yellow    {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25}

*/
#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_PIN 2  // Any pin 2 to 12 (not 13) and A0 to A5
#define CURRENT_SENSOR_PIN 0

//Precision of temperature sensors in number of bits (1 - 12)
#define TEMPERATURE_PRECISION 12

//How often the temoerature probes are polled
const uint8_t TEMPERATURE_REFRESH_RATE = 10; //Hz


Adafruit_ADS1115 ads1(0x48);
Adafruit_ADS1115 ads2(0x49);

OneWire  oneWire(ONE_WIRE_BUS_PIN);  // Create a 1-wire object
DallasTemperature sensors(&oneWire);

const uint8_t TEMP_PROBE_COUNT = 6;
const uint8_t TEMP_PROBE_ADDRESSES[][8] = {
  {0x28, 0xAA, 0x39, 0xDF, 0x12, 0x13, 0x02, 0x61},
  {0x28, 0xAA, 0xD6, 0xB8, 0x13, 0x13, 0x02, 0x69},
  {0x28, 0xDC, 0x6D, 0x45, 0x92, 0x0B, 0x02, 0xC3},
  {0x28, 0x5B, 0x3D, 0xDD, 0x1B, 0x13, 0x01, 0x50},
  {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE},
  {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25}
};

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
  sensors.setWaitForConversion(false);  // makes it async

  // set the resolution to 12 bit (Can be 9 to 12 bits .. lower is faster)
  for (int i = 0; i < TEMP_PROBE_COUNT; i++) {
    sensors.setResolution(TEMP_PROBE_ADDRESSES[i], TEMPERATURE_PRECISION);
  }

  ads1.begin();
  ads2.begin();

}

uint32_t tempStartTime = millis();
void loop()
{
  sensors.requestTemperatures();

  if (millis() - tempStartTime > (1000 / TEMPERATURE_REFRESH_RATE)) {
    tempStartTime = millis();
    for (int i = 0; i < TEMP_PROBE_COUNT; i++) {
      Serial.print(temp, 3);
      Serial.print("\t");
    }
    Serial.print("\n");
  }
  //  Serial.print(GetCurrentDraw());
  //  Serial.print("\t");
  //
  //  Serial.print(GetCellVoltage(0));
  //  Serial.print("\t");
  //
  //  Serial.println(GetCellVoltage(1));
}

double GetTempProbeC(uint8_t Index) {
  return sensors.getTempC(TEMP_PROBE_ADDRESSES[Index]);
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
