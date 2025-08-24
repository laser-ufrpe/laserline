class DRIVERS {
public:
  PINS<1,4> motors;

  gents_t DRIVERS(Ts... pins) : motors(pins...) {
    motors.pwmio(12,20000);
  }
  f32_t rcalibrate(f32_t rspeed){
    return rspeed;
  }
  void move(f32_t lspeed, f32_t rspeed) {
    rspeed = rcalibrate(rspeed);
    motors.pwm(
      (lspeed<0) ? -(int)(lspeed*4095) : 0, 
      (lspeed>0) ?  (int)(lspeed*4095) : 0,
      (rspeed<0) ? -(int)(rspeed*4095) : 0, 
      (rspeed>0) ?  (int)(rspeed*4095) : 0
    );
  }
  void moveStop(f32_t lspeed, f32_t rspeed, int time) {
    move(lspeed, rspeed);
    delay(time);
    move(0,0);
  }
};
