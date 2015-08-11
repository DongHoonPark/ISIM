#pragma once

#include <QFrame>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

class VideoFrame : public QFrame {

	Q_OBJECT

	public:
		VideoFrame(QWidget *parent = 0);
		~VideoFrame();
		const cv::Mat& curFrame();
		void setResult(const cv::Mat& input);

	private:
		QTimer mVideoTimer;
		cv::VideoCapture mVideo;
		cv::Mat mCurFrame;
		cv::Mat mResult;

	protected:
		void paintEvent(QPaintEvent* event);

	private slots:
		void startVieo();
		void loadFrame();
};