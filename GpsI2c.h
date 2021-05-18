#ifndef __GPS_I2C__
#define __GPS_I2C__
#include "config.h"
#include "MS5611Buzzer.h"
#include "Power.h"

#define  SEND_FLAG      0x80

#define  REQ_LATITUDE   0x00
#define  REQ_LONGITUDE  0x01
#define  REQ_FIXAGE     0x02
#define  REQ_ALTITUDE   0x03  
#define  REQ_TIME       0x04
#define  REQ_DATE       0x05
#define  REQ_SPEED      0x06
#define  REQ_CURSE      0x07


#define  INVALID_UNIT32 0xFFFFFFFF
#define  INVALID_NIT32  0x7FFFFFFF

class GpsI2c;

class GPsData {

  TinyGPSLocation   location_;
  TinyGPSDate       date_;
  TinyGPSTime       time_;
  TinyGPSSpeed      speed_;
  TinyGPSCourse     course_;
  TinyGPSAltitude   altitude_;
  TinyGPSInteger    satellites_;
  TinyGPSDecimal    hdop_;

  double     altitude2_;
  double     temperature_;
  double     pitch_;
  double     pressure_;
  int        battPercentage_;
  
  double     acceleration_;
  
  friend     GpsI2c;

public:
  GPsData();
  TinyGPSLocation getLocation() const;
  TinyGPSDate     getDate() const;
  TinyGPSTime     getTime() const;
  TinyGPSSpeed    getSpeed() const;
  TinyGPSCourse   getCourse() const;
  TinyGPSAltitude getAltitude() const;
  TinyGPSInteger  getSatellites() const;
  TinyGPSDecimal  getHdop() const;;

  double      getAltitude2() const;
  double      getTemperature() const;
  double      getPitch() const;
  double      getPressure() const;

  double      getAcceleration() const;
  
  int         getBattPercentage() const;

};

class GpsI2c : MS5611Buzzer, Power, TinyGPSPlus { // @suppress("Class has a virtual method and non-virtual destructor")
  MPU6050         &mpu_;
  KalmanFilter    mpuFilter_;
  HardwareSerial  gpsSerial_;
  GPsData         d_;
  float           qnh_;
  Vector          accCalib_;
  uint32_t        gpsLoopMillis_;
  void            fixQNH();
  bool            gpsHandler();
  
public: 
  GpsI2c();
  void  begin();
  void  loop();
  const GPsData &getGpsData() const;
};


#endif
