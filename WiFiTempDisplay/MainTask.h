#ifndef MAINTASK_H
#define MAINTASK_H

#include <OneWire.h> 
#include <DallasTemperature.h>
#include <U8g2lib.h>
#include "Globals.h"

class CMainTask
{
public:
  CMainTask();
  ~CMainTask();

  void    Clear();
  void    Setup();
  void    Loop();

private:
  void    Render();
  void    UpdateSensors();

private:
  const uint8_t     m_iOneWireBus = 13;
  OneWire           m_OneWire;
  DallasTemperature m_Sensors;
  const uint8_t     m_piSensorDeviceAddress[2][8] = { { 0x28, 0x44, 0xF3, 0xD0, 0x5C, 0x21, 0x01, 0xC5 },
                                                      { 0x28, 0x12, 0x56, 0xE8, 0x5C, 0x21, 0x01, 0xEC } };
  int16_t           m_iSensorTemperature[2] = {DEVICE_DISCONNECTED_RAW,DEVICE_DISCONNECTED_RAW};

  U8G2_SH1106_128X64_NONAME_F_HW_I2C  m_u8g2;

  //RTC_NOINIT_ATTR int m_iCounter = 0;

  static const byte m_iTempDataCount = 63;
  NOINIT static int16_t    m_pTempMin[2][m_iTempDataCount];
  NOINIT static int16_t    m_pTempMax[2][m_iTempDataCount];
  NOINIT static byte       m_iTempDataPointer;
  NOINIT static int16_t    m_iTempDataCounter;

};

#endif