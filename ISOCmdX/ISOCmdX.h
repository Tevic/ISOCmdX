
// ISOCmdX.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CISOCmdXApp:
// �йش����ʵ�֣������ ISOCmdX.cpp
//

class CISOCmdXApp : public CWinApp
{
public:
	CISOCmdXApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CISOCmdXApp theApp;