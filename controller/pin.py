#===========================================================
#                      PINS CONTROLLER
#===========================================================
from machine import Pin, ADC, PWM

class PIN:
  def __init__(self):
    return None
#===========================================================
  def color(self, isBlack, blackValue): 
    self.isBlack = isBlack
    self.blackValue = blackValue
#===========================================================  
  @micropython.native
  def set(self, pin, val): pin.value(val)
  @micropython.native
  def pwm(self, pin, val): pin.duty(val)

  @micropython.native
  def get(self, pin): return pin.value()
  @micropython.native
  def adc(self, pin): return pin.read()

