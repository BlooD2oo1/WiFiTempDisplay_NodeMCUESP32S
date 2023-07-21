#include "Globals.h"

void InitSemaphores()
{
  g_pDisplayText_sem = xSemaphoreCreateBinary();
  xSemaphoreGive( g_pDisplayText_sem );
  g_pDisplayTime_sem = xSemaphoreCreateBinary();
  xSemaphoreGive( g_pDisplayTime_sem );  
  g_Temp_sem = xSemaphoreCreateBinary();
  xSemaphoreGive( g_Temp_sem );
}

SemaphoreHandle_t g_pDisplayText_sem;
char g_pDisplayText[80] = {0};
void SetDisplayText( const char* pText )
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

SemaphoreHandle_t g_pDisplayTime_sem;
tm g_sDisplayTime = {0};
void SetDisplayTime( const tm* pTime )
{
  xSemaphoreTake( g_pDisplayTime_sem, portMAX_DELAY );
  memcpy( &g_sDisplayTime, pTime, sizeof( tm ) );
  xSemaphoreGive( g_pDisplayTime_sem );
}
void GetDisplayTime( tm* pTime )
{
  xSemaphoreTake( g_pDisplayTime_sem, portMAX_DELAY );
  memcpy( pTime, &g_sDisplayTime, sizeof( tm ) );
  xSemaphoreGive( g_pDisplayTime_sem );
}

SemaphoreHandle_t g_Temp_sem;
long g_pTempSum[] = {0};
long g_iTempCount = 0;
void AddTemp( long* pTemp )
{
  xSemaphoreTake( g_Temp_sem, portMAX_DELAY );
  if ( g_iTempCount >= 65535 )
  // 256Cfok = 128*256 = 32768 long-kent.
  // arduino long max 2147483647 / 32768 = 65535.9999
  // vagyis ha vegig 256 fok van meg akkor is tudunk 65ezer mintat osszegezni
  {
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
      g_pTempSum[iSensorInd] = pTemp[iSensorInd];
    }
    g_iTempCount = 1;
  }
  else
  {
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
      g_pTempSum[iSensorInd] += pTemp[iSensorInd];
    }
    g_iTempCount++;
  }
  xSemaphoreGive( g_Temp_sem );
}
void GetTempAndReset( long* pTempSum, long& iTempCount )
{
  xSemaphoreTake( g_Temp_sem, portMAX_DELAY );
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    pTempSum[iSensorInd] = g_pTempSum[iSensorInd];
    g_pTempSum[iSensorInd] = 0;
  }  
  iTempCount = g_iTempCount;
  g_iTempCount = 0;
  xSemaphoreGive( g_Temp_sem );
}

void SetBuiltInLED( bool bOn )
{
#ifndef HOLOGRAM
  digitalWrite(LED_BUILTIN, bOn ? HIGH : LOW );
#else
  digitalWrite(LED_BUILTIN, LOW );
#endif
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
