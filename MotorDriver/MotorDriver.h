#ifndef MotorDriver_H
#define MotorDriver_H
#include "Arduino.h"
#include "Arduino_FreeRTOS.h"
#include "Adafruit_MotorShield.h"
#include "AccelStepper.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "utils.h"

class MotorDriver

{
   
    public:

    void prova();
    settings_t settings( char operation, float speed, float acceleration);
    void ahead();    
    void stopMotor();
    void avoid();
    void motorRun();
    void back();
    void turn(int direction, float turnSpeed);
    void turnAngle(int direction, float turnSpeed,int angle);
    void operationCheck(Settings sett);
    void readPosition(QueueHandle_t xQueueP);
    void readSettings(QueueHandle_t xQueueP);
    void init();
    bool isBraked();
    bool isTurning();
    void prova2();
    void thisSetting(Settings sett);
    void setMove(float fMaxS, float fSpeedS, float sMaxS, float sSpeedS);
    char getOperation();
    void accAhead();
    AccelStepper getStepper(int stepperID);


    private:
    void setupTimer();
    static void globalTimerCallback(TimerHandle_t xTimer);
    
    
};
#endif
