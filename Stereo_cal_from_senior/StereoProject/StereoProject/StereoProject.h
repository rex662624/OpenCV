
// StereoProject.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CStereoProjectApp:
// �аѾ\��@�����O�� StereoProject.cpp
//

class CStereoProjectApp : public CWinApp
{
public:
	CStereoProjectApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CStereoProjectApp theApp;