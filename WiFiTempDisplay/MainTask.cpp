#include "MainTask.h"

  int16_t    CMainTask::m_pTempMin[SENSORCOUNT][m_iTempDataCount];
  int16_t    CMainTask::m_pTempMax[SENSORCOUNT][m_iTempDataCount];
  byte       CMainTask::m_iTempDataPointer;
  int16_t   CMainTask::m_iTempDataCurrCounter;

CMainTask::CMainTask()
: m_OneWire(m_iOneWireBus)
, m_Sensors(&m_OneWire)
, m_u8g2( U8G2_R2 )
{

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
	m_iTempDataCurrCounter = 0;
	for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
	{
	  for ( byte iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
	  {
	    m_pTempMin[iSensorInd][iDataInd] = m_iSensorTemperature[iSensorInd];
		m_pTempMax[iSensorInd][iDataInd] = m_iSensorTemperature[iSensorInd];
	  }
	}
  }
}

void CMainTask::Loop()
{
  UpdateSensors();

  Render();
}


void CMainTask::Render()
{
  const uint iXOffset[SENSORCOUNT] = {0,65};

  m_u8g2.clearBuffer();
  
  m_u8g2.setContrast( 0 );
  //u8g2.sendF("c", 0xa7 );

  //u8g2.setFont( u8g2_font_blipfest_07_tr );
  //u8g2.setFont( u8g2_font_lastapprenticebold_tr );
  //u8g2.setFont( u8g2_font_VCR_OSD_tf );
  //u8g2.setFont( u8g2_font_logisoso16_tf );

  m_u8g2.setFont( u8g2_font_blipfest_07_tr );
  
  static char pText[80] = {0};

  GetDisplayText( pText );
  m_u8g2.drawStr( 0, 63, pText );

  unsigned long iRealTimeOffsetSec;
  if ( GetRealTimeOffsetSec( iRealTimeOffsetSec ) )
  {
    iRealTimeOffsetSec = ( iRealTimeOffsetSec + millis()/1000UL ) % HOURMINSEC_2_SEC( 24UL, 0UL, 0UL );
    PrintSec( iRealTimeOffsetSec, pText );
    u8g2_uint_t iTextWidth = m_u8g2.getStrWidth( pText );
    m_u8g2.drawStr( 128 - iTextWidth, 63, pText );
  }

  //randomSeed(666);
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    {
        m_u8g2.setFont( u8g2_font_helvB14_tf );
        sprintf( pText, "%.2f", m_Sensors.rawToCelsius( m_iSensorTemperature[iSensorInd] ) );  
        u8g2_uint_t iTextWidth = m_u8g2.getStrWidth( pText );
        m_u8g2.drawStr( iXOffset[iSensorInd] + ( m_iTempDataCount - iTextWidth ) / 2, 47, pText);
    }
    const uint iTickLenX = 6;
    const uint iTickLenY = 3;
    m_u8g2.drawHLine( iXOffset[iSensorInd], 0, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd], 1, iTickLenY );

    m_u8g2.drawHLine( iXOffset[iSensorInd]+m_iTempDataCount-iTickLenX, 0, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd]+m_iTempDataCount-1, 1, iTickLenY );
    
    m_u8g2.drawHLine( iXOffset[iSensorInd], 32, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd], 32-iTickLenY, iTickLenY );

    m_u8g2.drawHLine( iXOffset[iSensorInd]+m_iTempDataCount-iTickLenX, 32, iTickLenX );
    m_u8g2.drawVLine( iXOffset[iSensorInd]+m_iTempDataCount-1, 32-iTickLenY, iTickLenY );

    //u8g2.drawFrame( iXOffset[iSensorInd], 0, m_iTempDataCount, 33 );
  }

  int16_t iMinMin[2] = {32000,32000};
  int16_t iMaxMax[2] = {-32000,-32000};
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    for ( byte iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
    {
      iMinMin[iSensorInd] = min( iMinMin[iSensorInd], m_pTempMin[iSensorInd][iDataInd] );
      iMaxMax[iSensorInd] = max( iMaxMax[iSensorInd], m_pTempMax[iSensorInd][iDataInd] );
    }

	{
		m_u8g2.setFont( u8g2_font_blipfest_07_tr );
		sprintf( pText, "%.1f", (float)iMinMin[iSensorInd] * 0.0078125f );
		m_u8g2.drawStr( iXOffset[iSensorInd] + m_iTempDataCount / 2 - 20, 55, pText);
		sprintf( pText, "%.1f", (float)iMaxMax[iSensorInd] * 0.0078125f );
		m_u8g2.drawStr( iXOffset[iSensorInd] + m_iTempDataCount / 2 + 5, 55, pText);
	}
  }

  int16_t iMinMinMin = 32000;
  int16_t iMaxMaxMax = -32000;
  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
	  iMinMinMin = min( iMinMin[iSensorInd], iMinMinMin );
	  iMaxMaxMax = max( iMaxMax[iSensorInd], iMaxMaxMax );
  }

  if ( iMaxMaxMax - iMinMinMin < 16*32 )
  {
    int16_t iAvg = ( iMaxMaxMax + iMinMinMin ) / 2;
    iMinMinMin = iAvg - 8*32;
    iMaxMaxMax = iAvg + 8*32;
  }

  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    for ( int16_t iDataInd = 0; iDataInd < m_iTempDataCount; iDataInd++ )
    {
      int16_t iInd = ( iDataInd + m_iTempDataPointer + 1 ) % m_iTempDataCount;
      const int16_t iRndScale = 512;    // 512-es scale-el 63 szeles kijelzo meg belefer
      
      int16_t iA = ( ( m_pTempMax[iSensorInd][iInd] - iMinMinMin ) * 32*iRndScale ) / ( iMaxMaxMax - iMinMinMin );
      int16_t iB = ( ( m_pTempMin[iSensorInd][iInd] - iMinMinMin ) * 32*iRndScale ) / ( iMaxMaxMax - iMinMinMin );

      iA = 32*iRndScale - iA;
      iB = 32*iRndScale - iB;
      int16_t iDotCount = ( iB - iA ) / iRndScale;
      for ( int i = 0; i <= iDotCount; i++ )
      {
        const int16_t iRndX = random( iRndScale );
        const int16_t iRndY = random( iRndScale*2 )-iRndScale;
        
        m_u8g2.drawPixel( iXOffset[iSensorInd] + ( iDataInd*iRndScale + iRndX + ((m_iTempDataCurrCount-m_iTempDataCurrCounter)*iRndScale/m_iTempDataCurrCount) ) / iRndScale, ( iA + iRndY ) / iRndScale + i );
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

  if ( m_iTempDataCurrCounter > m_iTempDataCurrCount )
  {
    m_iTempDataCurrCounter = 0;
    m_iTempDataPointer++;
    m_iTempDataPointer %= m_iTempDataCount;
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
      m_pTempMin[iSensorInd][m_iTempDataPointer] = m_iSensorTemperature[iSensorInd];
      m_pTempMax[iSensorInd][m_iTempDataPointer] = m_iSensorTemperature[iSensorInd];
    }
  }
  else
  {
    for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
    {
        m_pTempMin[iSensorInd][m_iTempDataPointer] = min( m_pTempMin[iSensorInd][m_iTempDataPointer], m_iSensorTemperature[iSensorInd] );
        m_pTempMax[iSensorInd][m_iTempDataPointer] = max( m_pTempMax[iSensorInd][m_iTempDataPointer], m_iSensorTemperature[iSensorInd] );
    }
  }
  m_iTempDataCurrCounter++;

  for ( byte iSensorInd = 0; iSensorInd < SENSORCOUNT; iSensorInd++ )
  {
    Serial.print( m_Sensors.rawToCelsius( m_iSensorTemperature[iSensorInd] ) );
    Serial.print( " C        ");
  }
  Serial.println( "");
}
