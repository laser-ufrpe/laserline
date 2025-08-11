#include <Arduino.h> 

void setup() {
  setCpuFrequencyMhz(80);  // Setando CPU para 80MHz
  Serial.begin(115200);
  Serial.println("CPU running at 80MHz");
}

void loop() {}