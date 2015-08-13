#pragma once

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include "ui_ISIM.h"

#include "videoFrame.h"
#include "imageProcessor.h"
#include "isimControl.h"

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
		void readData();

	private:
		Ui::MainWindow ui;
		VideoFrame* mVideoFrame;
		ImageProcessor mProcessor;
		QTimer mImageProcessTimer;
		MainWindow::State mCurState;
		QSerialPort mSerial;
		IsimControl** mIsim;

	private slots:
		void imageProcess();
};