#===========================================================
#                       SETUP CODE
#===========================================================
import time
from machine import Pin, ADC, PWM 

#===========================================================
#                  ENVIROMENT VARIABLES
#=========================================================== 
maxSpeed = 600         # Max PWM Speed (0-1023)
lineSpeed = 0.5        # Max line Speed  = 50%
curveSpeed = 1.0       # Max curve Speed = 100%
sensorDelay = 0.005    # time for wait between sensor read
#=========================================================== 
isBlack = 1            # Tape color
blackValue = 3800      # Set What is Black (0-4095)

speedPins = [22, 23]                        # set speed pins
motorPins  = [21, 19, 18, 5]                # set motors pins
sensorPins = [34, 35, 32, 33, 25, 26, 27]   # sensor pins

errorWeights = [0.0, 1.0, 1.0, 0.0, 0.0, 0.12, 0.17]
  
Stop = [0,0,0,0]                     # stop direction
Front, Back = [0,1,1,0], [1,0,0,1]   # front and back directions
Right, Left = [0,1,0,1], [1,0,1,0]   # right and left directions

#===========================================================
#                    SUPPORT FUNCTIONS
#===========================================================
@micropython.native
def setPin(pin, val): Pin(pin, Pin.OUT).value(val)
@micropython.native
def setPWM(pin, val): PWM(Pin(pin), freq=1000).duty(int(val*maxSpeed))

@micropython.native
def getPin(pin): return Pin(pin, Pin.IN).value()
@micropython.native
def getADC(pin): return ADC(Pin(pin, Pin.IN), atten=ADC.ATTN_11DB).read()
@micropython.native
def inTape(val): return isBlack ^ (val<blackValue)

#===========================================================
#                   DEBUG FUNCTIONS
#===========================================================
def debugSensorDig():
  while True:
    sensors = getSensorDig();         print(f'read: {sensors:07b}')
    sensors = handlerFail(sensors);   print(f'fixed: {sensors:07b}\n')
    time.sleep(1)

def debugSensorADC():
  while True:
    sensors = getSensorADC();        print(f'read: {sensors}')
    sensors = toDigital(sensors);    print(f'conv: {sensors:07b}')
    sensors = handlerFail(sensors);  print(f'fixed: {sensors:07b}\n')
    time.sleep(1)

#=========================================================== 
@micropython.native
def mv(dir, speed, delay):
  move(dir)
  setSpeed(speed[0], speed[1])
  time.sleep(delay)

@micropython.native
def curve(dir, speed, delay):
  maxSpeed=1023
  mv(Front, [1,1], 1) 
  mv(dir, speed, delay)
  move(Stop)


#===========================================================
#                   ERROR LOOKUP TABLE
#===========================================================
failList = [ 0, 0b_1111111 ]

errorLookup = {
  0b_1000000: -errorWeights[6],  # left 6
  0b_1100000: -errorWeights[5],  # left 5
  0b_1110000: -errorWeights[4],  # left 4
  0b_0111000: -errorWeights[3],  # left 3
  
  0b_0011111: -errorWeights[2],  # left 2 internal
  0b_0011110: -errorWeights[1],  # left 1 internal
  0b_0011100:  errorWeights[0],  # center
  0b_0111100: +errorWeights[1],  # right 1 internal
  0b_1111100: +errorWeights[2],  # right 2 internal
  
  0b_0001110: +errorWeights[3],  # right 3
  0b_0000111: +errorWeights[4],  # right 4
  0b_0000011: +errorWeights[5],  # right 5
  0b_0000001: +errorWeights[6],  # right 6
}
#===========================================================
#                   MOVIMENT FUNCTIONS
#===========================================================
@micropython.native
def move(dir):
  for i in range(4):
    setPin(motorPins[i], dir[i])

@micropython.native
def setSpeed(left, right):
  setPWM(speedPins[0], left)
  setPWM(speedPins[1], right)

#===========================================================
#                   SENSORS FUNCTIONS
#===========================================================
def getSensorDig():
  sum = 0
  for pin in sensorPins:
    sum = (sum<<1) + getPin(pin)
  return sum

@micropython.native
def getSensorADC():
  sensorArr = []
  for pin in sensorPins:
    sensorVal = getADC(pin)
    sensorArr.append(sensorVal)
  return sensorArr

@micropython.native
def toDigital(sensorArr):
  sum = 0
  for val in sensorArr:
    sum = (sum<<1) + inTape(val)
  return sum
#===========================================================
#                  SENSOR FAIL HANDLER
#===========================================================
@micropython.native
def sortMoreBit(bit, list):
  return sorted(list, key = lambda k: -bin(k).count(bit))

@micropython.native
def bitEquality(a, b):
  mask = (1<<7)-1
  return bin(~(a ^ b) & mask).count('1')

#===========================================================
sortedErrMap = sortMoreBit('1', errorLookup.keys())

@micropython.native
def handlerFail(sensor):
  if sensor in failList + list(errorLookup):
    return sensor

  return max(sortedErrMap, key = lambda k: bitEquality(k,sensor))

#===========================================================
#                  ADJUST SENSOR SPEED
#===========================================================
@micropython.native
def adjustSpeed(sensor):
  if sensor not in failList:
    error = errorLookup[sensor]      # get error value
    lbreak, rbreak = 0.0, 0.0        # start break in zero
    lbreak = -(error<0.0)*error      # adjust left break
    rbreak = (error>0.0)*error       # adjust right break
    setSpeed(1-lbreak, 1-rbreak)     # set correct speed

@micropython.native
def adjustSpeedBasic(sensor):
  if sensor not in failList:
    error = errorLookup[sensor]      # get error value
    if error == 0:
      setSpeed(lineSpeed,lineSpeed)
      move(Front)
    if error != 0:
      setSpeed(curveSpeed,curveSpeed)
    if error < -0.09:
      move(Left)
    if error > +0.09:
      move(Right)

#===========================================================
#                      LOOP FUNCTION
#===========================================================
@micropython.native
def start():
  move(Front)
  setSpeed(1.0, 1.0) # full speed
  time.sleep(0.15)

  while True:
    sensors = getSensorADC()         # get 7 sensor values
    sensors = toDigital(sensors)     # convert to digital
    sensors = handlerFail(sensors)   # handler read problems
    adjustSpeedBasic(sensors)        # adjust motors speed with PID
    time.sleep(sensorDelay)          # wait to next interaction

#===========================================================
#                NEW FUNCTION TO MOVIMENT
#===========================================================
def movecar(lspeed, rspeed):
  # LIMITA OS VALORES DE VELOCIDADE ENTRE -1 E 1
  lspeed = max(min(lspeed, 1.0), -1.0)
  rspeed = max(min(rspeed, 1.0), -1.0)

  # CONTROLE DO MOTOR ESQUERDO
  if lspeed > 0:
    lpwm = round(lspeed * 1023)
    esquerda_p2.duty(lpwm)
    esquerda_p1.duty(0)
  elif lspeed < 0:
    lpwm = round(-lspeed * 1023)
    esquerda_p1.duty(lpwm)
    esquerda_p2.duty(0)
  else:
    esquerda_p1.duty(0)
    esquerda_p2.duty(0)

  # CONTROLE DO MOTOR DIREITO
  if rspeed > 0:
    rpwm = round(rspeed * 1023)
    direita_p2.duty(rpwm)
    direita_p1.duty(0)
  elif rspeed < 0:
    rpwm = round(-rspeed * 1023)
    direita_p1.duty(rpwm)
    direita_p2.duty(0)
  else:
    direita_p1.duty(0)
    direita_p2.duty(0)

