#pragma once

#include <Arduino.h>

class SleepTimer;
class GasManager;


class ConfigurationManager
{
	
const uint8_t EEPROM_SIZE = sizeof(int) + 			// TIMER
							sizeof(double) * (2 + 8);	// GASES
	
const uint8_t EEPROM_TIMER_OFFSET = 0;
const uint8_t EEPROM_GAS_SLOPE_OFFSET = 4;
const uint8_t EEPROM_GAS_INTERCEPT_OFFSET = 12;
const uint8_t EEPROM_GAS_AIR_TC_OFFSET = 20;
const uint8_t EEPROM_GAS_O2_TC_OFFSET = 28;
const uint8_t EEPROM_GAS_N2_TC_OFFSET = 36;
const uint8_t EEPROM_GAS_He_TC_OFFSET = 44;
const uint8_t EEPROM_GAS_H2_TC_OFFSET = 52;
const uint8_t EEPROM_GAS_ArCH4_TC_OFFSET = 60;


	SleepTimer* m_sleepTimer;
	GasManager* m_gasManager;

	bool m_loadAllInProgress = false;

public:

	ConfigurationManager(GasManager* gasManager, SleepTimer* sleepTimer);
	
	void init();
	
	void loadFromEEPROM();
		
	void saveTimerIntervalToEEPROM(int interval);
	void saveSlopeToEEPROM(double slope);
	void saveInterceptToEEPROM(double intercept);
	
	void saveGasThermalConductivity(String gasName, double tc);

	void setupEEPROM();
	
	void clearEEPROM();

};