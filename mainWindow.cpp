#include "mainWindow.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <cvblob.h>

#include "imageProcess.h"

const int IMAGE_PROCESS_PERIOD = 33;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  mImageProcessTimer(this),
										  mCurState(CALIBRATION),
										  mSubtractor(500, 4.0f*4.0f, true) {
	ui.setupUi(this);
	this->setWindowTitle("Calibrating");
	mVideoFrame = this->findChild<VideoFrame*>("video");
	connect(&mImageProcessTimer, SIGNAL(timeout()), this, SLOT(imageProcess()));
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);
	cv::namedWindow("wow!");
}

MainWindow::~MainWindow() {
}

void MainWindow::imageProcess() {
	cv::Mat result;
	switch (mCurState) {
		case CALIBRATION : {
			if (this->calibrate(mVideoFrame->curFrame())) {
				mCurState = FIND_OBJECT;
				this->setWindowTitle("Find Object");
			}
			break;
		}
		case FIND_OBJECT: {
			this->mVideoFrame->setResult(this->findObject(mVideoFrame->curFrame()));
			break;
		}
	}
	result = this->mVideoFrame->curFrame();
	this->mVideoFrame->setResult(result);
}

// returns true if calibration finished
bool MainWindow::calibrate(const cv::Mat& frame) {
	static unsigned int trigger = 0, delay = 0;
	const int MAX_TRIGGER = 100, MAX_DELAY = 30;
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
	static cvb::CvBlobs blob;
	cv::Mat mask;
	mSubtractor(frame, mask, 0);
	cv::erode(mask, mask, cv::Mat());
	cv::dilate(mask, mask, cv::Mat());
	cv::imshow("wow!", mask);
	IplImage iplMask = mask, imgResult = frame;
	//auto bgBlob = cvCreateImage(cvSize(1280, 720), IPL_DEPTH_LABEL, 1);
	//cvLabel(&iplMask, bgBlob, blob);
	//cvFilterByShell(blob, 3000, 3000000);
	//cvRenderBlobs(bgBlob, blob, &imgResult, &imgResult);
	//cvReleaseImage(&bgBlob);
	return cv::cvarrToMat(&imgResult);
}