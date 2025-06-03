class PID:
    def __init__(self, p, i, d):
        self.reset()
        self.conf(p, i, d)

    def reset(self):
        self.last = 0
        self.hist = 0

    def conf(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd

    def calc(self, err):
        self.hist += err
        p = self.kp * err
        i = self.ki * self.hist
        d = self.kd * (err - self.last)
        self.last = err
        return p+i+d

