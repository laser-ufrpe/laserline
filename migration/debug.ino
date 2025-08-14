// ===========================================================
//                    DEBUG FUNCTIONS
// ===========================================================

void printBinary8(uint8_t value) {
  for (int i = 7; i >= 0; i--) { 
    Serial.print((value >> i) & 1); 
  }
  Serial.println();
}

void debugSensorDig(){
  uint8_t sensors;
  while(true){
    sensors = getSensorDig();       
    Serial.print("read: "); printBinary8(sensors);
    sensors = handleFail(sensors);  
    Serial.print("fixed: ") ; printBinary8(sensors);
  }
}
