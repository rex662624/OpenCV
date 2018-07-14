#pragma warning(disable:4996)

#include "opencv2\opencv.hpp"

#include <windows.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <direct.h>

using namespace std;
using namespace cv;

#define n_corner_cols 6 //�ѽL�I���
#define n_corner_rows 4 //�ѽL�I�C��
#define corner_size 42  //�ѽL��j�p (mm)


void fprintMatrix(Mat matrix, string name) //��X�x�}
{
	FILE * fp;
	fp = fopen(name.c_str(), "w");
	int i, j;
	printf("%s size %d %d\n", name.c_str(), matrix.cols, matrix.rows);
	for (i = 0; i< matrix.rows; ++i)
	{
		for (j = 0; j< matrix.cols; ++j)
		{
			fprintf(fp, "%lf ", matrix.at<  double >(i, j));
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}

vector <string> getAllFilesName(string inputFolder) //���o��Ƨ����Ҧ����ɮ�
{
	vector <string> allFileName;

	int n_files = 0;
	wstring inputFolder_wchar = wstring(inputFolder.begin(), inputFolder.end());
	const wchar_t* lookForFile = inputFolder_wchar.c_str();
	WIN32_FIND_DATA findFileFullName;

	HANDLE hFind;
	hFind = FindFirstFile(lookForFile, &findFileFullName);

	if (hFind != INVALID_HANDLE_VALUE)
	{

		do
		{
			wstring ws(findFileFullName.cFileName); //�Q��wstring �Nwchar���A���ɮצW���নstring���A
			string findFileFullName_str(ws.begin(), ws.end());

			allFileName.push_back(findFileFullName_str);

			n_files++;
		} while (FindNextFile(hFind, &findFileFullName) == TRUE);
	}

	cout << endl << "Number of files: " << n_files << endl;

	return allFileName;
}

void calibration(string folder_colorImg, string folder_calibration)
{
	vector <string> fileNames;
	fileNames = getAllFilesName(folder_colorImg + "*.jpg"); //�]�w�n�䪺��Ƭ�.jpg��

	vector<vector<Point2f>> corner_all_2D;
	vector<vector<Point3f>> corner_all_3D;

	Size image_size = imread(folder_colorImg + fileNames[0],CV_LOAD_IMAGE_COLOR).size();

	for (int index_file = 0; index_file < fileNames.size(); index_file++)
	{
		cout << fileNames[index_file] << endl;

		//read files
		Mat img_gray = imread(folder_colorImg + fileNames[index_file], CV_LOAD_IMAGE_GRAYSCALE);
		Mat img_color = imread(folder_colorImg + fileNames[index_file], CV_LOAD_IMAGE_COLOR);
		imshow("img gray", img_gray);

		//find corners 
		vector <Point2f> corner_per_2D;
		vector <Point3f> corner_per_3D;

		bool isFindCorner = findChessboardCorners(img_gray, Size(n_corner_cols, n_corner_rows), corner_per_2D);
		if (isFindCorner && (corner_per_2D.size() == n_corner_rows * n_corner_cols))
		{

			//2D corners
			cv::TermCriteria param(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
			cornerSubPix(img_gray, corner_per_2D, cv::Size(5, 5), cv::Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			drawChessboardCorners(img_color, Size(n_corner_cols, n_corner_rows), corner_per_2D, true);

			imshow("img color show", img_color);
			corner_all_2D.push_back(corner_per_2D);


			//3D corners
			for (int r = 0; r < n_corner_rows; r++)
			{
				for (int c = 0; c < n_corner_cols; c++)
				{
					corner_per_3D.push_back(Point3f(c*corner_size, r*corner_size, 0));
				}
			}
			corner_all_3D.push_back(corner_per_3D);

		}
		else
		{
			putText(img_color, "NO FOUND", Point(0, img_color.rows / 2), FONT_HERSHEY_DUPLEX, 3, Scalar(0, 0, 255));
			imshow("img color show", img_color);
		}
		//cv::waitKey(0); //��o�ӵ��Ѩ������i�H�ݳ�i���ѽL�檺���p
	}

	//camera calibration
	vector< Mat> rvecs, tvecs;
	Mat intrinsic_Matrix(3, 3, CV_64F);
	Mat distortion_coeffs(8, 1, CV_64F);

	calibrateCamera(corner_all_3D, corner_all_2D, image_size, intrinsic_Matrix, distortion_coeffs, rvecs, tvecs);
	cout << "Finished IMAGE Camera calibration" << endl;

	//save part
	fprintMatrix(intrinsic_Matrix, folder_calibration + "intrinsic.txt");
	fprintMatrix(distortion_coeffs, folder_calibration + "distortion.txt");

}


int main()
{
	string folder_colorImg = "./image/";    //�Ϥ�����Ƨ�

	string folder_calibration = "./output/"; //�n��m�۾��B����ƪ���Ƨ�
	_mkdir((folder_calibration).c_str());           //�إ߬۾��ե���ƪ���Ƨ�

	calibration(folder_colorImg, folder_calibration);

	std::system("pause");
	return 0;
}