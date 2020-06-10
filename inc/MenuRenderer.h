#pragma once

class Menu;
class SSD1306Wire;
class Adafruit_ADS1115;
class GasManager;
class GasMenuItem;
class RunMenuItem;
class SleepTimer;

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
	Adafruit_ADS1115* 		m_ads1115;
	GasManager*				m_gasManager;
	
public:

	SSD1306RunMenuRenderer(SSD1306Wire* display, Adafruit_ADS1115* ads1115, GasManager* gasManager);

	void render(Menu* menu);
};

class SSD1306SleepTimerMenuRenderer : public SSD1306MenuRenderer
{
	SleepTimer* m_sleepTimer;
	
public:	
	
	SSD1306SleepTimerMenuRenderer(SSD1306Wire* display, SleepTimer* sleepTimer);
	
	void render(Menu* menu);
};


