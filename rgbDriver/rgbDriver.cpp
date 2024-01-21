
#include "rgbDriver.h"
TaskHandle_t* moveHandler;
//glonal timer handler
TimerHandle_t rgbGlobalTimer;
// positions queue handler
QueueHandle_t xQueueP;
Adafruit_TCS34725 tcs;

float minHue;
float maxHue;
float minSaturation;
float maxSaturation;
float width;                                                      //board width in cm    
float height;                                                     // board height in cm
float previousRed, previousGreen, previousBlue;
float red, green, blue;


BaseType_t xStatus;
const TickType_t xTicksToWait = pdMS_TO_TICKS(700);
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

//calculate chroma
float rgbDriver::calculateChroma(float V, float m) {
  return V - m;
}
//calculate Hue
float rgbDriver::calculateHue(float R, float G, float B, float V, float C) {

  if (C == 0) return 0.0;

  float hue;

  if (R == V) {
    hue = G - B;
  }
  else if (G == V) {
    hue = 2.0 * C + B - R;
  }
  else if (B == V) {
    hue = 4.0 * C + R - G;
  }

  hue = hue / (C * 6.0);

  if (hue == 1.0) {
    hue = 0.0;
  } else if (hue < 0.0) {
    hue = hue + 1.0;
  }

  return hue;
}

//calculate Saturation
float rgbDriver:: calculateSaturation(float C, float V) {
  if (V == 0) return 0;
  else return C / V;
}

//conversion from  RGB to HSV values
HSV rgbDriver::RGBToHSV(float red, float green, float blue) {
  float R = from0_255To0_1(red);
  float G = from0_255To0_1(green);
  float B = from0_255To0_1(blue);

  float Value = calculateValue(R, G, B);
  float minimum = calculateMinimum(R, G, B);
  float Chroma = calculateChroma(Value, minimum);
  float Hue = calculateHue(R, G, B, Value, Chroma);
  float Saturation = calculateSaturation(Chroma, Value);

  return {Hue, Saturation, Value};
}

float rgbDriver::getAnalyticX(float H) {
  return ((H - minHue) * width) / (maxHue - minHue);
}

float rgbDriver::getAnalyticY(float S) {
  return ((S - minSaturation) * height) / (maxSaturation - minSaturation);
}

float rgbDriver::getRealX(float H) {
  float x = -1;

  if (H <= 0.3731) {
    x = + 154352 * pow(H, 6)
        - 111687 * pow(H, 5)
        + 18167 * pow(H, 4)
        + 4852.9 * pow(H, 3)
        - 2118.7 * pow(H, 2)
        + 425.59 * H
        - 4.4536;
  } else if (H <= 0.4551) {
    x = - 578.73 * pow(H, 2)
        + 850.64 * H
        - 161.02;
  } else if (H <= 0.5548) {
    x =  + 8714.6 * pow(H, 3)
         - 11991 * pow(H, 2)
         + 5798.6 * H
         - 870.58;
  } else  {
    x = + 2822.8 * pow(H, 3)
        - 6522.9 * pow(H, 2)
        + 5095.7 * H
        - 1157.8;
  }

  if (x > width) {
    x = x - width;
  }

  return abs(x);
}

float rgbDriver::getRealY(float S, float H) {
  float y = -1;

  if (H <= 0.009) {
    y = + 52280.42 * pow(S, 6)
        - 131029.79 * pow(S, 5)
        + 134657.29 * pow(S, 4)
        - 71723.29 * pow(S, 3)
        + 20653.10 * pow(S, 2)
        - 2636.16 * S
        + 112.12;
  } else if (H <= 0.061) {
    y = - 165841.76 * pow(S, 6)
        + 380816.69 * pow(S, 5)
        - 348460.43 * pow(S, 4)
        + 162022.11 * pow(S, 3)
        - 40331.67 * pow(S, 2)
        + 5541.20 * S
        - 323.93;

  } else if (H <= 0.135) {
    y = + 181006.97 * pow(S, 6)
        - 337390.34 * pow(S, 5)
        + 256697.69 * pow(S, 4)
        - 101882.88 * pow(S, 3)
        + 22042.86 * pow(S, 2)
        - 1932.86 * S
        + 37.04;

  } else if (H <= 0.214) {
    y = - 1698484.47 * pow(S, 6)
        + 2817951.06 * pow(S, 5)
        - 1842576.78 * pow(S, 4)
        + 598966.02 * pow(S, 3)
        - 99953.47 * pow(S, 2)
        + 8523.25 * S
        - 316.22;

  } else if (H <= 0.311) {
    y = + 2506828.50 * pow(S, 6)
        - 4360420.65 * pow(S, 5)
        + 3150518.80 * pow(S, 4)
        - 1198145.49 * pow(S, 3)
        + 250124.07 * pow(S, 2)
        - 26180.23 * S
        + 1055.71;

  } else if (H <=  0.383) {
    y = + 211245477.06 * pow(S, 6)
        - 343088782.92 * pow(S, 5)
        + 229756594.00 * pow(S, 4)
        - 81216321.53 * pow(S, 3)
        + 15988396.81 * pow(S, 2)
        - 1661768.11 * S
        + 71216.26;

  } else if (H <=  0.418) {
    y = - 414746824.09 * pow(S, 6)
        + 779219578.14 * pow(S, 5)
        - 605619118.90 * pow(S, 4)
        + 249333799.32 * pow(S, 3)
        - 57368135.07 * pow(S, 2)
        + 6996999.40 * S
        - 353571.61;

  } else if (H <= 0.440) {
    y = + 4774781.51 * pow(S, 6)
        - 23565616.46 * pow(S, 5)
        + 30970306.32 * pow(S, 4)
        - 18491914.24 * pow(S, 3)
        + 5697556.53 * pow(S, 2)
        - 885087.76 * S
        + 54985.01;

  } else if (H <= 0.465) {
    y = - 50157813.74 * pow(S, 6)
        + 116314037.24 * pow(S, 5)
        - 109739536.84 * pow(S, 4)
        + 54142591.98 * pow(S, 3)
        - 14772900.19 * pow(S, 2)
        + 2118283.90 * S
        - 124937.16;

  } else if (H <= 0.499) {
    y = - 63276732.40 * pow(S, 6)
        + 143550964.49 * pow(S, 5)
        - 134173242.00 * pow(S, 4)
        + 66193358.75 * pow(S, 3)
        - 18191143.19 * pow(S, 2)
        + 2642492.84 * S
        - 158640.10;

  } else if (H <=  0.540) {
    y = - 8126118.17 * pow(S, 6)
        + 21539562.40 * pow(S, 5)
        - 23507810.84 * pow(S, 4)
        + 13529663.83 * pow(S, 3)
        - 4331961.81 * pow(S, 2)
        + 732061.82 * S
        - 51044.19;

  } else if (H <= 0.570) {
    y = + 2339485.02 * pow(S, 6)
        - 7243029.02 * pow(S, 5)
        + 9226092.31 * pow(S, 4)
        - 6182309.70 * pow(S, 3)
        + 2297333.82 * pow(S, 2)
        - 448351.11 * S
        + 35867.07;

  } else if (H <=  0.606) {
    y = + 8003026.79 * pow(S, 6)
        - 20541735.22 * pow(S, 5)
        + 21745122.85 * pow(S, 4)
        - 12148011.89 * pow(S, 3)
        + 3777095.63 * pow(S, 2)
        - 619302.98 * S
        + 41802.20;

  } else if (H <=  0.643) {
    y = - 12136360.06 * pow(S, 6)
        + 26164559.22 * pow(S, 5)
        - 23145754.37 * pow(S, 4)
        + 10765030.37 * pow(S, 3)
        - 2778582.29 * pow(S, 2)
        + 378052.24 * S
        - 21211.78;

  } else if (H <=  0.709) {
    y = + 5521843.21 * pow(S, 6)
        - 8705092.11 * pow(S, 5)
        + 5642822.00 * pow(S, 4)
        - 1916438.30 * pow(S, 3)
        + 358031.84 * pow(S, 2)
        - 34151.94 * S
        + 1289.99;

  } else if (H <= 1) {
    y = + 23465946.54 * pow(S, 6)
        - 33470580.48 * pow(S, 5)
        + 19594314.38 * pow(S, 4)
        - 6033166.71 * pow(S, 3)
        + 1032152.56 * pow(S, 2)
        - 92521.76 * S
        + 3380.24;
  }

  return y;
}
//store  HSV value into position struct
Position rgbDriver::getPosition(float H, float S, float R, float G, float B) {
  Position p;

  p.X = getRealX(H);
  p.Y = getRealY(S,H);
  p.R = R;
  p.G = G;
  p.B = B;
  return p;
}
//check for new color detection
bool rgbDriver :: newColorDetected() {

  if (abs(previousRed - red) >= toleranceRGB) return true;
  if (abs(previousGreen - green) >= toleranceRGB) return true;
  if (abs(previousBlue - blue) >= toleranceRGB) return true;

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
            previousRed = red;
            previousGreen = green;
            previousBlue = blue;
            float R = from0_255To0_1(red);
            float G = from0_255To0_1(green);
            float B = from0_255To0_1(blue);
            float V = calculateValue(R, G, B);
            float m = calculateMinimum(R, G, B);
            float C = calculateChroma(V, m);
            float H = calculateHue(R, G, B, V, C);
            float S = calculateSaturation(C, V);
            Position currentPosition = getPosition(H, S,R,G,B);
            sendPosition(currentPosition,xQueueP );
        }
    }
}

//send ant position to queue
void rgbDriver::sendPosition(Position pos, QueueHandle_t xQueueP)

{
    xStatus = xQueueSend(xQueueP,&pos,0);
    if(xStatus == pdPASS)
    {
        Serial.println("NEW POSITION SEND");
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