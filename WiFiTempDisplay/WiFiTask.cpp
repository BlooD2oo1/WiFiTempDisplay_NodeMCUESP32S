#include "WiFiTask.h"

#include <SPI.h>
#include <Wire.h>
#include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros

CWiFiTask::CWiFiTask()
: client()
, timeClient(ntpUDP)
, m_iThingSpeakLastUpdateTimeStamp( 0 )
, m_iSSIDInd( -1 )
, m_iEpochTimeLastUpdateTimeStamp( HOURMINSEC_2_MS( 24UL, 0UL, 0UL ) )
, m_iDisplayTextLastUpdateTimeStamp( HOURMINSEC_2_MS( 24UL, 0UL, 0UL ) )
, m_iIOTLastUpdateTimeStamp( 0 )
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
  UpdateTime();

  if ( WiFi.status() != WL_CONNECTED )
  {
    digitalWrite(LED_BUILTIN, LOW);
    ConnectToWiFi();
  }

  if ( WiFi.status() == WL_CONNECTED )
  {
    UpdateIOT();

    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void CWiFiTask::UpdateDisplayText()
{
  if ( millis() - m_iDisplayTextLastUpdateTimeStamp > HOURMINSEC_2_MS( 0UL, 0UL, 1UL ) )
  {
    if( WiFi.status() == WL_CONNECTED )
    {
      static char p[] = "Connected";
      //strcpy( &(p[14]), m_sWifiID[m_iSSIDInd] );
      SetDisplayText( p );
    }
    else
    {
      static char p[] = "Offline";
      SetDisplayText( p );
    }

    m_iDisplayTextLastUpdateTimeStamp = millis();
  }
}

void initTime()
{
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  Serial.printf("  Setting Timezone to %s\n","CET-1CEST,M3.5.0,M10.5.0/3");
  setenv("TZ","CET-1CEST,M3.5.0,M10.5.0/3",1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void CWiFiTask::UpdateTime()
{
  if ( millis() - m_iEpochTimeLastUpdateTimeStamp > HOURMINSEC_2_MS( 0UL, 0UL, 1UL ) )
  {
    struct tm timeinfo = {0};
    if(!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time 1");
      SetDisplayTime( &timeinfo );
    }
    else
    {
      SetDisplayTime( &timeinfo );
    }
    m_iEpochTimeLastUpdateTimeStamp = millis();
  }
}

void CWiFiTask::UpdateIOT()
{
  if ( millis() - m_iIOTLastUpdateTimeStamp > HOURMINSEC_2_MS( 0UL, 5UL, 0UL ) )
  {
    Serial.println( "Updating ThingSpeak" );
    SetDisplayText( "Updating IOT" );

    unsigned long pTempSum[SENSORCOUNT];
    unsigned long iTempCount;
    GetTempAndReset( pTempSum, iTempCount );

    float fA = ( (float)pTempSum[0]/(float)iTempCount ) * 0.0078125f;
    float fB = ( (float)pTempSum[1]/(float)iTempCount ) * 0.0078125f;
  
    Serial.print("AvgTempA: ");
    Serial.print( fA );
    Serial.print(" C");
    Serial.print("\t\tAvgTempB: ");
    Serial.print( fB );
    Serial.println(" C");
    
    ThingSpeak.setField( 1, fA );
    ThingSpeak.setField( 2, fB );
    ThingSpeak.setField( 3, fB-fA );
    ThingSpeak.setField( 4, (float)( millis()/1000UL )/3600.0f );


    int iRet = ThingSpeak.writeFields( m_iThingSpeakChannelNumber, m_sThingSpeakWriteAPIKey );

    if(iRet == 200)
    {
      Serial.println("ThingSpeak update successful.");
    }
    else
    {
      Serial.print("Problem updating ThingSpeak channels. HTTP error code: ");
      Serial.println(iRet);
    }
    m_iIOTLastUpdateTimeStamp = millis();
  }
}

void CWiFiTask::FindSSID()
{
  Serial.println( "Searching WiFi..." );

  SetDisplayText( "Searching WiFi..." );
  
  m_iSSIDInd = -1;

  WiFi.scanDelete();

  int8_t iNetworkCount = WiFi.scanNetworks();

  Serial.print( iNetworkCount );
  Serial.println( " network found." );
  
  if ( iNetworkCount > 0 )
  {
    bool bBreak = false;

    for ( int8_t iNetworkInd = 0; iNetworkInd < iNetworkCount && !bBreak; iNetworkInd++ )
    {
      for ( byte iAccInd = 0; iAccInd < m_sWifiAccCount && !bBreak; iAccInd++ )
      {
        if ( WiFi.SSID(iNetworkInd) == m_sWifiID[iAccInd] )
        {
          m_iSSIDInd = iAccInd;
          bBreak = true;
          Serial.print( "SSID found: ");
          Serial.println( m_sWifiID[m_iSSIDInd] );
          
        }
      }
    } 
  }

  WiFi.scanDelete();
}

void CWiFiTask::ConnectToWiFi()
{
  if ( m_iSSIDInd == -1 )
  {
    Serial.println( "WiFi SSID not found." );
    SetDisplayText( "WiFi SSID not found." );
    return;
  }

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(m_sWifiID[m_iSSIDInd]);

    static char p[] = "Connecting...";
    //strcpy( &(p[15]), m_sWifiID[m_iSSIDInd] );
    SetDisplayText( p );
    
    WiFi.begin(m_sWifiID[m_iSSIDInd], m_sWifiPass[m_iSSIDInd]); // Connect to WPA/WPA2 network. Change this line if using open or WEP network      

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
      initTime();
      Serial.println("\nConnected.");
    }
    else
    {
      Serial.println("\nFailed to connect.");
    }      
  }
}
