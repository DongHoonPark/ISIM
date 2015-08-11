#pragma once

#include <QImage>
#include <opencv2/core/core.hpp>
#include <cvblob.h>

QImage Mat2QImage(const cv::Mat& src);
cv::Mat findDiff(const cv::Mat& mat1, const cv::Mat& mat2);
void cvFilterByShell(cvb::CvBlobs &blobs, unsigned int minArea, unsigned int maxArea);