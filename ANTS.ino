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
  int color = 1;
 AccelStepper stepper1;
 AccelStepper stepper2;
 TimerHandle_t operationTimer;
 TaskHandle_t xHandle;
 TaskHandle_t xHandleRgb;
 TaskHandle_t xHandleProgrammed;
 bool isThere = false;
 bool timerMotor = false;
 bool turning = false;


 rgbDriver rgb;
 
 

 
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);                               //Serial initialitation
    motore.init();                                    //Setup motors
    motore.thisSetting(motore.settings('a',300,100)); //setup default speed 
    motore.ahead();                                   //run
  
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
         int  flag;
     for(;;)
     {
    
     
       if(motore.isBraked() == false)   
       {            
           // motore.motorRun();
       }
         flag = xBrain.readPosition(rgb.getQueue()); 
         Serial.println(flag);
       if(flag == PURPLE_CARD)
       {        
        color =PURPLE_CARD;
        motore.thisSetting(motore.settings('a',300,300));
        motore.ahead();
        motore.motorRun();             
        }else if(flag == BLUE_CARD)
      {
                  color =BLUE_CARD;
                  motore.thisSetting(motore.settings('a',300,100));
                  motore.ahead();
                  motore.motorRun();
        }else if(flag == GREEN_CARD)
      {
                motore.getStepper(1).setCurrentPosition(0);
                color =GREEN_CARD;
                motore.turnAngle(0,100,90);               
        }else if(flag == RED_CARD)
      {
                  Serial.print("STOP");
                  color =RED_CARD;
                  motore.stopMotor();

        }else if(YELLOW_CARD)
      {
                color =YELLOW_CARD;
             //   Serial.print("GO");
                motore.ahead();
                motore.motorRun();           
        }else if(flag == PINK_CARD)
      {      
                 color =PINK_CARD;         
        }
        else if(flag == ORANGE_CARD)
      {      
                 color =ORANGE_CARD;        
        }
        else
      {
                 color =0;        
        }
     vTaskDelay(1);

  }
  
  }

   void taskRgb(void* pvParameters){ 
  
    //read colors and send to queue
    rgb.rgbRead();
    vTaskDelay(1);
   }

    void taskRun(void* pvParameters){ 
  
  for(;;)

    {
      motore.ahead();
      motore.motorRun();
     
     }
   }

    
