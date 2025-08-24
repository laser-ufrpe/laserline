#===========================================================
#                    GET SENSOR VALUE
#===========================================================
def getSensorDig():
  sum = 0
  for SelectedPin in sensorPins:
    sum = (sum<<1) + pin.get(SelectedPin)
  return sum

@micropython.native
def tapeColor(sensors): return sensors^(isWhite*255)

#=========================================================== 
@micropython.native
def getSensorADC():
  sensorsADC = []
  for SelectedPin in sensorPins:
    sensorVal = pin.adc(SelectedPin)
    sensorsADC.append(sensorVal)
  return sensorsADC

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
def grayPin():
  buf = [0]*16
  pos = [0, 1, 0, 2, 0, 1, 0, 2]
  val = [1, 1, 0, 1, 1, 0, 0, 0]
  ord = [6, 4, 7, 2, 1, 0, 3, 5]     # MuxPinout: 
  muxOut = pin.arr('out', muxPins)
  
  for i in range(8):
    muxOut[ pos[i] ].value( val[i] )
    buf[ ord[i]   ] = pin.adc(muxIn[0])
    buf[ ord[i]+8 ] = pin.adc(muxIn[1])
  return buf

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
