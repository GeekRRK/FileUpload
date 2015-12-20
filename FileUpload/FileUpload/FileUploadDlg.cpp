
// FileUploadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileUpload.h"
#include "FileUploadDlg.h"
#include "afxdialogex.h"
#include "IniFile.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_ICONMESSAGE WM_USER + 101
#define ID_MENU_EXIT   WM_USER + 102
#define ID_MENU_SHOW   WM_USER + 103
#define ID_MENU_UPDATE WM_USER + 104

#define GETTEXT(CTRL, VALUE)	GetDlgItem(CTRL)->GetWindowText(VALUE)
#define SETTEXT(CTRL, VALUE)	GetDlgItem(CTRL)->SetWindowText(VALUE)

CInternetSession* CFileUploadDlg::m_pInetSession;
CFtpConnection* CFileUploadDlg::m_pFtpConnection;
CIniFile CFileUploadDlg::m_IniFile;
Config CFileUploadDlg::m_config;
CString CFileUploadDlg::m_exePath;

BOOL CFileUploadDlg::UploadFinished = TRUE;

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


// CFileUploadDlg 对话框




CFileUploadDlg::CFileUploadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileUploadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileUploadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileUploadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_COMMAND(ID_MENU_EXIT, OnMenuExit)
	ON_COMMAND(ID_MENU_SHOW, OnMenuShow)
	ON_COMMAND(ID_MENU_UPDATE, OnMenuUpdate)  
	ON_BN_CLICKED(IDC_BUTTON2, &CFileUploadDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CFileUploadDlg::OnBnClickedButton1)
	ON_EN_KILLFOCUS(IDC_EDIT_INTERVAL, &CFileUploadDlg::OnEnKillfocusEditInterval)
	ON_EN_KILLFOCUS(IDC_EDIT_BEFORE, &CFileUploadDlg::OnEnKillfocusEditBefore)
	ON_EN_KILLFOCUS(IDC_EDIT_NEARPATH, &CFileUploadDlg::OnEnKillfocusEditNearpath)
END_MESSAGE_MAP()


// CFileUploadDlg 消息处理程序

BOOL CFileUploadDlg::OnInitDialog()
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
	((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->AddString("FTP");	//给下拉列表加选项
	//((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->AddString("SFTP");

	((CEdit *)GetDlgItem(IDC_EDIT_PASSWD))->SetPasswordChar('*');	//设置密码框的输入显示为*
	InitNotifyIcon();	//创建系统托盘图标
	ModifyStyle(WS_THICKFRAME, 0, SWP_NOSIZE);	//固定对话框的大小

	GetExePath(m_exePath); //获取.exe运行程序所在的目录

	SetAutoRun(TRUE);	//开机自启动
	LoadConfig();	//加载config.ini
	
	/*if(!strcmp(__argv[1], "ONCE"))*/
	CString argv1 = __argv[1];
	if(argv1 != "ONCE")	//如果不是被Update.exe启动的就更新
	{
		//创建线程为了检测网络是否连通，如果连通则启动更新程序
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Update, 0, 0, NULL);
		CloseHandle(hThread);
	}

	//检查“间隔分钟”是否太小，如果太小，服务器又连不上会不断出提示窗口造成死机
	if(m_config.interval < 1)
		m_config.interval = 1;
	SetTimer(1, m_config.interval * 60 * 1000, NULL);	//根据加载的interval设置定时器
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileUploadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID == SC_CLOSE)
	{
		this->ShowWindow(SW_HIDE);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileUploadDlg::OnPaint()
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
HCURSOR CFileUploadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileUploadDlg::InitNotifyIcon(void)
{
	m_NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_NotifyIcon.hWnd = this->GetSafeHwnd();
	strcpy(m_NotifyIcon.szTip, "文件上传工具");//提示文字
	m_NotifyIcon.uCallbackMessage = WM_ICONMESSAGE;//回调消息
	m_NotifyIcon.uFlags = NIF_MESSAGE|NIF_TIP|NIF_ICON;//新增标志
	m_NotifyIcon.hIcon = m_hIcon;//图标句柄
	m_NotifyIcon.uID = IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_ADD,&m_NotifyIcon);//为系统托盘增加此图标
}


LRESULT CFileUploadDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)  
	{  
	case WM_ICONMESSAGE:  
		if(lParam == WM_RBUTTONDOWN)  
		{  
			CMenu menu;  
			CPoint point;  
			GetCursorPos(&point);  
			menu.CreatePopupMenu();  

			menu.AppendMenu(MF_STRING, ID_MENU_SHOW, _T("显示/隐藏"));  
			menu.AppendMenu(MF_STRING, ID_MENU_UPDATE, _T("检查更新"));  
			menu.AppendMenu(MF_STRING, ID_MENU_EXIT, _T("退出程序"));

			menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this, NULL);  
			menu.DestroyMenu();  
		}  
		else if (lParam == WM_LBUTTONDOWN)
		{  
			ShowWindow(SW_SHOW);
			LoadConfig();
		}
		break;
	default:  
		break;  
	}  
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}


void CFileUploadDlg::OnMenuExit(void)
{
	SendMessage(WM_CLOSE, 0, 0);//发送关闭程序消息  
}


void CFileUploadDlg::OnMenuShow(void)
{
	if(IsWindowVisible())//若当前窗口为显示状态则隐藏，否则反之  
    {  
        ShowWindow(SW_HIDE);  
    }  
    else  
    {  
        ShowWindow(SW_SHOW);
		LoadConfig();
    }  
}


BOOL CFileUploadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return true;
		}
	}
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4 ) 	//屏蔽ALT+F4
		return TRUE;

	if((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(_T('A')) & 0x8000))
    {
		CEdit *editInterval = (CEdit *)GetDlgItem(IDC_EDIT_INTERVAL);
		CEdit *editBefore = (CEdit *)GetDlgItem(IDC_EDIT_BEFORE);
		CEdit *editPort = (CEdit *)GetDlgItem(IDC_EDIT_PORT);
		CEdit *editUsername = (CEdit *)GetDlgItem(IDC_EDIT_USERNAME);
		CEdit *editPasswd = (CEdit *)GetDlgItem(IDC_EDIT_PASSWD);
		CEdit *editNearpath = (CEdit *)GetDlgItem(IDC_EDIT_NEARPATH);
		CEdit *editRemotepath = (CEdit *)GetDlgItem(IDC_EDIT_REMOTEPATH);
		CEdit *editUpdatepath = (CEdit *)GetDlgItem(IDC_EDIT_UPDATEPATH);
		CString txt;
		int start, end;

		if(pMsg->hwnd == editInterval->m_hWnd)
		{         
			editInterval->GetWindowText(txt);
			editInterval->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editInterval->SetSel(-1);         // 取消全选
			}
			else
			{
				editInterval->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editBefore->m_hWnd)
		{
			editBefore->GetWindowText(txt);
			editBefore->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editBefore->SetSel(-1);         // 取消全选
			}
			else
			{
				editBefore->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editPort->m_hWnd)
		{
			editPort->GetWindowText(txt);
			editPort->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editPort->SetSel(-1);         // 取消全选
			}
			else
			{
				editPort->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editUsername->m_hWnd)
		{
			editUsername->GetWindowText(txt);
			editUsername->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editUsername->SetSel(-1);         // 取消全选
			}
			else
			{
				editUsername->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editPasswd->m_hWnd)
		{
			editPasswd->GetWindowText(txt);
			editPasswd->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editPasswd->SetSel(-1);         // 取消全选
			}
			else
			{
				editPasswd->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editNearpath->m_hWnd)
		{
			editNearpath->GetWindowText(txt);
			editNearpath->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editNearpath->SetSel(-1);         // 取消全选
			}
			else
			{
				editNearpath->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editRemotepath->m_hWnd)
		{
			editRemotepath->GetWindowText(txt);
			editRemotepath->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editRemotepath->SetSel(-1);         // 取消全选
			}
			else
			{
				editRemotepath->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editUpdatepath->m_hWnd)
		{
			editUpdatepath->GetWindowText(txt);
			editUpdatepath->GetSel(start, end);
			if(txt.GetLength() == end - start)  // 处于全选状态
			{
				editUpdatepath->SetSel(-1);         // 取消全选
			}
			else
			{
				editUpdatepath->SetSel(0, -1);          // 全选
			}
			return TRUE;
		}
    }

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CFileUploadDlg::SetAutoRun(BOOL bAutoRun)
{
	HKEY hKey;  
	CString strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";//找到系统的启动项  
	TCHAR szModule[_MAX_PATH];  
	GetModuleFileName(NULL, szModule, _MAX_PATH);//得到本程序自身的全路径  

	if (bAutoRun)  
	{   
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) //打开启动项       
		{  
			DWORD dwSize=255, dwType=REG_SZ;
			char strTemp[256];
			if (ERROR_SUCCESS != ::RegQueryValueEx(hKey, "FileUpload", 0, &dwType, (BYTE *)strTemp, &dwSize))
			{
				RegSetValueEx(hKey,"FileUpload", 0, REG_SZ, (const BYTE*)(LPCSTR)szModule, strlen(szModule)); 
				RegCloseKey(hKey);
			}
		}  
		else  
		{    
			AfxMessageBox("系统参数错误,不能随系统启动");
		}  
	}  
	else  
	{  
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)        
		{  
			RegDeleteValue (hKey, szModule);     
			RegCloseKey(hKey);  
		}  
	}  
}


void CFileUploadDlg::OnMenuUpdate(void)
{
	Update();
}

void CFileUploadDlg::LoadConfig(void)
{
	//设置与应用程序同目录下的config.ini为配置文件
	m_IniFile.SetFileName(m_exePath + "/config.ini");

	//获得config.ini中的“间隔分钟”并显示到控件中
	m_config.interval = m_IniFile.GetProfileInt("Config", "interval");
	CString strInterval;
	strInterval.Format("%d", m_config.interval);
	SETTEXT(IDC_EDIT_INTERVAL, strInterval);

	//获得config.ini中的“N分钟前”并显示到控件中
	m_config.before = m_IniFile.GetProfileInt("Config", "before");
	CString strBefore;
	strBefore.Format("%d", m_config.before);
	SETTEXT(IDC_EDIT_BEFORE, strBefore);

	//获得config.ini中的“上传协议”并显示到控件中
	/*switch(m_config.protocol = m_IniFile.GetProfileInt("Config", "protocol"))
	{
	case 0:
		((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->SetCurSel(0);
		break;
	case 1:
		((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->SetCurSel(1);
		break;
	default:
		break;
	}*/
	((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->SetCurSel(0);

	//获得config.ini中的“远端IP”并显示到控件中
	m_IniFile.GetProfileString("Config", "ip", m_config.ip);
	DWORD dwIP; 
	dwIP = inet_addr(m_config.ip); 
	unsigned char *pIP = (unsigned char*)&dwIP; 
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1))->SetAddress(*pIP,   *(pIP+1),   *(pIP+2),   *(pIP+3));

	//获得config.ini中的“远端端口”并显示到控件中
	m_config.port = m_IniFile.GetProfileInt("Config", "port");
	CString strPort;
	strPort.Format("%d", m_config.port);
	SETTEXT(IDC_EDIT_PORT, strPort);


	//获得config.ini中的“用户名”并显示到控件中
	m_IniFile.GetProfileString("Config", "username", m_config.username);
	SETTEXT(IDC_EDIT_USERNAME, m_config.username);

	//获得config.ini中的“密码”并显示到控件中
	m_IniFile.GetProfileString("Config", "passwd", m_config.passwd);
	SETTEXT(IDC_EDIT_PASSWD, m_config.passwd);
	
	//获得config.ini中的“近端路径”并显示到控件中
	m_IniFile.GetProfileString("Config", "nearpath", m_config.nearpath);
	SETTEXT(IDC_EDIT_NEARPATH, m_config.nearpath);

	//获得config.ini中的“远端路径”并显示到控件中
	m_IniFile.GetProfileString("Config", "remotepath", m_config.remotepath);
	SETTEXT(IDC_EDIT_REMOTEPATH, m_config.remotepath);
	
	//获得config.ini中的“更新路径”并显示到控件中
	m_IniFile.GetProfileString("Config", "updatepath", m_config.updatepath);
	SETTEXT(IDC_EDIT_UPDATEPATH, m_config.updatepath);

	//获得config.ini中的“版本信息”并显示到控件中
	m_IniFile.GetProfileString("Config", "version", m_config.version);
}


void CFileUploadDlg::OnBnClickedButton2()
{
	//设置与应用程序同目录下的config.ini为配置文件
	m_IniFile.SetFileName(m_exePath + "/config.ini");

	//获得控件中的“间隔分钟”并保存到config.ini中
	CString strInterval;
	GETTEXT(IDC_EDIT_INTERVAL, strInterval);
	m_IniFile.SetProfileInt("Config", "interval", atoi(strInterval));
	m_config.interval = atoi(strInterval);

	//获得控件中的“N分钟前”并保存到config.ini中
	CString strBefore;
	GETTEXT(IDC_EDIT_BEFORE, strBefore);
	m_IniFile.SetProfileInt("Config", "before", atoi(strBefore));
	m_config.before = atoi(strBefore);

	//获得控件中的“上传协议”并保存到config.ini中
	m_IniFile.SetProfileInt("Config", "protocol", ((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->GetCurSel());
	m_config.protocol = ((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->GetCurSel();

	//获得控件中的“远端IP”并保存到config.ini中
	unsigned char *pIP; 
	DWORD dwIP; 
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP); 
	pIP = (unsigned char*)&dwIP; 
	m_config.ip.Format("%u.%u.%u.%u", *(pIP+3), *(pIP+2), *(pIP+1), *pIP);
	m_IniFile.SetProfileString("Config", "ip", m_config.ip);

	//获得控件中的“远端端口”并保存到config.ini中
	CString strPort;
	GETTEXT(IDC_EDIT_PORT, strPort);
	m_IniFile.SetProfileInt("Config", "port", atoi(strPort));
	m_config.port = atoi(strPort);

	//获得控件中的“用户名”并保存到config.ini中
	GETTEXT(IDC_EDIT_USERNAME, m_config.username);
	m_IniFile.SetProfileString("Config", "username", m_config.username);

	//获得控件中的“密码”并保存到config.ini中
	GETTEXT(IDC_EDIT_PASSWD, m_config.passwd);
	m_IniFile.SetProfileString("Config", "passwd", m_config.passwd);

	//获得控件中的“近端路径”并保存到config.ini中
	GETTEXT(IDC_EDIT_NEARPATH, m_config.nearpath);
	m_IniFile.SetProfileString("Config", "nearpath", m_config.nearpath);

	//获得控件中的“远端路径”并保存到config.ini中
	GETTEXT(IDC_EDIT_REMOTEPATH, m_config.remotepath);
	m_IniFile.SetProfileString("Config", "remotepath", m_config.remotepath);

	//获得控件中的“更新路径”并保存到config.ini中
	GETTEXT(IDC_EDIT_UPDATEPATH, m_config.updatepath);
	m_IniFile.SetProfileString("Config", "updatepath", m_config.updatepath);

	//结束当前的计时器并根据当前的配置信息开启新的计时器
	KillTimer(1);
	if(m_config.interval < 1)
		m_config.interval = 1;
	SetTimer(1, m_config.interval * 60 * 1000, NULL);

	ShowWindow(SW_HIDE);
}

void CFileUploadDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(UploadFinished)
	{
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UploadFile, 0, 0, NULL);
		CloseHandle(hThread);
		CDialogEx::OnTimer(nIDEvent);
	}
}

UINT CFileUploadDlg::UploadFile(LPVOID)
{	
	if(!ConnectRemote())
	{
		AfxMessageBox("无法连接到服务器，请检查网络是否正常，并打开配置界面检查信息是否有误，或者服务器没有开启相关服务");
		return 1;
	}

	//检查服务器路径是否正确
	if(!m_pFtpConnection->SetCurrentDirectory(m_config.remotepath))
	{
		AfxMessageBox("服务器存储路径填写有误");
		DisConnect();
		return 1;
	}

	//在本地路径下遍历文件
	if(!::SetCurrentDirectory(m_config.nearpath))
    {  
		AfxMessageBox("本地路径填写有误");
		DisConnect();
		return 1;
	}
	CFileFind finder;
	BOOL bWorking = finder.FindFile("*.*");
	//FILETIME fileTime;	//文件的文件时间
	//SYSTEMTIME fileSysTime;	//文件的系统时间
	//SYSTEMTIME curSysTime;	//当前的系统时间

	//此次上传线程没有完成
	UploadFinished = FALSE;

	while(bWorking)
	{  
		bWorking = finder.FindNextFile();
		if(finder.IsDots() || finder.IsDirectory())
		{
			continue;
		}
		//finder.GetCreationTime(&fileTime);	//得到文件进入当前目录的文件时间
		//FileTimeToSystemTime(&fileTime, &fileSysTime);	//把文件时间转换为系统时间
		//GetSystemTime(&curSysTime);	//得到当前系统时间

		////计算文件进入时间与“N分钟前”进行比较
		//int difMonth = (curSysTime.wMonth - fileSysTime.wMonth) * 30 * 24 * 60;
		//int difDay = (curSysTime.wDay - fileSysTime.wDay) * 24 * 60;
		//int difHour = (curSysTime.wHour - fileSysTime.wHour) * 60;
		//int difMinute = curSysTime.wMinute - fileSysTime.wMinute;
		//int dif = difMonth + difDay + difHour + difMinute;

		CTime fileTime, currentTime;
		finder.GetLastWriteTime(fileTime);
		currentTime = CTime::GetCurrentTime();
		CTimeSpan span = currentTime - fileTime;
		long dif = span.GetTotalMinutes();
		//如果大于“N分钟前”则进行上传
		if(dif > m_config.before)	
		{
			/*CString nearpath = finder.GetFilePath();
			CString remotepath = m_config.remotepath + "/" + finder.GetFileName();*/
			CString fileName = finder.GetFileName();
			if(!m_pFtpConnection->PutFile(fileName, fileName))
			{
				/*CHAR szBuf[80];
				DWORD dw = GetLastError();
				sprintf(szBuf, "GetLastError returned %u\n", dw);
				::MessageBox(NULL, szBuf, "Error", MB_OK);*/

				CFile::Rename(fileName, "X" + fileName + "X");

				DisConnect();
				if(!ConnectRemote())
				{
					AfxMessageBox("无法连接到服务器，请检查网络是否正常，并打开配置界面检查信息是否有误，或者服务器没有开启相关服务");

					//此次上传线程完成了
					UploadFinished = TRUE;

					return 1;
				}
				//检查服务器路径是否正确
				if(!m_pFtpConnection->SetCurrentDirectory(m_config.remotepath))
				{
					AfxMessageBox("服务器存储路径填写有误");
					DisConnect();
					//此次上传线程完成了
					UploadFinished = TRUE;
					return 1;
				}

				//设置当前路径
				if(!::SetCurrentDirectory(m_config.nearpath))
				{  
					AfxMessageBox("本地路径填写有误");
					DisConnect();
					//此次上传线程完成了
					UploadFinished = TRUE;
					return 1;
				}
			}
			else
			{
				::DeleteFile(fileName);

				//m_pFtpConnection->CreateDirectory("../" + m_config.remotepath + "-time");

				//time_t curtime = time(NULL);
				//tm *ptm = localtime(&curtime);
				//char buf2[64] = {0};
				//sprintf(buf2, "%d%02d%02d%02d%02d%02d", ptm->tm_year+1900, ptm->tm_mon+1,
				//ptm->tm_mday, ptm->tm_hour, ptm->tm_min/*, ptm->tm_sec*/);

				//std::ofstream out;
				//out.open(fileName, std::ios::binary);
				//out.write(buf2, 12);
				//out.close();

				//if(!m_pFtpConnection->PutFile(fileName, "../" + m_config.remotepath +  "-time/" + fileName))
				//{
				//	//AfxMessageBox(fileName + "上传失败");
				//}
				//::DeleteFile(fileName);
			}
		}
	}

	//此次上传线程完成了
	UploadFinished = TRUE;
	
	DisConnect();
	return 0;
}

BOOL CFileUploadDlg::ConnectRemote(void)
{
	m_pInetSession = new CInternetSession(NULL, 1, 
		PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
	try
	{
		m_pFtpConnection = m_pInetSession->GetFtpConnection((LPCTSTR)m_config.ip, 
			(LPCTSTR)m_config.username, (LPCTSTR)m_config.passwd, m_config.port, TRUE);
	}

	catch(CInternetException *pEx){
		pEx->Delete();
		m_pInetSession->Close();
		delete m_pInetSession;
		return FALSE;
	}

	return TRUE;
}


void CFileUploadDlg::DisConnect(void)
{
	m_pFtpConnection->Close();
	m_pInetSession->Close();
	delete m_pFtpConnection;
	delete m_pInetSession;
}

UINT CFileUploadDlg::Update(void)
{

	DWORD flags;
	while(!InternetGetConnectedState(&flags,0)) //检测连接状态
	{
		Sleep(5 * 60 * 1000);
	}
	ShellExecute(NULL, "open", m_exePath + "/Update.exe", NULL, NULL, SW_SHOWNORMAL);
	return 0;
}

void CFileUploadDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO stInfo = {NULL};

	LPCITEMIDLIST pIdlst;
	TCHAR szPath[MAX_PATH];
	//stInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	stInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS; 
	stInfo.lpszTitle = "请选择路径:";
	pIdlst = SHBrowseForFolder(&stInfo);
	if(!pIdlst) return ;
	if(!SHGetPathFromIDList(pIdlst, szPath)) return ;

	SETTEXT(IDC_EDIT_NEARPATH, szPath);
}


void CFileUploadDlg::OnEnKillfocusEditInterval()
{
	// TODO: 在此添加控件通知处理程序代码
	CString interval;
	GETTEXT(IDC_EDIT_INTERVAL, interval);
	if(atoi(interval) < 1)
	{
		SETTEXT(IDC_EDIT_INTERVAL, "1");
	}
	else if(atoi(interval) > 1200)
	{
		SETTEXT(IDC_EDIT_INTERVAL, "1200");
	}
}


void CFileUploadDlg::OnEnKillfocusEditBefore()
{
	// TODO: 在此添加控件通知处理程序代码
	CString before;
	GETTEXT(IDC_EDIT_BEFORE, before);
	if(atoi(before) < 10)
	{
		SETTEXT(IDC_EDIT_BEFORE, "10");
	}
	else if(atoi(before) > 1200)
	{
		SETTEXT(IDC_EDIT_BEFORE, "1200");
	}
}


void CFileUploadDlg::GetExePath(CString &path)
{
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
	path.ReleaseBuffer(); 
	int pos = path.ReverseFind('\\'); 
	path = path.Left(pos); 
	path.Replace('\\', '/');
}


void CFileUploadDlg::OnEnKillfocusEditNearpath()
{
	// TODO: 在此添加控件通知处理程序代码
	CString path;
	GETTEXT(IDC_EDIT_NEARPATH, path);
	if(!SetCurrentDirectory(path))
	{
		MessageBox("请检查本地路径是否填写有误");
	}
}
