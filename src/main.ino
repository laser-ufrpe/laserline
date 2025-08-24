LINE lineSensor(36,39,16,17,18);

void setup() {
  lineSensor.sensors.amuxio(12, ADC_11db, 5,7,6,4,3,0,1,2);
  Debug.ble("abacate");
}

void loop() {
  lineSensor.debugADC();
  delay(1000);
}