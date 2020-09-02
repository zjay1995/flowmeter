#pragma once

class Menu;
class SSD1306Wire;
class OLED;
class Adafruit_ADS1115;
class GasManager;
class GasMenuItem;
class RunMenuItem;
class SleepTimer;
class DataLogger;
class DataSource;
class TimeSync;


class MenuRenderer
{
public:

	virtual void render(Menu* menu) = 0;
};

class SSD1306MenuRenderer : public MenuRenderer
{
protected:	
	SSD1306Wire* 			m_display;
	
public:

	SSD1306MenuRenderer(SSD1306Wire* display) : m_display(display)
	{}

	void render(Menu* menu);
	
};

class SSD1306GasMenuRenderer : public SSD1306MenuRenderer
{


public:

	SSD1306GasMenuRenderer(SSD1306Wire* display);
	
	void render(Menu* menu);
	
};

class SSD1306RunMenuRenderer : public SSD1306MenuRenderer
{
	DataSource* 		m_dataSource;
	GasManager*				m_gasManager;
	
public:

	SSD1306RunMenuRenderer(SSD1306Wire* display, DataSource* dataSource, GasManager* gasManager);

	void render(Menu* menu);
};

class SSD1306SleepTimerMenuRenderer : public SSD1306MenuRenderer
{
	SleepTimer* m_sleepTimer;
	
public:	
	
	SSD1306SleepTimerMenuRenderer(SSD1306Wire* display, SleepTimer* sleepTimer);
	
	void render(Menu* menu);
};

class SSD1306FlashLoggerMenuRenderer : public SSD1306MenuRenderer
{
	DataLogger* m_dataLogger;
	
public:	
	
	SSD1306FlashLoggerMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger);
	
	void render(Menu* menu);
};

class SSD1306WiFiDumpMenuRenderer : public SSD1306MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SSD1306WiFiDumpMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SSD1306WiFiRealTimeDumpMenuRenderer : public SSD1306MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SSD1306WiFiRealTimeDumpMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SSD1306NTPSyncMenuRenderer : public SSD1306MenuRenderer
{
	TimeSync* m_timeSync;

public:

	SSD1306NTPSyncMenuRenderer(SSD1306Wire* display, TimeSync* timeSync);

	void render(Menu* menu);
};

class SSD1306ShowTimeMenuRenderer : public SSD1306MenuRenderer
{

public:

	SSD1306ShowTimeMenuRenderer(SSD1306Wire* display);

	void render(Menu* menu);
};


///////////
//// SH1106
///////////

class SH1106MenuRenderer : public MenuRenderer
{
protected:
	OLED* m_display;

public:

	SH1106MenuRenderer(OLED* display) : m_display(display)
	{}

	void render(Menu* menu);

};

class SH1106GasMenuRenderer : public SH1106MenuRenderer
{
public:

	SH1106GasMenuRenderer(OLED* display);

	void render(Menu* menu);

};

class SH1106RunMenuRenderer : public SH1106MenuRenderer
{
	DataSource* m_dataSource;
	GasManager* m_gasManager;

public:

	SH1106RunMenuRenderer(OLED* display, DataSource* dataSource, GasManager* gasManager);

	void render(Menu* menu);
};

class SH1106SleepTimerMenuRenderer : public SH1106MenuRenderer
{
	SleepTimer* m_sleepTimer;

public:

	SH1106SleepTimerMenuRenderer(OLED* display, SleepTimer* sleepTimer);

	void render(Menu* menu);
};

class SH1106FlashLoggerMenuRenderer : public SH1106MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SH1106FlashLoggerMenuRenderer(OLED* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SH1106WiFiDumpMenuRenderer : public SH1106MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SH1106WiFiDumpMenuRenderer(OLED* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SH1106WiFiRealTimeDumpMenuRenderer : public SH1106MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SH1106WiFiRealTimeDumpMenuRenderer(OLED* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SH1106NTPSyncMenuRenderer : public SH1106MenuRenderer
{
	TimeSync* m_timeSync;

public:

	SH1106NTPSyncMenuRenderer(OLED* display, TimeSync* timeSync);

	void render(Menu* menu);
};

class SH1106ShowTimeMenuRenderer : public SH1106MenuRenderer
{

public:

	SH1106ShowTimeMenuRenderer(OLED* display);

	void render(Menu* menu);
};



