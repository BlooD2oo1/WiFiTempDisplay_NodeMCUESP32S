#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define NOINIT RTC_NOINIT_ATTR

#define HOURMINSEC_2_SEC( h, m, s ) ((h*60UL+m)*60UL+s)
#define HOURMINSEC_2_MS( h, m, s ) (((h*60UL+m)*60UL+s)*1000UL)

#undef abs

float lerp( float a, float b, float x );

float absi( int16_t a );
float absf( float a );

void PrintSec( unsigned long iSec );


#endif