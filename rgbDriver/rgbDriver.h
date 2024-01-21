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
    void checkSensor();
    void sendPosition(Position pos, QueueHandle_t queue);
    Adafruit_TCS34725 setupSensor();
    private:
    void setupTimer();
    static void globalTimerCallback(TimerHandle_t xTimer);
    float from0_255To0_1(float value);
    HSV RGBToHSV(float red, float green, float blue);
    float getAnalyticX(float H);
    float getAnalyticY(float S);
    float getRealX(float H);
    float getRealY(float S, float H);   
    Position getPosition(float H, float S);
    bool newColorDetected();
    float calculateValue(float red, float green, float blue);
    float calculateMinimum(float red, float green, float blue);
    float calculateChroma(float V, float m);
    float calculateHue(float R, float G, float B, float V, float C);
    float calculateSaturation(float C, float V);
};
#endif
