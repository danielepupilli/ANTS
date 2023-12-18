
#include "rgbDriver.h"
TaskHandle_t* moveHandler;
//glonal timer handler
TimerHandle_t rgbGlobalTimer;
// positions queue handler
QueueHandle_t xQueueP;
Adafruit_TCS34725 tcs;                                                  // board height in cm
float previousRed, previousGreen, previousBlue;
float red, green, blue = 0;
BaseType_t xStatus;
bool rgbResumedTask;
bool rgbGlobalExpired;
/* setup sensor, integration time values (TCS34725_INTEGRATIONTIME_2_4MS = 0xFF,
                                          TCS34725_INTEGRATIONTIME_24MS = 0xF6,
                                          TCS34725_INTEGRATIONTIME_50MS = 0xEB,                                         
                                          TCS34725_INTEGRATIONTIME_101MS = 0xD5,
                                          TCS34725_INTEGRATIONTIME_154MS = 0xC0,
                                          TCS34725_INTEGRATIONTIME_700MS = 0x00)
                 gain values             (TCS34725_GAIN_1X = 0x00, no gain,
                                          TCS34725_GAIN_4X = 0x01, /**< 2x gain,
                                          TCS34725_GAIN_16X = 0x02, /**< 16x gain,
                                          TCS34725_GAIN_60X = 0x03 /**< 60x gain)                                          
                                          
                                          */
Adafruit_TCS34725 rgbDriver ::setupSensor()
{
    tcs34725IntegrationTime_t integrationTime = TCS34725_INTEGRATIONTIME_700MS;
    tcs34725Gain_t gain = TCS34725_GAIN_4X;
    tcs = Adafruit_TCS34725(integrationTime, gain);

    //setup coda per posizioni

    xQueueP = xQueueCreate( 6, sizeof(int) );
}
//value conversion
float rgbDriver::from0_255To0_1(float value) {
  return value / 255.0;
}
//value calculate max value
 float rgbDriver :: calculateValue(float red, float green, float blue) {
  return max(max(red, green), blue);
}
//valure calculate min value
float rgbDriver ::calculateMinimum(float red, float green, float blue) {
  return min(min(red, green), blue);
}


//return queue heandler

QueueHandle_t rgbDriver::getQueue()
{
return xQueueP;
}



//check for new color detection
bool rgbDriver :: newColorDetected() {
if(red < 10  || blue < 10 ||green < 10  )
  {
    return false;
  }
 if(red > 250 || blue >250 || green > 250)
 {
  return false;
 } 
  if (abs(previousRed - red) >= 5) return true;
  if (abs(previousGreen - green) >= 5) return true;
  if (abs(previousBlue - blue) >= 5) return true;
 
  return false;
}
//check for Sensor status
void rgbDriver ::checkSensor()
{
  while (1) {
    if (tcs.begin()) {
      tcs.clearInterrupt();
      Serial.println("Sensor found!");
      Serial.println();
      break;
    }
    else {
      Serial.println("No TCS34725 found, waiting for the connection..");
    }
  } }

  void rgbDriver ::rgbRead()
{
    setupSensor();
    for(;;)
    {
      tcs.getRGB(&red, &green, &blue);
   
    if(newColorDetected())
        {
          /*Serial.print(red); Serial.print("\n");Serial.print(blue); Serial.print("\n");Serial.print(green);Serial.print("\n");*/
            previousRed = red;
            previousGreen = green;
            previousBlue = blue;
            Position currentPosition;
            currentPosition.id = 1;
            currentPosition.R = red;
            currentPosition.G = green;
            currentPosition.B = blue;

            if (getColorFlag(currentPosition) != 0){         
              sendPosition(getColorFlag(currentPosition),xQueueP);
            }
     
            vTaskDelay(1);
        }
    }
}

int rgbDriver::readPosition()

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


//send ant position to queue
void rgbDriver::sendPosition(int pos, QueueHandle_t xQueueP)

{

    xStatus = xQueueSend(xQueueP,&pos,0);
   
    if(xStatus == pdPASS)
    {
      Serial.print("SENDING:");
      Serial.print(pos); 

    }else
    {
      Serial.print("Errore nell' invio posizione\n");
    }

}

//returns 0 if no significant color is found
//returns the flag of the specific color
int rgbDriver :: getColorFlag(Position currentPosition)
{

    if(currentPosition.R > 115 && currentPosition.R<125 && currentPosition.G > 55 && currentPosition.G<65  && currentPosition.B > 80 && currentPosition.B<90)

    {       
        return PURPLE_CARD;
    }else if(currentPosition.R > 60 && currentPosition.R<70 && currentPosition.G > 72 && currentPosition.G<83  && currentPosition.B > 111 && currentPosition.B<125)
    {
      return BLUE_CARD;
    }else if(currentPosition.R > 82 && currentPosition.R<100 && currentPosition.G > 100 && currentPosition.G<110  && currentPosition.B > 56 && currentPosition.B<65)
    {    
      return GREEN_CARD;
    }else if(currentPosition.R > 162 && currentPosition.R<172 && currentPosition.G > 45 && currentPosition.G<55  && currentPosition.B > 45 && currentPosition.B<55)
    {    
      return RED_CARD;
    }else if(currentPosition.R > 105 && currentPosition.R<117 && currentPosition.G > 91 && currentPosition.G<101  && currentPosition.B > 41 && currentPosition.B<51)
    {     
      return YELLOW_CARD;
    }else if(currentPosition.R > 113 && currentPosition.R<123 && currentPosition.G > 54 && currentPosition.G<64  && currentPosition.B > 78 && currentPosition.B<88)
    {     
        return PINK_CARD;
    }else if(currentPosition.R > 143 && currentPosition.R<153 && currentPosition.G > 79 && currentPosition.G<89  && currentPosition.B > 52 && currentPosition.B<62)
    {     
      return ORANGE_CARD;
    }else
    {
      return 0;
    }

}
 //setup global timer
void rgbDriver :: setupTimer() {
  rgbGlobalTimer = xTimerCreate("Global_Timer",( 10000 / portTICK_PERIOD_MS ),  pdFALSE,( void * ) 0, globalTimerCallback );
}
//callback function for global Timer
void rgbDriver ::globalTimerCallback( TimerHandle_t xTimer ) {

  if(rgbResumedTask == false && moveHandler != NULL ) {
    vTaskResume( *moveHandler );
    rgbResumedTask = true;
  }
  rgbGlobalExpired= true;
}