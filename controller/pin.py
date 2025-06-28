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

#===========================================================
  def arr(self, mode, pins, freq=1000):
    PinList = []
    for pinNumber in pins:
      pin = self.new(mode, pinNumber, freq)
      PinList.append(pin)
    return PinList

  def new(self, mode, pinNumber, freq=20000):
    PinModes = {
      "in":       lambda: Pin(pinNumber, Pin.IN),
      "out":      lambda: Pin(pinNumber, Pin.OUT),
      "pwm":      lambda: PWM(Pin(pinNumber), freq=freq),
      "adc":      lambda: ADC(Pin(pinNumber, Pin.IN), atten=ADC.ATTN_11DB),
      "pullup":   lambda: Pin(pinNumber, Pin.IN, Pin.PULL_UP),
      "pulldown": lambda: Pin(pinNumber, Pin.IN, Pin.PULL_DOWN),
    }
    return PinModes[mode]()

