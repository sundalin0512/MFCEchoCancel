
// MFCEchoCancel.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCEchoCancelApp: 
// �йش����ʵ�֣������ MFCEchoCancel.cpp
//

class CMFCEchoCancelApp : public CWinApp
{
public:
	CMFCEchoCancelApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCEchoCancelApp theApp;
