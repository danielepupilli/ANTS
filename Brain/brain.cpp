#include "brain.h"

int antNumber = 1;
Position antPos[10];
TimerHandle_t xGlobalTimer;
Settings s;
bool timerExpired = false;
bool isNewPos = false;
bool isNewSet = false;
//send position into queue
void brain::sendPosition(Position pos, QueueHandle_t xQueuePosition)
{
    BaseType_t xStatus;
    const TickType_t = pdMS_TO_TICKS(700);
    xStatus= xQueueSend(xQueuePosition, &pos,0);
    if(xStatus == pdPASS)
     {
         // Serial.println("new Settings sent!\n");
     }
     else
     {
        //Serial.println("ERROR! Impossible to send Position\n");
     }
    isNewPos == false; 

}
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

//read and store new positions
void brain::readPosition(QueueHandle_t xQueueP)
{

  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(700);
  Position xReceivedPosition;
  while (1) 
  {
    xStatus = xQueueReceive(xQueueP, &xReceivedPosition, xTicksToWait);
    if (xStatus == pdPASS) 
    {
      
        isNewPos == true;
        bool hasPos = false;
          
        for(int i=0; i< 10;i++)
        {
                if(antPos[i].id == xReceivedPosition.id )
                {
                    if(cmp(antPos[i].Y,xReceivedPosition.Y)==false || cmp(antPos[i].X,xReceivedPosition.X)==true)
                    {
                        antPos[i].X == xReceivedPosition.X;
                        antPos[i].Y == xReceivedPosition.Y;
                    }
                    hasPos == true;
                } 
        }
        if(hasPos == false)
        {
            antPos[antNumber + 1] = xReceivedPosition;
            antNumber++;
        }

        else 
        {
           // Serial.println("Queue is empty!"); 
        }
    }
  }
}


