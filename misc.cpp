#include "misc.h"

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