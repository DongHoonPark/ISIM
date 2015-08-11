#include "mainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QPainter>
#include <QMessageBox>
#include <cvblob.h>

#include "imageProcess.h"

const int IMAGE_PROCESS_PERIOD = 100;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  mImageProcessTimer(this),
										  mCurState(CALIBRATION),
										  mSubtractor(500, 6.0f*6.0f, true) {
	ui.setupUi(this);
	this->setWindowTitle("Calibrating");
	mVideoFrame = this->findChild<VideoFrame*>("video");
	connect(&mImageProcessTimer, SIGNAL(timeout()), this, SLOT(imageProcess()));
	mImageProcessTimer.start(IMAGE_PROCESS_PERIOD);
	cv::namedWindow("test");
}

MainWindow::~MainWindow() {
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
	static cvb::CvBlobs blob;
	static auto bgBlob = cvCreateImage(cvSize(1280, 720), IPL_DEPTH_LABEL, 1);
	cv::Mat mask, result = frame;
	mSubtractor(frame, mask, 0);
	cv::erode(mask, mask, cv::Mat());
	cv::dilate(mask, mask, cv::Mat());
	cv::imshow("test", mask);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); ++i) {
		cv::Scalar color = cv::Scalar(rand() % 255, rand() % 255, rand() % 255);
		cv::drawContours(result, contours, i, color, 1, 8, hierarchy);
	}
	//IplImage iplMask = mask, imgResult = frame;
	//cvLabel(&iplMask, bgBlob, blob);
	//cvFilterByArea(blob, 30000, 3000000);
	//cvRenderContour(blob, &imgResult);
	//cvRenderBlobs(bgBlob, blob, &imgResult, &imgResult,
	//			  CV_BLOB_RENDER_BOUNDING_BOX | CV_BLOB_RENDER_ANGLE);
	//return cv::Mat(&imgResult);
	return result;
}