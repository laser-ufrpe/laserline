from machine import mem32
import micropython

@micropython.viper
def input(bitmask: int):
    mem32[0x3FF44028] = bitmask  # GPIO_ENABLE_W1TC_REG

@micropython.viper
def output(bitmask: int):
    mem32[0x3FF44024] = bitmask  # GPIO_ENABLE_W1TS_REG

@micropython.viper
def pullup(bitmask: int):
    pass

@micropython.viper
def pulldown(bitmask: int):
    pass

#==================================================================
@micropython.viper
def high(bitmask: int):
    mem32[0x3FF44008] = bitmask  # GPIO_OUT_W1TS_REG

@micropython.viper
def low(bitmask: int):
    mem32[0x3FF4400C] = bitmask  # GPIO_OUT_W1TC_REG
