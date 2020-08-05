#pragma once

#include <vector>

#include <Arduino.h>

#include "ConfigurationManager.h"



class Gas
{
	String m_name;	
	double m_slope;
	double m_intercept;
	double m_thermalConductivity;
	
public:
	
	Gas(String name, double thermalConductivity = 1.0) : m_name(name),
													   m_thermalConductivity(thermalConductivity)
	{
		
	}
	
	String getName() { return m_name; }

	double getThermalConductivity() const 					{ return m_thermalConductivity; }
	void setThermalConductivity(double thermalConductivity) { m_thermalConductivity = thermalConductivity; }
	
};



class GasManager : public ParamChangeListener
{
	std::vector<Gas> m_gases;
	
	int m_selectedGas = 0;
	
	double m_slope;
	double m_intercept;
	
	ConfigurationManager* m_configurationManager;
	
public:

	GasManager(double slope, double intercept) : m_slope(slope),
												 m_intercept(intercept)
	{}	
	
	~GasManager() {}	
	
	double getSlope() const { return m_slope; }
	double getIntercept() const { return m_intercept; }
	void setSlope(double s) { m_slope = s; }
	void setIntercept(double i) { m_intercept = i; }
	
	
	double calculateSLM(double voltage) {
		double val = (m_slope * voltage + m_intercept) * getSelectedGas().getThermalConductivity();
		//Serial.println("calculateSLM " + String(val) + " " + String(voltage) + " " + String(getSelectedGas().getThermalConductivity()) + " " + String(m_slope) + " " + String(m_intercept));
		return val < 0 ? 0 : val;
	}
	
	void setConfigurationManager(ConfigurationManager* c) { m_configurationManager = c; }
	
	void addGas(Gas g) { m_gases.push_back(g); }
	
	void selectNextGas()
	{
		m_selectedGas = (m_selectedGas + 1) % m_gases.size();
	}
	
	void selectGasByIndex(int index)
	{
		if(index >= 0 && index < m_gases.size())
		{
			m_selectedGas = index;
			
		}
	}
	
	void selectPreviousGas()
	{
		if(m_selectedGas == 0)
			m_selectedGas = m_gases.size() - 1;
		else
			m_selectedGas = m_selectedGas - 1;
	}
	
	Gas& getSelectedGas() { return m_gases[m_selectedGas]; }
	
	
	std::vector<Gas>& getAllGases() { return m_gases; }
	
	void onParamChange(String param, String value)
	{

		Serial.println("onGasParamChange: " + param + "=" + value);
		
		if(param.equals(String("SLOPE")))
		{
			m_slope = value.toDouble();
			Serial.println("Setting SLOPE: " + value);
		}
		else if(param.equals(String("INTECEPT")))
		{
			m_intercept = value.toDouble();
			Serial.println("Setting INTECEPT: " + value);
		}
		else
		{
			for (auto& gas : m_gases)
			{
				if (gas.getName().equals(param))
				{
					double tc = value.toDouble();
					gas.setThermalConductivity(tc);
					Serial.println("Setting TC for gas: " + gas.getName() + " " + String(tc));
					break;
				}
			}
		}
	}
	
	
};