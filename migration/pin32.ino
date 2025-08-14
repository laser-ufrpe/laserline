#include "soc/gpio_reg.h"
#include "driver/gpio.h"

volatile uint32_t PINS_MASK ( (1 << 2) | (1 << 25) | (1 << 26) | (1 << 27) | (1 << 33));  // Máscara dos pinos escolhidos

class pin32 { 
  public: 
  pin32(); 
  void input( int bitmask);
  void output( int bitmask);
  void high( int bitmask);
  void low( int bitmask);
  // void pullup( int bitmask);
  // void pulldown( int bitmask);
};

pin32::pin32() {}

void pin32::input( int bitmask){
  REG_WRITE(GPIO_ENABLE_W1TC_REG, bitmask); // GPIO_ENABLE_W1TC_REG
}
 
void pin32::output( int bitmask){
   REG_WRITE(GPIO_ENABLE_W1TS_REG, bitmask);  // GPIO_ENABLE_W1TS_REG
}
 
// void pin32::pullup( int bitmask){
//   pass;
// }
 
// void pin32::pulldown( int bitmask){
//   pass;
// }
 
void pin32::high( int bitmask){
   REG_WRITE(GPIO_OUT_W1TS_REG, bitmask);  // GPIO_OUT_W1TS_REG
}
    
void pin32::low( int bitmask){
   REG_WRITE(GPIO_OUT_W1TC_REG, bitmask);  // GPIO_OUT_W1TC_REG
}

pin32 pins;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Teste Serial");

  pins.output(PINS_MASK);
}

void loop() {
  pins.high(PINS_MASK);
  delay(200);
  pins.low(PINS_MASK);
  delay(200);

}
