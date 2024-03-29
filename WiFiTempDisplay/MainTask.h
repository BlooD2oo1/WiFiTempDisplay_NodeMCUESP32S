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

  
  void    Setup( bool bInitialize );
  void    Loop();

private:
  void    RenderDefault();
  void    RenderBasic();
  void    UpdateSensors();
  void    UpdateTouchSensors();

  void    PrintSensorDeviceAddresses();

private:
  const uint8_t     m_iOneWireBus = 13;
  OneWire           m_OneWire;
  DallasTemperature m_Sensors;

 const uint8_t     m_piSensorDeviceAddress[SENSORCOUNT][8] =
  {  
#ifdef DEVICE_KORNYE_KAZAN
    { 0x28, 0x22, 0x43, 0x97, 0x94, 0x07, 0x03, 0x3C },
    { 0x28, 0x12, 0x56, 0xE8, 0x5C, 0x21, 0x01, 0xEC }
#endif
#ifdef DEVICE_TUZOLTO
    { 0x28, 0x44, 0xF3, 0xD0, 0x5C, 0x21, 0x01, 0xC5 },
    { 0x28, 0x4C, 0x83, 0xD0, 0x5C, 0x21, 0x01, 0x09 }
#endif  
  };

  int16_t           m_iSensorTemperature[SENSORCOUNT] = {DEVICE_DISCONNECTED_RAW,DEVICE_DISCONNECTED_RAW};

  U8G2_SH1106_128X64_NONAME_F_HW_I2C  m_u8g2;
  bool                                m_bDisplayOn;

  //RTC_NOINIT_ATTR int m_iCounter = 0;

  NOINIT static bool       m_bFirstDayOfPowerOn;

  static const int16_t     m_iTempDataCount = 63;
  NOINIT static int32_t    m_pTemp[SENSORCOUNT][m_iTempDataCount];
  NOINIT static int16_t    m_iTempDataPointer;
  static const int16_t     m_iTempDataCurrCount = 1828;  //1828: 750ms-es homerovel igy pont 1 napot rajzol ki a grafikon
  NOINIT static int16_t    m_iTempDataCurrCounter;

  static const byte        m_bTouchSensorCount = 2;
  bool                     m_bTouch[m_bTouchSensorCount];
  bool                     m_bTouchTransient[m_bTouchSensorCount];
};

#endif