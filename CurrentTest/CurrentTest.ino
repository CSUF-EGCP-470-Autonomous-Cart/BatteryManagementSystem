#define CURRENT_PIN A0

const double offset = 324.58;
const uint8_t averageSize = 50;
uint32_t aveArray[averageSize];

void setup() {
  Serial.begin(115200);

  for(int i=0;i<averageSize;i++) {
    aveArray[i] = offset;
  }
}

void loop() {
  uint32_t analogValue = analogRead(CURRENT_PIN);
  double current = 0;
  double zeroedAnalog = analogValue - offset;

  for (int i = averageSize-1; i > 0; --i) {
    aveArray[i] = aveArray[i - 1];
  }
  aveArray[0] = analogValue;
  uint32_t sum = 0;
  for (int i = 0; i < averageSize; i++) {
    sum += aveArray[i];
  }
  double average = sum / (averageSize*1.0);
  double zeroedAverage = average - offset;

//  Serial.print(analogValue);
  Serial.print(zeroedAnalog);
  //  Serial.print("\t");
//    Serial.print(current);
  Serial.print("\t");
//  Serial.print(average);
  Serial.print(zeroedAverage);
  Serial.println("\n");

  delay(100);

}
