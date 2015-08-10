#include "videoFrame.h"

#include <iostream>
#include <vector>

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QPainter>
#include <QMessageBox>
#include <cvblob.h>

#define NUM_CORES 2
#define CAMERA_INDEX 1
#define MIN_HESSIAN 400

void cvFilterByShell(cvb::CvBlobs &blobs,
					 unsigned int minArea, unsigned int maxArea) {
	auto it = blobs.begin();
	while (it != blobs.end()) {
		cvb::CvBlob* blob = it->second;
		unsigned int shellArea = (blob->maxx - blob->minx)
			* (blob->maxy - blob->miny);
		if (shellArea < minArea || shellArea > maxArea) {
			cvReleaseBlob(blob);
			auto tmp = it;
			++it;
			blobs.erase(tmp);
		}
		else ++it;
	}
}

VideoFrame::VideoFrame(QWidget *parent) : QFrame(parent),
										  mVideoTimer(this),
										  mVideo(CAMERA_INDEX),
										  mCurState(VideoFrame::State::BACKGROUND),
										  mDetector(MIN_HESSIAN),
										  mMatcher(cv::NORM_L2){
	//assign mIdImage
	mDetector.detect(mIdImage, this->mIdKey);
	mExtractor.compute(mIdImage, this->mIdKey, mIdDesc);
	connect(&mVideoTimer, SIGNAL(timeout()), this, SLOT(update()));
	mVideoTimer.start(100);
}

VideoFrame::~VideoFrame() {
}

cv::Mat VideoFrame::findBackground(cv::Mat& curFrame) {
	cvb::CvBlobs blob;
	cv::Mat bgMask;
	auto bgBlob = cvCreateImage(cvSize(1280, 720), IPL_DEPTH_LABEL, 1);
	IplImage iplMask, imgResult;
	mBgDetector(curFrame, bgMask);
	erode(bgMask, bgMask, cv::Mat());
	dilate(bgMask, bgMask, cv::Mat());
	QElapsedTimer timer;
	timer.start();
	iplMask = bgMask;
	imgResult = curFrame;
	cvLabel(&iplMask, bgBlob, blob);
	cvFilterByShell(blob, 3000, 3000000);
	cvRenderBlobs(bgBlob, blob, &imgResult, &imgResult);
	this->window()->setWindowTitle(QString::number(timer.elapsed()));
	cvReleaseImage(&bgBlob);
	return cv::cvarrToMat(&imgResult);
}
cv::Mat VideoFrame::findISIM(cv::Mat& curFrame) {
	std::vector<cv::KeyPoint> keyFrame;
	cv::Mat descFrame;
	std::vector<cv::DMatch> matches;
	mDetector.detect(curFrame, keyFrame);
	mExtractor.compute(curFrame, keyFrame, descFrame);
	mMatcher.match(mIdDesc, descFrame, matches);
	drawMatches(mIdImage, mIdKey, curFrame, keyFrame, matches, curFrame);
	return curFrame;
}

void VideoFrame::paintEvent(QPaintEvent* event) {
	// opencv calculation
	cv::Mat curFrame, imgDraw;
	if (!mVideo.read(curFrame)) {
		QMessageBox::critical(this, "Video error", "Failed to load image from camera!");
		QApplication::quit();
	}
	switch (this->mCurState) {
		case VideoFrame::State::BACKGROUND: imgDraw = this->findBackground(curFrame); break;
		case VideoFrame::State::FIND_ISIM: imgDraw = this->findISIM(curFrame); break;
	}

	// draw on frame
	QPainter painter(this);
	QImage imgQt = QImage((uchar*)imgDraw.data,
		imgDraw.cols,
		imgDraw.rows,
		imgDraw.step,
		QImage::Format_RGB888);
	painter.drawImage(QRect(0, 0, this->width(), this->height()), imgQt);
	painter.end();
}