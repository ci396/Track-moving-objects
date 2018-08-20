//part 1
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace std;
using namespace cv;

const static int SENSITIVITY_VALUE = 15;
const static int BLUR_SIZE = 20;

int theObject[2] = { 0, 0 };
Rect objectBoundingRectangle = Rect(0, 0, 0, 0);

string intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}
//part 2
void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
	bool objectDetected = false;
	Mat temp;
	thresholdImage.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	if (contours.size()>0)objectDetected = true;
	else objectDetected = false;
	if (objectDetected){
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size() - 1));
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
		int ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;
		theObject[0] = xpos, theObject[1] = ypos;
	}
	int x = theObject[0];
	int y = theObject[1];
	cv::rectangle(cameraFeed, objectBoundingRectangle, cv::Scalar(0, 0, 255), 5)
}
void searchForMovementc(Mat thresholdImage, Mat &cameraFeed){
	bool objectDetectedc = false;
	Mat temp;
	thresholdImage.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	if (contours.size()>0)objectDetectedc = true;
	else objectDetectedc = false;
    if (objectDetectedc){
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size() - 1));
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
		int ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;
		theObject[0] = xpos, theObject[1] = ypos;
	}
	int x = theObject[0];
	int y = theObject[1];
	drawContours(cameraFeed, contours, -1, Scalar(0));
}
//part 3
int main(){
	bool objectDetected = false;
	bool objectDetectedc = false;
	bool debugMode = false;
	bool trackingEnabled = false;
	bool trackingEnabledc = false;
	bool pause = false;
	Mat frame1, frame2;
	Mat grayImage1, grayImage2;
	Mat differenceImage;
	Mat thresholdImage;
	VideoCapture capture;
	capture.open(0);

	if (!capture.isOpened()){
		cout << "ERROR ACQUIRING VIDEO FEED\n";
		getchar();
		return -1;
	}

	while (1){
		capture.read(frame1);
		cv::cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
		capture.read(frame2);
		cv::cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
		cv::absdiff(grayImage1, grayImage2, differenceImage);
		cv::threshold(differenceImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
		if (debugMode == true){
			cv::imshow("Difference Image", differenceImage);
			cv::imshow("Threshold Image", thresholdImage);
		}
		else{
			cv::destroyWindow("Difference Image");
			cv::destroyWindow("Threshold Image");
		}
		cv::blur(thresholdImage, thresholdImage, cv::Size(BLUR_SIZE, BLUR_SIZE));
		cv::threshold(thresholdImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
		if (debugMode == true){
			imshow("Final Threshold Image", thresholdImage);
		}
		else {
			cv::destroyWindow("Final Threshold Image");
		}
		if (trackingEnabled){
			searchForMovement(thresholdImage, frame1);
		}
		if (trackingEnabledc){
			searchForMovementc(thresholdImage, frame1);
		}
		imshow("Frame1", frame1);
		switch (waitKey(10)){
		case 27: 
			return 0;
		case 116: 
			trackingEnabled = !trackingEnabled;
			if (trackingEnabled == false) cout << "Tracking disabled." << endl;
			else cout << "Tracking enabled." << endl;
			break;
		case 99: 
			trackingEnabledc = !trackingEnabledc;
			if (objectDetectedc = false) cout << "Tracking disabled." << endl;
			else cout << "Tracking enabled." << endl;
			break;
		case 100: 
			debugMode = !debugMode;
			if (debugMode == false) cout << "Debug mode disabled." << endl;
			else cout << "Debug mode enabled." << endl;
			break;
		case 112: 
			pause = !pause;
			if (pause == true){
				cout << "Code paused, press 'p' again to resume" << endl;
				while (pause == true){
					switch (waitKey()){
		case 112:				
						pause = false;
						cout << "Code Resumed" << endl;
						break;
					}
				}
			}



		}
	}


	return 0;

}
