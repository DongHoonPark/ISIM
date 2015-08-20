#pragma once

#include <QByteArray>
#include <QtSerialPort/QSerialPort>

class IsimControl {
public :
	IsimControl(int id, QSerialPort* xbee);
	~IsimControl();
	
	void sendInstruction(quint8 length, quint8 instruction, float* params);
	void setWheelSpeed(float leftSpeed, float rightSpeed);
	void setDxlPosition(float leftAngle, float rightAngle);
	void setMagnetPower(float leftMagnet, float rightManget);
	void setGyroscopeData(float* ypr);
	void setYaw(float yaw);

	void updateYaw();
	void updateGyroscopeData();
	void updateSwitchPressed();

	int   getId();
	float getYaw();
	float getRoll();
	float getPitch();
	bool  getSwitchPressed();
	
	float getRmotorValue();
	float getLmotorValue();
	float getRdxlValue();
	float getLdxlValue();
	float getRmagnetValue();
	float getLmagnetValue();


private:
	QSerialPort* xbee;
	int id;
	float yaw;
	float roll;
	float pitch;
	bool switchPressed;
	float rmotorValue;
	float lmotorValue;
	float rmagnetValue;
	float lmagnetValue;
	float rdxlValue;
	float ldxlValue;
	
};