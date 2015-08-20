#include "imageProcessor.h"

const int MAX_TRIGGER = 100;
const int MAX_DELAY = 30;
const int ISIM_SIZE_MIN = 200;
const int ISIM_SIZE_MAX = 1000;

ImageProcessor::ImageProcessor() : mSubtractor(500, 6.0f*6.0f, true){
	mIsimColorMin[0] = cv::Scalar(20 * 0.5, 30 * 2.55, 45 * 2.55);
	mIsimColorMin[1] = cv::Scalar(110 * 0.5, 30 * 2.55, 45 * 2.55);
	mIsimColorMin[2] = cv::Scalar(0, 0, 0);
	mIsimColorMin[3] = cv::Scalar(0, 0, 0);
	mIsimColorMin[4] = cv::Scalar(0, 0, 0);
	mIsimColorMax[0] = cv::Scalar(45 * 0.5, 100 * 2.55, 100 * 2.55);
	mIsimColorMax[1] = cv::Scalar(150 * 0.5, 100 * 2.55, 100 * 2.55);
	mIsimColorMax[2] = cv::Scalar(0, 0, 0);
	mIsimColorMax[3] = cv::Scalar(0, 0, 0);
	mIsimColorMax[4] = cv::Scalar(0, 0, 0);
}

ImageProcessor::~ImageProcessor() {

}

// returns true if calibration finished
bool ImageProcessor::calibrate(const cv::Mat& frame) {
	static unsigned int trigger = 0, delay = 0;
	// initial delay for camera
	if (delay < MAX_DELAY) {
		++delay;
		return false;
	}
	cv::Mat mask;
	mSubtractor(frame, mask);
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat());
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

cv::Mat ImageProcessor::findISIM(const cv::Mat& frame) {
	cv::Mat result = frame;
	for (int i = 0; i < 5; ++i) {
		cv::Mat thresFrame;
		cv::inRange(frame, thresFrame, mIsimColorMin[0], mIsimColorMax[0]);
		cv::morphologyEx(thresFrame, thresFrame, cv::MORPH_OPEN, cv::Mat());
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(thresFrame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		for (auto it = contours.begin(); it != contours.end(); ++it) {
			if (ISIM_SIZE_MIN <= it->size() && it->size() < ISIM_SIZE_MIN) {
				auto rect = cv::minAreaRect(*it);
				cv::Point2f rectPoints[4];
				rect.points(rectPoints);
				for (int j = 0; j < 4; ++j) {
					cv::line(result, rectPoints[j], rectPoints[(j + 1) % 4],
						cv::Scalar(0, 0, 255), 1, 8);
				}
			}
		}
	}
	return result;
}

cv::Mat ImageProcessor::findObject(const cv::Mat& frame) {
	cv::Mat mask, result = frame;
	mSubtractor(frame, mask, 0);
	cv::erode(mask, mask, cv::Mat());
	cv::dilate(mask, mask, cv::Mat());
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
			cv::line(result, rectPoints[j], rectPoints[(j + 1) % 4],
					 cv::Scalar(0, 0, 255), 1, 8);
		}
	}
	return result;
}