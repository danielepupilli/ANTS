#ifndef utils_H
#define utils_H
#include <math.h>
#include "queue.h"


#define YELLOW_CARD  1
#define ORANGE_CARD  2
#define PINK_CARD  3
#define RED_CARD  4
#define GREEN_CARD  5
#define PURPLE_CARD  6
#define BLUE_CARD  7


typedef struct settings_t
 {
    char operation;
    float maxS;
    float speedS;
  }   Settings;
  

typedef struct position_t
{
    int id;
    float R;
    float G;
    float B;

} Position;

static float toleranceRGB = 0.2;

const TickType_t readingInterval = pdMS_TO_TICKS(700);

const TickType_t sendingInterval = pdMS_TO_TICKS(700);

static void  setTolerance(float newTolerance)
{
  toleranceRGB = newTolerance;
}
 static bool cmp(float a, float b)
{
    return (fabs(a-b)< 0.005f);
}

static bool cmpSettings(Settings a, Settings b)
{
    bool flag = true;
   if(a.operation != b.operation)
   {
     flag = false;
   }
   else if(cmp(a.maxS,b.maxS)== false)
   {
     flag = false;
   }
   else if(cmp(a.speedS,b.speedS)== false)
   {
     flag == false;
   }

   return flag;
}
#endif