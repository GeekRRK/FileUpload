
// UpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Update.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"
#include "Tlhelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInternetSession *CUpdateDlg::m_pInetSession;
CFtpConnection *CUpdateDlg::m_pFtpConnection;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CUpdateDlg 对话框




CUpdateDlg::CUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUpdateDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CUpdateDlg 消息处理程序

BOOL CUpdateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Update();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUpdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpdateDlg::Update()
{
	
	//读取config.ini中的相关系统进行FTP连接下载最新版本FileUpload.exe
	CString path;
	GetExePath(path);
	m_IniFile.SetFileName(path + "/config.ini");
	int port;
	CString strIP, strUsername, strPasswd, strUpdatepath, oldVersion;
	m_IniFile.GetProfileString("Config", "version", oldVersion);
	m_IniFile.GetProfileString("Config", "ip", strIP);
	port = m_IniFile.GetProfileInt("Config", "port");
	m_IniFile.GetProfileString("Config", "username", strUsername);
	m_IniFile.GetProfileString("Config", "passwd", strPasswd);
	m_IniFile.GetProfileString("Config", "updatePath", strUpdatepath);
		
	m_pInetSession = new CInternetSession(NULL, 1, 
		PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
	try
	{
		m_pFtpConnection = m_pInetSession->GetFtpConnection((LPCTSTR)strIP, (LPCTSTR)strUsername, (LPCTSTR)strPasswd, port, TRUE);
	}
	//若登陆不成功则抛出异常，以下是针对异常的处理
	catch(CInternetException *pEx){
		/*TCHAR szError[1024] = {0};
		if(pEx->GetErrorMessage(szError,1024))
			AfxMessageBox(szError);
		else
			AfxMessageBox(_T("There was an exception"));*/
		MessageBox("无法连接到服务器，请检查网络是否正常，并打开配置界面检查信息是否有误，或者服务器没有开启相关服务");
		pEx->Delete();
		m_pInetSession->Close();
		delete m_pInetSession;

		HANDLE hself = GetCurrentProcess();
		TerminateProcess(hself, 0);
	}

	if(PathFileExists(path + "/update.ini"))
	{
		DeleteFile(path + "/update.ini");
	}

	//进行FTP连接并下载最新版本相关信息
	if(!m_pFtpConnection->GetFile(strUpdatepath + "/update.ini", path + "/update.ini"), FALSE)
	{
		//MessageBox("update.ini下载失败，请打开配置界面检查服务器更新路径是否有误，或者权限不足");
	}
	else
	{
		//设置与应用程序同目录下的update.ini为配置文件并获得更新路径和最新版本信息
		m_IniFile.SetFileName(path + "/update.ini");
		CString newVersion;
		m_IniFile.GetProfileString("Update", "version", newVersion);
		DeleteFile(path + "/update.ini");
		
		//当前版本与最新版本信息进行比较，如果不一致就进行更新，并启动新版本，并把应用程序当前路径作为参数传递过去
		if(newVersion != oldVersion)
		{
			m_IniFile.SetFileName(path + "/config.ini");

			KillProcessFromName("FileUpload.exe");
			CFile::Rename(path + "/FileUpload.exe", path + "/FileUploadX.exe");
			
			if(!m_pFtpConnection->GetFile(strUpdatepath + "/FileUpload.exe", path + "/FileUpload.exe"), FALSE)
			{
				MessageBox("更新失败，请打开配置界面检查更新路径是否有误");
				CFile::Rename(path + "/FileUploadX.exe", path + "/FileUpload.exe");
			}
			else
			{
				Sleep(1000);
				DeleteFile(path + "/FileUploadX.exe");
				m_IniFile.SetProfileString("Config", "version", newVersion);
			}
			ShellExecute(NULL, "open", path + "/FileUpload.exe", "ONCE", NULL, SW_SHOWNORMAL);
		}
	}

	m_pFtpConnection->Close();
	m_pInetSession->Close();
	delete m_pFtpConnection;
	delete m_pInetSession;

	HANDLE hself = GetCurrentProcess();
	TerminateProcess(hself, 0);
}

BOOL CUpdateDlg::KillProcessFromName(CString strProcessName)
{
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)  
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
  
    //PROCESSENTRY32进程快照的结构体  
    PROCESSENTRY32 pe;  
  
    //实例化后使用Process32First获取第一个快照的进程前必做的初始化操作  
    pe.dwSize = sizeof(PROCESSENTRY32);  
  
  
    //下面的IF效果同:  
    //if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄  
    if(!Process32First(hSnapShot,&pe))  
    {  
        return FALSE;  
    }  
  
    //将字符串转换为小写  
    strProcessName.MakeLower();  
  
    //如果句柄有效  则一直获取下一个句柄循环下去  
    while (Process32Next(hSnapShot,&pe))  
    {  
  
        //pe.szExeFile获取当前进程的可执行文件名称  
        CString scTmp = pe.szExeFile;  
  
  
        //将可执行文件名称所有英文字母修改为小写  
        scTmp.MakeLower();  
  
        //比较当前进程的可执行文件名称和传递进来的文件名称是否相同  
        //相同的话Compare返回0  
        if(!scTmp.Compare(strProcessName))  
        {  
  
            //从快照进程中获取该进程的PID(即任务管理器中的PID)  
            DWORD dwProcessID = pe.th32ProcessID;  
            HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);  
            ::TerminateProcess(hProcess,0);  
            CloseHandle(hProcess);  
            return TRUE;  
        }  
        scTmp.ReleaseBuffer();  
    }  
    strProcessName.ReleaseBuffer();  
    return FALSE; 
}


void CUpdateDlg::GetExePath(CString &path)
{
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
	path.ReleaseBuffer(); 
	int pos = path.ReverseFind('\\'); 
	path = path.Left(pos); 
	path.Replace('\\', '/');
}
