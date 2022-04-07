#include <esp_task_wdt.h>

#include "Globals.h"
#include "MainTask.h"
#include "WiFiTask.h"

CMainTask   g_cMainTask;
CWiFiTask   g_cWiFiTask;

void setup()
{
  Serial.begin(9600);
  Serial.println( "\n\nSetup begin" );

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  g_pDisplayText_sem = xSemaphoreCreateBinary();
  xSemaphoreGive( g_pDisplayText_sem );
  g_iRealTimeOffsetSec_sem = xSemaphoreCreateBinary();
  xSemaphoreGive( g_iRealTimeOffsetSec_sem );
  
  esp_reset_reason_t iResetReason = esp_reset_reason();
  if ( iResetReason != ESP_RST_SW )
  {
    Serial.println( "Clear" );
    g_iRealTimeOffsetSec = 0;
    g_cMainTask.Clear();
  }

  g_cMainTask.Setup();

  TaskHandle_t hTaskWiFi;
  xTaskCreatePinnedToCore(WiFiTask,"WiFi",10000,NULL,1,&hTaskWiFi,0);  

  Serial.println( "Setup finished\n" );
}

void loop()
{
  UpdateDailyReset();

  g_cMainTask.Loop();
  
  esp_task_wdt_reset();
}

void WiFiTask( void * parameter )
{
  Serial.print("WiFiTask running on core ");
  Serial.println(xPortGetCoreID());

  g_cWiFiTask.Setup();

  while(1)
  {
    g_cWiFiTask.Loop();
    delay(1); //wdt
  } 
}

void UpdateDailyReset()
{
  if ( millis() >= HOURMINSEC_2_MS( 0UL, 0UL, 20UL ) )
  {
    Serial.println( "Daily reset.\n" );
    ESP.restart();
  }
}
