#include "videoFrame.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>

#include "imageProcess.h"

const int CAMERA_INDEX = 2;
const int VIDEO_PERIOD = 33;
const int VIDEO_START_DELAY = 1000;

VideoFrame::VideoFrame(QWidget *parent) : QFrame(parent),
										  mVideoTimer(this),
										  mVideo(CAMERA_INDEX),
										  mUpdate(true) {
	mVideo.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	mVideo.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	connect(&mVideoTimer, SIGNAL(timeout()), this, SLOT(update()));
	connect(&mVideoTimer, SIGNAL(timeout()), this, SLOT(loadFrame()));
	QTimer::singleShot(VIDEO_START_DELAY, this, SLOT(startVieo()));
}

VideoFrame::~VideoFrame() {
	mVideoTimer.stop();
}

const cv::Mat& VideoFrame::curFrame() {
	return this->mCurFrame;
}

void VideoFrame::setResult(const cv::Mat& input) {
	this->mResult = input.clone();
}

void VideoFrame::setUpdate(bool input) {
	this->mUpdate = input;
}

void VideoFrame::startVieo() {
	mVideoTimer.start(VIDEO_PERIOD);
}

void VideoFrame::loadFrame() {
	if (this->mUpdate && !mVideo.read(this->mCurFrame)) {
		QMessageBox::critical(this, "Video error", "Failed to load image from camera!");
		QApplication::quit();
	}
}

void VideoFrame::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	QImage imgQt = Mat2QImage(mResult);
	painter.drawImage(QRect(0, 0, this->width(), this->height()), imgQt);
	painter.end();
}