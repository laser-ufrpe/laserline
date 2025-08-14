#include "soc/gpio_reg.h"
#include "driver/gpio.h"

volatile uint32_t PINS_MASK((1 << 2) | (1 << 25) | (1 << 26) | (1 << 27) | (1 << 33));  // Máscara dos pinos escolhidos

//  ---------- CLASS PIN32 ----------
class pin32 {
public:
  pin32();
  void input(int bitmask);
  void output(int bitmask);
  void high(int bitmask);
  void low(int bitmask);
  // void pullup( int bitmask);
  // void pulldown( int bitmask);
};

//  pin32 construtor
pin32::pin32() {}

// Define the pins as input
void pin32::input(int bitmask) {
  REG_WRITE(GPIO_ENABLE_W1TC_REG, bitmask);  // GPIO_ENABLE_W1TC_REG
}

// Define the pins as output
void pin32::output(int bitmask) {
  REG_WRITE(GPIO_ENABLE_W1TS_REG, bitmask);  // GPIO_ENABLE_W1TS_REG
}

// void pin32::pullup( int bitmask){
//   pass;
// }

// void pin32::pulldown( int bitmask){
//   pass;
// }

// sets the pins to high level
void pin32::high(int bitmask) {
  REG_WRITE(GPIO_OUT_W1TS_REG, bitmask);  // GPIO_OUT_W1TS_REG
}

// sets the pins to low level
void pin32::low(int bitmask) {
  REG_WRITE(GPIO_OUT_W1TC_REG, bitmask);  // GPIO_OUT_W1TC_REG
}
