#include "Globals.h"

float lerp( float a, float b, float x )
{ 
  return a + x * (b - a);
}

float absi( int16_t a )
{
  if ( a < 0 )
  {
    return -a;
  }
  else
  {
    return a;
  }
}

float absf( float a )
{
  if ( a < 0.0f )
  {
    return -a;
  }
  else
  {
    return a;
  }
}

void PrintSec( unsigned long iSec )
{
    iSec = iSec % 86400UL;
    unsigned int iSecond = iSec % 60UL;
    iSec = iSec / 60UL;
    unsigned int iHour = iSec / 60UL;
    unsigned int iMinute = iSec % 60UL;

    char sTime[] = "TIME:00:00:00";
    sTime[12] = iSecond % 10 + 48;
    sTime[11] = iSecond / 10 + 48;
    sTime[9]  = iMinute % 10 + 48;
    sTime[8]  = iMinute / 10 + 48;
    sTime[6]  = iHour   % 10 + 48;
    sTime[5]  = iHour   / 10 + 48;

    Serial.println( sTime );
}
