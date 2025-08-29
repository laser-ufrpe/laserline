#===========================================================
#                       SETUP CODE
#===========================================================
import time
import micropython
from machine import PWM, Pin, ADC
from pybotcup import pin32
from controller.pin import PIN
from controller.bot import BOT
from controller.pid import PID
from time import ticks_us, ticks_diff
from array import array

#===========================================================
#                  ENVIROMENT VARIABLES
#===========================================================
motorPin = [27, 4, 13, 14]
errorCount = 0

pin = PIN()
pid = PID(1.2, 0.0, 0)
bot = BOT(1023, 1023)
lpwm, rpwm = 1023, 1023
pwmFreq = 50000

whiteValue = 4070

bot.setDelay(0.001)
bot.limit(-1,1)
bot.baseSpd(0.6, 0.6)

pid.limit(-1,1)
bot.curveErr(0.9)
bot.curveSpd(-0.5, 0.45)
#=========================================================== 
muxIn  = [36,39]            # change to ADC
muxSeq = [5,7,6,4,3,0,1,2]  # CD4051 (pinout)
muxMask = 16                # muxPin = [16,17,18]
buffer = 16*[0]
btn = pin.new("pullup", 5)

#=========================================================== 
pin32.output(0b111 << muxMask)                                  # SETUP: muxPin

for i in range( len(muxIn) ):                                   # SETUP: muxIn
    muxIn[i] = ADC(Pin(muxIn[i], Pin.IN), atten=ADC.ATTN_11DB)

for i in range( len(motorPin) ):                                # SETUP: motorPin
    motorPin[i] = PWM(Pin(motorPin[i]), freq=pwmFreq, duty=0)

#===========================================================
#                   ERROR LOOKUP TABLE
#===========================================================
failList = [ 0, 0b_1111_1111_1111_1111 ]

#===========================================================
#                   MOVEMENT FUNCTIONS
#===========================================================
@micropython.native
def move(lspeed, rspeed):
  motorPin[0].duty( int(-(lspeed<0)*lspeed*lpwm)) # left-back    
  motorPin[1].duty( int( (lspeed>0)*lspeed*lpwm)) # left-front
  motorPin[2].duty( int(-(rspeed<0)*rspeed*rpwm)) # right-back
  motorPin[3].duty( int( (rspeed>0)*rspeed*rpwm)) # right-front

@micropython.native
def moveStop(lspeed, rspeed, delay):
  move(lspeed, rspeed)
  time.sleep(delay)
  move(0,0)

#===========================================================
#                   SENSORS FUNCTIONS
#===========================================================
@micropython.native
def inTape(val): return (val>whiteValue)

@micropython.native
def handleColor(sensorsRead):
  return (0b_1_1111_1111_1111_1111)*(sensorsRead > 0b_1111_1111_1111_1111) ^ sensorsRead

@micropython.viper
def sensor32() -> object:
    for i in range(8):
        pin32.low(0b111 << int(muxMask))
        pin32.high(muxSeq[i] << muxMask)
        buffer[  i  ] = muxIn[0].read()
        buffer[ i+8 ] = muxIn[1].read()
    return buffer


GPIO_IN_REG = 0x3FF4403C
GPIO_IN1_REG = 0x3FF44040
@micropython.viper
def read_grouped_pins() -> int:
    reg_low = mem32[0x3FF4403C]
    reg_high = mem32[0x3FF44040]

    result = (
        ((reg_low & 0x6000) >> 13) |
        ((reg_low & 0xF0000) >> 14) |
        ((reg_low & 0xE00000) >> 15) |
        ((reg_low & 0xE000000) >> 16) |
        ((reg_high & 0b11) << 12)
    )
    return result


@micropython.native
def toDigital(sensorsADC):
  sum, bitCount = 0, 0
  for val in sensorsADC:
    bitCount += inTape(val)
    sum = (sum<<1) + inTape(val)
  return sum + ((bitCount>8)<<16)

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
sortedErrMap = sortMoreBit('1', bot.errorLookup.keys())

@micropython.native
def handleFail(sensor):
  if sensor in failList + list(bot.errorLookup):
    return sensor

  return max(sortedErrMap, key = lambda k: bitEquality(k,sensor))

#===========================================================
#                  ADJUST SENSOR SPEED
#===========================================================
@micropython.native
def adjustSpeedPID(sensor):
  if sensor not in failList:
    error = bot.errorLookup[sensor]        # get error value
  else:
    error = pid.prev

  correction = pid.calc(error)              # calculate PID correction
  left = bot.BaseLSpeed - correction        # reduce left if turning right
  right = bot.BaseRSpeed + correction       # increase right if turning right
  left = min(max(left, bot.min), bot.max)
  right = min(max(right, bot.min), bot.max)
  return [left, right, error]

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
def start(delay, Spd, pidlim, selectpid, weights):
  bot.newLookup(weights)
  setup(delay, Spd, pidlim, selectpid)       # SETUP DATA

  while pin.get(btn) != 0:
    time.sleep(0.001)
  time.sleep(0.3)

  while True:
    beginTime = time.ticks_us()
    sensors = sensor32()                       # get 16 sensor values
    sensors = toDigital(sensors)               # get 16 sensor values
    sensors = handleColor(sensors)             # handle color
    sensors = handleFail(sensors)              # handle read problems
    adjust  = adjustSpeedPID(sensors)          # adjust motors speed with PID
    
    move(adjust[0], adjust[1])
    time.sleep(delay)                          # wait to next interaction
    
    if pin.get(btn) == 0:
      time.sleep(0.2)
      move(0,0)
      time.sleep(0.2)
      print("HEI, I STOP HERE!!!")
      return 0

    loopTime = time.ticks_diff(time.ticks_us(), beginTime)
    print(f'{sensors:016b}', pid.prev, adjust, loopTime)

#===========================================================
# setup(delay, [mid, min, max], [pidmin, pidmax], [kp, ki, kd], [weights])
#===========================================================
move(0,0)
# start(0.1, [0.35, 0, 0.8], [-1, 1], [1, 0, 0], [0,0,0,0, 0.05,0.05,0.05,0.05, 0,0,0,0, 0,0,0,0])
