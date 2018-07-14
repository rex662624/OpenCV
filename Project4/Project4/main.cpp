#include "calibration.h"

int main() {
	CameraCalibrator myCameraCalibrator;
	myCameraCalibrator.setFilename();
	myCameraCalibrator.setBorderSize(Size(6, 4));
	myCameraCalibrator.addChessboardPoints();

	Mat src = imread("./image/chessboard05.jpg", 0);
	Mat dst;
	myCameraCalibrator.calibrate(src, dst);

	imshow("Original Image", src);
	imshow("Undistorted Image", dst);
	waitKey();
	return 0;
}