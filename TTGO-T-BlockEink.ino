/*
* Based on the test code written by GxEPD / U8g2_for_Adafruit_GFX
* https://github.com/ZinggJM/GxEPD
* https://github.com/olikraus/U8g2_for_Adafruit_GFX
*
*/
#include "config.h"
#include "GpsI2c.h"
#include "Screan.h"

GpsI2c *gpsI2c=NULL;
Screan *screan=NULL;



TTGOClass *twatch = nullptr;
Button2 *btn = nullptr;
bool pwIRQ = false;
bool touch_vaild = false;


void sensorTask(void * pvParameters) {
  gpsI2c->begin(); 
  for(;;) {
    gpsI2c->loop();
    delayMicroseconds(10);
  }
}

void screanTask(void * pvParameters) {
  screan->begin();  
  for(;;) {
    screan->update(gpsI2c->getGpsData());
    delayMicroseconds(10);
  }
}

void setup()
{
    Serial.begin(115200);

    delayMicroseconds(5000);

    // Get watch object
    twatch = TTGOClass::getWatch();

    twatch->begin();
    
    //! Open gps power
    twatch->enableLDO3();


    btn = twatch->button;

    // Use compile time as RTC input time
    twatch->rtc->check();

    // Turn on power management button interrupt
    twatch->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);

    // Clear power interruption
    twatch->power->clearIRQ();

    // Set MPU6050 to sleep
    //twatch->mpu->setSleepEnabled(true);

    // Set Pin to interrupt
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        pwIRQ = true;
    }, FALLING);


    btn->setPressedHandler([]() {
        // esp_restart();
        touch_vaild = !touch_vaild;

        if (touch_vaild) {
            twatch->ePaper->fillScreen( GxEPD_WHITE);
            twatch->ePaper->update();
        } else {
            twatch->ePaper->fillScreen( GxEPD_WHITE);
            // TODO mainPage(true);
        }
    });

    btn->setDoubleClickHandler([]() {
        esp_restart();
    });


    // Reduce CPU frequency
    setCpuFrequencyMhz(80);
    gpsI2c = new GpsI2c(); 

    screan = new Screan(*twatch->ePaper, *twatch->mpu);


    // Sensors
    xTaskCreatePinnedToCore(
                    sensorTask,     // Function to implement the task 
                    "sensorTask",   // Name of the task 
                    4096,           // Stack size in words 
                    NULL,           // Task input parameter 
                    20,             // Priority of the task 
                    NULL,           // Task handle. 
                    1);             // Core where the task should run 

    // Screan
    xTaskCreatePinnedToCore(
                    screanTask,     // Function to implement the task 
                    "screanTask",   // Name of the task 
                    4096,           // Stack size in words 
                    NULL,           // Task input parameter 
                    20,             // Priority of the task 
                    NULL,           // Task handle. 
                    0);             // Core where the task should run 

}




void loop()
{
    btn->loop();
    if (pwIRQ) {
        pwIRQ = false;

        // Get interrupt status
        twatch->power->readIRQ();

        if (twatch->power->isPEKShortPressIRQ()) {
            //do something
        }
        // After the interruption, you need to manually clear the interruption status
        twatch->power->clearIRQ();
    }

    while (touch_vaild) {
        btn->loop();
        int16_t x, y;
        if (twatch->getTouch(x, y)) {
            Serial.printf("X:%d Y:%d\n", x, y);
            twatch->ePaper->fillCircle(x, y, 5, GxEPD_BLACK);
            twatch->ePaper->update();
        }
    }

}
