#ifndef _SCREAN_H_
#define _SCREAN_H_

#include "config.h"
#include "GpsI2c.h"

#define APV_LINE_COUNT 24

class Screan {
  GxEPD_Class &display_;
  MPU6050 &mpu_;
  int wide_;
  int height_;

  //AnalogPitchView
  int apvWide_;
  int apvHeight_;
  int xPos[APV_LINE_COUNT];
  int yPos[APV_LINE_COUNT];

  void initTablo(int nr, char *info);
  void updateTablo(bool debug, int nr, int txtSize, const char *fmt, ...);
  void updateBatary(int value);
  
public:
  Screan(GxEPD_Class &display, MPU6050 &mpu, int wide=GxEPD_WIDTH, int height=GxEPD_HEIGHT);
  
  void begin();
  void update(const GPsData& gpsData);
  
};


#endif
