#include <Adafruit_ADS1015.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <vector>
#include "inc/GasManager.h"
#include "inc/Button.h"
#include "inc/Menu.h"
#include "inc/MenuRenderer.h"
#include "inc/WebServer.h"
#include "inc/SleepTimer.h"
#include "inc/ConfigurationManager.h"

using namespace std;

#define MAX_SCCM 5000

GasManager g_gasManager(1.73231201, -2.054456771);

WebServer g_webServer;

CompositeMenu* g_mainMenu = nullptr; 

Adafruit_ADS1115 ads1115;
SSD1306 display(0x3c, 5, 4);

SleepTimer g_sleepTimer(&display);

ConfigurationManager g_configurationManager(&g_gasManager, &g_sleepTimer);

const char* ssid     = "ESP32-Access-Point";
const char* password = "91239";

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
	// Display
	display.init();
	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_16);

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
	MenuRenderer* gasMenuRenderer = new SSD1306GasMenuRenderer(&display);	
	MenuRenderer* runMenuRenderer = new SSD1306RunMenuRenderer(&display, &ads1115, &g_gasManager);
	MenuRenderer* sleepTimerMenuRenderer = new SSD1306SleepTimerMenuRenderer(&display, &g_sleepTimer);		
		
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
	
	sleepTimerMenus.push_back(new SleepTimerMenuItem("5", "TIMER",  0, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("60", "TIMER", 1, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("120", "TIMER", 2, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("360", "TIMER", 3, &g_sleepTimer, sleepTimerMenuRenderer));
	sleepTimerMenus.push_back(new SleepTimerMenuItem("CONTINUOUS", "TIMER", 4, &g_sleepTimer, sleepTimerMenuRenderer));
	
	CompositeMenu* timerMenu = new CompositeMenu("TIMER","Main Menu" , sleepTimerMenus);	
	
	vector<Menu*> horizontalMenus;
	
	horizontalMenus.push_back(runMenu);
	horizontalMenus.push_back(libraryMenu);
	horizontalMenus.push_back(timerMenu);
	Serial.println("horizontal menu " + String(horizontalMenus.size())); 
	CompositeMenu* verticalMenu = new CompositeMenu("Main Menu", "", horizontalMenus);
	
	g_mainMenu = verticalMenu;
	
	g_mainMenu->print();
	
	setupButtons();
	
	g_webServer.init(&g_gasManager);
	g_sleepTimer.init(&g_configurationManager);
	
	g_configurationManager.init();
	g_configurationManager.loadFromEEPROM();
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
		CALIBRATION_MODE = !CALIBRATION_MODE;		
	});
	
}


void loop() 
{
	ButtonPressDetector::handleTick();
	
	g_sleepTimer.handleTick();	
	
	if(!CALIBRATION_MODE)
	{
		if(WiFi.getMode() != WIFI_OFF) {
			Serial.println("WIFI OFF START");
			WiFi.mode(WIFI_OFF);
			while(WiFi.getMode() != WIFI_OFF)
				delay(10);
			Serial.println("WIFI OFF END");
			g_webServer.stop();
			Serial.println("g_webServer STOP");
		}
		
		g_mainMenu->render();
		
	}
	else
	{
		g_sleepTimer.resetIdleCounter();
    
		if(WiFi.getMode() == WIFI_OFF)
		{
			setupWiFi();
		}
		//Serial.println("g_webServer BEFORE TICK");
		g_webServer.handleTick();
		//Serial.println("g_webServer AFTER TICK");
		display.clear();
		display.setColor(WHITE);
		display.setTextAlignment(TEXT_ALIGN_CENTER);

		display.drawString(64, 0,"CALIBRATION MODE");
		display.drawString(64, 20,WiFi.softAPIP().toString());
	//	display.drawString(64, 30,"SLOPE:" + String(SLOPE));
	//	display.drawString(64, 40,"INTERCEPT:" + String(INTERCEPT));
		display.display();
	} //END IS_CAL IF
	delay(10);
}
