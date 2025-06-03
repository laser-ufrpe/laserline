#===========================================================
#                       BOT CONTROLLER
#===========================================================
class BOT:
  def __init__(self, LPwm, RPwm):
    self.maxPwm(LPwm, RPwm)

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
