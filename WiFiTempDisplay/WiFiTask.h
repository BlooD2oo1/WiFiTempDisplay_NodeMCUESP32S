#ifndef WIFITASK_H
#define WIFITASK_H

#include <WiFi.h>
#include <NTPClient.h>
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

  static const byte   g_sWifiAccCount = 3;
  const char*         g_sWifiID[g_sWifiAccCount] = { "BlooD2oo1", "Vodafone-AC86", "BlooD2oo2" };
  const char*         g_sWifiPass[g_sWifiAccCount] = { "kakamatyi", "kzpr5j2hCxab", "Kakamatyi1" };
  const unsigned long g_iThingSpeakChannelNumber = 1685795;
  const char*         g_sThingSpeakWriteAPIKey = "PPAVQ6Z3DYDVFYC8";

  WiFiClient          client;
  WiFiUDP             ntpUDP;
  NTPClient           timeClient;

  unsigned long       g_iThingSpeakLastUpdateTimeStamp;
  int8_t              g_iSSIDInd;

  unsigned long       g_iEpochTimeLastUpdateTimeStamp;

  unsigned long       g_iDisplayTextLastUpdateTimeStamp;

  unsigned long       g_iIOTLastUpdateTimeStamp;
};

#endif