#ifndef rgbDriver_H
#define rgbDriver_H
#include "Arduino_FreeRTOS.h"
#include "timers.h" 
#include <Arduino.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>
#include "queue.h"
#include "utils.h"
typedef struct HSV
{
    float H;
    float S;
    float V;

} HSV;

class rgbDriver
{
    public:
    void rgbRead();
    QueueHandle_t getQueue();
    void checkSensor();
    void sendPosition(int  color, QueueHandle_t queue);
    int readPosition();
    int getColorFlag(Position p);
    Adafruit_TCS34725 setupSensor();
    private:
    void setupTimer();
    static void globalTimerCallback(TimerHandle_t xTimer);
    float from0_255To0_1(float value);
    bool newColorDetected();
    float calculateValue(float red, float green, float blue);
    float calculateMinimum(float red, float green, float blue);
   
};
#endif
