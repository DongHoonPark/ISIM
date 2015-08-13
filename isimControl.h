#pragma once

#include "qbytearray.h"
#include "qserialport.h"

class IsimControl {
public :
	IsimControl(int id, QSerialPort* xbee);
	~IsimControl();
	
	void sendInstruction(quint8 length, quint8 instruction, float* params);
	void setWheelSpeed(float leftSpeed, float rightSpeed);
	void setDxlPosition(float leftAngle, float rightAngle);
	void setMagnetPower(float leftMagnet, float rightManget);
	
	void updateGyroscopeData();
	void updateSwitchPressed();

	int   getId();
	float getYaw();
	float getRoll();
	float getPitch();
	bool  getSwitchPressed();

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