#include "mainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>

const int IMAGE_PROCESS_PERIOD = 33;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  mImageProcessTimer(this),
										  mCurState(FIND_ISIM),
										  mSerial(this),
										  mProcessor(){
	// basic parameter settings
	ui.setupUi(this);
	this->setWindowTitle("Calibrating");
	mVideoFrame = this->findChild<VideoFrame*>("video");
	connect(&mImageProcessTimer, SIGNAL(timeout()), this, SLOT(imageProcess()));

	mSerial.setBaudRate(115200);
	serial = new QSerialPort();
	*portInfoList = QSerialPortInfo::availablePorts();
	QList<QSerialPortInfo> *portInfoList = new QList<QSerialPortInfo>();
	if (portInfoList->size() == 0){
		ui.serialCombox->addItem("No port");
	}
	for (int i = 0; i < portInfoList.size(); ++i){
		ui.serialCombox->addItem(portInfoList[i].portName());
	}
	
	connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
	
	for (int i = 0; i < 5; i++){
		isim[i] = new IsimControl(i+1, serial);
	}
	// this line should be last line of this constructor
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);
	isimCurrentControl = isim[0];
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
	char data[30];
	if (mSerial.isReadable()){
		if (mSerial.canReadLine()){
			mSerial.readLine(data, 30);

			QString cmd = QString(data).mid(0, 2);
			QString cmdWithoutOpcode = QString(data).mid(2);

			QStringList strParams = cmdWithoutOpcode.split('\t');
			std::vector<float> params;
			for (auto it = strParams.begin(); it != strParams.end(); ++it) {
				params.push_back(it->toFloat());
			}

			if (cmd == "PN") {
				QMessageBox::information(this, "Ping", "Ping recieved from ISIM!");
			}
			else if (cmd == "GY") {

			}
		}
	}
}

void MainWindow::pingBtnClicked(){
	float pingNulldata[] = { 0, 0, 0 };
	isimCurrentControl->sendInstruction(0, 0x05, pingNulldata);
}

void MainWindow::isimControlSelectionChanged(int selectionValue){
	isimCurrentControl = isim[selectionValue];
}

void MainWindow::isimHomeSelectionChanged(int selectionValue){

}

void MainWindow::isimControlValueChanged(){
	if (serial->isOpen()){

	}
	else{
		QMessageBox serialErrorMessageBox;
		serialErrorMessageBox.setText("Please open serialport first!");
		serialErrorMessageBox.exec();
	}
}