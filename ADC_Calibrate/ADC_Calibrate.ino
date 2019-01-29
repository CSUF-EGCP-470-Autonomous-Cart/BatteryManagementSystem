#include <Wire.h>
#include <Adafruit_ADS1015.h>

#define ADC_GAIN GAIN_SIXTEEN

Adafruit_ADS1115 ads1(0x48);
Adafruit_ADS1115 ads2(0x49);

void setup() {
  Serial.begin(115200);
  
  ads1.begin();
  ads2.begin();
  ads1.setGain(ADC_GAIN);
  ads2.setGain(ADC_GAIN);

}

void loop() {
  Serial.print("adc1 A0: "); Serial.println(ads1.readADC_SingleEnded(0));
  Serial.print("adc1 A1: "); Serial.println(ads1.readADC_SingleEnded(1));
  Serial.print("adc1 A2: "); Serial.println(ads1.readADC_SingleEnded(2));
  Serial.print("adc1 A3: "); Serial.println(ads1.readADC_SingleEnded(3));
  
  Serial.print("adc2 A1: "); Serial.println(ads2.readADC_SingleEnded(0));
  Serial.print("adc2 A2: "); Serial.println(ads2.readADC_SingleEnded(1));
  Serial.print("adc2 A3: "); Serial.println(ads2.readADC_SingleEnded(2));
  Serial.print("adc2 A4: "); Serial.println(ads2.readADC_SingleEnded(3));

  Serial.println("\n\n");

  delay(2000);

}
