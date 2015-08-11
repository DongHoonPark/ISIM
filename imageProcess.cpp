#include "imageProcess.h"

QImage Mat2QImage(const cv::Mat& src) {
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
	for (int y = 0; y < src.rows; ++y) {
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) {
			cv::Vec3b color = src.at<cv::Vec3b>(y, x);
			destrow[x] = qRgba(color.val[2], color.val[1], color.val[0], 255);
		}
	}
	return dest;
}

cv::Mat findDiff(const cv::Mat& mat1, const cv::Mat& mat2) {
	cv::Mat result = cv::Mat(mat1.rows, mat1.cols, CV_8U);
	for (int y = 0; y < mat1.rows; ++y) {
		for (int x = 0; x < mat1.cols; ++x) {
			cv::Vec3b color1 = mat1.at<cv::Vec3b>(y, x);
			cv::Vec3b color2 = mat2.at<cv::Vec3b>(y, x);
			if (color1[0] == color2[0] && color1[1] == color2[1] && color1[2] && color2[2]) {
				result.at<uchar>(y, x) = 0;
			}
			else result.at<uchar>(y, x) = 255;
		}
	}
	return result;
}

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

void cvRenderContour(cvb::CvBlobs &blob, IplImage* imgDest) {
	for (auto it = blob.begin(); it != blob.end(); ++it) {
		cvRenderContourChainCode(
			&it->second->contour, imgDest, cvScalar(rand() % 255, rand() % 255, rand() % 255)
		);
	}
}