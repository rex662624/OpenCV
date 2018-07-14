// stereoslam.cpp : Defines the entry point for the console application.

#include "VideoInput.h"
#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  
using namespace cv;
using namespace std;


videoInput VI;   //video input controller
int camID_left = 0;
int camID_right = 1;
unsigned char * Buffer1, *Buffer2;
int m_ImgWidth = 0;
int m_ImgHeight = 0;
cv::Mat m_WebCamFrmL;
cv::Mat m_WebCamFrmR;
bool m_bCamRunning = false;
HANDLE m_hThread;
CRITICAL_SECTION cs;

int main()
{
	int numDevices = VI.listDevices();
	const char* camName_left = "Stereo Vision 2";
	const char* camName_right = "Stereo Vision 1";
	for (int i = 0; i<3; i++)
	{
		const char* camName = VI.getDeviceName(i);

		if (std::strcmp(camName, camName_left) == 0)
			camID_left = i;
		else if (std::strcmp(camName, camName_right) == 0)
			camID_right = i;
	}
	VI.setupDevice(camID_left);
	VI.setupDevice(camID_right);
	m_ImgWidth = VI.getWidth(camID_left);
	m_ImgHeight = VI.getHeight(camID_left);
	int m_Imgsize = VI.getSize(camID_left);
	Buffer1 = new unsigned char[m_Imgsize];

	if (VI.getSize(camID_right) != m_Imgsize)
	{
		return 0;
	}
	Buffer2 = new unsigned char[m_Imgsize];

	m_WebCamFrmL.create(m_ImgHeight, m_ImgWidth, CV_8UC3);
	m_WebCamFrmR.create(m_ImgHeight, m_ImgWidth, CV_8UC3);

	InitializeCriticalSection(&cs);
	m_bCamRunning = true;


	while (m_bCamRunning)
	{
		Sleep(30);
		EnterCriticalSection(&cs);
		if (VI.isFrameNew(camID_left)) {
			VI.getPixels(camID_left, Buffer1, false, true);    //fills pixels as a BGR (for openCV) unsigned char array - flipping
		}
		if (VI.isFrameNew(camID_right)) {
			VI.getPixels(camID_right, Buffer2, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - flipping
		}

		std::memcpy(m_WebCamFrmL.data, Buffer1, 3 * m_ImgWidth*m_ImgHeight * sizeof(unsigned char));
		std::memcpy(m_WebCamFrmR.data, Buffer2, 3 * m_ImgWidth*m_ImgHeight * sizeof(unsigned char));

		LeaveCriticalSection(&cs);

		imshow("left", m_WebCamFrmL);
		imshow("right", m_WebCamFrmR);
		if (cv::waitKey(30) >= 0) break;
	}


	return 0;
}