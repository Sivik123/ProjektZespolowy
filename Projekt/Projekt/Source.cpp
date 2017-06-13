
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv\cv.h>
#include <sstream>
#include <string>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;

int Rmin = 0;
int Rmax = 255;
int Gmin = 0;
int Gmax = 255;
int Bmin = 0;
int Bmax = 255;

int R = Rmin;
int R2 = Rmax;

int G = Gmin;
int G2 = Gmax;

int B = Bmin;
int B2 = Bmax;

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_NUM_OBJECTS = 50;
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;

void trackbars(){
	namedWindow("RGB - ustawienia", CV_WINDOW_AUTOSIZE);

	createTrackbar("Redmin", "RGB - ustawienia", &R, 255);
	createTrackbar("Redmax", "RGB - ustawienia", &R2, 255);

	createTrackbar("Greenmin", "RGB - ustawienia", &G, 255);
	createTrackbar("Greenmax", "RGB - ustawienia", &G2, 255);

	createTrackbar("Bluemin", "RGB - ustawienia", &B, 255);
	createTrackbar("Bluemax", "RGB - ustawienia", &B2, 255);
}

string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void Draw(int x, int y, Mat &frame){

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);


	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 50), 1, 1, Scalar(0, 255, 0), 3);

}

void Follow(int &x, int &y, Mat imgThresholded, Mat &imgOriginal){
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
				putText(imgOriginal, "sledzenie obiektu", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				Draw(x, y, imgOriginal);
			}
		}
		else putText(imgOriginal, "za duzo szumow", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}

}

int main(int argc, char** argv)
{
	trackbars();
	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		cout << "brak Kamery Internetowej" << endl;
		return -1;
	}

	// zapis obrazu z kamery do zmiennej tymczasowej
	Mat imgTmp;
	cap.read(imgTmp);

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // szczytanie obrazu z kamery

		Mat imgRGB;
		cvtColor(imgOriginal, imgRGB, COLOR_BGR2RGB); // konwersja obrazu BGR na rgb

		Mat imgThresholded;
		int x = 0, y = 0;
		inRange(imgRGB, Scalar(R, G, B), Scalar(R2, G2, B2), imgThresholded); //Konwersja obrazu 

		// Wykonanie filtracji obrazu
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		Follow(x, y, imgThresholded, imgOriginal);

		imshow("thresholded Image", imgThresholded); // ukazanie obrazu skonwertowanego
		imshow("original", imgOriginal); // ukazanie obrazu oryginalnego

		if (waitKey(30) == 27)
			break;


	}

	return 0;
}
