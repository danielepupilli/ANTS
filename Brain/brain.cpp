#include "brain.h"

int antNumber = 1;
TimerHandle_t xGlobalTimer;
Settings s;
bool timerExpired = false;
bool isNewSet = false;

//TODO implementare operazioni logiche
void start()
{
    //start comunication
    //start motors
    //start logic
}
void brain::sendSettings(QueueHandle_t xQueueSettings, Settings sett)
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(700);
    xStatus= xQueueSend(xQueueSettings, &sett,0);
        if(xStatus == pdPASS)
        {
         // Serial.println("new Settings sent!");
        }
    isNewSet = false;
}
//callback fucntion
void globalTimerCallback(TimerHandle_t xTimer)
{      
    //Serial.println("Global Timer Expired\n");
    timerExpired = true;


}

int brain::readPosition(QueueHandle_t xQueueP)

{
    BaseType_t xStatus;
  
    int  xReceivedColor;

    if(uxQueueMessagesWaiting!=0)    
    {
    xStatus = xQueueReceive(xQueueP, &xReceivedColor,readingInterval);
    if(xStatus == pdPASS)
    {
      
      return xReceivedColor;
      vTaskDelay(1); 
    }else
    {
      vTaskDelay(1); 
      return 0;
    }
    
    }
    vTaskDelay(1); 
    return 0;
    
}


//set random instruction
Settings brain ::chose()
{
    
    int num = random(1,6);
    
    if(num == 1)
    {
        s.operation = 'a';
    }
      if(num == 2)
    {
        s.operation = 'b';
    }
      if(num == 3)
    {
        s.operation = 's';
    }
      if(num == 4)
    {
        s.operation = 'l';
    }
      if(num == 3)
    {
        s.operation = 'r';
    }
    return  s;
    
}
//setup timer
void brain::setupTimer()

{   
    xGlobalTimer = xTimerCreate(
        "Global_Timer",               /* A text name, purely to help debugging. */
            ( pdMS_TO_TICKS(1000)),  /* The timer period. */
		    pdFALSE,						          /* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
		    ( void * ) 0,				          /* The ID is not used, so can be set to anything. */
		    globalTimerCallback           /* The callback function that inspects the status of all the other tasks. */
  );

}



