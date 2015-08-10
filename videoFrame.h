#pragma once

#include <QFrame>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/legacy/legacy.hpp>

class VideoFrame : public QFrame {

	Q_OBJECT

	private: enum State{
		CALIBRATION,
		BACKGROUND,
		CAPTURE,
		//EDIT,
		FIND_ISIM,
		FUSION,
		MANUAL
	};

	public:
		VideoFrame(QWidget *parent = 0);
		~VideoFrame();

	private:
		cv::Mat findBackground(cv::Mat& curFrame);
		cv::Mat findISIM(cv::Mat& curFrame);
		// basic variables
		VideoFrame::State mCurState;
		QTimer mVideoTimer;
		cv::VideoCapture mVideo;
		// ISIM parameter
		cv::Mat mIdImage;
		std::vector<cv::KeyPoint> mIdKey;
		cv::Mat mIdDesc;
		// for Background Substraction
		cv::BackgroundSubtractorMOG2 mBgDetector;
		// for Feature Detection
		cv::SURF mDetector;
		cv::FREAK mExtractor;
		cv::BFMatcher mMatcher;

	protected:
		void VideoFrame::paintEvent(QPaintEvent* event);
};