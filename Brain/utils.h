#ifndef utils_H
#define utils_H
#include <math.h>
typedef struct settings_t
 {
    char operation;
    float maxS;
    float speedS;
  } Settings;
typedef struct position_t
{
    float X;
    float Y;
    int id;
    int R;
    int G;
    int B;

} Position;

static float toleranceRGB = 0.005f;


static void  setTolerance(float newTolerance)
{
  toleranceRGB = newTolerance;
}
 static bool cmp(float a, float b)
{
    return (fabs(a-b)< 0.005f);
}
static bool cmpPosition(Position a, Position b)
{
    return (cmp(a.X,b.X) && cmp(a.Y, b.Y) && a.id == b.id);
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