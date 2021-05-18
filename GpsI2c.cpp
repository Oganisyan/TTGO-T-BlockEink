#include "GpsI2c.h"

///////////////////////////////////////////////
// class  GPsData
///////////////////////////////////////////////

GPsData::GPsData() : location_(),date_(), time_(), speed_(), course_(), altitude_(), satellites_(), hdop_(),
    altitude2_(999.99), temperature_(999.99), pitch_(99.99), pressure_(0.), battPercentage_(0), acceleration_(9.8)
{}

TinyGPSLocation GPsData::getLocation() const {
 return location_;
}
TinyGPSDate     GPsData::getDate() const{
 return date_;
}
TinyGPSTime     GPsData::getTime() const{
 return time_;
}
TinyGPSSpeed    GPsData::getSpeed() const{
 return speed_;
}
TinyGPSCourse   GPsData::getCourse() const{
 return course_;
}
TinyGPSAltitude GPsData::getAltitude() const{
 return altitude_;
}
TinyGPSInteger  GPsData::getSatellites() const{
 return satellites_;
}
TinyGPSDecimal  GPsData::getHdop() const{
 return hdop_;
}

double     GPsData::getAltitude2() const  {
 return   altitude2_;
}
double     GPsData::getTemperature() const  {
 return   temperature_;
}
double     GPsData::getPitch() const  {
 return   pitch_;
}
double      GPsData::getPressure() const  {
 return   pressure_;
}
int      GPsData::getBattPercentage() const {
  return battPercentage_;
}

double    GPsData::getAcceleration() const {
  return acceleration_;
}

///////////////////////////////////////////////
// class  GpsI2c
///////////////////////////////////////////////

GpsI2c::GpsI2c() : MS5611Buzzer(), Power(*TTGOClass::getWatch()), TinyGPSPlus(), mpu_(*TTGOClass::getWatch()->mpu), mpuFilter_(),
    gpsSerial_(1), d_(), qnh_(1013.25F), accCalib_(), gpsLoopMillis_(0)
{
  accCalib_.XAxis = -0.40F;  
  accCalib_.YAxis = 0.13F;  
  accCalib_.ZAxis = -0.62F;  
}

void GpsI2c::begin(){
  MS5611Buzzer::begin();
  MS5611Buzzer::setEnableDbg(true);
  Power::begin();
  gpsLoopMillis_ = millis();
  gpsSerial_.begin(9600, SERIAL_8N1, 33/* RX dummy */, 34 /* TX Workoround*/); //
}

float VarioMSCalculation(float altitude);

void GpsI2c::fixQNH() {
/* TODO  if(d_.altitude_ != INVALID_NIT32 &&  abs(d_.altitude_/100 - d_.altitude2_) >10 ) {
    qnh_ = MS5611::getQNH(((float)d_.altitude_)/100.F);
  } */
}

bool GpsI2c::gpsHandler() {
  for(int i=0; i < 88 && gpsSerial_.available(); i++) {
    if (TinyGPSPlus::encode(gpsSerial_.read())) {
      return true;
    }
  }
  return false;
}


void GpsI2c::loop() {
  MS5611Buzzer::loop();

  if(isReady()) {
    d_.altitude2_    = MS5611Buzzer::getAltitude(qnh_);
    d_.temperature_  = MS5611Buzzer::getTemperature();
    d_.pressure_     = MS5611Buzzer::getPressure();
    d_.pitch_        = VarioMSCalculation(d_.altitude2_);
    Vector accV      = mpu_.readNormalizeAccel();
    accV.XAxis +=accCalib_.XAxis;
    accV.YAxis +=accCalib_.YAxis;
    accV.ZAxis +=accCalib_.ZAxis;
    
    //Serial.print(accV.XAxis); Serial.print("\t"); Serial.print( accV.YAxis ); Serial.print("\t"); Serial.println( accV.ZAxis );
    double acc = sqrtf(accV.XAxis * accV.XAxis + accV.YAxis * accV.YAxis + accV.ZAxis * accV.ZAxis);    
    mpuFilter_.update(acc);
    d_.acceleration_ = mpuFilter_.get(); 
  }
  if (millis() - gpsLoopMillis_ > 1000) {
    gpsLoopMillis_ = millis();
    Power::loop();
    d_.battPercentage_ = Power::getBattPercentage();
      
  }
  if(gpsHandler()) {
    if (TinyGPSPlus::location.isValid()) {
      d_.location_  = TinyGPSPlus::location;
    }
    if (TinyGPSPlus::date.isValid()) {
      d_.date_ = TinyGPSPlus::date;
    }
    if (TinyGPSPlus::time.isValid()) {
      d_.time_ = TinyGPSPlus::time;
    }
    if (TinyGPSPlus::speed.isValid()) {
      d_.speed_ = TinyGPSPlus::speed;
    }
    if (TinyGPSPlus::course.isValid()) {
      d_.course_ = TinyGPSPlus::course;
    }
    if (TinyGPSPlus::altitude.isValid()) {
      d_.altitude_ = TinyGPSPlus::altitude;
    }
    if (TinyGPSPlus::satellites.isValid()) {
      d_.satellites_ = TinyGPSPlus::satellites;
    }
    if (TinyGPSPlus::hdop.isValid()) {
      d_.hdop_ = TinyGPSPlus::hdop;
    }
  }
  
}

const GPsData& GpsI2c::getGpsData() const {
  return d_;  
}



// Nicht mein Code
#define MAX_SAMPLES 10
byte samples = 8;    
float alt[MAX_SAMPLES+1] = {0.F,};
float tim[MAX_SAMPLES+1] = {0.F,};
float vario = 0;    
float N1 = 0;                       // Variable zur Mittelwertbildung                                 
float N2 = 0;                       // Variable zur Mittelwertbildung                                
float N3 = 0;                       // Variable zur Mittelwertbildung                                 
float D1 = 0;                       // Variable zur Mittelwertbildung                               
float D2 = 0;                       // Variable zur Mittelwertbildung       

// Variofunktion FROM ANDREI's Project https://www.instructables.com/id/DIY-Arduino-Variometer-for-Paragliding
float VarioMSCalculation(float altitude){
  N1 = 0;                                                                 // Variable zur Mittelwertbildung
  N2 = 0;                                                                 // Variable zur Mittelwertbildung
  N3 = 0;                                                                 // Variable zur Mittelwertbildung
  D1 = 0;                                                                 // Variable zur Mittelwertbildung
  D2 = 0;                                                                 // Variable zur Mittelwertbildung
  
  ///// ÐžÐ‘Ð�Ð£Ð›Ð•Ð�Ð˜Ð• Ð’Ð�Ð Ð˜ÐžÐœÐ•Ð¢Ð Ð� / ZERO VARIO /////
  vario = 0;       
  
  
  for(int cc=1; cc<=MAX_SAMPLES; cc++)                                     // Averager 
  {                                                                       // 
    alt[(cc-1)] = alt[cc];                                                // http://www.instructables.com/id/GoFly-paraglidinghanglidinggliding-altimeter-v/?ALLSTEPS
    tim[(cc-1)] = tim[cc];                                                // http://redhats.ru/variometer-arduino-2015/
  }                                                                       //
                                                                          //
  alt[MAX_SAMPLES] = altitude;                                            //
  tim[MAX_SAMPLES] = millis();                                            //
  float stime = tim[MAX_SAMPLES-samples];                                 //
                                                                          //
  for(int cc=(MAX_SAMPLES-samples); cc<MAX_SAMPLES; cc++)                   //
  {                                                                       //
    N1+=(tim[cc]-stime)*alt[cc];                                          //
    N2+=(tim[cc]-stime);                                                  //
    N3+=(alt[cc]);                                                        //
    D1+=(tim[cc]-stime)*(tim[cc]-stime);                                  //
    D2+=(tim[cc]-stime);                                                  //
  }                                                                       // Durchschnittliches KÃ¶rperende
  
  /////VARIO VALUES CALCULATING /////
  vario=1000*((samples*N1)-N2*N3)/(samples*D1-D2*D2);                     // BERECHNUNG VON VARIOMETERWERTEN

  //Serial.print("Vario Russ: ");
  //Serial.println(vario);
  return vario;
}
