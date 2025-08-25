class DEBUG {
public:
  vec<int> timers;
  DEBUG() : timers(10) {}

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
  template<typename T, size_t N>
  String strArray(const arr<T, N>& arr) {
    String msg = "[";
    for (size_t i = 0; i < N; i++) {
      msg += String(arr[i]);
      if (i < N - 1) msg += ", ";
    }
    msg += "]";
    return msg;
  }
 
};

DEBUG Debug;