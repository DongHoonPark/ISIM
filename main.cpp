#include <queue>

#include <opencv2/core/core.hpp>
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>

#include "mainWindow.h"

class ComparePair {
	public:
		inline bool operator() (const std::pair<unsigned int, cv::Point>& p1,
						 const std::pair<unsigned int, cv::Point>& p2) {
			return p1.first < p2.first;
		}
};

// implemented from pseudocode of http://www.redblobgames.com/pathfinding/a-star/introduction.html
std::vector<cv::Point> findPath(const cv::Mat& image, const cv::Point& src, const cv::Point& dest, const int gridSize) {
	// basic definitions
	using namespace std;
	typedef pair<uint, cv::Point> PointPair;
	const uchar NONE = 0, LEFT = 1, UP = 2, RIGHT = 3, DOWN = 4;
	const uchar THRES = 128;
	// basic variables for algorithm
	priority_queue<PointPair, vector<PointPair>, ComparePair> frontier;
	cv::Mat cameFrom = cv::Mat::zeros(image.size(), CV_8U); // zero value means not assigned
	cv::Mat cost = cv::Mat::zeros(image.size(), CV_16U); // zero-cost means not assigned
	// initialize variables
	frontier.push(make_pair(1, cv::Point(0, 0)));
	cost.at<ushort>(src) = 1;
	// calculate path
	cv::Point curPoint;
	while (!frontier.empty()) {
		// pop priority queue
		curPoint = frontier.top().second;
		frontier.pop();
		// return if algorithm reached dest
		if (abs(curPoint.x - dest.x) < gridSize && abs(curPoint.y - dest.y) < gridSize)
			break;
		ushort curCost = cost.at<ushort>(curPoint);
		// calculate neighbors
		cv::Point neighbors[4] = {
			cv::Point(-1, -1),
			cv::Point(-1, -1),
			cv::Point(-1, -1),
			cv::Point(-1, -1),
		};
		cv::Point target;
		if (curPoint.x < cost.size().width - gridSize
			&& image.at<uchar>(target = cv::Point(curPoint.x + gridSize, curPoint.y)) > THRES)
			neighbors[0] = target;
		if (curPoint.y < cost.size().height - gridSize
			&& image.at<uchar>(target = cv::Point(curPoint.x, curPoint.y + gridSize)) > THRES)
			neighbors[1] = target;
		if (curPoint.x >= gridSize
			&& image.at<uchar>(target = cv::Point(curPoint.x - gridSize, curPoint.y)) > THRES)
			neighbors[2] = target;
		if (curPoint.y >= gridSize
			&& image.at<uchar>(target = cv::Point(curPoint.x, curPoint.y - gridSize)) > THRES)
			neighbors[3] = target;
		for (int i = 0; i < 4; ++i) {
			auto curNeighbor = neighbors[i];
			// boundary check - remove this at Release build
			if (curNeighbor.x < 0 ) continue;
			// actual logic
			ushort neighborCost = cost.at<ushort>(curNeighbor);
			if (neighborCost == 0 || curCost + 1 < neighborCost) {
				cost.at<ushort>(curNeighbor) = curCost + 1;
				uint priority = neighborCost
							  + abs(dest.x - curNeighbor.x)
							  + abs(dest.y - curNeighbor.y);
				frontier.push(make_pair(priority, curNeighbor));
				cameFrom.at<uchar>(curNeighbor) = i + 1;
			}
		}
	}
	// reconstruct path from 'cameFrom'
	vector<cv::Point> result;
	while (abs(curPoint.x - src.x) > gridSize || abs(curPoint.y - src.y) > gridSize) {
		result.push_back(curPoint);
		auto direction = cameFrom.at<uchar>(curPoint);
		switch (direction) {
			case 0: return result;
			case LEFT: curPoint.x -= gridSize; break;
			case UP: curPoint.y -= gridSize; break;
			case RIGHT: curPoint.x += gridSize; break;
			case DOWN: curPoint.y += gridSize; break;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	//QApplication a(argc, argv);
	//MainWindow w;
	//w.show();
	//return a.exec();
	cv::Mat original = cv::imread("test.png", CV_LOAD_IMAGE_GRAYSCALE);
	QElapsedTimer timer;
	timer.start();
	auto path = findPath(original,
						 cv::Point(0, 0),
						 cv::Point(original.size().width - 1, original.size().height - 1),
						 3);
	qDebug() << timer.elapsed();
	cv::cvtColor(original, original, CV_GRAY2BGR);
	for (int i = 1; i < path.size(); ++i) {
		cv::line(original, path[i - 1], path[i], cv::Scalar(0, 0, 255), 2);
	}
	cv::namedWindow("test");
	cv::imshow("test", original);
	cv::imwrite("result.png", original);
	cv::waitKey();
}
