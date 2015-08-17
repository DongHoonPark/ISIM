#include "isimControl.h"

IsimControl::IsimControl(int id, QSerialPort* xbee){
	this->id = id;
	this->xbee = xbee;
	this->ldxlValue = 512;
	this->rdxlValue = 512;
	this->lmagnetValue = 0;
	this->rmagnetValue = 0;
	this->lmotorValue = 0;
	this->rmotorValue = 0;
}

IsimControl::~IsimControl(){

}

void IsimControl::sendInstruction(quint8 length, quint8 instruction, float* params){
	QByteArray* instructionByteArray = new QByteArray();
	int instIndex = 0;
	/*Insert Start Index*/
	for (int i = 0; i < 3; i++){
		instructionByteArray->insert(instIndex++, 0x55);
	}
	/*Insert ID, Length, Instruction*/
	instructionByteArray->insert(instIndex++,(char)(this->id) );
	instructionByteArray->insert(instIndex++, length);
	instructionByteArray->insert(instIndex++, instruction);
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


		qint16 param_H ;
		qint16 param_L ;
		
		param_H = (qint16)(((int)(params[i])) % 10000);
		param_L = (qint16)(((int)(params[i] * 10000.0)) % 10000);

		if (isPostive){
		}
		else{
			param_H = -(param_H);
			param_L = -(param_L);
		}

		instructionByteArray->insert(instIndex++, ((char*)&param_H) , 2);
		instIndex++;

		instructionByteArray->insert(instIndex++, ((char*)&param_L) , 2);
		instIndex++;
	}
	/*Insert CheckSum*/
	instructionByteArray->insert(instIndex++,0xFE);
	/*Send Data after checking port*/
	if (xbee->isOpen()){
		xbee->write(*instructionByteArray);
		xbee->waitForBytesWritten(-1);
	}
	else{

	}
	delete(instructionByteArray);
}

void IsimControl::setWheelSpeed(float leftSpeed, float rightSpeed){
	float wheelSpeed[2];
	
	wheelSpeed[0] = leftSpeed;
	wheelSpeed[1] = rightSpeed;
	
	this->lmotorValue = leftSpeed;
	this->rmotorValue = rightSpeed;

	sendInstruction(0x02, 0x01, wheelSpeed);
	return;
}

void IsimControl::setDxlPosition(float leftAngle, float rightAngle){
	float dxlPosition[2];

	dxlPosition[0] = leftAngle;
	dxlPosition[1] = rightAngle;

	this->ldxlValue = leftAngle;
	this->rdxlValue = rightAngle;

	sendInstruction(0x02, 0x04, dxlPosition);
	return;
}

void IsimControl::setMagnetPower(float leftMagnet, float rightManget){
	float magnetPower[2];
	
	magnetPower[0] = leftMagnet;
	magnetPower[1] = rightManget;

	this->lmagnetValue = leftMagnet;
	this->rmagnetValue = rightManget;

	sendInstruction(0x02, 0x03, magnetPower);
	return;
}

void IsimControl::setGyroscopeData(float* ypr){
	this->yaw = ypr[0];
	this->pitch = ypr[1];
	this->roll = ypr[2];
}

void IsimControl::setYaw(float yaw){
	this->yaw = yaw;
}

void IsimControl::updateGyroscopeData(){
	float GYaddress = 1.0f;
	sendInstruction(0x01, 0x06, &GYaddress);
}
void IsimControl::updateYaw(){
	float GYaddress = 1.0f;
	sendInstruction(0x01, 0x06, &GYaddress);
}
void IsimControl::updateSwitchPressed(){
	float SWaddress = 2.0f;
	sendInstruction(0x01, 0x06, &SWaddress);
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
float IsimControl::getRmotorValue(){
	return this->rmotorValue;
}
float IsimControl::getLmotorValue(){
	return this->lmotorValue;
}
float IsimControl::getRdxlValue(){
	return this->rdxlValue;
}
float IsimControl::getLdxlValue(){
	return this->ldxlValue;
}
float IsimControl::getRmagnetValue(){
	return this->rmagnetValue;
}
float IsimControl::getLmagnetValue(){
	return this->lmagnetValue;
}