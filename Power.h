#ifndef _POWER_H_
#define _POWER_H_
#include "config.h"


class Power {
    TTGOClass &ttgo_;
 public:
    Power(TTGOClass &ttgo);
    void  begin();
    void  loop();

    bool  isVBUSPlug();
    float getVbusVoltage();
    float getVbusCurrent();

    bool  isBattConnect();
    float getBattVoltage();
    bool  isChargeing();
    float getBattChargeCurrent();
    float getBattDischargeCurrent();
    int   getBattPercentage();
    
};



#endif
