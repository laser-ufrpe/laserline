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

    gents_t void setWeights(Ts... values) { weights = { values... }; initErrorLookup(); }

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

private:
    void initErrorLookup() {
        errorLookup = {
            {0b1000000000000000, -weights[15]},
            {0b1100000000000000, -weights[14]},
            {0b1110000000000000, -weights[13]},
            {0b0110000000000000, -weights[12]},
            {0b0111000000000000, -weights[11]},
            {0b0011000000000000, -weights[10]},
            {0b0011100000000000, -weights[9]},
            {0b0001100000000000, -weights[8]},
            {0b0001110000000000, -weights[7]},
            {0b0000110000000000, -weights[6]},
            {0b0000111000000000, -weights[5]},
            {0b0000011000000000, -weights[4]},
            {0b0000011100000000, -weights[3]},
            {0b0000001100000000, -weights[2]},
            {0b0000000100000000, -weights[1]},
            {0b0000000110000000,  weights[0]},
            {0b0000000010000000,  weights[1]},
            {0b0000000011000000,  weights[2]},
            {0b0000000011100000,  weights[3]},
            {0b0000000001100000,  weights[4]},
            {0b0000000001110000,  weights[5]},
            {0b0000000000110000,  weights[6]},
            {0b0000000000111000,  weights[7]},
            {0b0000000000011000,  weights[8]},
            {0b0000000000011100,  weights[9]},
            {0b0000000000001100,  weights[10]},
            {0b0000000000001110,  weights[11]},
            {0b0000000000000110,  weights[12]},
            {0b0000000000000111,  weights[13]},
            {0b0000000000000011,  weights[14]},
            {0b0000000000000001,  weights[15]}
        };
    }

};
