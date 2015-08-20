#include "mainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>

const int IMAGE_PROCESS_PERIOD = 33;
const float MOTOR_SPEED_SACLER = 0.15f;
const float DXL_ANGLE_SACLER = 3.4133333f;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  mImageProcessTimer(this),
										  mCurState(MANUAL),
										  mSerial(this),
										  mProcessor(){
	// basic parameter settings
	ui.setupUi(this);
	this->setWindowTitle("Calibrating");
	mVideoFrame = this->findChild<VideoFrame*>("video");
	connect(&mImageProcessTimer, SIGNAL(timeout()), this, SLOT(imageProcess()));

	cmdString = new QString();
	mSerial.setBaudRate(38400);
	// finding & adding ports
	
	const auto& portInfoList = QSerialPortInfo::availablePorts();
	if (portInfoList.size() == 0){
		ui.serialCombox->addItem("No port");
	}
	for (int i = 0; i < portInfoList.size(); ++i){
		ui.serialCombox->addItem(portInfoList.at(i).portName());
	}
	
	serialTheadTimer = new QTimer(this);
	sensorUpdateTimer = new QTimer(this);

	connect(serialTheadTimer, SIGNAL(timeout()), this, SLOT(readData()));
	//connect(&mSerial, SIGNAL(readyRead()), this, SLOT(readData()));
	serialTheadTimer->start(1);
	connect(sensorUpdateTimer, SIGNAL(timeout()), this, SLOT(updateSensor()));
	//sensorUpdateTimer->start(300);

	for (int i = 0; i < 5; i++){
		isim[i] = new IsimControl(i+1, &mSerial);
	}
	isimCurrentControl = isim[0];
	// this line should be last line of this constructor
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);
}

MainWindow::~MainWindow() {
	delete(serialTheadTimer);
}

void MainWindow::imageProcess() {
	cv::Mat result;
	QElapsedTimer elapsedTime;
	elapsedTime.start();
	switch (mCurState) {
		case CALIBRATION : {
			result = mVideoFrame->curFrame();
			if (mProcessor.calibrate(mVideoFrame->curFrame())) {
				mCurState = FIND_OBJECT;
				setWindowTitle("Find Object");
			}
			break;
		}
		case FIND_OBJECT : {
			result = mProcessor.findObject(mVideoFrame->curFrame());
			break;
		}
		case FIND_ISIM: {
			result = mProcessor.findISIM(mVideoFrame->curFrame());
			break;
		}
	}
	this->mVideoFrame->setResult(result);
	qDebug() << elapsedTime.elapsed();
}

void MainWindow::serialCtrlBtnClicked(){
	static bool isOpen = false;
	if (!isOpen){
		mSerial.setPortName(ui.serialCombox->currentText());

		if (mSerial.open(QIODevice::ReadWrite)){
			ui.serialCtrlBtn->setText("CLOSE");
			isOpen = true;
		}
		else{
			QMessageBox::critical(this, "Serial error", "Failed to open serial port!");
		}
	}
	else{
		mSerial.close();
		ui.serialCtrlBtn->setText("OPEN");
		isOpen = false;
	}
}

void MainWindow::serialSendBtnClicked(){
	if (mSerial.isOpen()){

	}
	else{
		QMessageBox::critical(this, "Serial Error", "Serialport is not open!");
	}
}


void MainWindow::payloadDetectionBtnClicked(){

}

void MainWindow::readData(){
		if (mSerial.canReadLine()){
			char data[30];
			mSerial.readLine(data, 30);

			QString strCmd(data);
			*cmdString += strCmd;

			ui.serialConsole->append(strCmd);
			
			strCmd.remove("\r\n");
			if (strCmd.at(0) == '#'){
				return;
			}
			QString cmd = strCmd.mid(0, 2);
			QString cmdWithoutOpcode = strCmd.mid(2);
			if ((cmdWithoutOpcode.at(0) > '9' || cmdWithoutOpcode.at(0) < '0')){
				return;
			}

			QStringList *strParams = new QStringList();
			*strParams = cmdWithoutOpcode.split('\t');

			int debug_int;
			float *params = new float[cmdWithoutOpcode.size()];
			for (int i = 0; i < strParams->size(); i++)
			{
				params[i] = (*strParams)[i].toFloat();
			}

			if (cmd == "PN") {
				QMessageBox::information(this, "Ping", "Ping recieved from ISIM!");
			}
			else if (cmd == "GY") {
				isim[((int)params[0]) - 1]->setYaw(params[1]);
			}
		}
	
}

void MainWindow::updateSensor(){
	if (mSerial.isOpen()){
		for (int i = 0; i < 5; i++){
			isim[i]->updateGyroscopeData();
			QThread::msleep(20);
		}
	}
}

void MainWindow::pingBtnClicked(){
	float pingNulldata[] = { 0, 0, 0 };
	isimCurrentControl->sendInstruction(0, 0x05, pingNulldata);
}

void MainWindow::isimControlSelectionChanged(int selectionValue){
	isimCurrentControl = isim[selectionValue];
	ui.rmotorSpinbox->setValue(isimCurrentControl->getRmotorValue() / MOTOR_SPEED_SACLER);
	ui.lmotorSpinbox->setValue(isimCurrentControl->getLmotorValue() / MOTOR_SPEED_SACLER);
	ui.rdxlSpinbox->setValue((isimCurrentControl->getRdxlValue() - 512.0f) / DXL_ANGLE_SACLER);
	ui.ldxlSpinbox->setValue((isimCurrentControl->getLdxlValue() - 512.0f) / DXL_ANGLE_SACLER);
	ui.rmagnetSpinbox->setValue(isimCurrentControl->getRmagnetValue());
	ui.lmagnetSpinbox->setValue(isimCurrentControl->getLmagnetValue());
}

void MainWindow::isimHomeSelectionChanged(int selectionValue){

}

void MainWindow::isimControlValueChanged(){
	
	if (mSerial.isOpen()){

		QStringList senderNames;
		senderNames << "rmotorSpinbox"; //0
		senderNames << "lmotorSpinbox"; //1
		senderNames << "rdxlSpinbox";   //2
		senderNames << "ldxlSpinbox";   //3
		senderNames << "rmagnetSpinbox";//4
		senderNames << "lmagnetSpinbox";//5

		QObject* obj = sender();
		switch (senderNames.indexOf( obj->objectName() ))
		{
		case 0:// right motor value changed
			isimCurrentControl->setWheelSpeed(isimCurrentControl->getLmotorValue(),MOTOR_SPEED_SACLER*((QSpinBox*)obj)->value());
			break;
		case 1:// left motor value changed
			isimCurrentControl->setWheelSpeed(MOTOR_SPEED_SACLER*((QSpinBox*)obj)->value(), isimCurrentControl->getRmotorValue());
			break;
		case 2:// right dxl value changed
			isimCurrentControl->setDxlPosition(isimCurrentControl->getLdxlValue(), ((QSpinBox*)obj)->value() * 3.4133333f + 512.0f);
			break;
		case 3:// left dxl value changed
			isimCurrentControl->setDxlPosition(((QSpinBox*)obj)->value() * 3.4133333f + 512.0f, isimCurrentControl->getRdxlValue());
			break;
		case 4:// right magnet value changed
			isimCurrentControl->setMagnetPower(isimCurrentControl->getLmagnetValue(), ((QSpinBox*)obj)->value());
			break;
		case 5:// left magnet value changed
			isimCurrentControl->setMagnetPower(((QSpinBox*)obj)->value() , isimCurrentControl->getRmagnetValue());
			break;
		}
	}
	else{
		QMessageBox serialErrorMessageBox;
		serialErrorMessageBox.setText("Please open serialport first!");
		serialErrorMessageBox.exec();
	}
}
void MainWindow::ldxlInfoChanged(int){

}
void MainWindow::assemblePathGenBtnClicked(){

}
void MainWindow::keyPressEvent(QKeyEvent* e){
	if (e->text() == "g"){
		sensorUpdateTimer->start(500);
	}
}
void MainWindow::gyroReadBtnClicked(){
	isimCurrentControl->updateYaw();
}