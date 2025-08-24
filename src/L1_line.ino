class LINE {
public:
    u32_t dataDig;
    int dataEnd;
    PINS<2, 5> sensors;
    arr<int, 16> dataADC;
    arr<f32_t, 16> weights;
    dic<i32_t, f32_t> errorLookup;
    arr<int, 16> failList;
    arr<int, 16> sortedErrMap;
    arr<int, 16> pins;
    int whiteValue;

    gents_t LINE(Ts... pins_) : sensors(pins_...), pins{ pins_... } {}

    gents_t void setWeights(Ts... values) { weights = { values... }; }

    void muxadc() {
        sensors.muxadc();
        dataADC = sensors.muxbuf;
    }

    bool inTape(int sensor) {
        return (sensor > whiteValue);
    }

    void toDigital() {
        uint16_t value = 0;
        for (int i = 0; i < 16; i++) {
            unsigned bit = inTape(dataADC[i]) ? 1u : 0u;
            value = (value << 1) | bit;
        }
        unsigned bitCount = __builtin_popcount(value);
        dataDig = static_cast<u32_t>(value) | ((bitCount > 8u) << 16);
    }

    void handleColor() {
        bool flag = (dataDig & (1u << 16)) != 0;
        if (flag) { dataDig = (0xFFFF * flag) ^ dataDig; }
    }

    void handleFail() {
        int sensor = static_cast<int>(dataDig & 0xFFFF); 
        if (std::find(failList.begin(), failList.end(), sensor) != failList.end() || errorLookup.count(sensor) > 0) { 
            dataEnd = sensor;
            return;
        }
        int bestMatch = sensor, bestScore = -1;
        for (int candidate : sortedErrMap) {
            int score = 16 - __builtin_popcount(candidate ^ sensor);
            if (score > bestScore) { bestScore = score; bestMatch = candidate; }
        }
        dataEnd = bestMatch;
    }

    void debugDig() {
        for (int i = 0; i < pins.size(); i++) {
            int val = digitalRead(pins[i]);
            Debug.blesend("Digital[" + String(pins[i]) + "] = " + String(val));
        }
    }

  void debugADC() {
    muxadc(); Debug.blesend("read: " + Debug.strArray(dataADC));
    delay(1000);
  }
};
