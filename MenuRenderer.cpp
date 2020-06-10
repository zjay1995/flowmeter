#include "inc/MenuRenderer.h"

#include "inc/Menu.h"
#include "inc/SleepTimer.h"
#include <Adafruit_ADS1015.h>
#include "SSD1306.h"
#include <Arduino.h>

SSD1306GasMenuRenderer::SSD1306GasMenuRenderer(SSD1306Wire* display) : SSD1306MenuRenderer(display)
																	 
{
	
	
}

void SSD1306GasMenuRenderer::render(Menu* menu)
{
	Serial.println("SSD1306GasMenuRenderer:" );
	Serial.flush();
	
	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0, menu->getParentName());
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30 ,menu->getName());
	m_display->display();
}



SSD1306RunMenuRenderer::SSD1306RunMenuRenderer(SSD1306Wire* display, Adafruit_ADS1115* ads1115, GasManager* gasManager) : SSD1306MenuRenderer(display),
																														  m_ads1115(ads1115),
																														  m_gasManager(gasManager)	
{
	
	
}

void SSD1306RunMenuRenderer::render(Menu* menu)
{
	const float multiplier = 0.125F; //GAIN 1
	
	int16_t ads_val = m_ads1115->readADC_SingleEnded(0); //  analogRead(36);
	//Serial.print("ADC A0: "); 
	//Serial.println(ads_val);
	//Serial.println("15: " + String(digitalRead(15)));  

	Gas& selectedGas = m_gasManager->getSelectedGas();

	m_display->clear();
	m_display->setColor(WHITE);
	m_display->setTextAlignment(TEXT_ALIGN_CENTER);
	m_display->drawString(64, 0,selectedGas.getName() + " " + String(m_gasManager->calculateSLM(ads_val * multiplier / 1000.0)) + "sccm");
	m_display->drawLine(10, 24, 256, 24);
	m_display->drawString(64, 30 ,String(ads_val * multiplier) + "mV");
	m_display->display();

}

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


