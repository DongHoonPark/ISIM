#include "isimControl.h"

IsimControl::IsimControl(int id, QSerialPort* xbee){
	this->id = id;
	this->xbee = xbee;
}

IsimControl::~IsimControl(){

}

void IsimControl::sendInstruction(quint8 length, quint8 instruction, float* params){
	QByteArray* instructionByteArray = new QByteArray();
	instructionByteArray->resize(3 + 1 + 1 + 1 + 4*length + 1);
	/*Insert Start Index*/
	for (int i = 0; i < 3; i++)
	{
		instructionByteArray->append(0xFF);
	}
	/*Insert ID, Length, Instruction*/
	instructionByteArray->append((char)(this->id));
	instructionByteArray->append(length);
	instructionByteArray->append(instruction);
	/*Insert Data*/
	for (int i = 0; i < length; i++)
	{
		bool isPostive;

		if (params[i] > 0){
			isPostive = true;
		}
		else{
			isPostive = false;
			params[i] = 0 - params[i];
		}

		qint16 *param_H ;
		qint16 *param_L ;
		
		*param_H = ((int)(params[i])) % 10000;
		*param_L = ((int)(params[i] * 1000.0)) % 10000;

		if (isPostive){
		}
		else{
			*param_H = -(*param_H);
			*param_L = -(*param_L);
		}
	}
	/*Insert CheckSum*/
	instructionByteArray->append(0xFE);
	/*Send Data after checking port*/
	if (xbee->isOpen()){
		xbee->write(*instructionByteArray);
	}
	else{
	}
	delete(instructionByteArray);
}

void IsimControl::setWheelSpeed(float leftSpeed, float rightSpeed){
	float wheelSpeed[2];
	
	wheelSpeed[0] = leftSpeed;
	wheelSpeed[1] = rightSpeed;
	
	sendInstruction(0x02, 0x01, wheelSpeed);
	return;
}

void IsimControl::setDxlPosition(float leftAngle, float rightAngle){
	float dxlPosition[2];

	dxlPosition[0] = leftAngle;
	dxlPosition[1] = rightAngle;

	sendInstruction(0x02, 0x04, dxlPosition);
	return;
}

void IsimControl::setMagnetPower(float leftMagnet, float rightManget){
	float magnetPower[2];
	
	magnetPower[0] = leftMagnet;
	magnetPower[1] = rightManget;

	sendInstruction(0x02, 0x03, magnetPower);
	return;
}
void IsimControl::updateGyroscopeData(){

}
void IsimControl::updateSwitchPressed(){

}

int IsimControl::getId(){
	return this->id;
}
float IsimControl::getYaw(){
	return this->yaw;
}
float IsimControl::getRoll(){
	return this->roll;
}
float IsimControl::getPitch(){
	return this->pitch;
}
bool  IsimControl::getSwitchPressed(){
	return this->switchPressed;
}