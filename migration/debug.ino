// Não Testado porquê não possuo a Placa,

#include <Arduino.h>

void debugSensorADC() {
  while (true) {
    // 1. Leitura bruta dos sensores
    sensor32();
    Serial.print("read: ");
    for (int i = 0; i < 16; i++) {
      Serial.print(buffer[i]);
      if (i < 15) Serial.print(", ");
    }
    Serial.println();

    // 2. Conversão para digital
    uint16_t sensors = toDigital();
    Serial.print("conv: ");
    for (int i = 16; i >= 0; i--) {
      Serial.print((sensors >> i) & 1);
    }
    Serial.println();

    // 3. Tratamento de cor
    sensors = handleColor(sensors);
    Serial.print("color: ");
    for (int i = 15; i >= 0; i--) {
      Serial.print((sensors >> i) & 1);
    }
    Serial.println();

    // 4. Tratamento de falhas
    sensors = handleFail(sensors);
    Serial.print("fixed: ");
    for (int i = 15; i >= 0; i--) {
      Serial.print((sensors >> i) & 1);
    }
    Serial.println();

    // 5. PID e ajustes
    float left, right, error;
    adjustSpeedPID(sensors, left, right, error);
    Serial.print("pid: ");
    Serial.println(error);
    Serial.print("left: ");
    Serial.println(left);
    Serial.print("right: ");
    Serial.println(right);

    Serial.println();
    delay(1000); // equivalente ao time.sleep(1) do Python
  }
}
