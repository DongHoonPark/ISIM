#include "videoFrame.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <blob.h>
#include <BlobResult.h>

#define NUM_CORES 2
#define CAMERA_INDEX 1

VideoFrame::VideoFrame(QWidget *parent) : QFrame(parent),
										  mVideoTimer(this),
										  mVideo(CAMERA_INDEX){
	connect(&mVideoTimer, SIGNAL(timeout()), this, SLOT(update()));
	mVideoTimer.start(100);

}

VideoFrame::~VideoFrame() {

}

void VideoFrame::paintEvent(QPaintEvent* event) {
	// opencv calculation
	cv::Mat imgOriginal, bgMask;
	if (!mVideo.read(imgOriginal)) {
		QMessageBox::critical(this, "Video error", "Failed to load image from camera!");
		QApplication::quit();
	}
	mBgDetector(imgOriginal, bgMask);
	erode(bgMask, bgMask, cv::Mat());
	dilate(bgMask, bgMask, cv::Mat());
	CBlobResult blob = CBlobResult(bgMask, cv::Mat(), NUM_CORES);
	for (int i = 0; i < blob.GetNumBlobs(); ++i) {
		CBlob curBlob = blob.GetBlob(i);
		curBlob.FillBlob(imgOriginal, cv::Scalar(0, 220, 0));
	}

	// draw on application
	QPainter painter(this);
	QImage imgQt = QImage((uchar*)imgOriginal.data,
		imgOriginal.cols,
		imgOriginal.rows,
		imgOriginal.step,
		QImage::Format_RGB888);
	painter.drawImage(QRect(0, 0, this->width(), this->height()), imgQt);
	painter.end();
}