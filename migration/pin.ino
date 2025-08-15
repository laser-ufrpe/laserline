#include <Arduino.h>
#include <vector>
#include <cstring> // Para strcmp

class PinController {
public:
//===============================
// configuração de cores ========
//===============================
    bool isBlack;
    int blackValue;

    void color(bool isBlack, int blackValue) {
        this->isBlack = isBlack;
        this->blackValue = blackValue;
    }
//================================
//operações básicas ==============
//================================
    void set(uint8_t pin, bool val) {
        digitalWrite(pin, val ? HIGH : LOW);
    }

    bool get(uint8_t pin) {
        return digitalRead(pin);
    }

    void pwm(uint8_t pin, int duty) {
        analogWrite(pin, duty);
    }

    int adc(uint8_t pin) {
        return analogRead(pin);
    }
//================================
// config. dos pinos =============
//================================
    uint8_t newPin(const char* mode, uint8_t pinNumber, int freq = 20000, int channel = 0) {
        if (strcmp(mode, "in") == 0) {
            pinMode(pinNumber, INPUT);
        }
        else if (strcmp(mode, "out") == 0) {
            pinMode(pinNumber, OUTPUT);
        }
        else if (strcmp(mode, "pullup") == 0) {
            pinMode(pinNumber, INPUT_PULLUP);
        }
        else if (strcmp(mode, "pulldown") == 0) {
            pinMode(pinNumber, INPUT);
            gpio_pulldown_en((gpio_num_t)pinNumber);
        }
        else if (strcmp(mode, "pwm") == 0) {
            ledcAttachPin(pinNumber, channel);
            ledcSetup(channel, freq, 8);
        }
        else if (strcmp(mode, "adc") == 0) {
            pinMode(pinNumber, INPUT);
        }
        return pinNumber;
    }
//====================================
// criar varios pinos de uma vez (arr)
//====================================
    std::vector<uint8_t> arr(const char* mode, std::vector<uint8_t> pins, int freq = 1000) {
        std::vector<uint8_t> pinList;
        int channel = 0;
        for (auto p : pins) {
            if (strcmp(mode, "pwm") == 0) {
                newPin(mode, p, freq, channel);
                channel++;
            } else {
                newPin(mode, p, freq);
            }
            pinList.push_back(p);
        }
        return pinList;
    }

    void begin() {
        
    }
};

