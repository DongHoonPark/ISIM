#pragma once

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QKeyEvent>
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
		void keyPressEvent(QKeyEvent* e);

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
		void updateSensor();
		void gyroReadBtnClicked();
		void ldxlInfoChanged(int);

	private:
		Ui::MainWindow ui;
		VideoFrame* mVideoFrame;
		ImageProcessor mProcessor;
		QTimer mImageProcessTimer;
		MainWindow::State mCurState;
		IsimControl* isim[5] ;
		IsimControl* isimCurrentControl;
		QTimer* serialTheadTimer;
		QTimer* sensorUpdateTimer;
		QString* cmdString;
		QSerialPort mSerial;

	private slots:
		void imageProcess();
};