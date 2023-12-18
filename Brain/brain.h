#ifndef brain_H
#define brain_H
#include <stdio.h>
#include "Arduino_FreeRTOS.h"
#include "queue.h"
#include <SoftwareSerial.h>
#include "timers.h"
#include <MotorDriver.h>



class brain
{
  public:
  Settings chose();
  void sendSettings(QueueHandle_t xQueueSettings,Settings settings);
  void start();
  void setupTimer();
  int readPosition(QueueHandle_t xQueueP);
  
  private:

  Settings newSettings();




};
#endif