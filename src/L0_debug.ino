class DEBUG {
public:
  vec<int> timers;
  bool isConnected;
  BluetoothSerial BLE;
  
  DEBUG() : timers(10), isConnected(false) {}

//========================================================================
  void timer(int timer)    { timers[timer] = micros(); }
  int  timerget(int timer) { return timers[timer]; }
  void timerend(int timer) { timers[timer] = micros() - timers[timer]; }

//========================================================================
  str_t bin(int num, int zeros) {
    str_t formatBin = "";
    for (int i = zeros - 1; i >= 0; i--) {
      formatBin += String((num >> i) & 1);
    }
    return formatBin;
  }

//========================================================================
  void ble(str_t name) {    
    BLE.begin(name);
    delay(30000);
    isConnected = BLE.hasClient();
  }

  void blesend(str_t msg) {
    (isConnected) ? BLE.println(msg) : Serial.println("BLE Not Connected");
  }

  char bleread() {
    return isConnected ? BLE.read() : '$';
  }
};

DEBUG Debug;