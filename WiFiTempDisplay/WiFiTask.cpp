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

  //timeClient.setTimeOffset(3600);

  FindSSID();
}

void CWiFiTask::Loop()
{
  if ( WiFi.status() != WL_CONNECTED )
  {
    digitalWrite(LED_BUILTIN, LOW);
    ConnectToWiFi();
  }

  if ( WiFi.status() == WL_CONNECTED )
  {
    timeClient.update();
    unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server
    PrintSec( unix_epoch );
    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}


void CWiFiTask::FindSSID()
{
  Serial.println("Searching wifi..." );
  
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
    return;
  }

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(g_sWifiID[g_iSSIDInd]);
    
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
