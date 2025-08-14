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


pin32 pin32;

// Function sensor32

int muxIn[] = { 36, 39 };                   //  change to ADC
int muxSeq[] = { 5, 7, 6, 4, 3, 0, 1, 2 };  // CD4051 (pinout)
int muxMask = 16;                           // muxPin = [16,17,18]
int buffer[16] = { 0 };

void sensor32(int buffer[]){
  for (int i = 0; i < 8; i++) {
    pin32.low(0b111 << int(muxMask));
    pin32.high(muxSeq[i] << muxMask);
    buffer[i] = analogRead(muxIn[0]);
    buffer[i + 8] = analogRead(muxIn[1]);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Teste Serial");

  pin32.output(PINS_MASK);
}

void loop() {
  pin32.high(PINS_MASK);
  delay(200);
  pin32.low(PINS_MASK);
  delay(200);
}
