#===========================================================
#                     PID CONTROLLER 
#===========================================================
import time 

class PIDController:
    def __init__(self, p=1.0, i=0.0, d=0.0):
        self.kp = p 
        self.ki = i
        self.kd = d 
        self.lstError = 0.0
        self.integral = 0.0
        self.lstTime = time.ticks_ms()

    def reset(self):
        self.prevError = 0.0
        self.integral = 0.0
        self.lstTime = time.ticks_ms()

    def compute(self, error:float) -> float:
        currentTime = time.ticks_ms()
        deltaTime = time.ticks_diff(currentTime, self.lstTime) / 1000.0

        self.integral += error * deltaTime
        derivative = (error - self.lstError) / deltaTime if deltaTime > 0 else 0.0 
      
        output:tuple = ( 
            self.kp * error +
            self.ki * self.integral +
            self.kd * derivative
        )
        self.lstError = error
        self.lstTime = currentTime
        
        return output
