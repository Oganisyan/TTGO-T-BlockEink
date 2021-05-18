#include "Screan.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;


float yd[] = {
  0.50F, 0.54F, 0.58F, 0.62F, 0.66F, 0.70F, 0.74F, 0.78F, 0.82F, 0.86F, 0.90F, 0.94F,
  0.02F, 0.06F, 0.10F, 0.14F, 0.18F, 0.22F, 0.26F, 0.30F, 0.34F, 0.38F, 0.42F, 0.46F
};


float xd[] = {
  0.000023F, 0.000823F, 0.002768F, 0.005864F, 0.010117F, 0.015538F, 0.022141F, 0.029943F, 0.038965F, 0.046483F, 0.054000F, 0.061518F, 
  0.061518F, 0.054000F, 0.046483F, 0.038965F, 0.029943F, 0.022141F, 0.015538F, 0.010117F, 0.005864F, 0.002768F, 0.000823F, 0.000023F
};

Screan::Screan(GxEPD_Class &display, MPU6050 &mpu, int wide, int height) : display_(display), mpu_(mpu),
  wide_(wide), height_(height)
{
}

void Screan::begin() {
// Debug on Serial  display_.init(115200);
  display_.fillScreen(GxEPD_WHITE);
  

  display_.fillCircle(1.88*height_,   height_*0.5, 1.8*height_, GxEPD_BLACK);  
  display_.fillCircle(1.88*height_+2, height_*0.5, 1.8*height_, GxEPD_WHITE);  

  display_.fillRect(0, 0, 2, height_, GxEPD_BLACK);
  
  display_.fillRect(0, 0, wide_, 0.02*height_-2, GxEPD_WHITE);
  display_.fillRect(0, 0.96*height_+2, wide_, height_, GxEPD_WHITE);
  for(int i=0; i < APV_LINE_COUNT; i++) {
    yPos[i] = yd[i]*height_;
    xPos[i] = xd[i]*wide_;  
  }
  

  display_.setTextColor(GxEPD_BLACK , GxEPD_WHITE);
 
  initTablo(0, "Time:");
  initTablo(2, "Alt:m");
  initTablo(3, "Sp:km/h");
  initTablo(4, "VSp:m/s");
  initTablo(6, "Rato:");
  initTablo(7, "Acc:m/s^2");

   u8g2Fonts.begin(display_);
  //BATTERY ICON
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);  // apply Adafruit GFX color
  display_.update();  
}

void Screan::update(const GPsData& gpsData){
  updateBatary(gpsData.getBattPercentage());

  double speed = constrain(gpsData.getSpeed().kmph(), -99.9, 99.9);
  float pitch = constrain(gpsData.getPitch(), -99.9F, 99.9F);
  float altitude = constrain(gpsData.getAltitude2(), -9999.F, 9999.F);
  int scala = pitch*30;
  scala = constrain(scala, -120, 120)/10;
  
  for(int i=0; i < 24; i++) {
    uint16_t color = (scala < -i) ? 
        GxEPD_BLACK : (scala < (24-i)) ? GxEPD_WHITE : GxEPD_BLACK;
    display_.fillRect(4, yPos[i], 14+xPos[i]-4, height_*0.032F, color);
  }

  display_.fillTriangle(wide_*0.10F, height_*0.51F, wide_*0.14F, height_* 0.61F , wide_*0.18F, height_*0.51F,  pitch > -0.09F ? GxEPD_WHITE : GxEPD_BLACK);
  display_.fillTriangle(wide_*0.10F, height_*0.49F, wide_*0.14F, height_* 0.39F , wide_*0.18F, height_*0.49F,  pitch <  0.09F ? GxEPD_WHITE : GxEPD_BLACK);
  display_.drawTriangle(wide_*0.10F, height_*0.51F, wide_*0.14F, height_* 0.61F , wide_*0.18F, height_*0.51F,  GxEPD_BLACK);
  display_.drawTriangle(wide_*0.10F, height_*0.49F, wide_*0.14F, height_* 0.39F , wide_*0.18F, height_*0.49F,  GxEPD_BLACK);

  float ratio = 9.6F;
  updateTablo(false, 0, 2, "%02d:%02d:%02d", gpsData.getTime().hour(), gpsData.getTime().minute(), gpsData.getTime().second());
  updateTablo(false, 2, 2, "%4d", (int)abs(altitude)); 
  updateTablo(false, 3, 2, "%2d", (int)abs(speed)); 
  updateTablo(false, 4, 3, abs(pitch) < 10.F ? " %.1f" : "%.1f", abs(pitch));
  updateTablo(false, 6, 2, abs(ratio) < 10.F ? " %.1f" : "%.1f", abs(ratio));
  updateTablo(false, 7, 2, gpsData.getAcceleration() < 10.F ? " %.1f" : "%.2f",   gpsData.getAcceleration()); 
  
  updateTablo(false, 8, 1, "%s%0.6f  ", gpsData.getLocation().lat() >= 0. ? "N " : "S ", abs(gpsData.getLocation().lat()));
  updateTablo(false, 9, 1, "%s%0.6f  ", gpsData.getLocation().lng() >= 0. ? "E " : "W ", abs(gpsData.getLocation().lng()));

  display_.updateWindow(0,0,wide_,height_, false);
}

void Screan::initTablo(int nr, char *info) {
   //Altitude
  int x = nr%2;
  int y = nr/2;
  x = (wide_* (0.2F+x*0.4F));
  y = (height_* (y*0.2F+0.03F));
    
  //display_.drawRect(x, y, wide_*0.4F, height_* 0.2F, GxEPD_BLACK);
  display_.setTextSize(1.5);
  display_.setCursor(x+wide_ * 0.01F, y - height_* 0.03F);
  display_.printf(info);
}

void Screan::updateBatary(int battPercentage) {
  //BATTERY ICON
  int level = battPercentage = constrain(battPercentage+10, 10, 110)/20; // 0,1,2,3,4,5
  
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);  // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_battery19_tn);
  u8g2Fonts.setCursor(175, 20);                // start writing at this position
  u8g2Fonts.setFontDirection(3);              // left to right (this is default)
  u8g2Fonts.print(level);
  u8g2Fonts.setFontDirection(0);              // left to right (this is default) 

}

void Screan::updateTablo(bool debug, int nr, int txtSize, const char *fmt, ...) {
  char buffer[256]={0,};
  va_list argp;
  
  int x = nr%2;
  int y = nr/2;
  x = (wide_* (0.2F+x*0.4F));
  y = (height_* (y*0.2F+0.03F + (5-txtSize)*0.022F));
  display_.setTextSize(txtSize);
  display_.setCursor(x,y);
  va_start(argp, fmt);
  vsprintf(buffer,fmt, argp);
  va_end(argp);
#if 0  
        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312a/*u8g2_font_inr38_mn*/); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        //ePaper->fillRect(lastX, lastY - u8g2Fonts.getFontAscent() - 3, lastW, lastH, GxEPD_WHITE);
        //ePaper->fillScreen(GxEPD_WHITE);
        display_.setTextColor(GxEPD_BLACK);
        int w = u8g2Fonts.getUTF8Width(buffer);
        int h = u8g2Fonts.getFontAscent();
        u8g2Fonts.setCursor(x, y);
        u8g2Fonts.print(buffer);
        display_.updateWindow(x, y - h, w, h, false);
#else
  display_.print(buffer); 
#endif  
  if(debug) {
    Serial.println(buffer);
  }
}
