
// Update.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUpdateApp:
// �йش����ʵ�֣������ Update.cpp
//

class CUpdateApp : public CWinApp
{
public:
	CUpdateApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	HANDLE m_hMutex;
	virtual int ExitInstance();
};

extern CUpdateApp theApp;