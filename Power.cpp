#include "Power.h"


Power::Power(TTGOClass &ttgo) : ttgo_(ttgo)
{}

void Power::begin()
{
   Wire.begin(21, 22);
   ttgo_.power->adc1Enable(
        AXP202_VBUS_VOL_ADC1 |
        AXP202_VBUS_CUR_ADC1 |
        AXP202_BATT_CUR_ADC1 |
        AXP202_BATT_VOL_ADC1,
        true);
}

#define _PRINT(x) {Serial.print(#x); Serial.print(" "); Serial.println(x());}

void Power::loop()
{
}

bool  Power::isVBUSPlug(){
  return  ttgo_.power->isVBUSPlug(); 
}
float Power::getVbusVoltage(){
  return  ttgo_.power->getVbusVoltage(); 
}
float Power::getVbusCurrent(){
  return  ttgo_.power->getVbusCurrent();
}

bool  Power::isBattConnect(){
  return  ttgo_.power->isBatteryConnect(); 
}
float Power::getBattVoltage(){
  return  ttgo_.power-> getBattVoltage();
}
bool  Power::isChargeing(){
  return  ttgo_.power->isChargeing();
}
float Power::getBattChargeCurrent(){
  return  ttgo_.power->getBattChargeCurrent();
}
float Power::getBattDischargeCurrent(){
  return  ttgo_.power->getBattDischargeCurrent(); 
}
int   Power::getBattPercentage(){
  int rv = ttgo_.power->getBattPercentage();
  //Serial.print("BattPercentage"); Serial.print(" "); Serial.println(rv);
  return  rv;
}
