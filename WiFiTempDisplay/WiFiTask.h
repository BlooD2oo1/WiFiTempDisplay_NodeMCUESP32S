#ifndef WIFITASK_H
#define WIFITASK_H

#include <WiFi.h>
#include <NTPClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include "Globals.h"


class CWiFiTask
{
public:
  CWiFiTask();
  ~CWiFiTask();

  void    Setup();
  void    Loop();

private:
  void    FindSSID();
  void    ConnectToWiFi();

  void    UpdateDisplayText();
  void    UpdateTime();
  void    UpdateIOT();

private:

  static const byte   m_sWifiAccCount = 4;
  const char*         m_sWifiID[m_sWifiAccCount] =	 { "BlooD2oo1", "Vodafone-AC86", "BlooD2oo2",  "Neocore Publeak" };
  const char*         m_sWifiPass[m_sWifiAccCount] = { "kakamatyi", "kzpr5j2hCxab",  "Kakamatyi1", "feudum007" };
  
#ifdef DEVICE_KORNYE_UZEM
  //const unsigned long m_iThingSpeakChannelNumber = 1685795;//kazan
  //const char*         m_sThingSpeakWriteAPIKey = "PPAVQ6Z3DYDVFYC8";//kazan
  const unsigned long m_iThingSpeakChannelNumber = 1668078;//muhely
  const char*         m_sThingSpeakWriteAPIKey = "39URU9ON2H6KX2IX";//muhely
#endif
#ifdef DEVICE_TUZOLTO
  const unsigned long m_iThingSpeakChannelNumber = 1990916;
  const char*         m_sThingSpeakWriteAPIKey = "Z9QQU95IHM3KVRWP";
#endif

  WiFiClient          client;
  WiFiUDP             ntpUDP;
  NTPClient           timeClient;



  unsigned long       m_iThingSpeakLastUpdateTimeStamp;
  int8_t              m_iSSIDInd;

  unsigned long       m_iEpochTimeLastUpdateTimeStamp;

  unsigned long       m_iDisplayTextLastUpdateTimeStamp;

  unsigned long       m_iIOTLastUpdateTimeStamp;
};

#endif