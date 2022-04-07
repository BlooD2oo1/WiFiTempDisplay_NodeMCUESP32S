#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define NOINIT RTC_NOINIT_ATTR

#define HOURMINSEC_2_SEC( h, m, s ) ((h*60UL+m)*60UL+s)
#define HOURMINSEC_2_MS( h, m, s ) (((h*60UL+m)*60UL+s)*1000UL)

extern SemaphoreHandle_t g_pDisplayText_sem;
extern char g_pDisplayText[80];
void SetDisplayText( char* pText );
void GetDisplayText( char* pText );

extern SemaphoreHandle_t g_iRealTimeOffsetSec_sem;
NOINIT extern unsigned long g_iRealTimeOffsetSec;
void SetRealTimeOffsetSec( unsigned long iRealTimeOffsetSec );
void GetRealTimeOffsetSec( unsigned long& iRealTimeOffsetSec );

#undef abs

float lerp( float a, float b, float x );

float absi( int16_t a );
float absf( float a );

void PrintSec( unsigned long iSec );
void PrintSec( unsigned long iSec, char* sTime );

#endif