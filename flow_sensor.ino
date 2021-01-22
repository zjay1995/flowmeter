#include <Adafruit_ADS1015.h>
#include <SSD1306.h>
#include <U8g2lib.h>
#include <Wire.h>

#include <WiFi.h>
#include <vector>

#include "inc/TimeSync.h"
#include "inc/GasManager.h"
#include "inc/Button.h"
#include "inc/Menu.h"
#include "inc/MenuRenderer.h"
#include "inc/WebServer.h"
#include "inc/SleepTimer.h"
#include "inc/Globals.h"
#include "inc/DataLogger.h"
#include "inc/DataSource.h"

using namespace std;


#define USE_SSD1306_DISPLAY
//#define USE_SSD1327_DISPLAY


#define MAX_SCCM 5000

#define wifi_ssid "1134ea"
#define wifi_password "DNm11i223344"


GasManager g_gasManager(1.73231201, -2.054456771);

WebServer g_webServer;

CompositeMenu* g_mainMenu = nullptr; 

Adafruit_ADS1115 ads1115;

#ifdef USE_SSD1306_DISPLAY
SSD1306 display(0x3c, 5, 4);
#endif

#ifdef USE_SSD1327_DISPLAY
U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI display(U8G2_R0, /* clock=*/ 27, /* data=*/ 26, /* cs=*/ 25, /* dc=*/ 33, /* reset=*/ 32);
#endif

SleepTimer g_sleepTimer;

DataLogger g_dataLogger;

TimeSync g_timeSync;

const char* ssid     = "ESP32-Access-Point";
const char* password = "Polaroid1";

volatile bool CALIBRATION_MODE = false;

void setupWiFi() {
	Serial.print("Setting AP (Access Point)…");
	// Remove the password parameter, if you want the AP (Access Point) to be open
	WiFi.softAP(ssid, password);

	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);

	g_webServer.begin(80);
}

void IRAM_ATTR dummyTouchISR(){}

void setup() {
	Serial.begin(115200);
	// DEEP-SLEEP init

	//esp_sleep_enable_ext1_wakeup(0x8004, ESP_EXT1_WAKEUP_ANY_HIGH);
	esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);
	// ADC
	ads1115.begin();
	ads1115.setGain(GAIN_ONE);
	AnalogSourceInput* ads1115AnalogSourceInput = new ADS1115AnalogSourceInput(&ads1115);
	DataSource* dataSource = new DataSource(&g_gasManager, ads1115AnalogSourceInput);

	// Gas Manager
	g_gasManager.setConfigurationManager(&g_configurationManager);
	
	g_gasManager.addGas(Gas("Air", 1.0));
	g_gasManager.addGas(Gas("O2", 1.0));
	g_gasManager.addGas(Gas("N2", 1.0));
	g_gasManager.addGas(Gas("He", 5.73));
	g_gasManager.addGas(Gas("H2", 6.84));
	g_gasManager.addGas(Gas("ArCH4", 0.85));
	//	
	/// Menus	
	//
	
	#ifdef USE_SSD1306_DISPLAY
	
	// Display
	display.init();
	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_16);
	
	MenuRenderer* gasMenuRenderer = new SSD1306GasMenuRenderer(&display);	
	MenuRenderer* runMenuRenderer = new SSD1306RunMenuRenderer(&display, dataSource, &g_gasManager);
	MenuRenderer* sleepTimerMenuRenderer = new SSD1306SleepTimerMenuRenderer(&display, &g_sleepTimer);		
	MenuRenderer* flashLoggerMenuRenderer = new SSD1306FlashLoggerMenuRenderer(&display, &g_dataLogger);		
	MenuRenderer* wifiDumpMenuRenderer = new SSD1306WiFiDumpMenuRenderer(&display, &g_dataLogger);
	MenuRenderer* wifiRealTimeDumpMenuRenderer = new SSD1306WiFiRealTimeDumpMenuRenderer(&display, &g_dataLogger);
	MenuRenderer* NTPSyncMenuRenderer = new SSD1306NTPSyncMenuRenderer(&display, &g_timeSync);
	MenuRenderer* showTimeMenuRenderer = new SSD1306ShowTimeMenuRenderer(&display);
	#endif
	
	#ifdef USE_SSD1327_DISPLAY
	
	// Display
	display.begin();
	
	MenuRenderer* gasMenuRenderer = new SSD1327GasMenuRenderer(&display);	
	MenuRenderer* runMenuRenderer = new SSD1327RunMenuRenderer(&display, dataSource, &g_gasManager);
	MenuRenderer* sleepTimerMenuRenderer = new SSD1327SleepTimerMenuRenderer(&display, &g_sleepTimer);		
	MenuRenderer* flashLoggerMenuRenderer = new SSD1327FlashLoggerMenuRenderer(&display, &g_dataLogger);		
	MenuRenderer* wifiDumpMenuRenderer = new SSD1327WiFiDumpMenuRenderer(&display, &g_dataLogger);
	MenuRenderer* wifiRealTimeDumpMenuRenderer = new SSD1327WiFiRealTimeDumpMenuRenderer(&display, &g_dataLogger);
	MenuRenderer* NTPSyncMenuRenderer = new SSD1327NTPSyncMenuRenderer(&display, &g_timeSync);
	MenuRenderer* showTimeMenuRenderer = new SSD1327ShowTimeMenuRenderer(&display);
	#endif

	vector<Menu*> runMenus;
	
	runMenus.push_back(new RunMenuItem("RUN", "RUN", &g_gasManager, runMenuRenderer));
	CompositeMenu* runMenu = new CompositeMenu("RUN", "Main Menu", runMenus);
	
	// Gas Menus
	vector<Menu*> gasMenus;
	
	gasMenus.push_back(new GasMenuItem("Air", "LIBRARY",  0, &g_gasManager, gasMenuRenderer));
	gasMenus.push_back(new GasMenuItem("O2", "LIBRARY", 1, &g_gasManager, gasMenuRenderer));
	gasMenus.push_back(new GasMenuItem("N2", "LIBRARY", 2, &g_gasManager, gasMenuRenderer));
	gasMenus.push_back(new GasMenuItem("He", "LIBRARY",  0, &g_gasManager, gasMenuRenderer));
	gasMenus.push_back(new GasMenuItem("H2", "LIBRARY", 1, &g_gasManager, gasMenuRenderer));
	gasMenus.push_back(new GasMenuItem("ArCH4", "LIBRARY", 2, &g_gasManager, gasMenuRenderer));
	
	CompositeMenu* libraryMenu = new CompositeMenu("LIBRARY","Main Menu" , gasMenus);	
	
	// Timer Menus
	vector<Menu*> sleepTimerMenus;
/*	
	sleepTimerMenus.push_back(new SleepTimerMenuItem("5", "TIMER",  0, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("60", "TIMER", 1, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("120", "TIMER", 2, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("360", "TIMER", 3, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("CONTINUOUS", "TIMER", 4, &g_sleepTimer, sleepTimerMenuRenderer));
	
	CompositeMenu* timerMenu = new CompositeMenu("TIMER","Main Menu" , sleepTimerMenus);	
	*/
	// DataLogger Menus
	vector<Menu*> dataLoggerMenus;
	
	dataLoggerMenus.push_back(new DataLoggerFlashStoreMenuItem("FLASH LOGGER", "DATALOGGER", 	&g_dataLogger, flashLoggerMenuRenderer));
	dataLoggerMenus.push_back(new WiFiDumpMenuItem("WIFI DUMP", "DATALOGGER", 				 	&g_dataLogger, wifiDumpMenuRenderer));
	dataLoggerMenus.push_back(new WiFiRealTimeDumpMenuItem("WIFI REAL-TIME DUMP", "DATALOGGER", &g_dataLogger, wifiRealTimeDumpMenuRenderer));
	
	
	CompositeMenu* dataLoggerMenu = new CompositeMenu("DATALOGGER", "Main Menu" , dataLoggerMenus);		
	
	// DateTime menu
	vector<Menu*> dateTimeMenus;
	
	dateTimeMenus.push_back(new NTPSyncMenuItem("NTP Sync", "DATETIME", &g_timeSync, NTPSyncMenuRenderer));
	dateTimeMenus.push_back(new ShowTimeMenuItem("Current DateTime", "DATETIME", showTimeMenuRenderer));

	CompositeMenu* dateTimeMenu = new CompositeMenu("DATETIME", "Main Menu" , dateTimeMenus);		

////////////////////////////////////	
	vector<Menu*> horizontalMenus;
	
	horizontalMenus.push_back(runMenu);
	horizontalMenus.push_back(libraryMenu);
//	horizontalMenus.push_back(timerMenu);
	horizontalMenus.push_back(dataLoggerMenu);
	horizontalMenus.push_back(dateTimeMenu);
	Serial.println("horizontal menu " + String(horizontalMenus.size())); 
	CompositeMenu* verticalMenu = new CompositeMenu("Main Menu", "", horizontalMenus);
	
	g_mainMenu = verticalMenu;
	
	g_mainMenu->print();
	
	setupButtons();
	
	g_webServer.init(&g_gasManager);
	g_sleepTimer.init(&g_configurationManager);
	
	g_dataLogger.init(dataSource, &g_gasManager);
	
	g_webServer.addParamChangeListener((ParamChangeListener*)&g_configurationManager);
	g_webServer.addParamChangeListener((ParamChangeListener*)&g_gasManager);
	
	g_configurationManager.addParamChangeListener((ParamChangeListener*)&g_timeSync);
	g_configurationManager.addParamChangeListener((ParamChangeListener*)&g_gasManager);
	g_configurationManager.addParamChangeListener((ParamChangeListener*)g_dataLogger.getMqttFlashPublisher());
	g_configurationManager.addParamChangeListener((ParamChangeListener*)g_dataLogger.getMqttRealTimePublisher());
	
	g_configurationManager.init();
	g_configurationManager.loadFromEEPROM();
	
	g_timeSync.initTimeFromRTC();
}

void setupButtons()
{
	//pinMode(14, INPUT_PULLUP);
	//pinMode(2, INPUT_PULLUP);
	//pinMode(0, INPUT_PULLUP);
	//pinMode(15, INPUT_PULLUP);
	
	Keyboard* keyboard = new Keyboard();
		
	keyboard->addOnDownPressedFctor([]{
		
		g_sleepTimer.resetIdleCounter();
		
		if(CALIBRATION_MODE)
			return;
		Serial.println("PRESS DOWWWNNNN");
		g_mainMenu->print();
		g_mainMenu->moveToNext();	
	
	});
	keyboard->addOnSPressedFctor([]{
		
		g_sleepTimer.resetIdleCounter();
		
		if(CALIBRATION_MODE)
			return;
		Serial.println("PRESS S");
		g_mainMenu->action();		
	});
	keyboard->addOnRightPressedFctor([]{
		
		g_sleepTimer.resetIdleCounter();
		
		if(CALIBRATION_MODE)
			return;
		Serial.println("PRESS RIGHT");
		((CompositeMenu*)g_mainMenu->getCurrentMenu())->moveToNext();		
	});
	
	keyboard->addOnCalibrationComboPressedFctor([]{
		
		g_sleepTimer.resetIdleCounter();
		
		Serial.println("PRESS CALIBRATION");
		
		if(CALIBRATION_MODE)
		{
			Serial.println("WIFI OFF START");
			WiFi.mode(WIFI_OFF);
			while(WiFi.getMode() != WIFI_OFF)
				delay(10);
			Serial.println("WIFI OFF END");
			g_webServer.stop();
			Serial.println("g_webServer STOP");
		}

		CALIBRATION_MODE = !CALIBRATION_MODE;		
	});
	
}


void loop() 
{
	ButtonPressDetector::handleTick();
	
	g_sleepTimer.handleTick();	
	g_dataLogger.handleTick();
	
	if(!CALIBRATION_MODE)
	{
		g_mainMenu->render();
	}
	else
	{
		g_sleepTimer.resetIdleCounter();
    
		if(WiFi.getMode() == WIFI_OFF)
		{
			setupWiFi();
		}

		g_webServer.handleTick();

		#ifdef USE_SSD1306_DISPLAY
		display.clear();
		display.setColor(WHITE);
		display.setTextAlignment(TEXT_ALIGN_CENTER);
		display.drawString(64, 0,"CALIBRATION MODE");
		display.drawString(64, 20,WiFi.softAPIP().toString().c_str());
		display.display();
		#endif
		
		#ifdef USE_SSD1327_DISPLAY
		display.clearBuffer();
		display.drawStr(64, 0,"CALIBRATION MODE");
		display.drawStr(64, 20,WiFi.softAPIP().toString().c_str());
		display.sendBuffer();
		#endif
		
	} //END IS_CAL IF
	delay(10);
}
