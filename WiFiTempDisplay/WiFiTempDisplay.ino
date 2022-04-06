#include <esp_task_wdt.h>
#include <WiFi.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros

#define NOINIT RTC_NOINIT_ATTR

#define HOURMINSEC_2_SEC( h, m, s ) ((h*60UL+m)*60UL+s)
#define HOURMINSEC_2_MS( h, m, s ) (((h*60UL+m)*60UL+s)*1000UL)

#undef abs

#define WDT_TIMEOUT_SEC 3

TaskHandle_t    g_hTaskWiFi = 0;

#define ONE_WIRE_BUS 13
OneWire g_OneWire(ONE_WIRE_BUS);
DallasTemperature g_Sensors(&g_OneWire);
const uint8_t g_piSensorDeviceAddress[2][8] = { { 0x28, 0x44, 0xF3, 0xD0, 0x5C, 0x21, 0x01, 0xC5 },
                                                { 0x28, 0x12, 0x56, 0xE8, 0x5C, 0x21, 0x01, 0xEC } };
int16_t       g_iSensorTemperature[2] = {DEVICE_DISCONNECTED_RAW};

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2 );

//RTC_NOINIT_ATTR int g_iCounter = 0;

const byte    g_iTempDataCount = 65;

void ClearStaticVariables()
{
  Serial.println( "Clear" );
  //g_iCounter = 0;
}

void setup()
{
  //esp_task_wdt_init(WDT_TIMEOUT_SEC, true); //enable panic so ESP32 restarts
  //esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.begin(9600);
  Serial.println( "\n\nSetup begin" );

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  esp_reset_reason_t iResetReason = esp_reset_reason();
  if ( iResetReason != ESP_RST_SW )
  {
    ClearStaticVariables();
  }

  g_Sensors.begin();
  g_Sensors.setResolution( 12 );
  g_Sensors.setWaitForConversion(false);
  g_Sensors.requestTemperatures();

  u8g2.begin();

  Serial.print("MainTask running on core ");
  Serial.println(xPortGetCoreID());

  xTaskCreatePinnedToCore(WiFiTask,"WiFi",10000,NULL,1,&g_hTaskWiFi,0);  

  Serial.println( "Setup finished\n" );
}

void loop()
{
  UpdateDailyReset();

  UpdateSensors();

  Render();
  
  esp_task_wdt_reset();
}

void WiFiTask( void * parameter )
{

  Serial.print("WiFiTask running on core ");
  Serial.println(xPortGetCoreID());

  while(1)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    delay(1); //wdt
  } 
}

void Render()
{
  const uint iXOffset[2] = {0,63};

  u8g2.clearBuffer();
  
  static char pTemp[40];
  //u8g2.setFont( u8g2_font_blipfest_07_tr );
  //u8g2.setFont( u8g2_font_lastapprenticebold_tr );
  //u8g2.setFont( u8g2_font_VCR_OSD_tf );
  //u8g2.setFont( u8g2_font_logisoso16_tf );

  u8g2.setFont( u8g2_font_blipfest_07_tr );
  u8g2.drawStr( 0, 63, "NodeMCU Arduino Temp Sensor v0.1");

  for ( byte iSensorInd = 0; iSensorInd < 2; iSensorInd++ )
  {
    u8g2.setFont( u8g2_font_helvB14_tf );
    sprintf( pTemp, "%.2f", g_Sensors.rawToCelsius( g_iSensorTemperature[iSensorInd] ) );  
    u8g2.drawStr( iXOffset[iSensorInd], 49, pTemp);

    const uint iTickLenX = 6;
    const uint iTickLenY = 3;
    u8g2.drawHLine( iXOffset[iSensorInd], 0, iTickLenX );
    u8g2.drawVLine( iXOffset[iSensorInd], 1, iTickLenY );

    u8g2.drawHLine( iXOffset[iSensorInd]+g_iTempDataCount-iTickLenX, 0, iTickLenX );
    u8g2.drawVLine( iXOffset[iSensorInd]+g_iTempDataCount-1, 1, iTickLenY );
    
    u8g2.drawHLine( iXOffset[iSensorInd], 32, iTickLenX );
    u8g2.drawVLine( iXOffset[iSensorInd], 32-iTickLenY, iTickLenY );

    u8g2.drawHLine( iXOffset[iSensorInd]+g_iTempDataCount-iTickLenX, 32, iTickLenX );
    u8g2.drawVLine( iXOffset[iSensorInd]+g_iTempDataCount-1, 32-iTickLenY, iTickLenY );

    //u8g2.drawFrame( iXOffset[iSensorInd], 0, g_iTempDataCount, 33 );
  }

  u8g2.sendBuffer();
}

void UpdateDailyReset()
{
  if ( millis() >= HOURMINSEC_2_MS( 0UL, 0UL, 30UL ) )
  {
    Serial.println( "Daily reset." );
    ESP.restart();
  }
}

void UpdateSensors()
{
  static unsigned long g_iSensorLastUpdateTimeMs = 0;
  if ( millis() - g_iSensorLastUpdateTimeMs < 750 )
  {
    return;
  }

  for ( byte iSensorInd = 0; iSensorInd < 2; iSensorInd++ )
  {
    g_iSensorTemperature[iSensorInd] = g_Sensors.getTemp(g_piSensorDeviceAddress[iSensorInd]);
  }
  g_Sensors.requestTemperatures();
  g_iSensorLastUpdateTimeMs = millis();

  for ( byte iSensorInd = 0; iSensorInd < 2; iSensorInd++ )
  {
    Serial.print( g_Sensors.rawToCelsius( g_iSensorTemperature[iSensorInd] ) );
    Serial.print( " C        ");
  }
  Serial.println( "");
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
