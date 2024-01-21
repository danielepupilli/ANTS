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
  void sendPosition(Position pos,QueueHandle_t xQueuePosition);
  void readPosition(QueueHandle_t xQueuePosition);
  void start();
  void setupTimer();
  
  private:

  Settings newSettings();




};
#endif