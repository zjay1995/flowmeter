#include <Arduino.h>

#include <vector>
#include <algorithm>
#include <functional>

#include "pins.h"

class ButtonPressListener 
{
public:

	virtual void onButtonPressUp(int pinNum, unsigned long pressDuration);
	virtual void onButtonPressDown(int pinNum);
};

class ButtonPressDetector 
{
	
protected:

	int m_pin;
	std::vector<ButtonPressListener*> m_listeners;

	static std::vector<ButtonPressDetector*> s_ButtonPressDetectors;	
	
public:
	ButtonPressDetector(int pin) : m_pin(pin)
	{
		
	}
	
	virtual ~ButtonPressDetector()
	{
		for(auto l : m_listeners)
			delete l;
	}
		
	virtual void tick()=0;
	
	void addListener(ButtonPressListener* l) { m_listeners.push_back(l); }
	
	void notifyPressDownListeners() 
	{
		for(auto l : m_listeners)
			l->onButtonPressDown(m_pin);	
	}
	
	void notifyPressUpListeners(unsigned int pressDuration) 
	{
		for(auto l : m_listeners)
			l->onButtonPressUp(m_pin, pressDuration);	
	}
	
	int getPinNum() const { return m_pin; }
	
	static void handleTick()
	{
		for(auto d: s_ButtonPressDetectors)
			d->tick();
	}
	
};

/*

	void tick()
	{
		int pinState = digitalRead(m_pin);
 		if(pinState != m_currentPinState)
		{
			if(pinState == m_activePinState)
			{
				m_pressDownStartMillis = millis();
			}
			else
			{
				unsigned long elapsedMillis = millis() - m_pressDownStartMillis;
				if(elapsedMillis > m_pressDurationMilis && elapsedMillis < m_pressDurationUpperBoundMilis )
				{				
					m_pressDownStartMillis = 0;
					Serial.println("CLICK:" + String(m_pin) + " " + String(elapsedMillis) );
					//notify button
					notifyPressUpListeners()
				}
			}
			m_currentPinState = pinState;
		}
		
	}	
*/


class DigitalReadSingleButtonPressDetector : public ButtonPressDetector
{

	int 	m_currentPinState;
	int		m_activePinState;
	
	unsigned long m_pressDownStartMillis;
	
public:

	DigitalReadSingleButtonPressDetector(int pinNum, int pressDurationMilis = 150) : ButtonPressDetector(pinNum)
	{
		m_activePinState = 0;
		pinMode(pinNum, INPUT_PULLUP);
		m_currentPinState = digitalRead(m_pin);
		
		s_ButtonPressDetectors.push_back(this);
	}

	~DigitalReadSingleButtonPressDetector()
	{
		remove_if(s_ButtonPressDetectors.begin(), s_ButtonPressDetectors.end(), [&, this](ButtonPressDetector* d){return this == d;} );
	}
	
	void tick()
	{
		int pinState = digitalRead(m_pin);
 		if(pinState != m_currentPinState)
		{
			if(pinState == m_activePinState)
			{
				m_pressDownStartMillis = millis();
				notifyPressDownListeners();
			}
			else
			{
				unsigned long elapsedMillis = millis() - m_pressDownStartMillis;
				notifyPressUpListeners(elapsedMillis);
				m_pressDownStartMillis = 0;
			}
			m_currentPinState = pinState;
		}
	}	
};

class Keyboard : ButtonPressListener
{
	
	unsigned long m_buttonDownTotalMillis;
	unsigned long m_buttonRightTotalMillis;
	unsigned long m_buttonSTotalMillis;

	ButtonPressDetector* m_buttonDownDetector;
	ButtonPressDetector* m_buttonSDetector;
	ButtonPressDetector* m_buttonRightDetector;
	
	std::function<void()> m_onCalibrationComboPressed;
	std::function<void()> m_onDownPressed;
	std::function<void()> m_onSPressed;
	std::function<void()> m_onRightPressed;
	
public:

	Keyboard()
	{
		m_buttonDownDetector = new DigitalReadSingleButtonPressDetector(BUTTON_DOWN_PIN);
		m_buttonSDetector = new DigitalReadSingleButtonPressDetector(BUTTON_S_PIN);
		m_buttonRightDetector = new DigitalReadSingleButtonPressDetector(BUTTON_RIGHT_PIN);
		
		m_buttonDownDetector->addListener(this);
		m_buttonSDetector->addListener(this);
		m_buttonRightDetector->addListener(this);
		
		m_buttonDownTotalMillis = 0;
		m_buttonRightTotalMillis = 0;
		m_buttonSTotalMillis = 0;
	}

	~Keyboard()=default;
	
	void addOnDownPressedFctor(const std::function<void()>& f) { m_onDownPressed = f; }
	void addOnSPressedFctor(const std::function<void()>& f) { m_onSPressed = f; }
	void addOnRightPressedFctor(const std::function<void()>& f) { m_onRightPressed = f; }
	
	void addOnCalibrationComboPressedFctor(const std::function<void()>& f) { m_onCalibrationComboPressed = f; }
	
	void onButtonPressUp(int pinNum, unsigned long pressDuration)
	{
		Serial.println("Press duration for pinNum:" + String(pinNum) + " duration: " + String(pressDuration));
		if(m_buttonDownDetector->getPinNum() == pinNum)
		{
			m_buttonDownTotalMillis = pressDuration;
			if(
				pressDuration > BUTTON_SINGLE_CLICK_HOLD_DURATION && 
				pressDuration < BUTTON_SINGLE_CLICK_HOLD_DURATION + 200 &&
				m_buttonRightTotalMillis == 0 && 
				m_buttonSTotalMillis == 0
			)
			{
				m_onDownPressed();
				
				m_buttonDownTotalMillis = 0;
				return;
			}
		}
		else if(m_buttonRightDetector->getPinNum() == pinNum)
		{
			m_buttonRightTotalMillis = pressDuration;
			if(
				pressDuration > BUTTON_SINGLE_CLICK_HOLD_DURATION && 
				pressDuration < BUTTON_SINGLE_CLICK_HOLD_DURATION + 200 &&
				m_buttonSTotalMillis == 0 && 
				m_buttonDownTotalMillis == 0
			)
			{
				m_onRightPressed();
				
				m_buttonRightTotalMillis = 0;
				return;
			}
		}
		else if(m_buttonSDetector->getPinNum() == pinNum)
		{
			m_buttonSTotalMillis = pressDuration;
			
			if(
				pressDuration > BUTTON_SINGLE_CLICK_HOLD_DURATION && 
				pressDuration < BUTTON_SINGLE_CLICK_HOLD_DURATION + 200 &&
				m_buttonRightTotalMillis == 0 && 
				m_buttonDownTotalMillis == 0
			)
			{
				m_onSPressed();
				
				m_buttonSTotalMillis = 0;
				return;
			}
			
		}

		if( m_buttonDownTotalMillis != 0 && m_buttonRightTotalMillis != 0 && m_buttonSTotalMillis == 0)
		{
			if(m_buttonDownTotalMillis > BUTTON_COMBO_CLICK_HOLD_DURATION && m_buttonRightTotalMillis > BUTTON_COMBO_CLICK_HOLD_DURATION)  		
				m_onCalibrationComboPressed();
			
			m_buttonDownTotalMillis = 0;
			m_buttonRightTotalMillis = 0;
			
			return;
		}
	}
	
	void onButtonPressDown(int pinNum)
	{
		if(m_buttonDownDetector->getPinNum() == pinNum)
			m_buttonDownTotalMillis = 0;
		if(m_buttonRightDetector->getPinNum() == pinNum)
			m_buttonRightTotalMillis = 0;
		if(m_buttonSDetector->getPinNum() == pinNum)
			m_buttonSTotalMillis = 0;
	}
	
};