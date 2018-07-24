#include<opencv2\opencv.hpp>
#include<iostream>
using namespace cv;

int main()
{
	//ref: http://monkeycoding.com/?p=649
	// opencv: http://oblivious9.pixnet.net/blog/post/200316565-opencv-%E5%AE%89%E8%A3%9D%E5%92%8C%E8%A8%AD%E5%AE%9A%28for-visual-studio-%29
	// 亂碼問題：http://blog.sina.com.cn/s/blog_9b70803f0102x70v.html


	VideoCapture video(0);	// org: 640 * 480

	video.set(CV_CAP_PROP_FRAME_WIDTH, 2560);
	video.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	// VideoCapture::VideoCapture(int device): 
	// 透過建構式不同的輸入參數，指定VideoCapture()的來源為影片檔或攝影機
	// device：裝置(像攝影機)的編號

	if (!video.isOpened()) {
		// bool VideoWriter::isOpened(): 
		// 檢查是否初始化成功，如果成功返回true，否則返回false
		return -1;
	}

	Mat leftFrame, rightFrame;
	Mat total;
	while (true) {
		// multi-head(stereo) camera:
		// https://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture-grab
		// https://stackoverflow.com/questions/22891129/opening-2-webcams-from-the-same-usb-cable

		if (video.grab()) {
			video.retrieve(leftFrame, 0);

			// https://blog.csdn.net/icvpr/article/details/8518863
			cv::Mat RImage, greyRImage;
			cv::Rect Rrect(0, 0,1280, 720);
			leftFrame(Rrect).copyTo(RImage);
			//cv::cvtColor(RImage, greyRImage, CV_BGR2GRAY);

			cv::Mat LImage, greyLImage;
			cv::Rect Lrect(1280, 0, 1280, 720);
			leftFrame(Lrect).copyTo(LImage);
			//cv::cvtColor(LImage, greyLImage, CV_BGR2GRAY);


			//video >> total;

			if (!leftFrame.empty()) {
				imshow("left demo", LImage);
				imshow("right demo", RImage);
				//imshow("total",total);
			}
			
		}

		// 等待 30 秒，若是按下 Esc（ASCII為27），結束視窗
		if (cvWaitKey(30) == 27) {
			break;
		}
	}

	video.release();
	
	return 0;
}
