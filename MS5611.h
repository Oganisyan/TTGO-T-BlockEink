#ifndef _MS5611_H_
#define _MS5611_H_
#include "config.h"
#include "KalmanFilter.h"

#ifndef INVALID_UNIT32
#define  INVALID_UNIT32 0xFFFFFFFF
#endif


class MS5611 // @suppress("Class has a virtual method and non-virtual destructor")
{
protected:  
    KalmanFilter kalmanFilter_;
    TwoWire &wire_;
    uint8_t     address_;
    uint16_t C[7];
    uint32_t callOrderTime_;
    uint8_t  callOrderCode_;
    double temperature_;
    double pressure_;
    double rawPressure_;


    double rawP_;
    double rawT_;
    double dT;
    double TEMP;
    double OFF;
    double SENS;
    double T2;
    double OFF2;
    double SENS2;


  
    uint32_t  getValue();
    void scaleValues();
    void orderValue(char code);
    bool pressureIsOk(double pressure);

  public:
    MS5611(TwoWire &wire);


    virtual void loop();
    virtual void begin();
    double  getTemperature();
    double  getPressure();
    double  getAltitude(double qnh=1013.25);
    double  getQNH(double altitude);
    bool    isReady();
};



#endif //_MS5611_H_
