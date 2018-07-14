#include "calibration.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iomanip>

using namespace cv;
using namespace std;

int main() {
	//------------------先拍照並存成file
	/*
	VideoCapture capl(0);
	namedWindow("Left");
	cout << "Press 'c' to capture ..." << endl;
	char choice = 'z';
	int count = 0;
	while (choice != 'q') {
		//grab frames quickly in succession  詳細見 獲取視頻可以有多種方法
		capl.grab();
		//execute the heavier decoding operations  
		Mat framel;
		capl.retrieve(framel);
		if (framel.empty()) break;
		imshow("Left", framel);
		if (choice == 'c') {
			//save files at proper locations if user presses 'c'
			stringstream l_name ;
			l_name << "./image/chessboard_" << count << ".jpg";
			imwrite(l_name.str(), framel);
			cout << "Saved set " << count << endl;
			count++;
		}
		choice = char(waitKey(1));
		//choice = getchar();
	}
	capl.release();*/
	//------------------開始做校正
	CameraCalibrator myCameraCalibrator;
	myCameraCalibrator.setFilename();
	myCameraCalibrator.setBorderSize(Size(6, 4));
	myCameraCalibrator.addChessboardPoints();

	Mat src = imread("./image/chessboard09.jpg", 0);
	Mat dst;
	myCameraCalibrator.calibrate(src, dst);

	imshow("Original Image", src);
	imshow("Undistorted Image", dst);
	waitKey();
	return 0;
}