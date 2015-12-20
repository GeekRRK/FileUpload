
// FileUpload.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "FileUpload.h"
#include "FileUploadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFileUploadApp

BEGIN_MESSAGE_MAP(CFileUploadApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFileUploadApp 构造

CFileUploadApp::CFileUploadApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CFileUploadApp 对象

CFileUploadApp theApp;


// CFileUploadApp 初始化

BOOL CFileUploadApp::InitInstance()
{
	this->m_hMutex = ::CreateMutex(NULL,FALSE, this->m_pszAppName);
	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		return FALSE;
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("河北索睿软件技术有限公司"));

	CFileUploadDlg dlg;
	m_pMainWnd = &dlg;

	dlg.Create(CFileUploadDlg::IDD); //创建为非模态对话框
	dlg.ShowWindow(SW_HIDE); //创建完毕后，可以设置对话框的显示方式，正常为“SW_HIDE”
	dlg.RunModalLoop(); //消息循环

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CFileUploadApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	::CloseHandle(this->m_hMutex);

	return CWinApp::ExitInstance();
}
