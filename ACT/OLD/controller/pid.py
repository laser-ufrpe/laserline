#===========================================================
#                       PID CONTROLLER
#===========================================================
class PID:
  def __init__(self, p, i, d):
    self.reset()
    self.conf(p, i, d)
    self.limit(0,1)

  def reset(self):
    self.prev = 0
    self.hist = 0

  def conf(self, kp, ki, kd):
    self.kp = kp
    self.ki = ki
    self.kd = kd

  def limit(self, min, max):
    self.min = min
    self.max = max

  def calc(self, err):
    self.hist += err
    p = self.kp * err
    i = self.ki * self.hist
    d = self.kd * (err - self.prev)
    self.prev = err
    return min(max(p+i+d, self.min), self.max) 

