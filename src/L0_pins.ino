class PIN {
public:
  int gpio;
  PIN(int arg) : gpio(arg) {}

  int adc() const { return analogRead(gpio); }
  int get() const { return digitalRead(gpio); }

  void  dac(int val)           const { dacWrite(gpio, val); }
  void  pwm(int val)           const { ledcWrite(gpio, val); }
  void  set(int val)           const { digitalWrite(gpio, val); }
  void freq(int val, int time) const { ledcWriteTone(gpio, val); delay(time); }

  void input()      const { pinMode(gpio, INPUT); }
  void output()     const { pinMode(gpio, OUTPUT); }
  void pullup()     const { pinMode(gpio, INPUT_PULLUP); }
  void pulldown()   const { pinMode(gpio, INPUT_PULLDOWN); }
  void opendrain()  const { pinMode(gpio, OUTPUT_OPEN_DRAIN); }

  void pwmio(int res, int freq) const {
    pinMode(gpio, OUTPUT);
    ledcAttach(gpio, freq, res);
  }

  void adcio(int res, adc_attenuation_t attn) const {
    pinMode(gpio, INPUT);
    analogReadResolution(res);
    analogSetAttenuation(attn);
  }
};
//==========================================================
//                   MULTIPLE PINS CLASS
//==========================================================
template<int MuxQt, int PinQt, int MuxIOs =  1 << (PinQt-MuxQt)>
class PINS {
public:
  arr< PIN, PinQt> pins;
  arr< int, PinQt> pinbuf;
  arr< int, MuxIOs> muxseq;
  arr< int, MuxIOs*MuxQt> muxbuf;

  constexpr int size()           const { return pins.size(); }
  PIN& operator[](int idx)             { return pins[idx]; }
  const PIN& operator[](int idx) const { return pins[idx]; }

  template<typename... Ts>
  PINS(Ts... args) : pins{ PIN(args)... } {}

  gents_t void pwm(Ts... vals) const {
    int values[] = { vals... };
    for (int i = 0; i < PinQt; i++) {
      pins[i].pwm(values[i]);
    }
  }
//==========================================================
  gents_t void dmuxio(int mode, Ts... order) {
    muxseq = { order... };
    for (int i = 0; i < MuxQt; i++) pinMode(pins[i].gpio, mode);
    for (int i = MuxQt; i < PinQt; i++) pins[i].output();
  }
  gents_t void amuxio(int res, adc_attenuation_t attn, Ts... order) {
    muxseq = { order... };
    for (int i = 0; i < MuxQt; i++) pins[i].adcio(res, attn);
    for (int i = MuxQt; i < PinQt; i++) pins[i].output();
  }
//==========================================================
  void muxget() {
    for (int i = 0; i < MuxIOs; i++) {
      set(1, muxseq[i]);
      for (int j = 0; j < MuxQt; j++)
        muxbuf[i+j*MuxIOs] = pins[j].get();
    }
  }
  void muxadc() {
    for (int i = 0; i < MuxIOs; i++) {
      set(1, muxseq[i]);
      for (int j = 0; j < MuxQt; j++)
        muxbuf[i+j*MuxIOs] = pins[j].adc();
    }
  }
//==========================================================
  void input()      const { for (auto& p : pins) p.input(); }
  void output()     const { for (auto& p : pins) p.output(); }
  void pullup()     const { for (auto& p : pins) p.pullup(); }
  void pulldown()   const { for (auto& p : pins) p.pulldown(); }
  void opendrain()  const { for (auto& p : pins) p.opendrain(); }

  void pwmio(int res, int freq)               const { for (auto& p : pins) p.pwmio(res, freq); }
  void adcio(int res, adc_attenuation_t attn) const { for (auto& p : pins) p.adcio(res, attn); }  
//==========================================================
  void set(int firstpin, int bitmask) {
    for(int i = PinQt-1; i >= firstpin; i--) {
      pins[i].set(bitmask & 1);
      bitmask = bitmask>>1;
    }
  }
//==========================================================
  void get() const {
    for (int i = 0; i < PinQt; i++) { pinbuf[i] = pins[i].get(); }
  }

  void adc() const {
    for (int i = 0; i < PinQt; i++) { pinbuf[i] = pins[i].adc(); }
  }
};
//==========================================================
template<typename... PinList>
PINS(PinList... args) -> PINS<1, sizeof...(PinList)>;
