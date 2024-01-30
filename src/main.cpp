#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#define WAIT std::cin.get()

/* virtual painting */
cv::Mat img, imgFlip;
std::vector<std::vector<int>> newPoints;
bool indicator;

std::vector<std::vector<int>> myColors = { {24,108,102,32,231,194},  // Yellow
											{44,118,76,104,232,220} };  // Green

std::vector<cv::Scalar> myColorValues = { {255, 0, 255}, // Yellow
											{0, 255, 0} };  // Green

static cv::Point getContours(cv::Mat imgDil) {
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());

	cv::Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++) {
		int area = cv::contourArea(contours[i]);
		//std::cout << area << std::endl;

		if (area > 600) {
			float peri = cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			boundRect[i] = cv::boundingRect(conPoly[i]);

			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;
			std::cout << myPoint.x << "," << myPoint.y << std::endl;


			cv::drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), 2);
			cv::rectangle(img, boundRect[i], cv::Scalar(0, 255, 0), 1);
		}

	}
	return myPoint;
}

static void drawOnCanvas(std::vector<std::vector<int>> newPoints, std::vector<cv::Scalar> myColorValues) {
	for (int i = 0; i < newPoints.size(); i++) {

		cv::circle(img, cv::Point(newPoints[i][0], newPoints[i][1]), 5, myColorValues[newPoints[i][2]], cv::FILLED);
	
	}
}


static void findColor(cv::Mat img) {

	cv::Mat imgHSV;
	cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++) {
		cv::Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		cv::Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		cv::Mat mask;
		cv::inRange(imgHSV, lower, upper, mask);
		cv::Point myPoint = getContours(mask);
		//cv::imshow(std::to_string(i), mask);
		
		if (myPoint.x != 0 && myPoint.y != 0) {
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}

	}
	

}

/* READ WEBCAM */
int main() {

	std::cout << "Hello, Virtual Paint!" << std::endl;

	cv::VideoCapture cap(0);

	while (true) {
		cap.read(img);

		findColor(img);

		drawOnCanvas(newPoints, myColorValues);

		cv::flip(img, imgFlip, 1);

		cv::imshow("Real-time", imgFlip);
		cv::waitKey(1);
	}


	return 0;
}