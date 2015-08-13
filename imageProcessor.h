#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

class ImageProcessor {
	public:
		ImageProcessor();
		~ImageProcessor();
		bool calibrate(const cv::Mat& frame);
		cv::Mat findObject(const cv::Mat& frame);
		cv::Mat findISIM(const cv::Mat& frame);
	private:
		cv::Scalar mIsimColorMin[5];
		cv::Scalar mIsimColorMax[5];
		cv::BackgroundSubtractorMOG2 mSubtractor;
};