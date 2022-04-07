#include "MainTask.h"
#include "Globals.h"

CMainTask::CMainTask()
: m_OneWire(m_iOneWireBus)
, m_Sensors(&m_OneWire)
, m_u8g2( U8G2_R2 )
{

}

CMainTask::~CMainTask()
{

}

void CMainTask::Clear()
{

}

void CMainTask::Setup()
{
  Serial.print("MainTask setup on core ");
  Serial.println(xPortGetCoreID());

  m_Sensors.begin();
  m_Sensors.setResolution( 12 );
  m_Sensors.setWaitForConversion(false);
  m_Sensors.requestTemperatures();

  m_u8g2.begin();
  m_u8g2.setContrast( 0 );

}

void CMainTask::Loop()
{
  UpdateSensors();

  Render();
}


void CMainTask::Render()
{
  const uint iXOffset[2] = {0,65};

  m_u8g2.clearBuffer();
  
  m_u8g2.setContrast( 0 );
  //u8g2.sendF("c", 0xa7 );

  static char pTemp[40];
  //u8g2.setFont( u8g2_font_blipfest_07_tr );
  //u8g2.setFont( u8g2_font_lastapprenticebold_tr );
  //u8g2.setFont( u8g2_font_VCR_OSD_tf );
  //u8g2.setFont( u8g2_font_logisoso16_tf );

  m_u8g2.setFont( u8g2_font_blipfest_07_tr );
  
  static char pText[80] = {0};

  GetDisplayText( pText );
  m_u8g2.drawStr( 0, 63, pText );

  unsigned long iRealTimeOffsetSec;
  GetRealTimeOffsetSec( iRealTimeOffsetSec );
  iRealTimeOffsetSec = ( iRealTimeOffsetSec + millis()/1000UL ) % HOURMINSEC_2_SEC( 24UL, 0UL, 0UL );
  PrintSec( iRealTimeOffsetSec, pText );
  u8g2_uint_t  iTextWidth = m_u8g2.getStrWidth( pText );
  m_u8g2.drawStr( 128 - iTextWidth, 63, pText );

  for ( byte iSensorInd = 0; iSensorInd < 2; iSensorInd++ )
  {
    m_u8g2.setFont( u8g2_font_helvB14_tf );
    sprintf( pTemp, "%.2f", m_Sensors.rawToCelsius( m_iSensorTemperature[iSensorInd] ) );  
    m_u8g2.drawStr( iXOffset[iSensorInd], 49, pTemp);

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

  m_u8g2.sendBuffer();
}

void CMainTask::UpdateSensors()
{
  static unsigned long m_iSensorLastUpdateTimeMs = 0;
  if ( millis() - m_iSensorLastUpdateTimeMs < 750 )
  {
    return;
  }

  for ( byte iSensorInd = 0; iSensorInd < 2; iSensorInd++ )
  {
    m_iSensorTemperature[iSensorInd] = m_Sensors.getTemp(m_piSensorDeviceAddress[iSensorInd]);
  }
  m_Sensors.requestTemperatures();
  m_iSensorLastUpdateTimeMs = millis();

  for ( byte iSensorInd = 0; iSensorInd < 2; iSensorInd++ )
  {
    Serial.print( m_Sensors.rawToCelsius( m_iSensorTemperature[iSensorInd] ) );
    Serial.print( " C        ");
  }
  Serial.println( "");
}
