
#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv\cv.h>
#include <sstream>
#include <string>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;


int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
int iLowH = H_MIN;
int iHighH = H_MAX;
int iLowS = S_MIN;
int iHighS = S_MAX;
int iLowV = V_MIN;
int iHighV = V_MAX;
const int width = 640;
const int height = 480;
const int MAX_NUM_OBJECTS = 50;
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;


string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void Rysowanie(int x, int y, Mat &frame){
	


	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25 <height)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, height), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25< width)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(width, y), Scalar(0, 255, 0), 2);


	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);

}

void Sledzenie(int &x, int &y, Mat imgThresholded, Mat &imgOriginal){
	Mat temp;
	imgThresholded.copyTo(temp);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0){
		int numObjects = hierarchy.size();
		if (numObjects < MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]){
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;
			}
			if (objectFound == true){
				putText(imgOriginal, "Sledzenie obiektu", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				drawObject(x, y, imgOriginal);
			}
		}
		else putText(imgOriginal, "Za duzo szumow", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}

}
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

