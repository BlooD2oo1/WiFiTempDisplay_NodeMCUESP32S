#include "Globals.h"

SemaphoreHandle_t g_pDisplayText_sem;
char g_pDisplayText[80] = {0};

void SetDisplayText( char* pText )
{
  xSemaphoreTake( g_pDisplayText_sem, portMAX_DELAY );
  memcpy( g_pDisplayText, pText, 80 );
  xSemaphoreGive( g_pDisplayText_sem );
}
void GetDisplayText( char* pText )
{
  xSemaphoreTake( g_pDisplayText_sem, portMAX_DELAY );
  memcpy( pText, g_pDisplayText, 80 );
  xSemaphoreGive( g_pDisplayText_sem );
}

SemaphoreHandle_t g_iRealTimeOffsetSec_sem;
unsigned long g_iRealTimeOffsetSec = 0;
void SetRealTimeOffsetSec( unsigned long iRealTimeOffsetSec )
{
  xSemaphoreTake( g_iRealTimeOffsetSec_sem, portMAX_DELAY );
  g_iRealTimeOffsetSec = iRealTimeOffsetSec;
  xSemaphoreGive( g_iRealTimeOffsetSec_sem );
}
void GetRealTimeOffsetSec( unsigned long& iRealTimeOffsetSec )
{
  xSemaphoreTake( g_iRealTimeOffsetSec_sem, portMAX_DELAY );
  iRealTimeOffsetSec = g_iRealTimeOffsetSec;
  xSemaphoreGive( g_iRealTimeOffsetSec_sem );
}

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

void PrintSec( unsigned long iSec, char* sTime )
{
    iSec = iSec % 86400UL;
    unsigned int iSecond = iSec % 60UL;
    iSec = iSec / 60UL;
    unsigned int iHour = iSec / 60UL;
    unsigned int iMinute = iSec % 60UL;    
    
    sTime[8] = 0;
    sTime[7] = iSecond % 10 + 48;
    sTime[6] = iSecond / 10 + 48;
    sTime[5] = ':';
    sTime[4]  = iMinute % 10 + 48;
    sTime[3]  = iMinute / 10 + 48;
    sTime[2] = ':';
    sTime[1]  = iHour   % 10 + 48;    
    sTime[0]  = iHour   / 10 + 48;
}
