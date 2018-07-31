
// StereoProjectDlg.cpp : 實作檔
//



#include "stdafx.h"
#include "StereoProject.h"
#include "StereoProjectDlg.h"
#include "afxdialogex.h"
#include "opencv2\highgui\highgui.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning( disable : 4996 ) 

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStereoProjectDlg 對話方塊



CStereoProjectDlg::CStereoProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStereoProjectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStereoProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStereoProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CStereoProjectDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CStereoProjectDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CStereoProjectDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CStereoProjectDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CStereoProjectDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CStereoProjectDlg 訊息處理常式

BOOL CStereoProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	AllocConsole();
	freopen ("CONOUT$", "w", stdout );

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CStereoProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CStereoProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CStereoProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CameraSizeW = 640;//單一camera的寬
int CameraSizeH = 480;//單一camera的高

CvSize imgsize=cvSize(CameraSizeW, CameraSizeH);
//modified
int n_boards= 35; //需要板子數
int board_w = 9; //版寬點個數
int board_h = 6; //板高點個數
int board_n = board_w * board_h;
CvSize board_sz = cvSize( board_w, board_h );
int L_corner_count;
int R_corner_count;

CvCapture* caprure;
IplImage* img;
CvRect rect_roi;
CvMat* mat_roi = cvCreateMat(CameraSizeW, CameraSizeH,CV_8UC3);

IplImage* L_img;
IplImage* R_img;
IplImage* L_drawimg;
IplImage* R_drawimg;
IplImage* L_drawlineimg;
IplImage* R_drawlineimg;
IplImage* L_gray_img;
IplImage* R_gray_img;
CvPoint2D32f* L_corners = new CvPoint2D32f[ board_n ];
CvPoint2D32f* R_corners = new CvPoint2D32f[ board_n ];
CvMat* L_image_points = cvCreateMat(n_boards*board_n,2,CV_32FC1);
CvMat* R_image_points = cvCreateMat(n_boards*board_n,2,CV_32FC1);
CvMat* object_points = cvCreateMat(n_boards*board_n,3,CV_32FC1);
CvMat* point_counts = cvCreateMat(n_boards,1,CV_32SC1);

CvMat* L_intrinsic_matrix = cvCreateMat(3,3,CV_32FC1);
CvMat* L_distortion_coeffs = cvCreateMat(5,1,CV_32FC1);
CvMat* R_intrinsic_matrix = cvCreateMat(3,3,CV_32FC1);
CvMat* R_distortion_coeffs = cvCreateMat(5,1,CV_32FC1);
CvMat* R = cvCreateMat(3,3,CV_64FC1);
CvMat* T = cvCreateMat(3,1,CV_64FC1);
CvMat* E = cvCreateMat(3,3,CV_64FC1);
CvMat* F = cvCreateMat(3,3,CV_64FC1);

CvMat* L_Rrectify = cvCreateMat(3,3,CV_64F);
CvMat* R_Rrectify = cvCreateMat(3,3,CV_64F);
CvMat* L_Prectify = cvCreateMat(3,4,CV_64F);
CvMat* R_Prectify = cvCreateMat(3,4,CV_64F);
CvMat* L_mapx ;
CvMat* L_mapy ;
CvMat* R_mapx ;
CvMat* R_mapy ;

void CStereoProjectDlg::OnBnClickedButton1()
{
	/*
	CString szFileName=0;
	CFileDialog JPGDlg(TRUE);

	JPGDlg.DoModal(); //開啟選單
	szFileName = JPGDlg.GetPathName();	//獲得圖片路徑
	CT2A filename(szFileName);
	CvCapture* caprure=cvCreateFileCapture(filename);//讀取影片檔
	*/
	printf("讀取影片完成\n\r");
		
	int successes=0;//成功次數
	int step =0;//第幾次計算*點的數目
	//int frame;//計數用
	//把影像存成圖片~~~~~~~~~~~~~
	int frame=0;
	int board_dt=20;
	L_img = cvCreateImage( cvSize(CameraSizeW, CameraSizeH), IPL_DEPTH_8U, 3);
	R_img = cvCreateImage( cvSize(CameraSizeW, CameraSizeH), IPL_DEPTH_8U, 3);

	// ============== NEW ======================
	int imgnum = 1; 
	int k = 0;
	cv::String path("./pic_12cm/*.jpg"); //select only jpg
	vector<cv::String> fn;
	//vector<cv::Mat> data;
	cv::glob(path, fn, true); // recurse

	// =========================================

	while(successes < n_boards){
		// ref: https://blog.csdn.net/likezhaobin/article/details/6764583
		//char *imgName = (char*)malloc(sizeof(char)* 10);
		//sprintf(imgName, "%d.PNG", imgnum++);
		//printf("%s\n", imgName);
		//img = cvLoadImage(imgName);
		printf("%s\n", fn[k]);
		cv::Mat im = cv::imread(fn[k++]);
		//imshow("Mat", im);
		cvWaitKey(300);
		img = &IplImage(im);

		//img=cvQueryFrame(caprure);
		if(!img) break;
		rect_roi = cvRect(0,0, CameraSizeW, CameraSizeH);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, L_img);
		rect_roi = cvRect(CameraSizeW,0, CameraSizeW, CameraSizeH);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, R_img);
	
//		if(frame++ % board_dt==0){  //每20個frame取一次
		
			//找到chessboard的corner座標並存到corners
			int L_found = cvFindChessboardCorners(L_img, board_sz, L_corners, &L_corner_count,
				CV_CALIB_CB_FILTER_QUADS);
			int R_found = cvFindChessboardCorners(R_img, board_sz, R_corners, &R_corner_count,
				CV_CALIB_CB_FILTER_QUADS);
			//拿到corners的 Subpixel accuracy 
			L_gray_img = cvCreateImage(imgsize,8,1);//subpixel
			R_gray_img = cvCreateImage(imgsize,8,1);//subpixel
	
			cvCvtColor(L_img,L_gray_img, CV_BGR2GRAY);
			cvCvtColor(R_img,R_gray_img, CV_BGR2GRAY);
	
			cvFindCornerSubPix(L_gray_img,L_corners,L_corner_count,cvSize(9,6),cvSize(-1,-1),
				cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
			cvFindCornerSubPix(R_gray_img,R_corners,R_corner_count,cvSize(9,6),cvSize(-1,-1),
				cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
		
			L_drawimg = cvCloneImage(L_img);//複製一份完整的IplImage資料結構圖形及設定
			R_drawimg = cvCloneImage(R_img);//複製一份完整的IplImage資料結構圖形及設定
			L_drawlineimg = cvCloneImage(L_img);//複製一份完整的IplImage資料結構圖形及設定
			R_drawlineimg = cvCloneImage(R_img);//複製一份完整的IplImage資料結構圖形及設定
	
			cvDrawChessboardCorners(L_drawimg, board_sz, L_corners,L_corner_count, L_found);//畫出棋盤格連線
			cvDrawChessboardCorners(R_drawimg, board_sz, R_corners,R_corner_count, R_found);//畫出棋盤格連線
	
			cvNamedWindow("L_DrawChessboardCorners",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
			cvNamedWindow("R_DrawChessboardCorners",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
			cvResizeWindow("L_DrawChessboardCorners", CameraSizeW, CameraSizeH);//調整視窗大小
			cvResizeWindow("R_DrawChessboardCorners", CameraSizeW, CameraSizeH);//調整視窗大小
			cvShowImage("L_DrawChessboardCorners", L_drawimg );//將圖片顯示在視窗上
			cvShowImage("R_DrawChessboardCorners", R_drawimg );//將圖片顯示在視窗上
	
			// If we got a good board, add it to our data
			if( L_corner_count == board_n && R_corner_count == board_n  ) {
				step = successes*board_n;
				for( int k=step, j=0; j<board_n; ++k,++j ) {
					CV_MAT_ELEM(*L_image_points, float,k,0) = L_corners[j].x;
					CV_MAT_ELEM(*L_image_points, float,k,1) = L_corners[j].y;
					CV_MAT_ELEM(*R_image_points, float,k,0) = R_corners[j].x;
					CV_MAT_ELEM(*R_image_points, float,k,1) = R_corners[j].y;
					CV_MAT_ELEM(*object_points,float,k,0) = j/board_w;
					CV_MAT_ELEM(*object_points,float,k,1) = j%board_w;
					CV_MAT_ELEM(*object_points,float,k,2) = 0.0f;
				}
				CV_MAT_ELEM(*point_counts, int,successes,0) = board_n;
				successes++;
				printf("已獲取 %d 個圖像資料\n", successes);
			}
//		 } //if 每20個frame取一次
	
		char c =cvWaitKey(100);
		if(c==32){       
		c=0;
		while(c!=32 && c!= 27){
			c=cvWaitKey(250);
				}
		}
		if(c==27)
		break;
		
	}//while

	printf("chessboard資訊收尋完畢\n\r");
	
	CV_MAT_ELEM( *L_intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *L_intrinsic_matrix, float, 1, 1 ) = 1.0f;
	CV_MAT_ELEM( *R_intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *R_intrinsic_matrix, float, 1, 1 ) = 1.0f;
	
	cvStereoCalibrate(object_points, L_image_points,R_image_points, point_counts,L_intrinsic_matrix, L_distortion_coeffs,
		R_intrinsic_matrix,R_distortion_coeffs,imgsize,R,T,E,F,
		cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
		CV_CALIB_FIX_ASPECT_RATIO +	CV_CALIB_ZERO_TANGENT_DIST +CV_CALIB_SAME_FOCAL_LENGTH );
	printf("cvStereoCalibrate完成\n\r");
	
	cvStereoRectify(L_intrinsic_matrix, R_intrinsic_matrix,L_distortion_coeffs,R_distortion_coeffs, imgsize,
		R,T,L_Rrectify, R_Rrectify, L_Prectify, R_Prectify, 0,CV_CALIB_ZERO_DISPARITY);
	
	L_mapx = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	L_mapy = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	R_mapx = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	R_mapy = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	// Precompute maps for cvRemap()
	// Computes undistortion+rectification map for a head of stereo camera
	cvInitUndistortRectifyMap(L_intrinsic_matrix,L_distortion_coeffs,L_Rrectify, L_Prectify,L_mapx,L_mapy);
	cvInitUndistortRectifyMap(R_intrinsic_matrix,R_distortion_coeffs,R_Rrectify, R_Prectify,R_mapx,R_mapy);
	printf("cvInitUndistortRectifyMap完成\n\r");

	cvDestroyWindow("L_DrawChessboardCorners");//清除視窗記憶體
	cvDestroyWindow("R_DrawChessboardCorners");//清除視窗記憶體
	
	//Always work in undistorted space
	CvMat _L_Rect_image_points=cvMat(1,n_boards*board_n,CV_32FC2,L_corners);
	CvMat _R_Rect_image_points=cvMat(1,n_boards*board_n,CV_32FC2,R_corners);
	CvMat* L_Rect_image_points = cvCreateMat(1,n_boards*board_n,CV_32FC2);
	CvMat* R_Rect_image_points = cvCreateMat(1,n_boards*board_n,CV_32FC2);
		
	cvUndistortPoints( &_L_Rect_image_points, L_Rect_image_points,L_intrinsic_matrix, L_distortion_coeffs
		,0,L_intrinsic_matrix);
	cvUndistortPoints( &_R_Rect_image_points, R_Rect_image_points,R_intrinsic_matrix, R_distortion_coeffs
		,0, R_intrinsic_matrix);
	
	CvMat* L_Rect_lines = cvCreateMat(n_boards*board_n,3,CV_32FC1);
	CvMat* R_Rect_lines = cvCreateMat(n_boards*board_n,3,CV_32FC1);

	cvComputeCorrespondEpilines(L_Rect_image_points, 1, F, L_Rect_lines );
	cvComputeCorrespondEpilines(R_Rect_image_points, 2, F, R_Rect_lines );
	
	float y1=0.0;
	float y2=0.0;
	/*
	L_drawlineimg = cvCloneImage(L_img);//複製一份完整的IplImage資料結構圖形及設定
	R_drawlineimg = cvCloneImage(R_img);//複製一份完整的IplImage資料結構圖形及設定
	*/
	
	for(int L_line=0*board_n;L_line<board_n;L_line++)//右線左圖
	{
		y1=-cvmGet(R_Rect_lines,L_line,2)/cvmGet(R_Rect_lines,L_line,1);//ax+by+c=0 x=0 y=-c/b
		y2=(-1)*((cvmGet(R_Rect_lines,L_line,0)*imgsize.width)+cvmGet(R_Rect_lines,L_line,2))/cvmGet(R_Rect_lines,L_line,1);
		cvLine(L_drawlineimg,cvPoint(0,y1),cvPoint(imgsize.width,y2),CV_RGB(255,0,0),1);
	
	}
	for(int R_line=0;R_line<board_n;R_line++)//左線右圖
	{
		y1=-cvmGet(L_Rect_lines,R_line,2)/cvmGet(L_Rect_lines,R_line,1);//ax+by+c=0 x=0 y=-c/b
		y2=(-1)*((cvmGet(L_Rect_lines,R_line,0)*imgsize.width)+cvmGet(L_Rect_lines,R_line,2))/cvmGet(L_Rect_lines,R_line,1);
		cvLine(R_drawlineimg,cvPoint(0,y1),cvPoint(imgsize.width,y2),CV_RGB(255,0,0),1);
	}
	printf("顯示Epipolar lines");
	cvNamedWindow("L_DrawEpilines",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvNamedWindow("R_DrawEpilines",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvResizeWindow("L_DrawEpilines", CameraSizeW, CameraSizeH);//調整視窗大小
	cvResizeWindow("R_DrawEpilines", CameraSizeW, CameraSizeH);//調整視窗大小
	cvShowImage("L_DrawEpilines", L_drawlineimg );//將圖片顯示在視窗上
	cvShowImage("R_DrawEpilines",R_drawlineimg );//將圖片顯示在視窗上
	
}




void CStereoProjectDlg::OnBnClickedButton2()
{

	CString szFileName = 0;
	/*
	CFileDialog JPGDlg(TRUE);

	JPGDlg.DoModal(); //開啟選單
	szFileName = JPGDlg.GetPathName();	//獲得圖片路徑
	CT2A filename(szFileName);
	CvCapture* caprure=cvCreateFileCapture(filename);//讀取影片檔
	printf("讀取影片完成(空白鍵=pause)\n\r");
	*/
	// ========== NEW ======================
	cv::VideoCapture video(0);	// org: 640 * 480

	video.set(CV_CAP_PROP_FRAME_WIDTH, CameraSizeW*2);
	video.set(CV_CAP_PROP_FRAME_HEIGHT,  CameraSizeH);

	if (!video.isOpened()) {
		// bool VideoWriter::isOpened(): 
		// 檢查是否初始化成功，如果成功返回true，否則返回false
		return;
	}
	cv::Mat leftFrame, rightFrame;

	// ======================================
	cvNamedWindow("disparity", 0);
	cvResizeWindow("disparity", CameraSizeW, CameraSizeH);//調整視窗大小
	cvNamedWindow("rectified", 0);
	cvResizeWindow("rectified", CameraSizeW*2, CameraSizeH);//調整視窗大小
										   // ====================
										   /*
										   cvNamedWindow("raw L", 0);
										   cvResizeWindow("raw L", 640, 480);//調整視窗大小
										   cvNamedWindow("raw R", 0);
										   cvResizeWindow("raw R", 640, 480);//調整視窗大小
										   */
										   // ============================
	while (1) {
		/*
		img=cvQueryFrame(caprure);
		if(!img) break;
		rect_roi = cvRect(0,0,640,480);
		cvGetSubRect(img, mat_roi, rect_roi);	// Makes a new matrix from <rect> subrectangle of input array. No data is copied.
		cvGetImage(mat_roi, L_img);				// Converts CvArr (IplImage or CvMat) to IplImage
		rect_roi = cvRect(640,0,640,480);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, R_img);
		*/
		// TODO: 在此加入控制項告知處理常式程式碼


		CvMat* pair = cvCreateMat(imgsize.height, imgsize.width * 2, CV_8UC3);
		CvMat* part = cvCreateMat(imgsize.height, imgsize.width, CV_8UC3);
		CvMat* img1r = cvCreateMat(imgsize.height, imgsize.width, CV_8U);
		CvMat* img2r = cvCreateMat(imgsize.height, imgsize.width, CV_8U);
		CvMat* disp = cvCreateMat(imgsize.height, imgsize.width, CV_16S);
		CvMat* vdisp = cvCreateMat(imgsize.height, imgsize.width, CV_8U);

		//Setup for finding stereo correspondences
		CvStereoBMState *BMState = cvCreateStereoBMState();
		assert(BMState != 0);

		BMState->preFilterSize = 41;
		BMState->preFilterCap = 31;
		BMState->SADWindowSize = 41;
		BMState->minDisparity = -64;	// 匹配搜苏从哪里开始
		BMState->numberOfDisparities = 128;	// 最大搜索视差数
		BMState->textureThreshold = 10;
		BMState->uniquenessRatio = 15; // 匹配功能函数


									   //cvCvtColor( L_img,img1r, CV_BGR2GRAY );
									   //cvCvtColor( R_img,img2r, CV_BGR2GRAY );
									   // ==================== NEW =========================
		if (!video.grab())
			break;
		video.retrieve(leftFrame, 0);

		//if (!leftFrame.empty())
		//	continue;


		// https://blog.csdn.net/icvpr/article/details/8518863
		cv::Mat RImage, greyRImage;
		cv::Rect Rrect(0, 0, CameraSizeW, CameraSizeH);
		leftFrame(Rrect).copyTo(RImage);
		cv::cvtColor(RImage, greyRImage, CV_BGR2GRAY);
		img2r = &CvMat(greyRImage);

		cv::Mat LImage, greyLImage;
		cv::Rect Lrect(CameraSizeW , 0, CameraSizeW, CameraSizeH);
		leftFrame(Lrect).copyTo(LImage);
		cv::cvtColor(LImage, greyLImage, CV_BGR2GRAY);
		img1r = &CvMat(greyLImage);
		/*
		cv::imshow("demo", leftFrame);
		cv::imshow("left demo", greyLImage);
		cv::imshow("right demo", greyRImage);
		*/
		// =================================================

		cvRemap(img1r, img1r, L_mapx, L_mapy);	// Performs generic geometric transformation using the specified coordinate maps
		cvRemap(img2r, img2r, R_mapx, R_mapy);
		//cvShowImage("raw L", img1r);	// ==========================
		//cvShowImage("row R", img2r);	// ==========================
		cvFindStereoCorrespondenceBM(img1r, img2r, disp, BMState);
		cvNormalize(disp, vdisp, 0, 256, CV_MINMAX);

		cvShowImage("disparity", vdisp);

		cvGetCols(pair, part, 0, imgsize.width);	//Selects column span of the input array: arr(:,start_col:end_col)
		cvCvtColor(img1r, part, CV_GRAY2BGR);		// Converts input array pixels from one color space to another
		cvGetCols(pair, part, imgsize.width, imgsize.width * 2);
		cvCvtColor(img2r, part, CV_GRAY2BGR);
		for (int j = 0; j < imgsize.height; j += 16)
			cvLine(pair, cvPoint(0, j), cvPoint(imgsize.width * 2, j), CV_RGB(0, 255, 0));

		cvShowImage("rectified", pair);

		char c = cvWaitKey(100);
		if (c == 32) {       //p
			c = 0;
			while (c != 32 && c != 27) {
				c = cvWaitKey(250);
			}
		}
		if (c == 27)
			break;
	}
	cvDestroyWindow("disparity");//清除視窗記憶體
	cvDestroyWindow("rectified");//清除視窗記憶體
								 //cvDestroyWindow("raw L");//清除視窗記憶體	// ======================
								 //cvDestroyWindow("raw R");//清除視窗記憶體	// ==================
}

void CStereoProjectDlg::OnBnClickedButton3()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	cv::Mat img_object = cv::imread("rec.jpg", cv::IMREAD_GRAYSCALE);
	if (!img_object.data) { std::cout << "Err: reading object image failed...\n"; }

	cv::VideoCapture cap(0);

	if (!cap.isOpened())
		return;

	char* method = "0";

	std::vector<cv::KeyPoint> keypoints_object, keypoints_scene;
	cv::Mat descriptors_object, descriptors_scene;

	cv::ORB orb;

	int minHessian = 400;
	cv::SurfFeatureDetector detector(minHessian);
	cv::SurfDescriptorExtractor extractor;

	//-- object
	if (method == 0) { //-- ORB
		orb.detect(img_object, keypoints_object);
		//cv::drawKeypoints(img_object, keypoints_object, img_object, cv::Scalar(0,255,255));
		//cv::imshow("template", img_object);

		orb.compute(img_object, keypoints_object, descriptors_object);
	}
	else { //-- SURF test
		detector.detect(img_object, keypoints_object);
		extractor.compute(img_object, keypoints_object, descriptors_object);
	}
	// http://stackoverflow.com/a/11798593
	//if(descriptors_object.type() != CV_32F)
	//    descriptors_object.convertTo(descriptors_object, CV_32F);


	for (;;) {
		cv::Mat frame;
		cap >> frame;

		cv::Mat img_scene = cv::Mat(frame.size(), CV_8UC1);
		cv::cvtColor(frame, img_scene, cv::COLOR_RGB2GRAY);

		if (method == 0) { //-- ORB
			orb.detect(img_scene, keypoints_scene);
			orb.compute(img_scene, keypoints_scene, descriptors_scene);
		}
		else { //-- SURF
			detector.detect(img_scene, keypoints_scene);
			extractor.compute(img_scene, keypoints_scene, descriptors_scene);
		}

		//-- matching descriptor vectors using FLANN matcher
		cv::FlannBasedMatcher matcher;
		std::vector<cv::DMatch> matches;
		cv::Mat img_matches;

		if (!descriptors_object.empty() && !descriptors_scene.empty()) {
			matcher.match(descriptors_object, descriptors_scene, matches);

			double max_dist = 0; double min_dist = 100;

			//-- Quick calculation of max and min idstance between keypoints
			for (int i = 0; i < descriptors_object.rows; i++)
			{
				double dist = matches[i].distance;
				if (dist < min_dist) min_dist = dist;
				if (dist > max_dist) max_dist = dist;
			}
			//printf("-- Max dist : %f \n", max_dist );
			//printf("-- Min dist : %f \n", min_dist );
			//-- Draw only good matches (i.e. whose distance is less than 3*min_dist)
			std::vector< cv::DMatch >good_matches;

			for (int i = 0; i < descriptors_object.rows; i++)
			{
				if (matches[i].distance < 3 * min_dist)
				{
					good_matches.push_back(matches[i]);
				}
			}

			cv::drawMatches(img_object, keypoints_object, img_scene, keypoints_scene, \
				good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
				std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

			//-- localize the object
			std::vector<cv::Point2f> obj;
			std::vector<cv::Point2f> scene;

			for (size_t i = 0; i < good_matches.size(); i++) {
				//-- get the keypoints from the good matches
				obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
				scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
			}
			if (!obj.empty() && !scene.empty() && good_matches.size() >= 4) {
				cv::Mat H = cv::findHomography(obj, scene, cv::RANSAC);

				//-- get the corners from the object to be detected
				std::vector<cv::Point2f> obj_corners(4);
				obj_corners[0] = cv::Point(0, 0);
				obj_corners[1] = cv::Point(img_object.cols, 0);
				obj_corners[2] = cv::Point(img_object.cols, img_object.rows);
				obj_corners[3] = cv::Point(0, img_object.rows);

				std::vector<cv::Point2f> scene_corners(4);

				cv::perspectiveTransform(obj_corners, scene_corners, H);

				//-- Draw lines between the corners (the mapped object in the scene - image_2 )
				
				cv::line(img_matches, \
					scene_corners[0] + cv::Point2f(img_object.cols, 0), \
					scene_corners[1] + cv::Point2f(img_object.cols, 0), \
					cv::Scalar(0, 255, 0), 4);
				cv::line(img_matches, \
					scene_corners[1] + cv::Point2f(img_object.cols, 0), \
					scene_corners[2] + cv::Point2f(img_object.cols, 0), \
					cv::Scalar(0, 255, 0), 4);
				cv::line(img_matches, \
					scene_corners[2] + cv::Point2f(img_object.cols, 0), \
					scene_corners[3] + cv::Point2f(img_object.cols, 0), \
					cv::Scalar(0, 255, 0), 4);
				cv::line(img_matches, \
					scene_corners[3] + cv::Point2f(img_object.cols, 0), \
					scene_corners[0] + cv::Point2f(img_object.cols, 0), \
					cv::Scalar(0, 255, 0), 4);
				
			}
		}
		cv::imshow("match result", img_matches);

		if (cv::waitKey(30) >= 0) break;
	}
}





void CStereoProjectDlg::OnBnClickedButton4()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	//ref: http://monkeycoding.com/?p=649
	// opencv: http://oblivious9.pixnet.net/blog/post/200316565-opencv-%E5%AE%89%E8%A3%9D%E5%92%8C%E8%A8%AD%E5%AE%9A%28for-visual-studio-%29
	// 亂碼問題：http://blog.sina.com.cn/s/blog_9b70803f0102x70v.html

	int ct = 0;
	char tipka;
	char filename[100]; // For filename
	int  c = 1; // For filename
	VideoCapture video(0);	// org: 640 * 480

	video.set(CV_CAP_PROP_FRAME_WIDTH, CameraSizeW*2);
	video.set(CV_CAP_PROP_FRAME_HEIGHT, CameraSizeH);
	// VideoCapture::VideoCapture(int device): 
	// 透過建構式不同的輸入參數，指定VideoCapture()的來源為影片檔或攝影機
	// device：裝置(像攝影機)的編號

	if (!video.isOpened()) {
		// bool VideoWriter::isOpened(): 
		// 檢查是否初始化成功，如果成功返回true，否則返回false
		return;
	}

	cv::Mat leftFrame, rightFrame;

	while (true) {
		// multi-head(stereo) camera:
		// https://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture-grab
		// https://stackoverflow.com/questions/22891129/opening-2-webcams-from-the-same-usb-cable

		if (video.grab()) {
			video.retrieve(leftFrame, 0);

			// https://blog.csdn.net/icvpr/article/details/8518863
			cv::Mat RImage, greyRImage;
			cv::Rect Rrect(0, 0, CameraSizeW, CameraSizeH);
			leftFrame(Rrect).copyTo(RImage);
			cv::cvtColor(RImage, greyRImage, CV_BGR2GRAY);

			cv::Mat LImage, greyLImage;
			cv::Rect Lrect(CameraSizeW , 0, CameraSizeW, CameraSizeH);
			leftFrame(Lrect).copyTo(LImage);
			cv::cvtColor(LImage, greyLImage, CV_BGR2GRAY);

			cv::Mat greyRL;
			cv::cvtColor(leftFrame, greyRL, CV_BGR2GRAY);

			if (!leftFrame.empty()) {
				imshow("left demo", greyLImage);
				imshow("right demo", greyRImage);
				//imshow("press P to screen shot", greyRL);
			}
			Sleep(5); // Sleep is mandatory - for no leg!

			tipka = cv::waitKey(30);
			// screen shot: https://stackoverflow.com/questions/44060798/take-screenshot-of-webcam-feed-on-keypress-using-opencv-and-c
			if (tipka == 'p') {

				sprintf_s(filename, "pic_12cm/%d.jpg", c); // select your folder - filename is "Frame_n"
				cv::waitKey(10);

				imwrite(filename, greyRL);
				cout << "Frame_" << c << endl;
				c++;
			}
		}
		/*
		if (waitKey(5) == 27)			//ESC 27, ENTER 13, SPACE 32
		{
			break;
		}
		*/
		
		// 等待 30 秒，若是按下 Esc（ASCII為27），結束視窗
		if (tipka == 'q') {
			break;
		}
		
	}

	video.release();
	destroyAllWindows();
}



void CStereoProjectDlg::OnBnClickedButton5()
{



	CString szFileName = 0;

	cv::VideoCapture video(0);	// org: 640 * 480

	video.set(CV_CAP_PROP_FRAME_WIDTH, CameraSizeW*2);
	video.set(CV_CAP_PROP_FRAME_HEIGHT, CameraSizeH);

	if (!video.isOpened()) {
		// bool VideoWriter::isOpened(): 
		// 檢查是否初始化成功，如果成功返回true，否則返回false
		return;
	}
	cv::Mat leftFrame, rightFrame;

	// ======================================

	int count = 0;
	//幾次算一次平均
	int size = 5;

	double average1 = 0, average2 = 0;//用來記disparity和Distance的平均

	CvMat* img1r = cvCreateMat(imgsize.height, imgsize.width, CV_8U);
	CvMat* img2r = cvCreateMat(imgsize.height, imgsize.width, CV_8U);


	
	//NEW=== Create a Control Window and Trackbars
	namedWindow("Control", 0);	// Control Window
	resizeWindow("Control",380,300);
	int iLowH = 17;
	int iHighH = 54;

	int iLowS = 54;
	int iHighS = 201;

	int iLowV = 91;
	int iHighV = 255;

	int erodeSize = 5;
	int dilateSize = 15;

	createTrackbar("erode", "Control", &erodeSize, 50);		//Value (0 - 255)
	createTrackbar("dilate", "Control", &dilateSize, 50);

	// Create Trackbars in Control Window
	createTrackbar("LowH", "Control", &iLowH, 179);		//Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255);		//Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);		//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);



	while (1) {

		// TODO: 在此加入控制項告知處理常式程式碼

		if (!video.grab())
			break;
		video.retrieve(leftFrame, 0);

		//if (!leftFrame.empty())
		//	continue;


		// https://blog.csdn.net/icvpr/article/details/8518863
		cv::Mat RImage, greyRImage;
		cv::Rect Rrect(0, 0, CameraSizeW, CameraSizeH);
		leftFrame(Rrect).copyTo(RImage);
		img2r = &CvMat(RImage);

		cv::Mat LImage, greyLImage;
		cv::Rect Lrect(CameraSizeW, 0, CameraSizeW, CameraSizeH);
		leftFrame(Lrect).copyTo(LImage);
		img1r = &CvMat(LImage);

		cvRemap(img1r, img1r, L_mapx, L_mapy);	// Performs generic geometric transformation using the specified coordinate maps
		cvRemap(img2r, img2r, R_mapx, R_mapy);

		Mat RImage_after = cvarrToMat(img2r);
		Mat LImage_after = cvarrToMat(img1r);



		//======NEW 校正完開始偵測距離=======

		//bool bSuccess = cap.read(imgOriginal);		// Read a new frame from video
		//// If not success, break loop
		//if (!bSuccess) 
		//{
		//	cout << "Cannot read a frame from video stream" << endl;
		//	break;
		//}

		// Convert the captured frame from BGR to HSV
		Mat imgHSV_L, imgHSV_R;
		cvtColor(LImage_after, imgHSV_L, COLOR_BGR2HSV);
		cvtColor(RImage_after, imgHSV_R, COLOR_BGR2HSV);
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
		erode(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));
		dilate(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		erode(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));
		dilate(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		// Morphological Closing (Removes small holes from the foreground)先膨脹再侵蝕 閉合(Closing)
		//處理A物體內有許多細小的洞，需要把洞消除。
		dilate(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(dilateSize, dilateSize)));
		erode(imgThresholded_L, imgThresholded_L, getStructuringElement(MORPH_ELLIPSE, Size(erodeSize, erodeSize)));

		dilate(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(dilateSize, dilateSize)));
		erode(imgThresholded_R, imgThresholded_R, getStructuringElement(MORPH_ELLIPSE, Size(erodeSize, erodeSize)));

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
		if (dArea_L > 10000 && dArea_R > 10000)
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
			//PosX值從0~1280(所以單位是pixel)
			int Radius_L = sqrt((dArea_L / 255) / 3.14);//先算半徑 由pi R^2 = Area
			if (posX_L >= 0 && posY_L >= 0)
			{
				circle(LImage_after, Point(posX_L, posY_L), Radius_L, Scalar(0, 0, 255), 2);//在圖上畫一個圓
			}

			int Radius_R = sqrt((dArea_L / 255) / 3.14);//先算半徑 由pi R^2 = Area
			if (posX_R >= 0 && posY_R >= 0)
			{
				circle(RImage_after, Point(posX_R, posY_R), Radius_R, Scalar(0, 0, 255), 2);//在圖上畫一個圓
			}

			//==================
			/*
			int SSD = 0;
			for (int i = 0; i < arraySize; i++)
				SSD += (array1[i] - array2[i])*(array1[i] - array2[i]);

			*/
			//==================

			// Calculate the Distance between the Object and the camera
			/*
			int realR = 4;
			int f = -10;
			int fpx = 750;
			int d = (realR*fpx) / Radius_L + f;
			cout << "Distance = " << d << endl;
			cout << endl;
			*/
			//參考課本P668 因為兩者的原點都在中央(1280/2的位置，所以xl=pox_L-640 xr=pox_R-640)
			//cout<<"posX_L: "<< posX_L<<" ,posX_R: "<< posX_R <<",posY_L: "<< posY_L<<",posY_R: "<< posY_R <<endl;
			
			//double disparity = (posX_R - CameraSizeW/2) - (posX_L - CameraSizeW / 2);
			//cout << (posX_L - 640) << "," << (posX_R - 640) << "," << disparity<<endl;
			//double disparity = posX_R- - posX_L + 26.4 ;//我們的LR是從我們看過去，但應該要從攝影機射出去看
			double fix=30.3;//31.9,30
			double disparity = posX_R - posX_L+fix;
			if (count < size) {

				average1 += disparity;
				//TEST1 : 15cm ，disparity value 101.6
				average2 += (double)(105.3 + fix )*15 / (double)(disparity);
				count++;

			}
			else {
				average1 /= (double)size;
				average2 /= (double)size;
				count = 0;
				cout << "averageDisparity: " << average1 << endl;
				cout << "averageDistance: " << average2 << endl;
				average1 = 0;
				average2 = 0;
			}





			//cout <<"Disparity: "<< disparity << endl;

			//if(posX_R - posX!=0)
			//cout << posX_R - posX_L << ", "<< (double)175*15/ (double)(posX_R - posX_L)<<"cm"<<endl;
			

			//cout << "Distance: " << (double)98 * 25.9 / (double)(disparity) << "cm" << endl;
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
		imshow("Original_L", LImage_after);
		imshow("Original_R", RImage_after);

		// Wait for key is pressed then break loop
		if (waitKey(5) == 27)			//ESC 27, ENTER 13, SPACE 32
		{
			break;
		}

	}

}

