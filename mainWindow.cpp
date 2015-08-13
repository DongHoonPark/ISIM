#include "mainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QserialPort>
#include <QserialPortInfo>

const int IMAGE_PROCESS_PERIOD = 33;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  mImageProcessTimer(this),
										  mCurState(CALIBRATION),
										  mProcessor(){
	ui.setupUi(this);
	this->setWindowTitle("Calibrating");
	mVideoFrame = this->findChild<VideoFrame*>("video");
	connect(&mImageProcessTimer, SIGNAL(timeout()), this, SLOT(imageProcess()));

	const auto& portInfoList = QSerialPortInfo::availablePorts();
	if (portInfoList.size() == 0){
		ui.serialCombox->addItem("No port");
	}
	for (int i = 0; i < portInfoList.size(); ++i){
		ui.serialCombox->addItem(portInfoList[i].portName());
	}
	/*
	connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
	
	for (int i = 0; i < 6; i++){
		isim[i] = new IsimControl(i, serial);
	}
	*/
	// this line should be last line of this constructor
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);
}

MainWindow::~MainWindow() {
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
	if (ui.serialCtrlBtn->text() == "OPEN"){
		serial->setPortName(ui.serialCombox->itemText(ui.serialCombox->currentIndex()));
		serial->setBaudRate(115200);

		if (serial->open(QIODevice::ReadWrite)){
			ui.serialCtrlBtn->setText("CLOSE");
		}
		else{
			QMessageBox::critical(this, "Serial error", "Failed to open serial port!");
		}
	}
	else{
		serial->close();
		ui.serialCtrlBtn->setText("OPEN");
	}
}

void MainWindow::serialSendBtnClicked(){
	if (serial->isOpen()){

	}
	else{
		QMessageBox::critical(this, "Serial Error", "Serialport is not open!");
	}
}


void MainWindow::payloadDetectionBtnClicked(){

}

void MainWindow::readData(){
	char data[30];
	if (serial->isReadable()){
		if (serial->canReadLine()){
			serial->readLine(data, 30);

			QString strCmd(data);
			QString cmd = strCmd.mid(0, 2);
			QString cmdWithoutOpcode = strCmd.mid(2);

			QStringList *strParams = new QStringList();
			*strParams = cmdWithoutOpcode.split('\t');
			float *params = new float[cmdWithoutOpcode.size()];
			for (int i = 0; i < cmdWithoutOpcode.size(); i++) {
				params[i] = (*strParams)[i].toFloat();
			}

			if (cmd == "PN") {
				QMessageBox::information(this, "Ping", "Ping recieved from ISIM!");
			}
			else if (cmd == "GY") {

			}
			else if (false) {

			}

			delete(params);
		}
		else{
			return;
		}
	}
	else{
		return;
	}

}

void MainWindow::pingBtnClicked(){
	ui.controlIsimSelectCombox->currentText().mid(4, 1);
}