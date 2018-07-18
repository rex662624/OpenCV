
// StereoProjectDlg.cpp : ��@��
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


// �� App About �ϥ� CAboutDlg ��ܤ��

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ܤ�����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
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


// CStereoProjectDlg ��ܤ��



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


// CStereoProjectDlg �T���B�z�`��

BOOL CStereoProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �N [����...] �\���[�J�t�Υ\���C

	// IDM_ABOUTBOX �����b�t�ΩR�O�d�򤧤��C
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

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w
	AllocConsole();
	freopen ("CONOUT$", "w", stdout );

	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
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

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CStereoProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CStereoProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//----------------------------------------start from here------------------------------------------------------------------

//https://blog.csdn.net/lijiayu2015/article/details/53079661
CvSize imgsize=cvSize(640,512);
int n_boards= 5; //�ݭn�O�l��(�Ӥ��ƶq!?)
int board_w = 4; //���e�I�Ӽ�
int board_h = 6; //�O���I�Ӽ�
int board_n = board_w * board_h;
CvSize board_sz = cvSize( board_w, board_h );
int L_corner_count;
int R_corner_count;

CvCapture* caprure;
IplImage* img;
CvRect rect_roi;//Region of Interest �P���쪺�����A�]�N�O�n�q�v������X�ѽL��
CvMat* mat_roi = cvCreateMat(640, 512,CV_8UC3);

IplImage* L_img;
IplImage* R_img;
IplImage* L_drawimg;
IplImage* R_drawimg;
IplImage* L_drawlineimg;
IplImage* R_drawlineimg;
IplImage* L_gray_img;
IplImage* R_gray_img;
CvPoint2D32f* L_corners = new CvPoint2D32f[ board_n ];//�s���Ϫ��C���I���y��
CvPoint2D32f* R_corners = new CvPoint2D32f[ board_n ];//�s�k�Ϫ��C���I���y��
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

	JPGDlg.DoModal(); //�}�ҿ��
	szFileName = JPGDlg.GetPathName();	//��o�Ϥ����|
	CT2A filename(szFileName);
	CvCapture* caprure=cvCreateFileCapture(filename);//Ū���v����
	cout<<"Ū���v������"<<"\n\r";
		
	int successes=0;//���\����
	int step =0;//�ĴX���p��*�I���ƥ�
	//int frame;//�p�ƥ�
	//��v���s���Ϥ�~~~~~~~~~~~~~
	int frame=0;
	int board_dt=20;
	L_img = cvCreateImage( cvSize(640,512), IPL_DEPTH_8U, 3);
	R_img = cvCreateImage( cvSize(640,512), IPL_DEPTH_8U, 3);

	while(successes < n_boards){
		
		img=cvQueryFrame(caprure);
		if(!img) break;
		//�]���v���O���k�\�b�@�_�A�ҥH�n��L���Φ��@�H�@�b
		rect_roi = cvRect(0,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);//�n�^�����ϰ�Orect_roi�ҩw�q�A�|��img��rect_roi�ϰ��^����mat_roi
		cvGetImage(mat_roi, L_img);//��誺 mat_roi�٤��O�Ϲ� �u�O�x�}(��type�N���D)�A�ҥH�n��o�ন�Ϲ�
		//�W���Oleft �U���Oright
		rect_roi = cvRect(640,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, R_img);
		//----------------------------------------------��o�̨��o�۾��Ϲ� �U���}�l��ѽL��
	
	if(frame++ % board_dt==0){  //�C20��frame���@��
		
	//���chessboard��corner�y�Шæs��L_corners/R_corners
	int L_found = cvFindChessboardCorners(L_img, board_sz, L_corners, &L_corner_count,
		CV_CALIB_CB_FILTER_QUADS);//���n���O�o�� L_corners �]�N�O�ѽL���C���I���y��.
	int R_found = cvFindChessboardCorners(R_img, board_sz, R_corners, &R_corner_count,
		CV_CALIB_CB_FILTER_QUADS);
	//����corners�� Subpixel accuracy  �]���W������ƥu���˴��X�@�Ӥj�����ȡA�U���O�h����T���� �� cvFindCornerSubPix
	L_gray_img = cvCreateImage(imgsize,8,1);//subpixel ���ŧi
	R_gray_img = cvCreateImage(imgsize,8,1);//subpixel
	
	cvCvtColor(L_img,L_gray_img, CV_BGR2GRAY);//��img�ন�Ƕ��A��ֱm��z�Z
	cvCvtColor(R_img,R_gray_img, CV_BGR2GRAY);
	
	cvFindCornerSubPix(L_gray_img,L_corners,L_corner_count,cvSize(board_w, board_h),cvSize(-1,-1),
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));//https://baike.baidu.com/item/cvFindCornerSubPix
	cvFindCornerSubPix(R_gray_img,R_corners,R_corner_count,cvSize(board_w, board_h),cvSize(-1,-1),
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
		
	L_drawimg = cvCloneImage(L_img);//�ƻs�@�����㪺IplImage��Ƶ��c�ϧΤγ]�w
	R_drawimg = cvCloneImage(R_img);//�ƻs�@�����㪺IplImage��Ƶ��c�ϧΤγ]�w
	
	cvDrawChessboardCorners(L_drawimg, board_sz, L_corners,L_corner_count, L_found);//�e�X�ѽL��s�u
	cvDrawChessboardCorners(R_drawimg, board_sz, R_corners,R_corner_count, R_found);//�e�X�ѽL��s�u
	
	cvNamedWindow("L_DrawChessboardCorners",0);//MFC�����]�w//0�i���ܤj�p,1�۰ʽվ�ϧΤj�p
	cvNamedWindow("R_DrawChessboardCorners",0);//MFC�����]�w//0�i���ܤj�p,1�۰ʽվ�ϧΤj�p
	cvResizeWindow("L_DrawChessboardCorners",640,512);//�վ�����j�p
	cvResizeWindow("R_DrawChessboardCorners",640,512);//�վ�����j�p
	cvShowImage("L_DrawChessboardCorners", L_drawimg );//�N�Ϥ���ܦb�����W
	cvShowImage("R_DrawChessboardCorners", R_drawimg );//�N�Ϥ���ܦb�����W
	
	//��o�̧��ѽL�y�СA�ӥB�w�g�e�b�ϤW
	// If we got a good board, add it to our data
	if( L_corner_count == board_n && R_corner_count == board_n ) {
	if (successes == 5&&flag==0) { flag = 1; }//new�]���Ǫ�����6�ӹϮ׬O����
	else{
	step = successes*board_n;
	//�Ncorner���y�Цs�J�u���n�s���a��AL_corners�O�C���|�N���Ȧs�AL_image_points�~�O�̲��x�s���a��
	for( int k=step, j=0; j<board_n; ++k,++j ) {
			CV_MAT_ELEM(*L_image_points, float,k,0) = L_corners[j].x;//�o�Ө�ƬO�Q�qCvMat��������A�ѼƤ��O�O�ǤJ�x�}/�ݴ���������type/�n��J����/���C
			CV_MAT_ELEM(*L_image_points, float,k,1) = L_corners[j].y;
			CV_MAT_ELEM(*R_image_points, float,k,0) = R_corners[j].x;
			CV_MAT_ELEM(*R_image_points, float,k,1) = R_corners[j].y;
			CV_MAT_ELEM(*object_points,float,k,0) = j/board_w;
			CV_MAT_ELEM(*object_points,float,k,1) = j%board_w;
			CV_MAT_ELEM(*object_points,float,k,2) = 0.0f;
		}
		CV_MAT_ELEM(*point_counts, int,successes,0) = board_n;
		successes++;
		cout<<"�w���"<<successes<<"�ӹϹ����";
				}
	}
	 } //if �C20��frame���@��
	
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

	cout<<"chessboard��T���M����\n\r";
	//��o�̧�ѽL�y�Цs��
	//����l�Ƭ۾����ѼƯx�}L_intrinsic_matrix�AR_intrinsic_matrix
	CV_MAT_ELEM( *L_intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *L_intrinsic_matrix, float, 1, 1 ) = 1.0f;
	CV_MAT_ELEM( *R_intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *R_intrinsic_matrix, float, 1, 1 ) = 1.0f;

	//�D�n�ѴѽL�檺�I��T�A�o�쪺�O���Ѽ� ���ܫY�� ����x�}(R) �����V�q(T) ����x�}(E) ��¦�x�}(F)
	cvStereoCalibrate(object_points, L_image_points,R_image_points, point_counts,L_intrinsic_matrix, L_distortion_coeffs,
		R_intrinsic_matrix,R_distortion_coeffs,imgsize,R,T,E,F, CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH,cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5) );
	
	cout<<"cvStereoCalibrate����\n\r";
	//��bouguet(P691)
	//�o�쪺�OL_Rrectify, R_Rrectify, L_Prectify, R_Prectify
	cvStereoRectify(L_intrinsic_matrix, R_intrinsic_matrix,L_distortion_coeffs,R_distortion_coeffs, imgsize,
		R,T,L_Rrectify, R_Rrectify, L_Prectify, R_Prectify, 0,CV_CALIB_ZERO_DISPARITY);
	
	L_mapx = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	L_mapy = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	R_mapx = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	R_mapy = cvCreateMat( imgsize.height,imgsize.width, CV_32F );
	//Precompute maps for cvRemap()�}�l��P696���Ҧ��ʧ@�A��ѼƶǤJ�ե���x�۾��ÿ�X�B���᪺L_mapx,L_mapy
	cvInitUndistortRectifyMap(L_intrinsic_matrix,L_distortion_coeffs,L_Rrectify, L_Prectify,L_mapx,L_mapy);
	cvInitUndistortRectifyMap(R_intrinsic_matrix,R_distortion_coeffs,R_Rrectify, R_Prectify,R_mapx,R_mapy);
	cout<<"cvInitUndistortRectifyMap����\n\r";

	cvDestroyWindow("L_DrawChessboardCorners");//�M�������O����
	cvDestroyWindow("R_DrawChessboardCorners");//�M�������O����
	
	//Always work in undistorted space �H�U���O���F�e���u�Ϊ�(P712)
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
	L_drawlineimg = cvCloneImage(L_img);//�ƻs�@�����㪺IplImage��Ƶ��c�ϧΤγ]�w
	R_drawlineimg = cvCloneImage(R_img);//�ƻs�@�����㪺IplImage��Ƶ��c�ϧΤγ]�w
	
	
	for(int L_line=0*board_n;L_line<board_n;L_line++)//�k�u����
	{
		y1=-cvmGet(R_Rect_lines,L_line,2)/cvmGet(R_Rect_lines,L_line,1);//ax+by+c=0 x=0 y=-c/b
		y2=(-1)*((cvmGet(R_Rect_lines,L_line,0)*imgsize.width)+cvmGet(R_Rect_lines,L_line,2))/cvmGet(R_Rect_lines,L_line,1);
		cvLine(L_drawlineimg,cvPoint(0,y1),cvPoint(imgsize.width,y2),CV_RGB(255,0,0),1);
	
	}
	for(int R_line=0;R_line<board_n;R_line++)//���u�k��
	{
		y1=-cvmGet(L_Rect_lines,R_line,2)/cvmGet(L_Rect_lines,R_line,1);//ax+by+c=0 x=0 y=-c/b
		y2=(-1)*((cvmGet(L_Rect_lines,R_line,0)*imgsize.width)+cvmGet(L_Rect_lines,R_line,2))/cvmGet(L_Rect_lines,R_line,1);
		cvLine(R_drawlineimg,cvPoint(0,y1),cvPoint(imgsize.width,y2),CV_RGB(255,0,0),1);
	}
	cout<<"���Epipolar lines";
	cvNamedWindow("L_DrawEpilines",0);//MFC�����]�w//0�i���ܤj�p,1�۰ʽվ�ϧΤj�p
	cvNamedWindow("R_DrawEpilines",0);//MFC�����]�w//0�i���ܤj�p,1�۰ʽվ�ϧΤj�p
	cvResizeWindow("L_DrawEpilines",640,512);//�վ�����j�p
	cvResizeWindow("R_DrawEpilines",640,512);//�վ�����j�p
	cvShowImage("L_DrawEpilines", L_drawlineimg );//�N�Ϥ���ܦb�����W
	cvShowImage("R_DrawEpilines",R_drawlineimg );//�N�Ϥ���ܦb�����W
	
}


//-------------------------------------------------disparity map

void CStereoProjectDlg::OnBnClickedButton2()
{



	CString szFileName=0;
	CFileDialog JPGDlg(TRUE);
	
	JPGDlg.DoModal(); //�}�ҿ��
	szFileName = JPGDlg.GetPathName();	//��o�Ϥ����|
	CT2A filename(szFileName);
	CvCapture* caprure=cvCreateFileCapture(filename);//Ū���v����
	cout<<"Ū���v������(�ť���=pause)"<<"\n\r";

	cvNamedWindow( "disparity",0 );
	cvResizeWindow("disparity",640,512);//�վ�����j�p
	cvNamedWindow( "rectified",0 );
	cvResizeWindow("rectified",1280,512);//�վ�����j�p
	while(1){
		//�@�˥����frame�M���Υ��k�e��
		img=cvQueryFrame(caprure);
		if(!img) break;

		rect_roi = cvRect(0,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, L_img);
		rect_roi = cvRect(640,0,640,512);
		cvGetSubRect(img, mat_roi, rect_roi);
		cvGetImage(mat_roi, R_img);
	
	// TODO: �b���[�J����i���B�z�`���{���X
	
	
	/*Rect_img_L=cvCloneImage( L_img );
	Rect_img_R=cvCloneImage( R_img );
	cvRemap( L_img, Rect_img_L,L_mapx, L_mapy );
	cvRemap( R_img, Rect_img_R,R_mapx, R_mapy );
	cvNamedWindow("Rectify_L",0);//MFC�����]�w//0�i���ܤj�p,1�۰ʽվ�ϧΤj�p
	cvResizeWindow("Rectify_L",640,512);//�վ�����j�p
	cvNamedWindow("Rectify_R",0);//MFC�����]�w//0�i���ܤj�p,1�۰ʽվ�ϧΤj�p
	cvResizeWindow("Rectify_R",640,512);//�վ�����j�p
	cvShowImage("Rectify_L", Rect_img_L);//�N�Ϥ���ܦb�����W
	cvShowImage("Rectify_R", Rect_img_R);//�N�Ϥ���ܦb�����W*/
	
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
	//�ϥΫe���p�⪺L_mapx�ML_mapy����ե��Ϥ��A��Ӭ۾����Ϥ��B��P�@�Ӭ۾������B����  ��img1r=img1r*�B�����Y��
	cvRemap(img1r, img1r, L_mapx, L_mapy );
	cvRemap(img2r, img2r, R_mapx, R_mapy );
	cvFindStereoCorrespondenceBM(img1r,img2r,disp,BMState);//��J���۾�img1r �k�۾�img2r ��Xdisparity map �`�N���k�۾��n�O��q�D �]�N�Ogray
	cvNormalize( disp, vdisp, 0, 256, CV_MINMAX );//�k�@��
	
	//vShowImage( "disparity", vdisp );
	//�W���O��� disparitymap �U���O����B���L�᪺��
	cvGetCols( pair, part, 0, imgsize.width );
	cvCvtColor(img1r, part, CV_GRAY2BGR );
	cvGetCols( pair, part, imgsize.width,imgsize.width*2 );
	cvCvtColor(img2r, part, CV_GRAY2BGR );
	for(int j = 0; j < imgsize.height; j += 16 )
	cvLine( pair, cvPoint(0,j),cvPoint(imgsize.width*2,j),	CV_RGB(0,255,0));

	cvShowImage( "rectified", pair );
	
	//�p�GP�Npause
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
	cvDestroyWindow("disparity");//�M�������O����
	cvDestroyWindow("rectified");//�M�������O����
	

}
