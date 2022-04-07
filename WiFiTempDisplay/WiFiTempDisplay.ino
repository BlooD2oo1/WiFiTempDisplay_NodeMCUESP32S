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

  InitSemaphores();

  esp_reset_reason_t iResetReason = esp_reset_reason();
  if ( iResetReason != ESP_RST_SW )
  {
    Serial.println( "Clear" );
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
  g_cWiFiTask.Setup();

  while(1)
  {
    g_cWiFiTask.Loop();
    delay(1); //wdt
  } 
}

void UpdateDailyReset()
{
  if ( millis() >= HOURMINSEC_2_MS( 24UL, 0UL, 0UL ) )
  {
    Serial.println( "Daily reset.\n" );
    ESP.restart();
  }
}
