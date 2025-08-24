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
    sensors = sensor32();                     print(f'read: {sensors}')
    sensors = toDigital(sensors);            print(f'conv: {sensors:017b}')
    sensors = handleColor(sensors);          print(f'color: {sensors:016b}')
    sensors = handleFail(sensors);           print(f'fixed: {sensors:016b}\n')

    adjust = adjustSpeedPID(sensors);
    print(f'pid: {adjust[2]}\n left: {adjust[0]}\n right: {adjust[1]}\n')
    time.sleep(1)
