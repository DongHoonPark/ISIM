#pragma once

#include <QFrame>
#include <QTimer>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

class VideoFrame : public QFrame {

	Q_OBJECT

	public:
		VideoFrame(QWidget *parent = 0);
		~VideoFrame();

	private:
		QTimer mVideoTimer;
		cv::VideoCapture mVideo;
		cv::BackgroundSubtractorMOG2 mBgDetector;

	protected:
		void VideoFrame::paintEvent(QPaintEvent* event);
};