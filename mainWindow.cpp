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
										  mCurState(FIND_ISIM),
										  mSubtractor(500, 6.0f*6.0f, true) {
	ui.setupUi(this);
	this->setWindowTitle("Calibrating");
	mVideoFrame = this->findChild<VideoFrame*>("video");
	connect(&mImageProcessTimer, SIGNAL(timeout()), this, SLOT(imageProcess()));
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);
	cv::namedWindow("test");

	serial = new QSerialPort();
	QList<QSerialPortInfo> *portInfoList = new QList<QSerialPortInfo>();
	*portInfoList = QSerialPortInfo::availablePorts();
	if (portInfoList->size() == 0){
		ui.serialCombox->addItem("No port");
	}
	for (int i = 0; i < portInfoList->size(); i++){
		ui.serialCombox->addItem(portInfoList->at(i).portName());
	}
	
	serialTheadTimer = new QTimer(this);

	connect(serialTheadTimer, SIGNAL(timeout()), this, SLOT(readData()));
	serialTheadTimer->start(5);
	
	for (int i = 0; i < 5; i++){
		isim[i] = new IsimControl(i+1, serial);
	}
	isimCurrentControl = isim[0];
}

MainWindow::~MainWindow() {
	delete(&ui);
	delete(serial);
	delete(serialTheadTimer);
}

void MainWindow::imageProcess() {
	cv::Mat result;
	QElapsedTimer time;
	time.start();
	switch (mCurState) {
		case CALIBRATION: {
			result = this->mVideoFrame->curFrame();
			if (this->calibrate(mVideoFrame->curFrame())) {
				mCurState = FIND_OBJECT;
				this->setWindowTitle("Find Object");
			}
			break;
		}
		case FIND_OBJECT: {
			result = this->findObject(mVideoFrame->curFrame());
			break;
		}
	}
	this->mVideoFrame->setResult(result);
	//qDebug() << time.elapsed();
}

// returns true if calibration finished
bool MainWindow::calibrate(const cv::Mat& frame) {
	static unsigned int trigger = 0, delay = 0;
	const int MAX_TRIGGER = 100, MAX_DELAY = 30;
	// initial delay for camera
	if (delay < MAX_DELAY) {
		++delay;
		return false;
	}
	cv::Mat mask;
	mSubtractor(frame, mask);
	cv::erode(mask, mask, cv::Mat());
	cv::dilate(mask, mask, cv::Mat());
	// initial delay for subtractor
	if (trigger < MAX_TRIGGER) {
		trigger++;
		return false;
	}
	// calibration
	if (cv::countNonZero(mask) < 1) {
		trigger = 0;
		return true;
	}
	else return false;
}

cv::Mat MainWindow::findObject(const cv::Mat& frame) {
	cv::Mat mask, result = frame;
	mSubtractor(frame, mask, 0);
	cv::erode(mask, mask, cv::Mat());
	cv::dilate(mask, mask, cv::Mat());
	cv::imshow("test", mask);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < contours.size(); ++i) {
		if (contours[i].size() < 200 || hierarchy[i][3] != -1) continue;
		//cv::Scalar color = cv::Scalar(rand() % 255, rand() % 255, rand() % 255);
		//cv::drawContours(result, contours, i, color, 1, 8, hierarchy);
		auto rect = cv::minAreaRect(contours[i]);
		cv::Point2f rectPoints[4];
		rect.points(rectPoints);
		for (int j = 0; j < 4; ++j) {
			cv::line(result,
					 rectPoints[j],
					 rectPoints[(j + 1) % 4],
					 cv::Scalar(0, 0, 255),
					 1,
					 8);
		}
	}
	return result;
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