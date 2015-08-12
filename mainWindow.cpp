#include "mainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>

const int IMAGE_PROCESS_PERIOD = 33;

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
	qDebug() << time.elapsed();
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