
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

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
//----------------------------------------start from here------------------------------------------------------------------

//https://blog.csdn.net/lijiayu2015/article/details/53079661
CvSize imgsize=cvSize(640,512);
int n_boards= 5; //需要板子數(照片數量!?)
int board_w = 4; //版寬點個數
int board_h = 6; //板高點個數
int board_n = board_w * board_h;
CvSize board_sz = cvSize( board_w, board_h );
int L_corner_count;
int R_corner_count;

CvCapture* caprure;
IplImage* img;
CvRect rect_roi;//Region of Interest 感興趣的部分，也就是要從影像中找出棋盤格
CvMat* mat_roi = cvCreateMat(640, 512,CV_8UC3);

IplImage* L_img;
IplImage* R_img;
IplImage* L_drawimg;
IplImage* R_drawimg;
IplImage* L_drawlineimg;
IplImage* R_drawlineimg;
IplImage* L_gray_img;
IplImage* R_gray_img;
CvPoint2D32f* L_corners = new CvPoint2D32f[ board_n ];//存左圖的每個點的座標
CvPoint2D32f* R_corners = new CvPoint2D32f[ board_n ];//存右圖的每個點的座標
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
int flag = 0;
void CStereoProjectDlg::OnBnClickedButton1()
{
	CString szFileName=0;
	CFileDialog JPGDlg(TRUE);

	JPGDlg.DoModal(); //開啟選單
	szFileName = JPGDlg.GetPathName();	//獲得圖片路徑
	CT2A filename(szFileName);
	CvCapture* caprure=cvCreateFileCapture(filename);//讀取影片檔
	cout<<"讀取影片完成"<<"\n\r";
		
	int successes=0;//成功次數
	int step =0;//第幾次計算*點的數目
	//int frame;//計數用
	//把影像存成圖片~~~~~~~~~~~~~
	int frame=0;
	int board_dt=20;
	L_img = cvCreateImage( cvSize(640,512), IPL_DEPTH_8U, 3);
	R_img = cvCreateImage( cvSize(640,512), IPL_DEPTH_8U, 3);

	while(successes < n_boards){
		
		img=cvQueryFrame(caprure);
		if(!img) break;
		//因為影片是左右擺在一起，所以要把他切割成一人一半
		rect_roi = cvRect(0,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);//要擷取的區域是rect_roi所定義，會把img的rect_roi區域擷取給mat_roi
		cvGetImage(mat_roi, L_img);//剛剛的 mat_roi還不是圖像 只是矩陣(看type就知道)，所以要把她轉成圖像
		//上面是left 下面是right
		rect_roi = cvRect(640,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, R_img);
		//----------------------------------------------到這裡取得相機圖像 下面開始找棋盤格
	
	if(frame++ % board_dt==0){  //每20個frame取一次
		
	//找到chessboard的corner座標並存到L_corners/R_corners
	int L_found = cvFindChessboardCorners(L_img, board_sz, L_corners, &L_corner_count,
		CV_CALIB_CB_FILTER_QUADS);//重要的是得到 L_corners 也就是棋盤的每個點的座標.
	int R_found = cvFindChessboardCorners(R_img, board_sz, R_corners, &R_corner_count,
		CV_CALIB_CB_FILTER_QUADS);
	//拿到corners的 Subpixel accuracy  因為上面的函數只能檢測出一個大略的值，下面是去找更精確的值 用 cvFindCornerSubPix
	L_gray_img = cvCreateImage(imgsize,8,1);//subpixel 先宣告
	R_gray_img = cvCreateImage(imgsize,8,1);//subpixel
	
	cvCvtColor(L_img,L_gray_img, CV_BGR2GRAY);//把img轉成灰階，減少彩色干擾
	cvCvtColor(R_img,R_gray_img, CV_BGR2GRAY);
	
	cvFindCornerSubPix(L_gray_img,L_corners,L_corner_count,cvSize(board_w, board_h),cvSize(-1,-1),
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));//https://baike.baidu.com/item/cvFindCornerSubPix
	cvFindCornerSubPix(R_gray_img,R_corners,R_corner_count,cvSize(board_w, board_h),cvSize(-1,-1),
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
		
	L_drawimg = cvCloneImage(L_img);//複製一份完整的IplImage資料結構圖形及設定
	R_drawimg = cvCloneImage(R_img);//複製一份完整的IplImage資料結構圖形及設定
	
	cvDrawChessboardCorners(L_drawimg, board_sz, L_corners,L_corner_count, L_found);//畫出棋盤格連線
	cvDrawChessboardCorners(R_drawimg, board_sz, R_corners,R_corner_count, R_found);//畫出棋盤格連線
	
	cvNamedWindow("L_DrawChessboardCorners",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvNamedWindow("R_DrawChessboardCorners",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvResizeWindow("L_DrawChessboardCorners",640,512);//調整視窗大小
	cvResizeWindow("R_DrawChessboardCorners",640,512);//調整視窗大小
	cvShowImage("L_DrawChessboardCorners", L_drawimg );//將圖片顯示在視窗上
	cvShowImage("R_DrawChessboardCorners", R_drawimg );//將圖片顯示在視窗上
	
	//到這裡找到棋盤座標，而且已經畫在圖上
	// If we got a good board, add it to our data
	if( L_corner_count == board_n && R_corner_count == board_n ) {
	if (successes == 5&&flag==0) { flag = 1; }//new因為學長的第6個圖案是錯的
	else{
	step = successes*board_n;
	//將corner的座標存入真的要存的地方，L_corners是每次疊代的暫存，L_image_points才是最終儲存的地方
	for( int k=step, j=0; j<board_n; ++k,++j ) {
			CV_MAT_ELEM(*L_image_points, float,k,0) = L_corners[j].x;//這個函數是想從CvMat獲取元素，參數分別是傳入矩陣/待提取的元素type/要放入哪行/哪列
			CV_MAT_ELEM(*L_image_points, float,k,1) = L_corners[j].y;
			CV_MAT_ELEM(*R_image_points, float,k,0) = R_corners[j].x;
			CV_MAT_ELEM(*R_image_points, float,k,1) = R_corners[j].y;
			CV_MAT_ELEM(*object_points,float,k,0) = j/board_w;
			CV_MAT_ELEM(*object_points,float,k,1) = j%board_w;
			CV_MAT_ELEM(*object_points,float,k,2) = 0.0f;
		}
		CV_MAT_ELEM(*point_counts, int,successes,0) = board_n;
		successes++;
		cout<<"已獲取"<<successes<<"個圖像資料";
				}
	}
	 } //if 每20個frame取一次
	
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

	cout<<"chessboard資訊收尋完畢\n\r";
	//到這裡把棋盤座標存完
	//先初始化相機的參數矩陣L_intrinsic_matrix，R_intrinsic_matrix
	CV_MAT_ELEM( *L_intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *L_intrinsic_matrix, float, 1, 1 ) = 1.0f;
	CV_MAT_ELEM( *R_intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *R_intrinsic_matrix, float, 1, 1 ) = 1.0f;

	//主要由棋盤格的點資訊，得到的是內參數 畸變係數 旋轉矩陣(R) 平移向量(T) 本質矩陣(E) 基礎矩陣(F)
	cvStereoCalibrate(object_points, L_image_points,R_image_points, point_counts,L_intrinsic_matrix, L_distortion_coeffs,
		R_intrinsic_matrix,R_distortion_coeffs,imgsize,R,T,E,F, CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH,cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5) );
	
	cout<<"cvStereoCalibrate完成\n\r";
	//用bouguet(P691)
	//得到的是L_Rrectify, R_Rrectify, L_Prectify, R_Prectify
	cvStereoRectify(L_intrinsic_matrix, R_intrinsic_matrix,L_distortion_coeffs,R_distortion_coeffs, imgsize,
		R,T,L_Rrectify, R_Rrectify, L_Prectify, R_Prectify, 0,CV_CALIB_ZERO_DISPARITY);
	
	L_mapx = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	L_mapy = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	R_mapx = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	R_mapy = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	//Precompute maps for cvRemap()開始做P696的所有動作，把參數傳入校正整台相機並輸出矯正後的L_mapx,L_mapy
	cvInitUndistortRectifyMap(L_intrinsic_matrix,L_distortion_coeffs,L_Rrectify, L_Prectify,L_mapx,L_mapy);
	cvInitUndistortRectifyMap(R_intrinsic_matrix,R_distortion_coeffs,R_Rrectify, R_Prectify,R_mapx,R_mapy);
	cout<<"cvInitUndistortRectifyMap完成\n\r";

	cvDestroyWindow("L_DrawChessboardCorners");//清除視窗記憶體
	cvDestroyWindow("R_DrawChessboardCorners");//清除視窗記憶體
	
	//Always work in undistorted space 以下都是為了畫紅線用的(P712)
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
	L_drawlineimg = cvCloneImage(L_img);//複製一份完整的IplImage資料結構圖形及設定
	R_drawlineimg = cvCloneImage(R_img);//複製一份完整的IplImage資料結構圖形及設定
	
	
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
	cout<<"顯示Epipolar lines";
	cvNamedWindow("L_DrawEpilines",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvNamedWindow("R_DrawEpilines",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvResizeWindow("L_DrawEpilines",640,512);//調整視窗大小
	cvResizeWindow("R_DrawEpilines",640,512);//調整視窗大小
	cvShowImage("L_DrawEpilines", L_drawlineimg );//將圖片顯示在視窗上
	cvShowImage("R_DrawEpilines",R_drawlineimg );//將圖片顯示在視窗上
	
}


//-------------------------------------------------disparity map

void CStereoProjectDlg::OnBnClickedButton2()
{



	CString szFileName=0;
	CFileDialog JPGDlg(TRUE);
	
	JPGDlg.DoModal(); //開啟選單
	szFileName = JPGDlg.GetPathName();	//獲得圖片路徑
	CT2A filename(szFileName);
	CvCapture* caprure=cvCreateFileCapture(filename);//讀取影片檔
	cout<<"讀取影片完成(空白鍵=pause)"<<"\n\r";

	cvNamedWindow( "disparity",0 );
	cvResizeWindow("disparity",640,512);//調整視窗大小
	cvNamedWindow( "rectified",0 );
	cvResizeWindow("rectified",1280,512);//調整視窗大小
	while(1){
		//一樣先獲取frame和切割左右畫面
		img=cvQueryFrame(caprure);
		if(!img) break;

		rect_roi = cvRect(0,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, L_img);
		rect_roi = cvRect(640,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, R_img);
	
	// TODO: 在此加入控制項告知處理常式程式碼
	
	
	/*Rect_img_L=cvCloneImage( L_img );
	Rect_img_R=cvCloneImage( R_img );
	cvRemap( L_img, Rect_img_L,L_mapx, L_mapy );
	cvRemap( R_img, Rect_img_R,R_mapx, R_mapy );
	cvNamedWindow("Rectify_L",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvResizeWindow("Rectify_L",640,512);//調整視窗大小
	cvNamedWindow("Rectify_R",0);//MFC視窗設定//0可改變大小,1自動調整圖形大小
	cvResizeWindow("Rectify_R",640,512);//調整視窗大小
	cvShowImage("Rectify_L", Rect_img_L);//將圖片顯示在視窗上
	cvShowImage("Rectify_R", Rect_img_R);//將圖片顯示在視窗上*/
	
	CvMat* pair = cvCreateMat( imgsize.height, imgsize.width*2,CV_8UC3 );
	CvMat* part = cvCreateMat( imgsize.height, imgsize.width,CV_8UC3 );
	CvMat* img1r = cvCreateMat( imgsize.height,imgsize.width, CV_8U );
	CvMat* img2r = cvCreateMat( imgsize.height,imgsize.width, CV_8U );
	CvMat* disp = cvCreateMat( imgsize.height,imgsize.width, CV_16S );
	CvMat* vdisp = cvCreateMat( imgsize.height,imgsize.width, CV_8U );
	
	//Setup for finding stereo correspondences
	CvStereoBMState *BMState = cvCreateStereoBMState();
	assert(BMState != 0);
	BMState->preFilterSize=41;
	BMState->preFilterCap=31;
	BMState->SADWindowSize=41;
	BMState->minDisparity=-64;
	BMState->numberOfDisparities=128;
	BMState->textureThreshold=10;
	BMState->uniquenessRatio=15;
	
	cvCvtColor( L_img,img1r, CV_BGR2GRAY );
	cvCvtColor( R_img,img2r, CV_BGR2GRAY );
	//使用前面計算的L_mapx和L_mapy立體校正圖片，兩個相機的圖片處於同一個相機平面且行對準  用img1r=img1r*矯正的係數
	cvRemap(img1r, img1r, L_mapx, L_mapy );
	cvRemap(img2r, img2r, R_mapx, R_mapy );
	cvFindStereoCorrespondenceBM(img1r,img2r,disp,BMState);//輸入左相機img1r 右相機img2r 輸出disparity map 注意左右相機要是單通道 也就是gray
	cvNormalize( disp, vdisp, 0, 256, CV_MINMAX );//歸一化
	
	//vShowImage( "disparity", vdisp );
	//上面是顯示 disparitymap 下面是顯示矯正過後的圖
	cvGetCols( pair, part, 0, imgsize.width );
	cvCvtColor(img1r, part, CV_GRAY2BGR );
	cvGetCols( pair, part, imgsize.width,imgsize.width*2 );
	cvCvtColor(img2r, part, CV_GRAY2BGR );
	for(int j = 0; j < imgsize.height; j += 16 )
	cvLine( pair, cvPoint(0,j),cvPoint(imgsize.width*2,j),	CV_RGB(0,255,0));

	cvShowImage( "rectified", pair );
	
	//如果P就pause
	char c =cvWaitKey(100);
	if(c==32){       //p
	c=0;
	while(c!=32 && c!= 27){
		c=cvWaitKey(250);
			}
	}
	if(c==27)
	break;
		}
	cvDestroyWindow("disparity");//清除視窗記憶體
	cvDestroyWindow("rectified");//清除視窗記憶體
	

}
