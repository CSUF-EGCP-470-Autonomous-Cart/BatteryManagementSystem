/*

  Cell 0    Black     {0x28, 0xAA, 0x39, 0xDF, 0x12, 0x13, 0x02, 0x61}    ads1.0
  Cell 1    White     {0x28, 0xAA, 0xD6, 0xB8, 0x13, 0x13, 0x02, 0x69}    ads1.1
  Cell 2    Blue      {0x28, 0xDC, 0x6D, 0x45, 0x92, 0x0B, 0x02, 0xC3}    ads1.2
  Cell 3    Green     {0x28, 0x5B, 0x3D, 0xDD, 0x1B, 0x13, 0x01, 0x50}    ads1.3
  Cell 4    Red       {0x28, 0xC1, 0x7D, 0x45, 0x92, 0x0B, 0x02, 0xAE}    ads2.0
  Cell 5    Yellow    {0x28, 0x2C, 0x8E, 0x45, 0x92, 0x0B, 0x02, 0x25}    ads2.1

*/
//Must be before ros.h is imported
#define ROSSERIAL_ARDUINO_TCP

#include <Adafruit_ADS1015.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ros.h>
#include <sensor_msgs/BatteryState.h>
#include <std_msgs/HEader.h>

const uint8_t CELL_COUNT = 6;
#define CELL_VOLTAGE_NOMINAL 8
#define CELL_VOLTAGE_CRITICAL 6
#define CELL_VOLTAGE_OVERCHARGED 10
#define CELL_CAPACITY 188.75  //Ah
#define BANK_VOLTAGE_NOMINAL 48
#define BANK_VOLTAGE_CRITICAL 44
#define BANK_VOLTAGE_OVERCHARGED 52

#define ONE_WIRE_BUS_PIN 2  // Any pin 2 to 12 (not 13) and A0 to A5
#define CURRENT_SENSOR_PIN 0

//Precision of temperature sensors in number of bits (1 - 12)
#define TEMPERATURE_PRECISION 12

//How often the BMS System updates and sends the date to ROS
const uint8_t RATE = 1;  //Hz

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

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 3, 21);
IPAddress server(192 , 168, 3, 11);
const uint16_t serverPort = 11411;

ros::NodeHandle nh;
sensor_msgs::BatteryState bms_msg;
std_msgs::Header bms_msg_header;
ros::Publisher bms("bms", &bms_msg);

Adafruit_ADS1115 ads1(0x48);
Adafruit_ADS1115 ads2(0x49);

OneWire  oneWire(ONE_WIRE_BUS_PIN);  // Create a 1-wire object
DallasTemperature sensors(&oneWire);
void setup()
{
  Serial.begin(115200);

  Ethernet.begin(mac, ip);
  Serial.print("Client IP: ");
  Serial.println(Ethernet.localIP());

  //wait for ethernet shield to initalize
  delay(1000);
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();
  nh.advertise(bms);

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

  bms_msg.header = bms_msg_header;
  bms_msg_header.seq = 0;
}

unsigned long prevPollTime =  millis();
void loop()
{
  //Update temperature sensors
  if (sensors.isConversionComplete())
    sensors.requestTemperatures();


  if ((millis() - prevPollTime) > (1000 / RATE)) {
      prevPollTime = millis();

    //------Update BMS Data------

    bms_msg.voltage = GetBankVoltageAtIndex(5);
    bms_msg.current = 0.0;
    //bms_msg.charge
    //bms_msg.capacity
    //bms_msg.design_capacity
    //bms_msg.percentage
    bms_msg.power_supply_status = sensor_msgs::BatteryState::POWER_SUPPLY_STATUS_UNKNOWN;
    bms_msg.power_supply_health = sensor_msgs::BatteryState::POWER_SUPPLY_HEALTH_UNKNOWN;
    bms_msg.power_supply_technology = sensor_msgs::BatteryState::POWER_SUPPLY_TECHNOLOGY_UNKNOWN;
    bms_msg.present = true;

    //TODO bank voltage logic

    bms_msg.cell_voltage_length = CELL_COUNT;
    float cellVoltages[CELL_COUNT];
    for (int i = 0; i < CELL_COUNT; i++) {
      //bms.cellTemp[i] = GetTempProbeC(i);
      //TODO Cell temp publish
      //TODO cell temp logic

      cellVoltages[i] = (float)GetCellVoltage(i);
      //bms_msg.cell_voltage.push_back((float)GetCellVoltage(i));
      //TODO cell voltage logic
    }
    bms_msg.cell_voltage = cellVoltages;
    //------------------------

    #ifdef DEBUG
      Serial.print("BMS: "); Serial.println(millis());
      Serial.print("Current: "); Serial.println(bms_msg.current);
      Serial.print("Bank Voltage: "); Serial.println(bms_msg.voltage);

      for (int i = 0; i < CELL_COUNT; i++) {
        Serial.print("Cell "); Serial.print(i); Serial.print(" Voltage: "); Serial.println(GetCellVoltage(i), 4);//Serial.println(bms.cellVoltage[i], 4);
      }
      for (int i = 0; i < CELL_COUNT; i++) {
        Serial.print("Cell "); Serial.print(i); Serial.print(" Temp: "); Serial.println(GetTempProbeC(i), 4);//Serial.println(bms.cellTemp[i], 4);
      }
      Serial.println();
    #endif

    bms_msg_header.seq = bms_msg_header.seq++;
    bms_msg.header = bms_msg_header;

    if (nh.connected()) {
      Serial.print("Conneced...");
      bms.publish(&bms_msg);
      Serial.println("published!");
    }
    else {
      Serial.println("Not conneced");
    }

  }

  nh.spinOnce();
  delay(1);
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
    return GetBankVoltageAtIndex(Index);
  }
  else if (Index > 0 && Index <= 5) {
    return (GetBankVoltageAtIndex(Index) - GetBankVoltageAtIndex(Index - 1));
  }
  return -1;
}

double GetBankVoltageAtIndex(uint8_t Index) {
  if (Index >= 0 && Index <= 3) {
    return (float(ads1.readADC_SingleEnded(Index) + ADC_OFFSET) / ADC_CONVERT) * CELL_MULTIPLIERS[Index];
  }
  else if (Index > 3 && Index <= 5) {
    return (float(ads2.readADC_SingleEnded(Index - 4) + ADC_OFFSET) / ADC_CONVERT) * CELL_MULTIPLIERS[Index];
  }
  return -1;
}

double GetCurrentDraw() {
  uint32_t analog = analogRead(CURRENT_SENSOR_PIN);
  double current = map(analog, 0, 1023, 0, 1000);
  return current;
}
