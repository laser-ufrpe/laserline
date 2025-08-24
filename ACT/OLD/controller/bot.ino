#include <Arduino.h>

// ============================================================
//                  BOT CONTROLLER CLASS
// ============================================================
class BOT {
  private:
    float LPwm, RPwm;
    float delayTime;
    float minSpeed, maxSpeed;
    float BaseLSpeed, BaseRSpeed;
    float CurveErr;
    float CurveISpeed, CurveESpeed;
    
    // Error lookup table
    typedef struct {
      uint16_t sensorPattern;
      float errorValue;
    } ErrorMap;
    
    ErrorMap errorLookup[32]; // Enough space for all patterns
    int errorMapSize = 0;

  public:
    // Constructor
    BOT(float lPwm, float rPwm) {
      maxPwm(lPwm, rPwm);
      float weights[16] = {0}; // Initialize with zeros
      newLookup(weights);
    }

    // Public methods
    void curveErr(float val) { CurveErr = val; }
 
    void setDelay(float delay) { delayTime = delay; }

    void limit(float min, float max) {
      minSpeed = min;
      maxSpeed = max;
    }

    void maxPwm(float lPwm, float rPwm) {
      LPwm = lPwm;
      RPwm = rPwm;
    }

    void baseSpd(float lspeed, float rspeed) {
      BaseLSpeed = lspeed;
      BaseRSpeed = rspeed;
    }

    void curveSpd(float internal, float external) {
      CurveISpeed = internal;
      CurveESpeed = external;
    }

    // Initialize error lookup table with weights
    void newLookup(float weights[16]) {
      errorMapSize = 0;
      
      // Negative weights (left side patterns)
      addErrorPattern(0b1000000000000000, -weights[15]);
      addErrorPattern(0b1100000000000000, -weights[14]);
      addErrorPattern(0b1110000000000000, -weights[13]);
      addErrorPattern(0b0110000000000000, -weights[12]);
      addErrorPattern(0b0111000000000000, -weights[11]);
      addErrorPattern(0b0011000000000000, -weights[10]);
      addErrorPattern(0b0010000000000000, -weights[10]); // Soldering error
      addErrorPattern(0b0011100000000000, -weights[9]);
      addErrorPattern(0b0010100000000000, -weights[9]); // Soldering error
      addErrorPattern(0b0001100000000000, -weights[8]);
      addErrorPattern(0b0001110000000000, -weights[7]);
      addErrorPattern(0b0000110000000000, -weights[6]);
      addErrorPattern(0b0000100000000000, -weights[6]); // Soldering error
      addErrorPattern(0b0000111000000000, -weights[5]);
      addErrorPattern(0b0001111000000000, -weights[5]); // Soldering error
      addErrorPattern(0b0000011000000000, -weights[4]);
      addErrorPattern(0b0001011000000000, -weights[4]); // Soldering error
      addErrorPattern(0b0000011100000000, -weights[3]);
      addErrorPattern(0b0001011100000000, -weights[3]); // Soldering error
      addErrorPattern(0b0000001100000000, -weights[2]);
      addErrorPattern(0b0000000100000000, -weights[1]);

      // Positive weights (right side patterns)
      addErrorPattern(0b0000000110000000, weights[0]);
      addErrorPattern(0b0000000010000000, weights[1]);
      addErrorPattern(0b0000000011000000, weights[2]);
      addErrorPattern(0b0000000011100000, weights[3]);
      addErrorPattern(0b0000000001100000, weights[4]);
      addErrorPattern(0b0000000001110000, weights[5]);
      addErrorPattern(0b0000000000110000, weights[6]);
      addErrorPattern(0b0000000000111000, weights[7]);
      addErrorPattern(0b0000000000011000, weights[8]);
      addErrorPattern(0b0000000000011100, weights[9]);
      addErrorPattern(0b0000000000001100, weights[10]);
      addErrorPattern(0b0000000000001110, weights[11]);
      addErrorPattern(0b0000000000000110, weights[12]);
      addErrorPattern(0b0000000000000111, weights[13]);
      addErrorPattern(0b0000000000000011, weights[14]);
      addErrorPattern(0b0000000000000001, weights[15]);
    }

    // Get error value from sensor pattern
    float getError(uint16_t sensorPattern) {
      for (int i = 0; i < errorMapSize; i++) {
        if (errorLookup[i].sensorPattern == sensorPattern) {
          return errorLookup[i].errorValue;
        }
      }
      return 0.0; // Default if pattern not found
    }

    // Getters for important values
    float getBaseLSpeed() { return BaseLSpeed; }
    float getBaseRSpeed() { return BaseRSpeed; }
    float getMinSpeed() { return minSpeed; }
    float getMaxSpeed() { return maxSpeed; }
    float getLPwm() { return LPwm; }
    float getRPwm() { return RPwm; }
    float getDelay() { return delayTime; }

  private:
    // Helper method to add patterns to the lookup table
    void addErrorPattern(uint16_t pattern, float error) {
      if (errorMapSize < 32) {
        errorLookup[errorMapSize].sensorPattern = pattern;
        errorLookup[errorMapSize].errorValue = error;
        errorMapSize++;
      }
    }
};

// ============================================================
//                  GLOBAL VARIABLES AND SETUP
// ============================================================
BOT myBot(1023, 1023); // Create bot instance with max PWM values

// Simulated sensor input for testing
uint16_t testPatterns[] = {
  0b1000000000000000,
  0b0000000000000001,
  0b0000000001100000,
  0b0000000110000000,
  0b0001100000000000
};
int currentTestPattern = 0;

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for serial to initialize

  // Configure bot with default values
  myBot.baseSpd(0.6, 0.6);
  myBot.limit(-1.0, 1.0);
  myBot.setDelay(0.001);
  
  // Configure weights (same as your Python example)
  float weights[16] = {
    0.05,  // 0
    0.05,  // 1
    0.05,  // 2
    0.05,  // 3
    0.05,  // 4
    0.05,  // 5
    0.05,  // 6
    0.05,  // 7
    0.05,  // 8
    0.05,  // 9
    0.05,  // 10
    0.05,  // 11
    0.05,  // 12
    0.05,  // 13
    0.05,  // 14
    0.05   // 15
  };
  myBot.newLookup(weights);
  
  Serial.println("Bot controller initialized");
  Serial.println("Testing error lookup table:");
}

// ============================================================
//                  MAIN LOOP WITH TESTING
// ============================================================
void loop() {
  // Cycle through test patterns
  uint16_t currentPattern = testPatterns[currentTestPattern];
  
  // Get error for current pattern
  float error = myBot.getError(currentPattern);
  
  // Calculate motor speeds
  float leftSpeed = myBot.getBaseLSpeed() - error;
  float rightSpeed = myBot.getBaseRSpeed() + error;
  
  // Print results
  Serial.print("Pattern: 0b");
  for(int i=15; i>=0; i--) {
    Serial.print((currentPattern >> i) & 1);
    if(i % 4 == 0 && i != 0) Serial.print("_");
  }
  Serial.print(" (0x");
  Serial.print(currentPattern, HEX);
  Serial.print(")");
  Serial.print(" | Error: ");
  Serial.print(error, 4);
  Serial.print(" | Speeds: L=");
  Serial.print(leftSpeed, 4);
  Serial.print(", R=");
  Serial.print(rightSpeed, 4);
  Serial.println();
  
  // Move to next pattern
  currentTestPattern = (currentTestPattern + 1) % (sizeof(testPatterns)/sizeof(testPatterns[0]));
  
  // Add delay between tests
  delay(2000);
}