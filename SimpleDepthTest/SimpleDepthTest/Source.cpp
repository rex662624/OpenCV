#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;
//我的充電器: HS:最大 V: 0~30
int main(int argc, char** argv)
{
	// Capture the video from Integrated Webcam
	VideoCapture video(0);
	video.set(CV_CAP_PROP_FRAME_WIDTH, 2560);
	video.set(CV_CAP_PROP_FRAME_HEIGHT,720);

	// If not success, exit program
	/*if (!cap.isOpened())
	{
	cout << "Cannot open the web cam" << endl;
	return -1;
	}*/

	// Create a Control Window and Trackbars
	namedWindow("Control", CV_WINDOW_AUTOSIZE);		// Control Window
													//initial HSV
	int iLowH = 49;
	int iHighH = 67;

	int iLowS = 61;
	int iHighS = 197;

	int iLowV = 73;
	int iHighV = 255;

	// Create Trackbars in Control Window
	createTrackbar("LowH", "Control", &iLowH, 179);		//Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255);		//Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);		//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	//// Capture a Temporary Image from the camera
	//Mat imgTmp;
	//cap.read(imgTmp);
	//// Create a Black Image with the size as the camera output
	//Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;
	Mat leftFrame, rightFrame;

	while (true)
	{
		Mat RImage;
		Mat LImage;
		Mat BGR_RImage, BGR_LImage;
		if (video.grab()) {
			video.retrieve(leftFrame, 0);

			// https://blog.csdn.net/icvpr/article/details/8518863
			
			cv::Rect Rrect(0, 0,1280, 720);
			leftFrame(Rrect).copyTo(RImage);
			//cv::cvtColor(RImage, BGR_RImage, CV_BGR2GRAY);
			//cv::cvtColor(BGR_RImage, RImage, CV_GRAY2BGR);
			
			cv::Rect Lrect(1280, 0, 1280, 720);
			leftFrame(Lrect).copyTo(LImage);
			//cv::cvtColor(BGR_LImage, LImage, CV_GRAY2BGR);
		
		}

		//bool bSuccess = cap.read(imgOriginal);		// Read a new frame from video
		//// If not success, break loop
		//if (!bSuccess) 
		//{
		//	cout << "Cannot read a frame from video stream" << endl;
		//	break;
		//}

		// Convert the captured frame from BGR to HSV
		Mat imgHSV_L, imgHSV_R;
		cvtColor(LImage, imgHSV_L, COLOR_BGR2HSV);
		cvtColor(RImage, imgHSV_R, COLOR_BGR2HSV);
		//cvtColor(RImage, imgHSV_R, COLOR_BGR2HSV);
		// Create the Thresholded Image 過濾出HSV圖 input: imgHSV ， output: imgThresholded
		Mat imgThresholded_L, imgThresholded_R;
		inRange(imgHSV_L, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded_L);
		inRange(imgHSV_R, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded_R);
		
		// Noise Reduction using Mathematical Morphology
		// Morphological Opening (Removes small objects from the foreground)先侵蝕再膨脹 斷開(Opening) 
		//https://ccw1986.blogspot.com/2012/11/opencverodedilate.html
		//http://monkeycoding.com/?p=577
		//處理AB物體連在一起，要斷開。因為侵蝕是去除掉邊界，讓A B物體分明，但是這樣會失真，所以還需要膨脹恢復
		erode(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		erode(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		// Morphological Closing (Removes small holes from the foreground)先膨脹再侵蝕 閉合(Closing)
		//處理A物體內有許多細小的洞，需要把洞消除。
		dilate(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		// Calculate the Moments of the Thresholded Image
		Moments oMoments_L = moments(imgThresholded_L);

		double dM01_L = oMoments_L.m01;
		double dM10_L = oMoments_L.m10;
		double dArea_L = oMoments_L.m00;

		Moments oMoments_R = moments(imgThresholded_R);

		double dM01_R = oMoments_R.m01;
		double dM10_R = oMoments_R.m10;
		double dArea_R = oMoments_R.m00;

		// If the area <= 10000, consider that it's because of the noise
		if (dArea_L > 10000&& dArea_R > 10000)
		{
			// Calculate the Centroid of the Object 算物體的質心
			//等同於 Point2f center = Point2f(mu.m10/mu.m00 , mu.m01/mu.m00);
			double posX_L = dM10_L / dArea_L;
			double posY_L = dM01_L / dArea_L;

			double posX_R = dM10_R / dArea_R;
			double posY_R = dM01_R / dArea_R;


			// Draw a Red Line following the Object
			// Initial Point of the Red Line
			/*int iLastX = -1;
			int iLastY = -1;
			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
			line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
			}

			iLastX = posX;
			iLastY = posY;*/

			// Draw a Red Circle tracking the Object
			int Radius_L = sqrt((dArea_L / 255) / 3.14);//先算半徑 由pi R^2 = Area
			if (posX_L >= 0 && posY_L >= 0)
			{
				circle(LImage, Point(posX_L, posY_L), Radius_L, Scalar(0, 0, 255), 2);//在圖上畫一個圓
			}

			int Radius_R = sqrt((dArea_L / 255) / 3.14);//先算半徑 由pi R^2 = Area
			if (posX_R >= 0 && posY_R >= 0)
			{
				circle(RImage, Point(posX_R, posY_R), Radius_R, Scalar(0, 0, 255), 2);//在圖上畫一個圓
			}

			// Calculate the Distance between the Object and the camera
			/*
			int realR = 4;
			int f = -10;
			int fpx = 750;
			int d = (realR*fpx) / Radius_L + f;
			cout << "Distance = " << d << endl;
			cout << endl;
			*/
			//cout<<"posX_L: "<< posX_L<<" ,posX_R: "<< posX_R <<",posY_L: "<< posY_L<<",posY_R: "<< posY_R <<endl;
			if(posX_R - posX_L!=0)
			cout << posX_R - posX_L << ", "<< (double)175*15/ (double)(posX_R - posX_L)<<"cm"<<endl;
			//cout << posX_R - posX_L * 15 / 175<<"cm"<<endl;
			//TEST1: 15cm ，posX_R - posX_L value 175
			// Find the Contour of the Object
			/*vector<vector<cv::Point>> contours;
			findContours(imgThresholded, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
			for (int i = 0; i< contours.size(); i++)
			{
			drawContours(imgLines, contours, i, Scalar(0, 0, 255), 2);
			}*/
		}

		// Show the Thresholded Image
		imshow("Thresholded_L Image", imgThresholded_L);
		imshow("Thresholded_R Image", imgThresholded_R);
		// Show the Tracked Image
		//imgOriginal = imgOriginal + imgLines;
		imshow("Original_L", LImage);
		imshow("Original_R", RImage);

		// Wait for key is pressed then break loop
		if (waitKey(5) == 27)			//ESC 27, ENTER 13, SPACE 32
		{
			break;
		}
	}

	return 0;
}