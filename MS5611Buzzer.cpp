#include "MS5611Buzzer.h"
#include <math.h> 


TwoWire twoWire(1);

MS5611Buzzer::MS5611Buzzer() : 
  MS5611(twoWire),   addressBuzzer_(0x12), lowpassSlow_(0.)
{
}

void MS5611Buzzer::sendData(uint8_t type, uint32_t data) {
    type = type | SEND_FLAG;
    char buffer[4];
    buffer[0] = (data & 0x000000FF);
    buffer[1] = (data & 0x0000FF00) >> 8;
    buffer[2] = (data & 0x00FF0000) >> 16;
    buffer[3] = (data & 0xFF000000) >> 24;

  
    wire_.beginTransmission(0x12);
    wire_.write((char)type);
    wire_.write(buffer[0]); 
    wire_.write(buffer[1]); 
    wire_.write(buffer[2]); 
    wire_.write(buffer[3]); 
    wire_.endTransmission(); 
}


uint32_t MS5611Buzzer::calcToneFreqence(double pressure) {
  pressure *=500.;
  if(lowpassSlow_ == 0.) {
    //Set Barriers
    lowpassFast_ = lowpassSlow_ = pressure;
    toneFreqLowpass_ = 0.;
    return (UP_BARRIER+DOWN_BARRIER)/2;
  } else {
    double toneFreq_; 
    lowpassFast_ = lowpassFast_ + (pressure - lowpassFast_) * 0.1;
    lowpassSlow_ = lowpassSlow_ + (pressure - lowpassSlow_) * 0.05;
     
    toneFreq_ = (lowpassSlow_ - lowpassFast_) * 50;
     
    toneFreqLowpass_ = toneFreqLowpass_ + (toneFreq_ - toneFreqLowpass_) * 0.1;
 
    toneFreq_ =  constrain(toneFreqLowpass_+(UP_BARRIER+DOWN_BARRIER)/2, 500., 8000.) ;
    Serial.println((uint32_t) toneFreq_);
    return (uint32_t) (toneFreq_);
  }
}  

void MS5611Buzzer::setEnableDbg(bool enable) {
  sendData(SEND_FLAG | SEND_DEBUG_ON, enable ? 0x00000001 : 0x00000000);
}

void MS5611Buzzer::setEnableBeep(bool enable) {
  sendData(SEND_FLAG | SEND_BEEP_ON, enable ? 0x00000001 : 0x00000000);  
}

    
void MS5611Buzzer::begin()
{
  wire_.setClock(3400000);
  wire_.begin(25,26);
  MS5611::begin();
}

void MS5611Buzzer::loop()
{
  MS5611::loop();
  if(isReady()) {
//    Serial.println((uint32_t) (getPressure()*100));
    uint32_t toneFreq = calcToneFreqence(getPressure());
    if(toneFreq >= 500 && toneFreq <=8000) {
      sendData(SEND_FLAG | SEND_FREQUENCE, toneFreq);
    }
  }
  
}
