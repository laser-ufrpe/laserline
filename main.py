#===========================================================
#                       SETUP CODE
#===========================================================
import time
from controller.pin import PIN
from controller.bot import BOT
from controller.pid import PID
# 3 5 1 6 2 8 4 7
#===========================================================
#                  ENVIROMENT VARIABLES
#=========================================================== 
pin = PIN()
pid = PID(1.2, 0.0, 0)
bot = BOT(1023, 1023)

whiteValue = 4070
weights = [0.000, 0.069, 0.100, 0.207, 0.258, 0.317, 0.382]

bot.setDelay(0.001)
bot.limit(-1,1)
bot.baseSpd(0.6, 0.6)

pid.limit(-1,1)
bot.curveErr(0.9)
bot.curveSpd(-0.5, 0.45)
#=========================================================== 
#                TEST MUX WITH 16 SENSORS
#=========================================================== 
muxIn=pin.arr("adc", [32,33])
muxOut=pin.arr("out", [25,26,27])

def testMux(bit1, bit2, bit3):
  pin.new("out", 25).value(bit1)
  pin.new("out", 26).value(bit2)
  pin.new("out", 27).value(bit3)

  print(pin.adc(IN[0]), pin.adc(IN[1]))
#=========================================================== 
btn = pin.new("in", 15)
btn_gnd = pin.new("pullup", 2)

motorPins  = pin.arr("pwm", [16, 17, 13, 12])            # set motors pins
sensorPins = pin.arr("adc", [34,35,32,33, 25,26,27,14])  # sensor pins

#===========================================================
#                   DEBUG FUNCTIONS
#===========================================================
def debugSensorDig():
  while True:
    sensors = getSensorDig();         print(f'read: {sensors:08b}')
    sensors = handleFail(sensors);   print(f'fixed: {sensors:08b}\n')
    time.sleep(1)

def debugSensorADC():
  while True:
    sensors = getSensorADC();                print(f'read: {sensors}')
    sensors = toDigital(sensors);            print(f'conv: {sensors:08b}')
    sensors = handleColor(sensors);          print(f'color: {sensors:08b}')
    sensors = handleFail(sensors);           print(f'fixed: {sensors:08b}\n')

    adjust = adjustSpeedPID(sensors);
    print(f'pid: {adjust[2]}\n left: {adjust[0]}\n right: {adjust[1]}\n')
    time.sleep(1)

#=========================================================== 
@micropython.native
def mv(lspeed, rspeed, delay):
  move(lspeed, rspeed)
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
failList = [ 0, 0b_11111111 ]

errorLookup = {
  0b_10000000: -weights[6],  # left 6
  0b_11000000: -weights[5],  # left 5
  0b_01000000: -weights[4],  # left 4
  0b_01100000: -weights[3],  # left 3
  0b_00100000: -weights[2],  # left 2
  0b_00010000: -weights[1],  # left 1

  0b_00011000:  weights[0],  # center

  0b_00001000: +weights[1],  # right 1
  0b_00000100: +weights[2],  # right 2
  0b_00000110: +weights[3],  # right 3
  0b_00000010: +weights[4],  # right 4
  0b_00000011: +weights[5],  # right 5
  0b_00000001: +weights[6],  # right 6
}
#===========================================================
#                   MOVEMENT FUNCTIONS
#===========================================================
@micropython.native
def move(lspeed, rspeed):
  pin.pwm(motorPins[0], int(-(lspeed<0)*lspeed*bot.LPwm)) # left-back    
  pin.pwm(motorPins[1], int( (lspeed>0)*lspeed*bot.LPwm)) # left-front
  pin.pwm(motorPins[2], int(-(rspeed<0)*rspeed*bot.RPwm)) # right-back
  pin.pwm(motorPins[3], int( (rspeed>0)*rspeed*bot.RPwm)) # right-front

#===========================================================
#                   SENSORS FUNCTIONS
#===========================================================
@micropython.native
def inTape(val): return (val>whiteValue)
@micropython.native
def tapeColor(sensors): return sensors^(isWhite*255)

@micropython.native
def handleColor(sensorsRead):
  return 511*(sensorsRead>255) ^ sensorsRead

def getSensorDig():
  sum = 0
  for SelectedPin in sensorPins:
    sum = (sum<<1) + pin.get(SelectedPin)
  return sum

@micropython.native
def getSensorADC():
  sensorsADC = []
  for SelectedPin in sensorPins:
    sensorVal = pin.adc(SelectedPin)
    sensorsADC.append(sensorVal)
  return sensorsADC

@micropython.native
def toDigital(sensorsADC):
  sum, bitCount = 0, 0
  for val in sensorsADC:
    bitCount += inTape(val)
    sum = (sum<<1) + inTape(val)
  return sum + ((bitCount>4)<<8)

#===========================================================
#                  SENSOR FAIL handle
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
def handleFail(sensor):
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
    error = errorLookup[sensor]   # get error value
    if error == 0:
      move(bot.BaseLSpeed, bot.BaseLSpeed)   # move to front
    if error < -bot.CurveErr:
      move(bot.CurveISpeed, bot.CurveESpeed) # move to left
    if error > +bot.CurveErr:
      move(bot.CurveESpeed, bot.CurveISpeed) # move to right

@micropython.native
def adjustSpeedPID(sensor):
  if sensor not in failList:
    error = errorLookup[sensor]               # get error value
  else:
    error = pid.prev

  correction = pid.calc(error)              # calculate PID correction
  left = bot.BaseLSpeed + correction        # reduce left if turning right
  right = bot.BaseRSpeed - correction       # increase right if turning right
  left = min(max(left, bot.min), bot.max)
  right = min(max(right, bot.min), bot.max)
  return [left, right, correction]

#===========================================================
#                      LOOP FUNCTION
#===========================================================
def setup(delay, Spd, pidlim, selectpid):
  pid.reset()
  pid.conf(selectpid[0], selectpid[1], selectpid[2])
  bot.setDelay(delay)
  bot.limit(Spd[1],Spd[2])
  bot.baseSpd(Spd[0], Spd[0])
  pid.limit(pidlim[0], pidlim[1])
  

@micropython.native
def start(delay, Spd, pidlim, selectpid):
  setup(delay, Spd, pidlim, selectpid)
  while pin.get(btn) != 0:
    time.sleep(0.001)
  time.sleep(0.3)
  mv(0.6,0.6, 0.15)

  while True:
    sensors = getSensorADC()          # get 7 sensor values
    sensors = toDigital(sensors)      # get 7 sensor values
    sensors = handleColor(sensors)             # handle color
    sensors = handleFail(sensors)              # handle read problems
    adjust = adjustSpeedPID(sensors)           # adjust motors speed with PID
    
    move(adjust[0], adjust[1])
    time.sleep(bot.delay)                       # wait to next interaction
    if pin.get(btn) == 0:
      time.sleep(0.2)
      move(0,0)
      time.sleep(0.2)
      print("HEI, I STOP HERE!!!")
      return 0

    print(f'{sensors:08b}', pid.prev, adjust)

#===========================================================
# setup(weights, delay, [mid, min, max], [pidmin, pidmax], [kp, ki, kd])
#===========================================================
start(0.005, [0.7, -0.3, 0.7], [-1, 1], [2.6, 0.005, 2])
