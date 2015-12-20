
// Update.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Update.h"
#include "UpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUpdateApp

BEGIN_MESSAGE_MAP(CUpdateApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUpdateApp ����

CUpdateApp::CUpdateApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CUpdateApp ����

CUpdateApp theApp;


// CUpdateApp ��ʼ��

BOOL CUpdateApp::InitInstance()
{
	this->m_hMutex = ::CreateMutex(NULL,FALSE, this->m_pszAppName);
	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		return FALSE;
	}
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CUpdateDlg dlg;
	m_pMainWnd = &dlg;

	dlg.Create(CUpdateDlg::IDD); //����Ϊ��ģ̬�Ի���
	dlg.ShowWindow(SW_HIDE); //������Ϻ󣬿������öԻ������ʾ��ʽ������Ϊ��SW_HIDE��
	dlg.RunModalLoop(); //��Ϣѭ��

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



int CUpdateApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	::CloseHandle(this->m_hMutex);
	return CWinApp::ExitInstance();
}
