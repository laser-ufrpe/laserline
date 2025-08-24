class LINE {
public:
  int dataDig;
  int dataEnd;
  PINS<2,5> sensors;
  arr<int, 16> dataADC;  
  arr<f32_t, 16> weights;
  dic<i32_t, f32_t> errorLookup;

  gents_t LINE(Ts... pins) : sensors(pins...) {}
  gents_t void setWeights(Ts... values){ weights = { values... }; }
  void muxadc(){ sensors.muxadc(); dataADC = sensors.muxbuf; }
};