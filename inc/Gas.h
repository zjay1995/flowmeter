#pragma once

class Gas
{
	String m_name;	
	double m_slope;
	double m_intercept;
	
public:
	
	void setSlope(double s) { m_slope = s; }
	void setIntercept(double i) { m_intercept = i; }
	
	double calculateSLM(int voltage) {
		return m_slope * voltage + m_intercept;
	}
};