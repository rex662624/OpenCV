#include "calibration.h"

void CameraCalibrator::setFilename() {
	m_filenames.clear();
	m_filenames.push_back("./image/chessboard_1.jpg");
	m_filenames.push_back("./image/chessboard_2.jpg");
	m_filenames.push_back("./image/chessboard_3.jpg");
	m_filenames.push_back("./image/chessboard_4.jpg");
	m_filenames.push_back("./image/chessboard_5.jpg");
	m_filenames.push_back("./image/chessboard_6.jpg");
	m_filenames.push_back("./image/chessboard_7.jpg");
	m_filenames.push_back("./image/chessboard_8.jpg");
	m_filenames.push_back("./image/chessboard_9.jpg");
	m_filenames.push_back("./image/chessboard_10.jpg");
	m_filenames.push_back("./image/chessboard_11.jpg");
	m_filenames.push_back("./image/chessboard_12.jpg");
	m_filenames.push_back("./image/chessboard_13.jpg");
	m_filenames.push_back("./image/chessboard_14.jpg");
}

void CameraCalibrator::setBorderSize(const Size &borderSize) {
	m_borderSize = borderSize;
}

void CameraCalibrator::addChessboardPoints() {
	vector<Point2f> srcCandidateCorners;
	vector<Point3f> dstCandidateCorners;
	for (int i = 0; i<m_borderSize.height; i++) {
		for (int j = 0; j<m_borderSize.width; j++) {
			dstCandidateCorners.push_back(Point3f(i, j, 0.0f));
		}
	}

	for (int i = 0; i<m_filenames.size(); i++) {
		Mat image = imread(m_filenames[i], CV_LOAD_IMAGE_GRAYSCALE);
		findChessboardCorners(image, m_borderSize, srcCandidateCorners);
		/*
		for (int i = 0; i < srcCandidateCorners.size(); i++) {
		cout << srcCandidateCorners[i] << " ";
		}
		cout << endl;*/
		cout << srcCandidateCorners.size() << endl;
		TermCriteria param(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1);
		cornerSubPix(image, srcCandidateCorners, Size(5, 5), Size(-1, -1), param);
		if (srcCandidateCorners.size() == m_borderSize.area()) {
			addPoints(srcCandidateCorners, dstCandidateCorners);
		}
	}
}

void CameraCalibrator::addPoints(const vector<Point2f> &srcCorners, const vector<Point3f> &dstCorners) {
	m_srcPoints.push_back(srcCorners);
	m_dstPoints.push_back(dstCorners);
}

void CameraCalibrator::calibrate(const Mat &src, Mat &dst) {
	Size imageSize = src.size();
	Mat cameraMatrix, distCoeffs, map1, map2;
	vector<Mat> rvecs, tvecs;
	calibrateCamera(m_dstPoints, m_srcPoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), Mat(), imageSize, CV_32F, map1, map2);
	remap(src, dst, map1, map2, INTER_LINEAR);
}