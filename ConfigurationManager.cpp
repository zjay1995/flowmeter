#include "inc/ConfigurationManager.h"
#include <EEPROM.h>

#include "inc/SleepTimer.h"
#include "inc/GasManager.h"

ConfigurationManager::ConfigurationManager(GasManager* gasManager, SleepTimer* sleepTimer) : m_gasManager(gasManager),
																		m_sleepTimer(sleepTimer)
{

}

void ConfigurationManager::init()
{
	setupEEPROM();
}

void ConfigurationManager::loadFromEEPROM()
{
	m_loadAllInProgress = true;
	
	int timerInterval = EEPROM.readInt(EEPROM_TIMER_OFFSET);
	Serial.println("config: timer interval: " + String(timerInterval));
	
	m_sleepTimer->selectIntervalByValue( timerInterval );	
			
	// SLOPE
	double slope = -1;
	slope = EEPROM.readDouble(EEPROM_GAS_SLOPE_OFFSET);
	if(slope != -1)
		m_gasManager->setGasThermalConductivity("SLOPE", slope);
	Serial.println("config: slope: " + String(slope));
	// INTERCEPT
	double intercept = -1;
	intercept = EEPROM.readDouble(EEPROM_GAS_INTERCEPT_OFFSET);		
	if(intercept != -1)
		m_gasManager->setGasThermalConductivity("INTERCEPT", intercept);	
	Serial.println("config: intercept: " + String(intercept));
	
	double AirTC = -1;
	AirTC =	EEPROM.readDouble(EEPROM_GAS_AIR_TC_OFFSET);
	if(AirTC != -1)
		m_gasManager->setGasThermalConductivity("Air", AirTC);
	Serial.println("config: Air TC: " + String(AirTC));
	
	double O2TC = -1;
	O2TC = EEPROM.readDouble(EEPROM_GAS_O2_TC_OFFSET);
	if(O2TC != -1)
		m_gasManager->setGasThermalConductivity("O2", O2TC);
	Serial.println("config: O2 TC: " + String(O2TC));		
	
	double N2TC = -1;
	N2TC = EEPROM.readDouble(EEPROM_GAS_N2_TC_OFFSET);	
	if(N2TC != -1)
		m_gasManager->setGasThermalConductivity("N2", N2TC);
	Serial.println("config: N2 TC: " + String(N2TC));
	
	double HeTC = -1;
	HeTC = EEPROM.readDouble(EEPROM_GAS_He_TC_OFFSET);
	if(HeTC != -1)
		m_gasManager->setGasThermalConductivity("He", HeTC);
	Serial.println("config: He TC: " + String(HeTC));
	
	double H2TC = -1;
	H2TC = EEPROM.readDouble(EEPROM_GAS_H2_TC_OFFSET);		
	if(H2TC != -1)
		m_gasManager->setGasThermalConductivity("H2", H2TC);
	Serial.println("config: H2 TC: " + String(H2TC));
	
	double ArCH4TC = -1;
	ArCH4TC = EEPROM.readDouble(EEPROM_GAS_ArCH4_TC_OFFSET);		
	if(ArCH4TC != -1)
		m_gasManager->setGasThermalConductivity("ArCH4", ArCH4TC);
	Serial.println("config: ArCH4 TC: " + String(ArCH4TC));

	m_loadAllInProgress = false;

	return;		
}

void ConfigurationManager::saveGasThermalConductivity(String gasName, double tc)
{
	if(m_loadAllInProgress)
		return;
	
	if(gasName.equals("Air"))
	{
		Serial.println("EEPROM save Air: " + String(tc));
		EEPROM.writeDouble(EEPROM_GAS_AIR_TC_OFFSET, tc);
	}
	else if(gasName.equals("O2"))
	{
		Serial.println("EEPROM save O2: " + String(tc));
		EEPROM.writeDouble(EEPROM_GAS_O2_TC_OFFSET, tc);
	}
	else if(gasName.equals("N2"))
	{
		Serial.println("EEPROM save N2: " + String(tc));
		EEPROM.writeDouble(EEPROM_GAS_N2_TC_OFFSET, tc);
	}
	else if(gasName.equals("He"))
	{
		Serial.println("EEPROM save He: " + String(tc));
		EEPROM.writeDouble(EEPROM_GAS_He_TC_OFFSET, tc);
	}
	else if(gasName.equals("H2"))
	{
		Serial.println("EEPROM save H2: " + String(tc));
		EEPROM.writeDouble(EEPROM_GAS_H2_TC_OFFSET, tc);
	}
	else if(gasName.equals("ArCH4"))
	{
		Serial.println("EEPROM save ArCH4: " + String(tc));
		EEPROM.writeDouble(EEPROM_GAS_ArCH4_TC_OFFSET, tc);
	}
	
	if(!EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}


void ConfigurationManager::saveSlopeToEEPROM(double slope)
{
	if(m_loadAllInProgress)
		return;
	
	Serial.println("EEPROM saveSlopeToEEPROM: " + String(slope));
	size_t written = EEPROM.writeDouble(EEPROM_GAS_SLOPE_OFFSET, slope);
	Serial.println("EEPROM written: " + String(written));
	if(!EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}

void ConfigurationManager:: saveInterceptToEEPROM(double intercept)
{
	if(m_loadAllInProgress)
		return;
	Serial.println("EEPROM saveInterceptToEEPROM: " + String(intercept));
	size_t written = EEPROM.writeDouble(EEPROM_GAS_INTERCEPT_OFFSET, intercept);
	Serial.println("EEPROM written: " + String(written));
	if(!EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}


void ConfigurationManager::saveTimerIntervalToEEPROM(int interval)
{	
	if(m_loadAllInProgress)
		return;
	Serial.println("EEPROM saveTimerIntervalToEEPROM: " + String(interval));
	size_t written = EEPROM.put(0, interval);
	Serial.println("EEPROM written: " + String(written));
	if(!EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}	
		

void ConfigurationManager::setupEEPROM()
{
	if (!EEPROM.begin(EEPROM_SIZE))
	{
		Serial.println("EEPROM start failed");
		delay(60000);
		return;
	}
	
	Serial.println("EEPROM begin!!");
}
	
void ConfigurationManager::clearEEPROM()
{
    for (size_t i = 0; i < EEPROM_SIZE; ++i)
        EEPROM.writeByte(i, 0xFF);
    EEPROM.commit();
}
