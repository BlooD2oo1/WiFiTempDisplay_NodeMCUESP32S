#include "MainTask.h"

  int32_t    CMainTask::m_pTemp[SENSORCOUNT][m_iTempDataCount];
  int16_t    CMainTask::m_iTempDataPointer;
  int16_t    CMainTask::m_iTempDataCurrCounter;

CMainTask::CMainTask()
: m_OneWire(m_iOneWireBus)
, m_Sensors(&m_OneWire)
, m_u8g2( U8G2_R2 )
, m_bDisplayOn( true )
{
  for ( int i = 0; i < m_bTouchSensorCount; i++ )
  {
    m_bTouch[i] = false;
    m_bTouchTransient[i] = false;
  }
}

CMainTask::~CMainTask()
{

}

void CMainTask::Setup( bool bInitialize )
{
  Serial.print("MainTask setup on core ");
  Serial.println(xPortGetCoreID());

  m_Sensors.begin();
  //PrintSensorDeviceAddresses();
  m_Sensors.setResolution( 12 );
  m_Sensors.setWaitForConversion(false);
  m_Sensors.requestTemperatures();

  m_u8g2.begin();
  m_u8g2.setContrast( 0 );

  if ( bInitialize )
  {
    delay( 750 );
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
      m_iSensorTemperature[iSensorInd] = m_Sensors.getTemp( m_piSensorDeviceAddress[iSensorInd] );
    }
    m_iTempDataPointer = 0;
    m_iTempDataCurrCounter = m_iTempDataCurrCount;
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
      for ( int16_t iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
      {
        m_pTemp[iSensorInd][iDataInd] = ( (int32_t)m_iSensorTemperature[iSensorInd] + (int32_t)absi((((int16_t)iDataInd)%6)-3)*12 ) * (int32_t)m_iTempDataCurrCount;
      }
    }
  }
}

void CMainTask::Loop()
{
  //UpdateTouchSensors();

  UpdateSensors();

  Render();
}

void CMainTask::Render()
{
  static tm sTimeInfo;
  GetDisplayTime( &sTimeInfo );

  bool bTurnOnDisplay = false;
  if ( sTimeInfo.tm_hour > 5 && sTimeInfo.tm_hour < 18 )
  {
    bTurnOnDisplay = true;
  }
  if ( m_bDisplayOn != bTurnOnDisplay )
  {
      m_bDisplayOn = bTurnOnDisplay;
      m_u8g2.setPowerSave( m_bDisplayOn ? 0 : 1 );
  }
  if ( m_bDisplayOn == false )
  {
    return;
  }

  const uint iXOffset[SENSORCOUNT] = {0,65};
  uint iYOffsetGraph;
  uint iYOffsetText;
  uint iYOffsetMsg;
  switch( (millis()/HOURMINSEC_2_MS( 0UL, 0UL, 30UL )) % 3 )
  {
    case 0:
    iYOffsetGraph = 0;
    iYOffsetText = 32;
    iYOffsetMsg = 58;    
    break;
    case 1:
    iYOffsetGraph = 24;
    iYOffsetText = 0;
    iYOffsetMsg = 58;    
    break;
    case 2:
    iYOffsetGraph = 6;
    iYOffsetText = 38;
    iYOffsetMsg = 0;    
    break;    
  }

  m_u8g2.clearBuffer();

  m_u8g2.sendF("c", 0xa6 );
  
  /*static byte bTouchCounter0 = 0;
  static byte bTouchCounter1 = 0;
  if ( m_bTouchTransient[0] )
  {
    bTouchCounter0++;
    m_u8g2.sendF("c", 0xa7 );
  }

  if ( m_bTouchTransient[1] )
  {
    bTouchCounter1++;
    m_u8g2.sendF("c", 0xa7 );
  }*/

  //u8g2.setFont( u8g2_font_blipfest_07_tr );
  //u8g2.setFont( u8g2_font_lastapprenticebold_tr );
  //u8g2.setFont( u8g2_font_VCR_OSD_tf );
  //u8g2.setFont( u8g2_font_logisoso16_tf );

  m_u8g2.setFont( u8g2_font_blipfest_07_tr );
  
  static char pText[80] = {0};

  GetDisplayText( pText );
  m_u8g2.drawStr( 0, 5+iYOffsetMsg, pText );


  strftime( pText, 80, "%b %d %H:%M:%S", &sTimeInfo );
  u8g2_uint_t iTextWidth = m_u8g2.getStrWidth( pText );
  m_u8g2.drawStr( 128 - iTextWidth, 5+iYOffsetMsg, pText );  

  if ( sTimeInfo.tm_hour > 10 && sTimeInfo.tm_hour < 14 )
  {
    m_u8g2.setContrast( 255 );
  }
  else
  {
    m_u8g2.setContrast( 0 );
  }

  //randomSeed(666);
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    {
        m_u8g2.setFont( u8g2_font_helvB14_tf );
        sprintf( pText, "%.2f", m_Sensors.rawToCelsius( m_iSensorTemperature[iSensorInd] ) );  
        u8g2_uint_t iTextWidth = m_u8g2.getStrWidth( pText );
        m_u8g2.drawStr( iXOffset[iSensorInd] + ( m_iTempDataCount - iTextWidth ) / 2, 15+iYOffsetText, pText);
    }
    const uint iTickLenX = 6;
    const uint iTickLenY = 3;
    m_u8g2.drawHLine( iXOffset[iSensorInd], 0+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd], 1+iYOffsetGraph, iTickLenY );

    m_u8g2.drawHLine( iXOffset[iSensorInd]+m_iTempDataCount-iTickLenX, 0+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd]+m_iTempDataCount-1, 1+iYOffsetGraph, iTickLenY );
    
    m_u8g2.drawHLine( iXOffset[iSensorInd], 32+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd], 32-iTickLenY+iYOffsetGraph, iTickLenY );

    m_u8g2.drawHLine( iXOffset[iSensorInd]+m_iTempDataCount-iTickLenX, 32+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd]+m_iTempDataCount-1, 32-iTickLenY+iYOffsetGraph, iTickLenY );

    //m_u8g2.drawFrame( iXOffset[iSensorInd], iYOffsetGraph, m_iTempDataCount, 32 );
  }

  float fMinMin[SENSORCOUNT] = {32760.0f,32760.0f};
  float fMaxMax[SENSORCOUNT] = {-32760.0f,-32760.0f};
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    for ( int16_t iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
    {
      float fDiv = (float)m_iTempDataCurrCount;
      if ( iDataInd == m_iTempDataPointer )
      {
        fDiv = (float)m_iTempDataCurrCounter;
      }
      float fTemp = (float)m_pTemp[iSensorInd][iDataInd] / fDiv;
      fMinMin[iSensorInd] = min( fMinMin[iSensorInd], fTemp );
      fMaxMax[iSensorInd] = max( fMaxMax[iSensorInd], fTemp );
    }

    {
      m_u8g2.setFont( u8g2_font_blipfest_07_tr );
      sprintf( pText, "%.1f", fMinMin[iSensorInd] * 0.0078125f );
      //sprintf( pText, "%.1f", m_fTouch1 );
      m_u8g2.drawStr( iXOffset[iSensorInd] + m_iTempDataCount / 2 - 20, 23+iYOffsetText, pText);
      sprintf( pText, "%.1f", fMaxMax[iSensorInd] * 0.0078125f );
      //sprintf( pText, "%.1f", m_fTouch2 );
      m_u8g2.drawStr( iXOffset[iSensorInd] + m_iTempDataCount / 2 + 7, 23+iYOffsetText, pText);
    }
  }

  float fMinMinMin = 32760.0f;
  float fMaxMaxMax = -32760.0f;
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
	  fMinMinMin = min( fMinMin[iSensorInd], fMinMinMin );
	  fMaxMaxMax = max( fMaxMax[iSensorInd], fMaxMaxMax );
  }

  if ( fMaxMaxMax - fMinMinMin < 8.0f*32.0f )
  {
    float fAvg = ( fMaxMaxMax + fMinMinMin ) / 2.0f;
    fMinMinMin = fAvg - 8.0f/2.0f*32.0f;
    fMaxMaxMax = fAvg + 8.0f/2.0f*32.0f;
  }

  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    float fB = 0;
    for ( int16_t iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
    {
      int16_t iInd = ( iDataInd + m_iTempDataPointer + 1 ) % m_iTempDataCount;
      float fDiv = (float)m_iTempDataCurrCount;
      if ( iInd == m_iTempDataPointer )
      {
        fDiv = (float)m_iTempDataCurrCounter;
      }
      float fTemp = (float)m_pTemp[iSensorInd][iInd] / fDiv;
      float fA = ( fTemp - fMinMinMin ) / ( fMaxMaxMax - fMinMinMin );
      fA *= 30.0f;

      if ( iDataInd == 0) fB = fA;

      float fFrom;
      int16_t iCount;
      if ( fA < fB )
      {
        fFrom = fA;
        iCount = (int16_t)(fB - fA);
      }
      else
      {
        fFrom = fB;
        iCount = (int16_t)(fA - fB);
      }

      fB = fA;

      float fSubPixelOffsetX = 1.0f - (float)(m_iTempDataCurrCounter) / (float)m_iTempDataCurrCount;
      //for ( byte c = 0; c < (bTouchCounter1%2)+1; c++ )
      for ( byte c = 0; c < 2; c++ )
      for ( int16_t i = 0; i <= iCount; i++ )
      {
        float fRndX = (float)random( 4096 ) / 2048.0f - 1.0f;
        float fRndY = (float)random( 4096 ) / 2048.0f - 1.0f;
        //fRndX *= absf( fRndX );
        //fRndY *= absf( fRndY );
        fRndX *= fRndX*fRndX;
        fRndY *= fRndY*fRndY;
        fRndX *= /*(float)(bTouchCounter0%3)*0.5f;//*/0.98f;
        fRndY *= /*(float)(bTouchCounter0%3)*0.5f;//*/0.98f;

        int16_t x = (int16_t)( fSubPixelOffsetX + fRndX ) + iDataInd;
        int16_t y = (int16_t)( fFrom + fRndY ) + i;

        m_u8g2.drawPixel( iXOffset[iSensorInd] + x, iYOffsetGraph + (30-y) );
      }
      //m_u8g2.drawVLine( iXOffset[iSensorInd]+iDataInd, iA, iB - iA + 1 );
    }
  }

  m_u8g2.sendBuffer();
}

void CMainTask::UpdateSensors()
{
  static unsigned long m_iSensorLastUpdateTimeMs = 0;
  if ( millis() - m_iSensorLastUpdateTimeMs < 750 )
  {
    return;
  }

  unsigned long pTemp[SENSORCOUNT];
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    m_iSensorTemperature[iSensorInd] = m_Sensors.getTemp(m_piSensorDeviceAddress[iSensorInd]);
    pTemp[iSensorInd] = m_iSensorTemperature[iSensorInd];
  }
  m_Sensors.requestTemperatures();
  m_iSensorLastUpdateTimeMs = millis();
  
  AddTemp( pTemp );

  if ( m_iTempDataCurrCounter >= m_iTempDataCurrCount )
  {
    m_iTempDataCurrCounter = 0;
    m_iTempDataPointer++;
    m_iTempDataPointer %= m_iTempDataCount;
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
      m_pTemp[iSensorInd][m_iTempDataPointer] = m_iSensorTemperature[iSensorInd];
    }
  }
  else
  {
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
        m_pTemp[iSensorInd][m_iTempDataPointer] += m_iSensorTemperature[iSensorInd];
    }
  }
  m_iTempDataCurrCounter++;

}

void CMainTask::UpdateTouchSensors()
{
  static float fTouch[m_bTouchSensorCount] = {100.0f,100.0f};
  fTouch[0] = lerp( (float)touchRead(12)-90.0f, fTouch[0], 0.94f );
  fTouch[1] = lerp( (float)touchRead(4)-70.0f, fTouch[1], 0.94f );

  for ( int i = 0; i < m_bTouchSensorCount; i++ )
  {
    bool bPrevTouch = m_bTouch[i];
    m_bTouch[i] = fTouch[i] < 0.0f;
    if ( m_bTouch[i] && !bPrevTouch )
    {
      m_bTouchTransient[i] = true;
    }
    else
    {
      m_bTouchTransient[i] = false;
    }
  }

  //Serial.print(touchRead(12));
  //Serial.print( "\t\t\t        " );
  //Serial.println(touchRead(4));
  //delay(200);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
    {
      Serial.print("0");
    }
    Serial.print(deviceAddress[i], HEX);
  }
}
void CMainTask::PrintSensorDeviceAddresses()
{
  int numberOfDevices; // Number of temperature devices found

  DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

  // Grab a count of devices on the wire
  numberOfDevices = m_Sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(m_Sensors.getAddress(tempDeviceAddress, i))
    {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
		}
    else
    {
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
		}
  }
}
