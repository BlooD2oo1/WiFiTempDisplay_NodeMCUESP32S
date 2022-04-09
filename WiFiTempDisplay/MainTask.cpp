#include "MainTask.h"

  int32_t    CMainTask::m_pTemp[SENSORCOUNT][m_iTempDataCount];
  byte       CMainTask::m_iTempDataPointer;
  int16_t   CMainTask::m_iTempDataCurrCounter;

CMainTask::CMainTask()
: m_OneWire(m_iOneWireBus)
, m_Sensors(&m_OneWire)
, m_u8g2( U8G2_R2 )
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
      for ( byte iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
      {
        m_pTemp[iSensorInd][iDataInd] = (int32_t)m_iSensorTemperature[iSensorInd] * (int32_t)m_iTempDataCurrCount;
      }
    }
  }
}

void CMainTask::Loop()
{
  UpdateTouchSensors();

  UpdateSensors();

  Render();
}


void CMainTask::Render()
{
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

  if ( m_bTouchTransient[1] )
  {
    static bool b = false;
    b = !b;
    if ( b )
    {
      m_u8g2.sendF("c", 0xa7 );
    }
    else
    {
      m_u8g2.sendF("c", 0xa6 );
    }
  }

  //u8g2.setFont( u8g2_font_blipfest_07_tr );
  //u8g2.setFont( u8g2_font_lastapprenticebold_tr );
  //u8g2.setFont( u8g2_font_VCR_OSD_tf );
  //u8g2.setFont( u8g2_font_logisoso16_tf );

  m_u8g2.setFont( u8g2_font_blipfest_07_tr );
  
  static char pText[80] = {0};

  GetDisplayText( pText );
  m_u8g2.drawStr( 0, 5+iYOffsetMsg, pText );

  unsigned long iRealTimeSec = millis()/1000UL;
  unsigned long iRealTimeOffsetSec;
  if ( GetRealTimeOffsetSec( iRealTimeOffsetSec ) )
  {
    iRealTimeSec = ( iRealTimeOffsetSec + iRealTimeSec ) % HOURMINSEC_2_SEC( 24UL, 0UL, 0UL );
    PrintSec( iRealTimeSec, pText );
    u8g2_uint_t iTextWidth = m_u8g2.getStrWidth( pText );
    m_u8g2.drawStr( 128 - iTextWidth, 5+iYOffsetMsg, pText );
  }

  if ( iRealTimeSec > HOURMINSEC_2_SEC( 10UL, 0UL, 0UL ) && iRealTimeSec < HOURMINSEC_2_SEC( 22UL, 0UL, 0UL ) )
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
    /*const uint iTickLenX = 6;
    const uint iTickLenY = 3;
    m_u8g2.drawHLine( iXOffset[iSensorInd], 0+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd], 1+iYOffsetGraph, iTickLenY );

    m_u8g2.drawHLine( iXOffset[iSensorInd]+m_iTempDataCount-iTickLenX, 0+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd]+m_iTempDataCount-1, 1+iYOffsetGraph, iTickLenY );
    
    m_u8g2.drawHLine( iXOffset[iSensorInd], 32+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd], 32-iTickLenY+iYOffsetGraph, iTickLenY );

    m_u8g2.drawHLine( iXOffset[iSensorInd]+m_iTempDataCount-iTickLenX, 32+iYOffsetGraph, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd]+m_iTempDataCount-1, 32-iTickLenY+iYOffsetGraph, iTickLenY );*/

    m_u8g2.drawFrame( iXOffset[iSensorInd], iYOffsetGraph, m_iTempDataCount, 32 );
  }

  float fMinMin[2] = {32000,32000};
  float fMaxMax[2] = {-32000,-32000};
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    for ( byte iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
    {
      float fTemp = ( (float)m_pTemp[iSensorInd][iDataInd] / (float)( (iDataInd==m_iTempDataPointer) ? m_iTempDataCurrCounter : m_iTempDataCurrCount ) );
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

  float fMinMinMin = 32000;
  float fMaxMaxMax = -32000;
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
      
      
      float fTemp = ( (float)m_pTemp[iSensorInd][iInd] / (float)( (iInd==m_iTempDataPointer) ? m_iTempDataCurrCounter : m_iTempDataCurrCount ) );
      float fA = ( ( fTemp - fMinMinMin ) * 32.0f ) / ( fMaxMaxMax - fMinMinMin );
      if ( iDataInd == 0) fB = fA;

      int16_t iFrom;
      int16_t iCount;
      if ( fA < fB )
      {
        iFrom = (int16_t)fA;
        iCount = (int16_t)fB - iFrom;
      }
      else
      {
        iFrom = (int16_t)fB;
        iCount = (int16_t)fA - iFrom;
      }

      fB = fA;

      float fSubPixelOffsetX = 1.0f - (float)(m_iTempDataCurrCounter) / (float)m_iTempDataCurrCount;
      //for ( byte c = 0; c < ( ( bGraphType & 1 ) ? 1 : 2 ); c++ )
      for ( int16_t i = 0; i <= iCount; i++ )
      {
        float fRndX = (float)random( 4096 ) / 2048.0f - 1.0f;
        float fRndY = (float)random( 4096 ) / 2048.0f - 1.0f;
        fRndX *= absf( fRndX );
        fRndY *= absf( fRndY );
        fRndX *= 0.98f;
        fRndY *= 0.98f;

        int16_t x = (int16_t)( fSubPixelOffsetX + fRndX + (float)iDataInd );
        int16_t y = (int16_t)( 32.0f - ( fA + fRndY ) + (float)i );

        m_u8g2.drawPixel( iXOffset[iSensorInd] + x, y + iYOffsetGraph );
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

  /*for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    Serial.print( m_Sensors.rawToCelsius( m_iSensorTemperature[iSensorInd] ) );
    Serial.print( " C        ");
  }
  Serial.println( "" );*/
}

void CMainTask::UpdateTouchSensors()
{
  static float fTouch[m_bTouchSensorCount] = {0.0f};
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
