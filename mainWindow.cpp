#include "mainWindow.h"
#include "videoFrame.h"

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
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);

	QList<QSerialPortInfo> *portInfoList = new QList<QSerialPortInfo>();
	*portInfoList = QSerialPortInfo::availablePorts();
	if (portInfoList->size() == 0){
		ui.serialCombox->addItem("No port");
	}
	for (int i = 0; i < portInfoList->size(); i++){
		ui.serialCombox->addItem(portInfoList->at(i).portName());
	}
	/*
	connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
	
	for (int i = 0; i < 6; i++){
		isim[i] = new IsimControl(i, serial);
	}
	*/
}

MainWindow::~MainWindow() {
}

void MainWindow::imageProcess() {
	cv::Mat result;
	QElapsedTimer time;
	time.start();
	switch (mCurState) {
		case CALIBRATION : {
			result = this->mVideoFrame->curFrame();
			if (mProcessor.calibrate(mVideoFrame->curFrame())) {
				mCurState = FIND_OBJECT;
				this->setWindowTitle("Find Object");
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
	qDebug() << time.elapsed();
}


void MainWindow::serialCtrlBtnClicked(){
	if (ui.serialCtrlBtn->text().operator == ("OPEN")){

		serial->setPortName(ui.serialCombox->itemText(ui.serialCombox->currentIndex()));
		serial->setBaudRate(115200);

		if (serial->open(QIODevice::ReadWrite)){
			ui.serialCtrlBtn->setText("CLOSE");
		}
		else{
			QMessageBox serialErrorMessageBox;
			serialErrorMessageBox.setText("Serialport cannnot open!");
			serialErrorMessageBox.exec();
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
		QMessageBox serialErrorMessageBox;
		serialErrorMessageBox.setText("Serialport is not open!");
		serialErrorMessageBox.exec();
	}
}


void MainWindow::payloadDetectionBtnClicked(){

}

void MainWindow::readData(){
	char *data;
	if (serial->isReadable()){
		if (serial->canReadLine()){
			data = new char[30];
			serial->readLine(data, 30);

			QString strCmd(data);
			QString cmd = strCmd.mid(0, 2);
			QString cmdWithoutOpcode = strCmd.mid(2);

			QStringList *strParams = new QStringList();
			*strParams = cmdWithoutOpcode.split('\t');
			float *params = new float[cmdWithoutOpcode.size()];
			for (int i = 0; i < cmdWithoutOpcode.size(); i++)
			{
				params[i] = (*strParams)[i].toFloat();
			}

			if (cmd.operator==("PN"))
			{
				QMessageBox serialErrorMessageBox;
				serialErrorMessageBox.setText("Ping recieved from ISIM");
				serialErrorMessageBox.exec();
			}
			else if (cmd.operator==("GY")){

			}
			else if (false){

			}

			delete(params);
			delete(data);

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