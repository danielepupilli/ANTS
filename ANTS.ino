#include "Adafruit_TCS34725.h"
#include <Adafruit_MotorShield.h>
#include <rgbDriver.h>
#include <MotorDriver.h>
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "queue.h"
#include <brain.h>
#include <stdio.h>
#include <SoftwareSerial.h>
#include <math.h>
#include "utils.h"
#include "MotorDriver.h"


 QueueHandle_t xQueuePosition;
 QueueHandle_t xQueueSettings;
 QueueHandle_t xQueueRgb;
 Position p;
 brain xBrain;
 MotorDriver motore;
 Settings sett = {'a', 300,200};
 AccelStepper stepper1;
 AccelStepper stepper2;
 TimerHandle_t operationTimer;
 TaskHandle_t xHandle;
 TaskHandle_t xHandleRgb;
 TaskHandle_t xHandleProgrammed;
 bool isThere = false;
 bool timerMotor = false;
 bool turning = false;
 int flag = PURPLE_CARD;
 int color;


 rgbDriver rgb;
 
 

 
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);                               //Serial initialitation
    motore.init();                                    //Setup motors
    motore.thisSetting(motore.settings('a',300,100)); //setup default speed 
    motore.ahead();                               //run
    rgb.setupSensor();
      
  //Tasks creation
  xTaskCreate(taskRgb,"read",128,NULL,1,&xHandleRgb);
  xTaskCreate(taskProgrammedRun,"programmed",128,NULL,1,&xHandleProgrammed);
 
  vTaskStartScheduler();
}


void loop() {
  
   
}
  void taskProgrammedRun(void *pvParameters)

  {      
    motore.ahead();
    motore.motorRun();
     
     for(;;)
     {      
      if(color == PURPLE_CARD)
         {
         motore.stopMotor();
          motore.thisSetting(motore.settings('a',300,300));
          motore.ahead();
          motore.motorRun();
          vTaskDelay(1);        
         }
      else if(color == YELLOW_CARD) 
      {
        motore.getStepper(1).setCurrentPosition(0);
        motore.turnAngle(0,100,90);
        motore.motorRun();
         vTaskDelay(1);
          
      }  else if(color == GREEN_CARD)
      {
        motore.getStepper(1).setCurrentPosition(0);
        motore.getStepper(0).setCurrentPosition(0);
        motore.stopMotor();
        vTaskDelay(1);
        
        } else if(color == BLUE_CARD)
      {
        motore.getStepper(0).setCurrentPosition(0);
        motore.turnAngle(1,100,90);
        motore.motorRun();
         vTaskDelay(1);
        }

  }
  
  }

   void taskRgb(void* pvParameters){ 
  
    //read colors and send to queue


     for(;;)

    {
     rgb.rgbRead();

     flag = xBrain.readPosition(rgb.getQueue()); 
     
     if(flag != 0)
     {
      color = flag;
     
     }
     vTaskDelay(200);
    }
   }
