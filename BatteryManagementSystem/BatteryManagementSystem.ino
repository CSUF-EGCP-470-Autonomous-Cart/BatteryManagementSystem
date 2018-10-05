#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads1(0x48);
Adafruit_ADS1115 ads2(0x49);

#define CURRENT_SENSOR_PIN 0

const double CELL_MULTIPLIERS[] = {
  1.95643,
  1.90253,
  0.0,
  0.0,
  0.0,
  0.0
};

void setup() {
  Serial.begin(115200);

  ads1.begin();
  ads2.begin();
}

void loop() {
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
