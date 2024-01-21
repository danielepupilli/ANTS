#include "MotorDriver.h"

Adafruit_StepperMotor *firstMotor;
Adafruit_StepperMotor *secondMotor;
// first motor handler
AccelStepper firstStepper;
//second motor handler
AccelStepper secondStepper;

//handler for task rgb
TaskHandle_t* rgbHandler;
//handler for task brain
TaskHandle_t* brainHandler;
//global timer
TimerHandle_t globalTimer;
//Semaphore 
SemaphoreHandle_t mutex;
// current ant position
Position currentPosition;
// current operation settings
Settings currentSettings;
bool globalExpired;
bool resumedTask;
// motor control flag
bool brake = false;

char MotorDriver::getOperation()
{
 return currentSettings.operation;
}
// return motor handler, used for testing
AccelStepper MotorDriver::getStepper(int id)
{
  if(id == 1)
  {
    return firstStepper;
  }
  else
  {
    return secondStepper;
  }
}


//set  maximum speed and speed for selected stepped
void MotorDriver::setMove(float fMaxS,float fSpeedS,float sMaxS,float sSpeedS)
{
  firstStepper.setMaxSpeed(fMaxS);
  firstStepper.setSpeed(fSpeedS);
  secondStepper.setMaxSpeed(sMaxS);
  secondStepper.setSpeed(sSpeedS);
         
}

//start motors
void MotorDriver::motorRun()
{
  if (brake == false)
  {
    firstStepper.run();
    secondStepper.run();  
  }
}
//setup global timer
void MotorDriver :: setupTimer() {
  globalTimer = xTimerCreate("Global_Timer",( 10000 / portTICK_PERIOD_MS ),  pdFALSE,( void * ) 0,globalTimerCallback );
}

//callback function for global Timer
void MotorDriver ::globalTimerCallback( TimerHandle_t xTimer ) {

  if(resumedTask == false && rgbHandler != NULL ) {
    vTaskResume( *rgbHandler );
    resumedTask = true;
  }
  globalExpired= true;
}
//method to setup first motor movement
void forward1()
{
  firstMotor ->onestep(FORWARD, DOUBLE);
}
//method to setup second motor movement
 void forward2()
{
  secondMotor->onestep(BACKWARD, DOUBLE);
}
//method to setup second motor movement
 void  backward2()
{
  secondMotor->onestep(FORWARD, DOUBLE);
}
//method to setup first motor movement
 void  backward1()
{
  firstMotor->onestep(BACKWARD, DOUBLE);
}
void MotorDriver :: prova2()
{
  ahead();
  if(firstStepper.currentPosition()>300)
  {
    currentSettings.speedS = currentSettings.speedS * -1;
  }
}


void MotorDriver:: prova()

{
    currentSettings.maxS =  500;
    currentSettings.speedS = 100;
    
     bool turning = false;
     while(turning == false) 
    {
        ahead();
      motorRun();
      if(firstStepper.currentPosition()>300)
      {
        turning = true;
        turn(1,300);
      }
     
     }   
           

  
}
// initialization
void MotorDriver :: init()
{
    Adafruit_MotorShield AFM(0x60);
    firstMotor = AFM.getStepper(200,1);
    secondMotor = AFM.getStepper(200,2);
    firstStepper = AccelStepper(forward1,backward1);
    secondStepper = AccelStepper(forward2,backward2);
    AFM.begin();
    
}
//check settings for instructions
void MotorDriver::operationCheck(Settings sett)
{
    switch (sett.operation)
    {
    case 'a':
      ahead();
      break;
    case 'b':
      avoid();
      break;
    case 'c':
      stopMotor();
      break;
      case 'l':
      turnAngle(1,100,90);
      break;
      case 'j':
      turn(1,100,45);
      break;
      case 'k':
      turn(0,100,45);
      break;
      case 'r':
      turn(0,100,90);
      break;
    default:
      break;
    }

}
void MotorDriver::accAhead()
{
  Serial.println("maxS:");
  Serial.println(currentSettings.maxS);
  firstStepper.moveTo(1000);
  firstStepper.setMaxSpeed(currentSettings.maxS);
  firstStepper.setAcceleration(10);  
  secondStepper.moveTo(1000);
  secondStepper.setMaxSpeed(currentSettings.maxS);
  secondStepper.setAcceleration(10);
  secondStepper.run();
  firstStepper.run();
   
}
// method for running straight
void MotorDriver::ahead()
{
   
    setMove(currentSettings.maxS,currentSettings.speedS,currentSettings.maxS,currentSettings.speedS);
    brake = false;
    motorRun();

}
bool MotorDriver::isBraked()
{
  return brake;
}
// reverse ant direction
void MotorDriver :: back()
{ 
      setMove(currentSettings.maxS, currentSettings.speedS *-1 , currentSettings.maxS, currentSettings.speedS * -1);
      brake = false;
      motorRun();
}
//method used for turning, requires "0/1" for direction, and turnSpeed to 
void MotorDriver :: turn(int direction,float turnSpeed)
{
    
    turnSpeed = turnSpeed + currentSettings.speedS;
    if (direction == 0)
    {

        setMove(currentSettings.maxS, currentSettings.speedS,currentSettings.maxS,turnSpeed );
        motorRun();
        return;
    }
    if (direction == 1)
    {
        setMove(currentSettings.maxS, turnSpeed,currentSettings.maxS,currentSettings.speedS);
        motorRun();
        return;
    }
    
}

void MotorDriver :: turnAngle(int direction,float turnSpeed,int angle)
{
    int steps = (angle*200)/360;
   turnSpeed = turnSpeed + currentSettings.speedS;
    if (direction == 0)
    {
      firstStepper.move(steps);
      secondStepper.moveTo(0);
      setMove(currentSettings.maxS, currentSettings.speedS,currentSettings.maxS,turnSpeed );
      motorRun();
      return;
    }
    if (direction == 1)
    {
      firstStepper.move(0);
      secondStepper.move(steps);
        setMove(currentSettings.maxS, turnSpeed,currentSettings.maxS,currentSettings.speedS);
        motorRun();
        return;
    }
}
//method to avoid collision, ants will turn right
void MotorDriver::avoid()
{
    turn(0,currentSettings.speedS);
    brake = false;
    motorRun();    
}

// set
Settings MotorDriver :: settings( char operation, float maxSpeed, float speed)
{
  Settings sett ={operation,maxSpeed,speed};
  return sett;
}
//method to stop the ant
void MotorDriver::stopMotor()
{
    brake = true;
}
//setup currentSettings
void MotorDriver::thisSetting(Settings sett)
{
  currentSettings = sett;
}
//read settings from queue
void MotorDriver::readSettings(QueueHandle_t xQueueP)
{

  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(700);
  
  Settings xReceivedSettings;
  while (1) {
    xStatus = xQueueReceive(xQueueP, &xReceivedSettings, xTicksToWait);
    if (xStatus == pdPASS)
    {
        currentSettings = xReceivedSettings;
        operationCheck(currentSettings);
    }
    else 
    {
      Serial.println("Queue is empty!");
    }
  }
}

// read current position from queue
void MotorDriver::readPosition(QueueHandle_t xQueueP)
{

  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(700);
  Position xReceivedPosition;
  while (1) {
    xStatus = xQueueReceive(xQueueP, &xReceivedPosition, xTicksToWait);
    if (xStatus == pdPASS) {
      
      if(cmpPosition(xReceivedPosition, currentPosition) == false)
      {
        currentPosition = xReceivedPosition;
      }
     
    }
    else {
      Serial.println("Queue is empty!");
    }
  }
}
