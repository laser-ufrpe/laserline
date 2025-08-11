#include <Arduino.h>
#include <ESP32PWM.h>

// ====================== DEFINIÇÕES ======================
const int motorPins[] = {27, 4, 13, 14};
const int muxIn[] = {36, 39};       // Pinos ADC
const int muxSeq[] = {5, 7, 6, 4, 3, 0, 1, 2}; // Sequência CD4051
const int muxMask = 16;             // Pinos de controle do MUX (16,17,18)
const int btnPin = 5;               // Botão

// Constantes
const int pwmFreq = 50000;
const int whiteValue = 4070;
const uint16_t failList[] = {0, 0b1111111111111111};

// Variáveis globais
ESP32PWM motors[4];
int buffer[16];
int errorCount = 0;
float lpwm = 1023, rpwm = 1023;

// ====================== CLASSES ======================
class PID {
  public:
    float kp, ki, kd;
    float prev_error, integral;
    float min_out, max_out;
    
    PID(float p, float i, float d) : kp(p), ki(i), kd(d), prev_error(0), integral(0), min_out(-1), max_out(1) {}
    
    void limit(float min, float max) {
      min_out = min;
      max_out = max;
    }
    
    float calc(float error) {
      integral += error;
      float derivative = error - prev_error;
      prev_error = error;
      return constrain(kp * error + ki * integral + kd * derivative, min_out, max_out);
    }
    
    void reset() {
      prev_error = 0;
      integral = 0;
    }
    
    void conf(float p, float i, float d) {
      kp = p;
      ki = i;
      kd = d;
      reset();
    }
};

class BOT {
  public:
    float BaseLSpeed, BaseRSpeed;
    float min, max;
    float delay;
    float CurveErr;
    float CurveISpeed, CurveESpeed;
    float LPwm, RPwm;
    
    BOT(float l, float r) : BaseLSpeed(0.6), BaseRSpeed(0.6), min(-1), max(1), delay(0.001) {
      maxPwm(l, r);
    }
    
    void curveErr(float val) { CurveErr = val; }
    void setDelay(float d) { delay = d; }
    void limit(float mn, float mx) { min = mn; max = mx; }
    void maxPwm(float l, float r) { LPwm = l; RPwm = r; }
    void baseSpd(float l, float r) { BaseLSpeed = l; BaseRSpeed = r; }
    void curveSpd(float internal, float external) { 
      CurveISpeed = internal; 
      CurveESpeed = external; 
    }
    
    void newLookup(float weights[16]) {
      // Implementação da tabela de erro como no Python
      // ... (igual ao código anterior que te mostrei)
    }
    
    float errorLookup(uint16_t sensor) {
      // Implementação da busca na tabela
      // ... (igual ao código anterior)
    }
};

// Instâncias globais
BOT bot(1023, 1023);
PID pid(1.2, 0.0, 0);

// ====================== FUNÇÕES ======================
void move(float lspeed, float rspeed) {
  motors[0].writeDuty(int(-(lspeed<0)*lspeed*lpwm)); // left-back
  motors[1].writeDuty(int((lspeed>0)*lspeed*lpwm));  // left-front
  motors[2].writeDuty(int(-(rspeed<0)*rspeed*rpwm)); // right-back
  motors[3].writeDuty(int((rspeed>0)*rspeed*rpwm));  // right-front
}

void moveStop(float lspeed, float rspeed, float delay) {
  move(lspeed, rspeed);
  delayMicroseconds(delay * 1000);
  move(0, 0);
}

bool inTape(int val) { return val > whiteValue; }

uint16_t handleColor(uint16_t sensorsRead) {
  return (0b11111111111111111) * (sensorsRead > 0b1111111111111111) ^ sensorsRead;
}

void sensor32() {
  for(int i=0; i<8; i++) {
    // Controla o MUX
    digitalWrite(muxMask, (muxSeq[i] & 0b001));
    digitalWrite(muxMask+1, (muxSeq[i] & 0b010));
    digitalWrite(muxMask+2, (muxSeq[i] & 0b100));
    
    delayMicroseconds(10); // Tempo de estabilização
    buffer[i] = analogRead(muxIn[0]);
    buffer[i+8] = analogRead(muxIn[1]);
  }
}

uint16_t toDigital() {
  uint16_t sum = 0;
  int bitCount = 0;
  
  for(int i=0; i<16; i++) {
    bitCount += inTape(buffer[i]);
    sum = (sum << 1) | inTape(buffer[i]);
  }
  return sum + ((bitCount > 8) << 16);
}

uint16_t handleFail(uint16_t sensor) {
  // Implementar lógica de tratamento de falhas
  // ... (similar ao Python)
  return sensor;
}

void adjustSpeedPID(uint16_t sensor, float &left, float &right, float &error) {
  if(sensor != 0 && sensor != 0b1111111111111111) {
    error = bot.errorLookup(sensor);
  } else {
    error = pid.prev_error;
  }
  
  float correction = pid.calc(error);
  left = constrain(bot.BaseLSpeed - correction, bot.min, bot.max);
  right = constrain(bot.BaseRSpeed + correction, bot.min, bot.max);
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  
  // Configura pinos do MUX
  pinMode(muxMask, OUTPUT);
  pinMode(muxMask+1, OUTPUT);
  pinMode(muxMask+2, OUTPUT);
  
  // Configura botão
  pinMode(btnPin, INPUT_PULLUP);
  
  // Configura motores
  for(int i=0; i<4; i++) {
    motors[i].attachPin(motorPins[i], pwmFreq, 10);
  }
  
  // Configura ADCs
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  // Configuração inicial
  bot.setDelay(0.001);
  bot.limit(-1, 1);
  bot.baseSpd(0.6, 0.6);
  pid.limit(-1, 1);
  bot.curveErr(0.9);
  bot.curveSpd(-0.5, 0.45);
  
  move(0, 0); // Para os motores inicialmente
}

// ===================== LOOP PRINCIPAL =====================
void loop() {
  static bool running = false;
  
  // Controle por botão
  if(digitalRead(btnPin) == LOW) {
    delay(200); // Debounce
    running = !running;
    if(!running) move(0, 0);
    while(digitalRead(btnPin) == LOW); // Espera soltar
  }
  
  if(!running) return;
  
  unsigned long beginTime = micros();
  
  // 1. Leitura dos sensores
  sensor32();
  uint16_t sensors = toDigital();
  
  // 2. Processamento
  sensors = handleColor(sensors);
  sensors = handleFail(sensors);
  
  // 3. Controle dos motores
  float left, right, error;
  adjustSpeedPID(sensors, left, right, error);
  move(left, right);
  
  // 4. Timing do loop
  unsigned long loopTime = micros() - beginTime;
  unsigned long desiredDelay = max(0, (long)(bot.delay*1000000 - loopTime));
  delayMicroseconds(desiredDelay);
  
  // 5. Log (equivalente ao print do Python)
  char binary[17];
  for(int i=0; i<16; i++) {
    binary[15-i] = (sensors & (1 << i)) ? '1' : '0';
  }
  binary[16] = '\0';
  
  Serial.print("Sensors: ");
  Serial.print(binary);
  Serial.print(" | Error: ");
  Serial.print(error);
  Serial.print(" | Speeds: ");
  Serial.print(left);
  Serial.print(", ");
  Serial.print(right);
  Serial.print(" | Time: ");
  Serial.println(loopTime);
}

// Função start equivalente ao Python
void start(float delay, float Spd[], float pidlim[], float selectpid[], float weights[]) {
  bot.newLookup(weights);
  
  // Configuração inicial
  pid.conf(selectpid[0], selectpid[1], selectpid[2]);
  bot.setDelay(delay);
  bot.limit(Spd[1], Spd[2]);
  bot.baseSpd(Spd[0], Spd[0]);
  pid.limit(pidlim[0], pidlim[1]);
  
  // Espera botão
  while(digitalRead(btnPin) {
    delay(1);
  }
  delay(300);
  
  // Loop principal já está no loop()
}

void setup() {
  // ... configurações iniciais
  
  float Spd[] = {0.35, 0, 0.8};
  float pidlim[] = {-1, 1};
  float selectpid[] = {1, 0, 0};
  float weights[] = {0,0,0,0, 0.05,0.05,0.05,0.05, 0,0,0,0, 0,0,0,0};
  
  start(0.1, Spd, pidlim, selectpid, weights);
}