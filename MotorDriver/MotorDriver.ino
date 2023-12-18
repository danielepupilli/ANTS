#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
#include "queue.h"
#include "MotorDriver.h"


void task1(void *pvParameters)

{
  
}
void setup()
{
 
  xTaskCreate(task1, "taks", 100, NULL, 1, NULL);
 
 vTaskStartScheduler();
}

void loop(){}
