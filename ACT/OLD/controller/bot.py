#===========================================================
#                       BOT CONTROLLER
#===========================================================
class BOT:
  def __init__(self, LPwm, RPwm):
    self.maxPwm(LPwm, RPwm)
    self.newLookup(16*[0])

  def curveErr(self, val):
    self.CurveErr = val
 
  def setDelay(self, delay):
    self.delay = delay

  def setWeights(self, weights):
    self.weights = weights

  def limit(self, min, max):
    self.min = min
    self.max = max

  def maxPwm(self, LPwm, RPwm):
    self.LPwm = LPwm
    self.RPwm = RPwm

  def baseSpd(self, lspeed, rspeed):
    self.BaseLSpeed = lspeed
    self.BaseRSpeed = rspeed

  def curveSpd(self, internal, external):
    self.CurveISpeed = internal
    self.CurveESpeed = external

  def newLookup(self, weights):
      self.errorLookup = {
          0b_1000_0000_0000_0000: -weights[15],
          0b_1100_0000_0000_0000: -weights[14],
          0b_1110_0000_0000_0000: -weights[13],
          0b_0110_0000_0000_0000: -weights[12],
          0b_0111_0000_0000_0000: -weights[11],
          0b_0011_0000_0000_0000: -weights[10], 0b_0010_0000_0000_0000: -weights[10], # ERRO DE SOLDAGEM
          0b_0011_1000_0000_0000: -weights[9],  0b_0010_1000_0000_0000: -weights[9], # ERRO DE SOLDAGEM
          0b_0001_1000_0000_0000: -weights[8],  
          0b_0001_1100_0000_0000: -weights[7],
          0b_0000_1100_0000_0000: -weights[6], 0b_0000_1000_0000_0000: -weights[6], # ERRO DE SOLDAGEM
          0b_0000_1110_0000_0000: -weights[5], 0b_0001_1110_0000_0000: -weights[5], # ERRO DE SOLDAGEM
          0b_0000_0110_0000_0000: -weights[4], 0b_0001_0110_0000_0000: -weights[4], # ERRO DE SOLDAGEM
          0b_0000_0111_0000_0000: -weights[3], 0b_0001_0111_0000_0000: -weights[3], # ERRO DE SOLDAGEM
          0b_0000_0011_0000_0000: -weights[2],
          0b_0000_0001_0000_0000: -weights[1],
        
          0b_0000_0001_1000_0000: weights[0],
            
          0b_0000_0000_1000_0000: +weights[1],
          0b_0000_0000_1100_0000: +weights[2],
          0b_0000_0000_1110_0000: +weights[3],
          0b_0000_0000_0110_0000: +weights[4],
          0b_0000_0000_0111_0000: +weights[5],
          0b_0000_0000_0011_0000: +weights[6],
          0b_0000_0000_0011_1000: +weights[7],
          0b_0000_0000_0001_1000: +weights[8],
          0b_0000_0000_0001_1100: +weights[9],
          0b_0000_0000_0000_1100: +weights[10],
          0b_0000_0000_0000_1110: +weights[11],
          0b_0000_0000_0000_0110: +weights[12],
          0b_0000_0000_0000_0111: +weights[13],
          0b_0000_0000_0000_0011: +weights[14],
          0b_0000_0000_0000_0001: +weights[15]
      }
