#include "inc/MenuRenderer.h"

#include "inc/Menu.h"
#include "inc/SleepTimer.h"
#include "inc/DataSource.h"
#include <Adafruit_ADS1015.h>
#include "SSD1306.h"
#include <Arduino.h>

SSD1306GasMenuRenderer::SSD1306GasMenuRenderer(SSD1306Wire* display) : SSD1306MenuRenderer(display)
																	 
{
	
	
}

void SSD1306GasMenuRenderer::render(Menu* menu)
{
	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0, menu->getParentName());
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30 ,menu->getName());
	m_display->display();
}



SSD1306RunMenuRenderer::SSD1306RunMenuRenderer(SSD1306Wire* display, DataSource* dataSource, GasManager* gasManager) : SSD1306MenuRenderer(display),
																														  m_dataSource(dataSource),
																														  m_gasManager(gasManager)	
{
	
	
}

void SSD1306RunMenuRenderer::render(Menu* menu)
{
	const float multiplier = 0.125F; //GAIN 1
	
	double sensor_val = m_dataSource->getDoubleValue();
	//Serial.print("ADC A0: "); 
	//Serial.println(ads_val);
	//Serial.println("15: " + String(digitalRead(15)));  

	Gas& selectedGas = m_gasManager->getSelectedGas();

	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0,selectedGas.getName() + " " + String(sensor_val) + "sccm");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30 ,String( m_dataSource->getRawMiliVolts() ) + "mV");
	m_display->display();

}

///////////////////////////

SSD1306SleepTimerMenuRenderer::SSD1306SleepTimerMenuRenderer(SSD1306Wire* display, SleepTimer* sleepTimer) : SSD1306MenuRenderer(display),
																											 m_sleepTimer(sleepTimer)
{
	
	
	
	
	
	
}

void SSD1306SleepTimerMenuRenderer::render(Menu* menu)
{
	//int interval = m_sleepTimer->getSelectedInterval();

	//String intervalText = interval > 0 ? String(interval) : "CONTINUOUS";

	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0,"TIMER SLEEP");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30 ,menu->getName());
	m_display->display();
}

///////////////////////////////

SSD1306FlashLoggerMenuRenderer::SSD1306FlashLoggerMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger) : SSD1306MenuRenderer(display),
																											    m_dataLogger(dataLogger)
{
	
	
}
	
void SSD1306FlashLoggerMenuRenderer::render(Menu* menu)
{
	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0,"DATA LOGGER");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30 ,menu->getName());
	m_display->drawString(64, 40 , m_dataLogger->isFlashStoreSessionRunning() ? "Started" : "Idle");
	m_display->display();
	
	
}

///////////////////////////////

SSD1306WiFiDumpMenuRenderer::SSD1306WiFiDumpMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger) : SSD1306MenuRenderer(display),
m_dataLogger(dataLogger)
{


}

void SSD1306WiFiDumpMenuRenderer::render(Menu* menu)
{
	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0, "WIFI DATA DUMP");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30, menu->getName());
	m_display->drawString(64, 40, m_dataLogger->isWiFiDumpRunning() ? "Started" : "Idle");
	m_display->display();


}

SSD1306WiFiRealTimeDumpMenuRenderer::SSD1306WiFiRealTimeDumpMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger) : SSD1306MenuRenderer(display),
m_dataLogger(dataLogger)
{


}

void SSD1306WiFiRealTimeDumpMenuRenderer::render(Menu* menu)
{
	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0, "WIFI REAL-TIME DUMP");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30, menu->getName());
	m_display->drawString(64, 40, m_dataLogger->isWiFiDumpRunning() ? "Started" : "Idle");
	m_display->display();


}

SSD1306NTPSyncMenuRenderer::SSD1306NTPSyncMenuRenderer(SSD1306Wire* display, TimeSync* timeSync) : SSD1306MenuRenderer(display),
																									m_timeSync(timeSync)
{

}

void SSD1306NTPSyncMenuRenderer::render(Menu* menu)
{
	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0, "NTP Sync");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30, menu->getName());
	m_display->drawString(64, 40, m_timeSync->isNTCSyncRunning() == true ? "In Progress!" : "Idle");
	m_display->display();

}

SSD1306ShowTimeMenuRenderer::SSD1306ShowTimeMenuRenderer(SSD1306Wire* display) : SSD1306MenuRenderer(display)
{

}

void SSD1306ShowTimeMenuRenderer::render(Menu* menu)
{
	int64_t startMicros = esp_timer_get_time();

	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);

	struct tm timeinfo;
	getLocalTime(&timeinfo, 10);

	int64_t passed = esp_timer_get_time() - startMicros;

	Serial.println("render time: " + String((uint32_t)(passed / 1000)));

	char dateString[30] = { 0 };
	char timeString[30] = { 0 };
	strftime(dateString, 30, "%b %d %y", &timeinfo);
	strftime(timeString, 30, "%H:%M:%S", &timeinfo);

	m_display->drawString(64, 0, "Current DateTime");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 28, String(dateString));
	m_display->drawString(64, 45, String(timeString));
	m_display->display();



}


