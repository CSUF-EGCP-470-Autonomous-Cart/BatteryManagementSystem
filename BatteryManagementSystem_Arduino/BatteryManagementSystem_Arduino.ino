/*

  Cell 0    Black     {0x28, 0xAA, 0x39, 0xDF, 0x12, 0x13, 0x02, 0x61}    ads1.0
  Cell 1    White     {0x28, 0xAA, 0xD6, 0xB8, 0x13, 0x13, 0x02, 0x69}    ads1.1
  Cell 2    Blue      {0x28, 0xDC, 0x6D, 0x45, 0x92, 0x0B, 0x02, 0xC3}    ads1.2
  Cell 3    Green     {0x28, 0x5B, 0x3D, 0xDD, 0x1B, 0x13, 0x01, 0x50}    ads1.3
  Cell 4    Red       {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE}    ads2.0
  Cell 5    Yellow    {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25}    ads2.1

*/
#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>
#include "printf.h"

const uint8_t CELL_COUNT = 6;

#define ONE_WIRE_BUS_PIN 2  // Any pin 2 to 12 (not 13) and A0 to A5
#define CURRENT_SENSOR_PIN 0

//Precision of temperature sensors in number of bits (1 - 12)
#define TEMPERATURE_PRECISION 12

//How often the BMS System updates and sends the date to ROS
const uint8_t BMS_REFRESH_RATE = 1;  //Hz

const uint8_t TEMP_PROBE_ADDRESSES[][8] = {
  {0x28, 0xAA, 0x39, 0xDF, 0x12, 0x13, 0x02, 0x61},
  {0x28, 0xAA, 0xD6, 0xB8, 0x13, 0x13, 0x02, 0x69},
  {0x28, 0xDC, 0x6D, 0x45, 0x92, 0x0B, 0x02, 0xC3},
  {0x28, 0x5B, 0x3D, 0xDD, 0x1B, 0x13, 0x01, 0x50},
  {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE},
  {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25}
};

#define ADC_GAIN GAIN_SIXTEEN
const int ADC_OFFSET = 18;
const double ADC_CONVERT = 129198.1566820;
const double CELL_MULTIPLIERS[] = {
  38.037037037,
  82.481481481,
  123.222222222,
  175.074174174,
  208.407407407,
  312.111111111
};

struct BMS {
  double cellVoltage[6];
  double cellTemp[6];
  double voltage;
  double current;
};

//byte mac[] = {
//  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
//};
//IPAddress ip(192, 168, 2, 177);
//EthernetServer server(80);
//EthernetClient rosClient;


Adafruit_ADS1115 ads1(0x48);
Adafruit_ADS1115 ads2(0x49);

OneWire  oneWire(ONE_WIRE_BUS_PIN);  // Create a 1-wire object
DallasTemperature sensors(&oneWire);
void setup()
{
  Serial.begin(115200);
  printf_begin();


  //  Ethernet.begin(mac, ip);

  sensors.begin();
  sensors.setWaitForConversion(false);  // makes it async
  sensors.setCheckForConversion(true);
  sensors.requestTemperatures();

  // set the temperature probe precision in number of bits (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(TEMPERATURE_PRECISION);

  ads1.begin();
  ads2.begin();
  ads1.setGain(ADC_GAIN);
  ads2.setGain(ADC_GAIN);

  //  server.begin();
  //  Serial.print("server is at ");
  //  Serial.println(Ethernet.localIP());

}

unsigned long prevPollTime =  millis();
void loop()
{
  if (sensors.isConversionComplete() )
  {
    sensors.requestTemperatures();
  }


  if ((millis() - prevPollTime) > (1000 / BMS_REFRESH_RATE)) {
    prevPollTime = millis();

    BMS bms;
    bms.voltage = GetBankVoltageAtIndex(5);
    bms.current = 1.1;

    for (int i = 0; i < CELL_COUNT; i++) {
      bms.cellVoltage[i] = GetCellVoltage(i);
      bms.cellTemp[i] = GetTempProbeC(i);
    }


    Serial.print("BMS: "); Serial.println(millis());
    Serial.print("Current: "); Serial.println(bms.current);
    Serial.print("Bank Voltage: "); Serial.println(bms.voltage);

    for (int i = 0; i < CELL_COUNT; i++) {
      Serial.print("Cell "); Serial.print(i); Serial.print(" Voltage: "); Serial.println(bms.cellVoltage[i], 4);
    }
    for (int i = 0; i < CELL_COUNT; i++) {
      Serial.print("Cell "); Serial.print(i); Serial.print(" Temp: "); Serial.println(bms.cellTemp[i], 4);
    }
    Serial.println();
  }

  //  if (rosClient && !rosClient.connected()) {
  //    rosClient.stop();
  //    Serial.println("Client disconnected");
  //  }
  //
  //  EthernetClient client = server.available();
  //  if (client && !rosClient) {
  //    rosClient = client;
  //    Serial.println("New client connected");
  //  }

  //  if (rosClient.connected()) {
  //    Serial.println("sending data");

  //
  //    //char bmsBuffer[sizeof(BMS)];
  //    //memcpy(bmsBuffer, &bms, sizeof(bms));
  //    //rosClient.write(bmsBuffer, sizeof(bmsBuffer));
  //
  //  }
}

double GetTempProbeC(uint8_t Index) {
  return sensors.getTempC(TEMP_PROBE_ADDRESSES[Index]);
}

double GetTempProbeF(uint8_t Index) {
  return sensors.getTempF(TEMP_PROBE_ADDRESSES[Index]);
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

  if (Index >= 0 && Index <= 3) {
    BankVoltage = (float(ads1.readADC_SingleEnded(Index) + ADC_OFFSET) / ADC_CONVERT) * CELL_MULTIPLIERS[Index];
  }
  else if (Index > 3 && Index <= 5) {
    BankVoltage = (float(ads2.readADC_SingleEnded(Index - 4) + ADC_OFFSET) / ADC_CONVERT) * CELL_MULTIPLIERS[Index];
  }
  else {
    BankVoltage = -1.0;
  }

  return BankVoltage;
}

double GetCurrentDraw() {
  uint32_t analog = analogRead(CURRENT_SENSOR_PIN);
  double current = map(analog, 0, 1023, 0, 1000);
  return current;
}
