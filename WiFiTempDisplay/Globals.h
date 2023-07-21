#ifndef GLOBALS_H
#define GLOBALS_H

//#define HOLOGRAM

//#define DEVICE_KORNYE_KAZAN
#define DEVICE_TUZOLTO


#include <Arduino.h>
#include <time.h>

#define NOINIT RTC_NOINIT_ATTR

#define HOURMINSEC_2_SEC( h, m, s ) ((h*60UL+m)*60UL+s)
#define HOURMINSEC_2_MS( h, m, s ) (((h*60UL+m)*60UL+s)*1000UL)

#define SENSORCOUNT 2

void InitSemaphores();
extern SemaphoreHandle_t g_pDisplayText_sem;
extern char g_pDisplayText[80];
void SetDisplayText( const char* pText );
void GetDisplayText( char* pText );

extern SemaphoreHandle_t g_pDisplayTime_sem;
extern tm g_sDisplayTime;
void SetDisplayTime( const tm* pTime );
void GetDisplayTime( tm* pTime );

extern SemaphoreHandle_t g_Temp_sem;
extern long g_pTempSum[SENSORCOUNT];
extern long g_iTempCount;
void AddTemp( long* pTemp );
void GetTempAndReset( long* pTempSum, long& iTempCount );

void SetBuiltInLED( bool bOn );

#undef abs

float lerp( float a, float b, float x );

float absi( int16_t a );
float absf( float a );

void PrintSec( unsigned long iSec );
void PrintSec( unsigned long iSec, char* sTime );

#endif