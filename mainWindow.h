#pragma once

#include <QSerialPort>
#include <QSerialPortInfo>
#include "isimControl.h"
#include <QMainWindow>
#include <QTimer>
#include "ui_ISIM.h"

class MainWindow : public QMainWindow {

	Q_OBJECT

	private: enum State{
		CALIBRATION,
		FIND_OBJECT,
		CAPTURE,
		//EDIT,
		FIND_ISIM,
		FUSION,
		MANUAL
	};
	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();
	public slots:
		void serialCtrlBtnClicked();
		void serialSendBtnClicked();
		void pingBtnClicked();
		void payloadDetectionBtnClicked();
		void isimControlSelectionChanged(int);
		void isimHomeSelectionChanged(int);
		void isimControlValueChanged();
		void assemblePathGenBtnClicked();
		void readData();
		void ldxlInfoChanged(int);

	private:
		bool calibrate(const cv::Mat& frame);
		cv::Mat findObject(const cv::Mat& frame);
		cv::BackgroundSubtractorMOG2 mSubtractor;
		VideoFrame* mVideoFrame;
		QTimer mImageProcessTimer;
		MainWindow::State mCurState;
		Ui::MainWindow ui;
		QSerialPort* serial;
		IsimControl* isim[6] ;
		IsimControl* isimCurrentControl;
		QTimer* serialTheadTimer;
		QString* cmdString;

	private slots:
		void imageProcess();
};