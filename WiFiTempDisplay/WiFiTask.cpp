#include "WiFiTask.h"
#include "Globals.h"

#include <SPI.h>
#include <Wire.h>
#include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros

CWiFiTask::CWiFiTask()
: client()
, timeClient(ntpUDP)
, g_iThingSpeakLastUpdateTimeStamp( 0 )
, g_iSSIDInd( -1 )
, g_iEpochTimeLastUpdateTimeStamp( HOURMINSEC_2_MS( 24UL, 0UL, 0UL ) )
, g_iDisplayTextLastUpdateTimeStamp( HOURMINSEC_2_MS( 24UL, 0UL, 0UL ) )
{

}

CWiFiTask::~CWiFiTask()
{

}

void CWiFiTask::Setup()
{
  Serial.print("WiFiTask setup on core ");
  Serial.println(xPortGetCoreID());
  
  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);  //Initialize ThingSpeak

  timeClient.setTimeOffset(7200);

  FindSSID();
}

void CWiFiTask::Loop()
{
  UpdateDisplayText();

  if ( WiFi.status() != WL_CONNECTED )
  {
    digitalWrite(LED_BUILTIN, LOW);
    ConnectToWiFi();
  }

  if ( WiFi.status() == WL_CONNECTED )
  {
    UpdateTime();

    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void CWiFiTask::UpdateDisplayText()
{
  if ( millis() - g_iDisplayTextLastUpdateTimeStamp > HOURMINSEC_2_MS( 0UL, 0UL, 1UL ) )
  {
    if( WiFi.status() == WL_CONNECTED )
    {
      static char p[] = "Connected to                                     ";
      strcpy( &(p[14]), g_sWifiID[g_iSSIDInd] );
      SetDisplayText( p );
    }
    else
    {
      static char p[] = "No Internet";
      SetDisplayText( p );
    }

    g_iDisplayTextLastUpdateTimeStamp = millis();
  }
}

void CWiFiTask::UpdateTime()
{
  if ( millis() - g_iEpochTimeLastUpdateTimeStamp > HOURMINSEC_2_MS( 0UL, 10UL, 0UL ) )
  {
    timeClient.update();
    unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server
    PrintSec( unix_epoch );
    unix_epoch = unix_epoch % HOURMINSEC_2_SEC( 24UL, 0UL, 0UL );
    unix_epoch = unix_epoch + HOURMINSEC_2_SEC( 24UL, 0UL, 0UL ) - (millis()/1000UL);
    SetRealTimeOffsetSec( unix_epoch );

    g_iEpochTimeLastUpdateTimeStamp = millis();
  }
}

void CWiFiTask::FindSSID()
{
  Serial.println("Searching WiFi..." );

  SetDisplayText( "Searching WiFi..." );
  
  g_iSSIDInd = -1;

  WiFi.scanDelete();

  int8_t iNetworkCount = WiFi.scanNetworks();

  Serial.print( iNetworkCount );
  Serial.println( " network found." );
  
  if ( iNetworkCount > 0 )
  {
    bool bBreak = false;

    for ( int8_t iNetworkInd = 0; iNetworkInd < iNetworkCount && !bBreak; iNetworkInd++ )
    {
      for ( byte iAccInd = 0; iAccInd < g_sWifiAccCount && !bBreak; iAccInd++ )
      {
        if ( WiFi.SSID(iNetworkInd) == g_sWifiID[iAccInd] )
        {
          g_iSSIDInd = iAccInd;
          bBreak = true;
          Serial.print( "SSID found: ");
          Serial.println( g_sWifiID[g_iSSIDInd] );
          
        }
      }
    } 
  }

  WiFi.scanDelete();
}

void CWiFiTask::ConnectToWiFi()
{
  if ( g_iSSIDInd == -1 )
  {
    Serial.println( "WiFi SSID not found." );
    SetDisplayText( "WiFi SSID not found." );
    return;
  }

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(g_sWifiID[g_iSSIDInd]);

    static char p[] = "Connecting to                                     ";
    strcpy( &(p[15]), g_sWifiID[g_iSSIDInd] );
    SetDisplayText( p );
    
    WiFi.begin(g_sWifiID[g_iSSIDInd], g_sWifiPass[g_iSSIDInd]); // Connect to WPA/WPA2 network. Change this line if using open or WEP network      

    for ( byte i = 0; i < 4; i++ )
    {
      if ( i != 0 ) delay(5000);     
      
      Serial.println("Connecting...");

      if ( WiFi.status() == WL_CONNECTED )
      {
        break;
      }
    }
    if(WiFi.status() == WL_CONNECTED)
    {
      timeClient.begin();
      Serial.println("\nConnected.");
    }
    else
    {
      Serial.println("\nFailed to connect.");
    }      
  }
}
