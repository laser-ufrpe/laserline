#include "soc/gpio_reg.h" // Arduino's own files
#include "driver/gpio.h"  // Arduino's own files

volatile uint32_t PINS_MASK ( (1 << 2) | (1 << 25) | (1 << 26) | (1 << 27) | (1 << 33));  // Máscara dos pinos escolhidos

void input( int bitmask){
  REG_WRITE(GPIO_ENABLE_W1TC_REG, bitmask); // GPIO_ENABLE_W1TC_REG
}
 
void output( int bitmask){
   REG_WRITE(GPIO_ENABLE_W1TS_REG, bitmask);  // GPIO_ENABLE_W1TS_REG
}
 
// void pullup( int bitmask){
//   pass;
// }
 
// void pulldown( int bitmask){
//   pass;
// }
 
void high( int bitmask){
   REG_WRITE(GPIO_OUT_W1TS_REG, bitmask);  // GPIO_OUT_W1TS_REG
}
    
void low( int bitmask){
   REG_WRITE(GPIO_OUT_W1TC_REG, bitmask);  // GPIO_OUT_W1TC_REG
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Teste Serial");
  output(PINS_MASK);

}

void loop() {
  high(PINS_MASK);
  delay(500);
  low(PINS_MASK);
  delay(500);

}
