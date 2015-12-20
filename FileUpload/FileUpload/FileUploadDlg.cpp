
// FileUploadDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CFileUploadDlg �Ի���




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


// CFileUploadDlg ��Ϣ�������

BOOL CFileUploadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->AddString("FTP");	//�������б��ѡ��
	//((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->AddString("SFTP");

	((CEdit *)GetDlgItem(IDC_EDIT_PASSWD))->SetPasswordChar('*');	//����������������ʾΪ*
	InitNotifyIcon();	//����ϵͳ����ͼ��
	ModifyStyle(WS_THICKFRAME, 0, SWP_NOSIZE);	//�̶��Ի���Ĵ�С

	GetExePath(m_exePath); //��ȡ.exe���г������ڵ�Ŀ¼

	SetAutoRun(TRUE);	//����������
	LoadConfig();	//����config.ini
	
	/*if(!strcmp(__argv[1], "ONCE"))*/
	CString argv1 = __argv[1];
	if(argv1 != "ONCE")	//������Ǳ�Update.exe�����ľ͸���
	{
		//�����߳�Ϊ�˼�������Ƿ���ͨ�������ͨ���������³���
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Update, 0, 0, NULL);
		CloseHandle(hThread);
	}

	//��顰������ӡ��Ƿ�̫С�����̫С���������������ϻ᲻�ϳ���ʾ�����������
	if(m_config.interval < 1)
		m_config.interval = 1;
	SetTimer(1, m_config.interval * 60 * 1000, NULL);	//���ݼ��ص�interval���ö�ʱ��
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileUploadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileUploadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileUploadDlg::InitNotifyIcon(void)
{
	m_NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_NotifyIcon.hWnd = this->GetSafeHwnd();
	strcpy(m_NotifyIcon.szTip, "�ļ��ϴ�����");//��ʾ����
	m_NotifyIcon.uCallbackMessage = WM_ICONMESSAGE;//�ص���Ϣ
	m_NotifyIcon.uFlags = NIF_MESSAGE|NIF_TIP|NIF_ICON;//������־
	m_NotifyIcon.hIcon = m_hIcon;//ͼ����
	m_NotifyIcon.uID = IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_ADD,&m_NotifyIcon);//Ϊϵͳ�������Ӵ�ͼ��
}


LRESULT CFileUploadDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch(message)  
	{  
	case WM_ICONMESSAGE:  
		if(lParam == WM_RBUTTONDOWN)  
		{  
			CMenu menu;  
			CPoint point;  
			GetCursorPos(&point);  
			menu.CreatePopupMenu();  

			menu.AppendMenu(MF_STRING, ID_MENU_SHOW, _T("��ʾ/����"));  
			menu.AppendMenu(MF_STRING, ID_MENU_UPDATE, _T("������"));  
			menu.AppendMenu(MF_STRING, ID_MENU_EXIT, _T("�˳�����"));

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
	SendMessage(WM_CLOSE, 0, 0);//���͹رճ�����Ϣ  
}


void CFileUploadDlg::OnMenuShow(void)
{
	if(IsWindowVisible())//����ǰ����Ϊ��ʾ״̬�����أ�����֮  
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
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return true;
		}
	}
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4 ) 	//����ALT+F4
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
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editInterval->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editInterval->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editBefore->m_hWnd)
		{
			editBefore->GetWindowText(txt);
			editBefore->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editBefore->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editBefore->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editPort->m_hWnd)
		{
			editPort->GetWindowText(txt);
			editPort->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editPort->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editPort->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editUsername->m_hWnd)
		{
			editUsername->GetWindowText(txt);
			editUsername->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editUsername->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editUsername->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editPasswd->m_hWnd)
		{
			editPasswd->GetWindowText(txt);
			editPasswd->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editPasswd->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editPasswd->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editNearpath->m_hWnd)
		{
			editNearpath->GetWindowText(txt);
			editNearpath->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editNearpath->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editNearpath->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editRemotepath->m_hWnd)
		{
			editRemotepath->GetWindowText(txt);
			editRemotepath->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editRemotepath->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editRemotepath->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
		else if(pMsg->hwnd == editUpdatepath->m_hWnd)
		{
			editUpdatepath->GetWindowText(txt);
			editUpdatepath->GetSel(start, end);
			if(txt.GetLength() == end - start)  // ����ȫѡ״̬
			{
				editUpdatepath->SetSel(-1);         // ȡ��ȫѡ
			}
			else
			{
				editUpdatepath->SetSel(0, -1);          // ȫѡ
			}
			return TRUE;
		}
    }

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CFileUploadDlg::SetAutoRun(BOOL bAutoRun)
{
	HKEY hKey;  
	CString strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";//�ҵ�ϵͳ��������  
	TCHAR szModule[_MAX_PATH];  
	GetModuleFileName(NULL, szModule, _MAX_PATH);//�õ������������ȫ·��  

	if (bAutoRun)  
	{   
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) //��������       
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
			AfxMessageBox("ϵͳ��������,������ϵͳ����");
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
	//������Ӧ�ó���ͬĿ¼�µ�config.iniΪ�����ļ�
	m_IniFile.SetFileName(m_exePath + "/config.ini");

	//���config.ini�еġ�������ӡ�����ʾ���ؼ���
	m_config.interval = m_IniFile.GetProfileInt("Config", "interval");
	CString strInterval;
	strInterval.Format("%d", m_config.interval);
	SETTEXT(IDC_EDIT_INTERVAL, strInterval);

	//���config.ini�еġ�N����ǰ������ʾ���ؼ���
	m_config.before = m_IniFile.GetProfileInt("Config", "before");
	CString strBefore;
	strBefore.Format("%d", m_config.before);
	SETTEXT(IDC_EDIT_BEFORE, strBefore);

	//���config.ini�еġ��ϴ�Э�顱����ʾ���ؼ���
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

	//���config.ini�еġ�Զ��IP������ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "ip", m_config.ip);
	DWORD dwIP; 
	dwIP = inet_addr(m_config.ip); 
	unsigned char *pIP = (unsigned char*)&dwIP; 
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1))->SetAddress(*pIP,   *(pIP+1),   *(pIP+2),   *(pIP+3));

	//���config.ini�еġ�Զ�˶˿ڡ�����ʾ���ؼ���
	m_config.port = m_IniFile.GetProfileInt("Config", "port");
	CString strPort;
	strPort.Format("%d", m_config.port);
	SETTEXT(IDC_EDIT_PORT, strPort);


	//���config.ini�еġ��û���������ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "username", m_config.username);
	SETTEXT(IDC_EDIT_USERNAME, m_config.username);

	//���config.ini�еġ����롱����ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "passwd", m_config.passwd);
	SETTEXT(IDC_EDIT_PASSWD, m_config.passwd);
	
	//���config.ini�еġ�����·��������ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "nearpath", m_config.nearpath);
	SETTEXT(IDC_EDIT_NEARPATH, m_config.nearpath);

	//���config.ini�еġ�Զ��·��������ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "remotepath", m_config.remotepath);
	SETTEXT(IDC_EDIT_REMOTEPATH, m_config.remotepath);
	
	//���config.ini�еġ�����·��������ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "updatepath", m_config.updatepath);
	SETTEXT(IDC_EDIT_UPDATEPATH, m_config.updatepath);

	//���config.ini�еġ��汾��Ϣ������ʾ���ؼ���
	m_IniFile.GetProfileString("Config", "version", m_config.version);
}


void CFileUploadDlg::OnBnClickedButton2()
{
	//������Ӧ�ó���ͬĿ¼�µ�config.iniΪ�����ļ�
	m_IniFile.SetFileName(m_exePath + "/config.ini");

	//��ÿؼ��еġ�������ӡ������浽config.ini��
	CString strInterval;
	GETTEXT(IDC_EDIT_INTERVAL, strInterval);
	m_IniFile.SetProfileInt("Config", "interval", atoi(strInterval));
	m_config.interval = atoi(strInterval);

	//��ÿؼ��еġ�N����ǰ�������浽config.ini��
	CString strBefore;
	GETTEXT(IDC_EDIT_BEFORE, strBefore);
	m_IniFile.SetProfileInt("Config", "before", atoi(strBefore));
	m_config.before = atoi(strBefore);

	//��ÿؼ��еġ��ϴ�Э�顱�����浽config.ini��
	m_IniFile.SetProfileInt("Config", "protocol", ((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->GetCurSel());
	m_config.protocol = ((CComboBox *)GetDlgItem(IDC_COMBO_PROTOCOL))->GetCurSel();

	//��ÿؼ��еġ�Զ��IP�������浽config.ini��
	unsigned char *pIP; 
	DWORD dwIP; 
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP); 
	pIP = (unsigned char*)&dwIP; 
	m_config.ip.Format("%u.%u.%u.%u", *(pIP+3), *(pIP+2), *(pIP+1), *pIP);
	m_IniFile.SetProfileString("Config", "ip", m_config.ip);

	//��ÿؼ��еġ�Զ�˶˿ڡ������浽config.ini��
	CString strPort;
	GETTEXT(IDC_EDIT_PORT, strPort);
	m_IniFile.SetProfileInt("Config", "port", atoi(strPort));
	m_config.port = atoi(strPort);

	//��ÿؼ��еġ��û����������浽config.ini��
	GETTEXT(IDC_EDIT_USERNAME, m_config.username);
	m_IniFile.SetProfileString("Config", "username", m_config.username);

	//��ÿؼ��еġ����롱�����浽config.ini��
	GETTEXT(IDC_EDIT_PASSWD, m_config.passwd);
	m_IniFile.SetProfileString("Config", "passwd", m_config.passwd);

	//��ÿؼ��еġ�����·���������浽config.ini��
	GETTEXT(IDC_EDIT_NEARPATH, m_config.nearpath);
	m_IniFile.SetProfileString("Config", "nearpath", m_config.nearpath);

	//��ÿؼ��еġ�Զ��·���������浽config.ini��
	GETTEXT(IDC_EDIT_REMOTEPATH, m_config.remotepath);
	m_IniFile.SetProfileString("Config", "remotepath", m_config.remotepath);

	//��ÿؼ��еġ�����·���������浽config.ini��
	GETTEXT(IDC_EDIT_UPDATEPATH, m_config.updatepath);
	m_IniFile.SetProfileString("Config", "updatepath", m_config.updatepath);

	//������ǰ�ļ�ʱ�������ݵ�ǰ��������Ϣ�����µļ�ʱ��
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
		AfxMessageBox("�޷����ӵ������������������Ƿ��������������ý�������Ϣ�Ƿ����󣬻��߷�����û�п�����ط���");
		return 1;
	}

	//��������·���Ƿ���ȷ
	if(!m_pFtpConnection->SetCurrentDirectory(m_config.remotepath))
	{
		AfxMessageBox("�������洢·����д����");
		DisConnect();
		return 1;
	}

	//�ڱ���·���±����ļ�
	if(!::SetCurrentDirectory(m_config.nearpath))
    {  
		AfxMessageBox("����·����д����");
		DisConnect();
		return 1;
	}
	CFileFind finder;
	BOOL bWorking = finder.FindFile("*.*");
	//FILETIME fileTime;	//�ļ����ļ�ʱ��
	//SYSTEMTIME fileSysTime;	//�ļ���ϵͳʱ��
	//SYSTEMTIME curSysTime;	//��ǰ��ϵͳʱ��

	//�˴��ϴ��߳�û�����
	UploadFinished = FALSE;

	while(bWorking)
	{  
		bWorking = finder.FindNextFile();
		if(finder.IsDots() || finder.IsDirectory())
		{
			continue;
		}
		//finder.GetCreationTime(&fileTime);	//�õ��ļ����뵱ǰĿ¼���ļ�ʱ��
		//FileTimeToSystemTime(&fileTime, &fileSysTime);	//���ļ�ʱ��ת��Ϊϵͳʱ��
		//GetSystemTime(&curSysTime);	//�õ���ǰϵͳʱ��

		////�����ļ�����ʱ���롰N����ǰ�����бȽ�
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
		//������ڡ�N����ǰ��������ϴ�
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
					AfxMessageBox("�޷����ӵ������������������Ƿ��������������ý�������Ϣ�Ƿ����󣬻��߷�����û�п�����ط���");

					//�˴��ϴ��߳������
					UploadFinished = TRUE;

					return 1;
				}
				//��������·���Ƿ���ȷ
				if(!m_pFtpConnection->SetCurrentDirectory(m_config.remotepath))
				{
					AfxMessageBox("�������洢·����д����");
					DisConnect();
					//�˴��ϴ��߳������
					UploadFinished = TRUE;
					return 1;
				}

				//���õ�ǰ·��
				if(!::SetCurrentDirectory(m_config.nearpath))
				{  
					AfxMessageBox("����·����д����");
					DisConnect();
					//�˴��ϴ��߳������
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
				//	//AfxMessageBox(fileName + "�ϴ�ʧ��");
				//}
				//::DeleteFile(fileName);
			}
		}
	}

	//�˴��ϴ��߳������
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
	while(!InternetGetConnectedState(&flags,0)) //�������״̬
	{
		Sleep(5 * 60 * 1000);
	}
	ShellExecute(NULL, "open", m_exePath + "/Update.exe", NULL, NULL, SW_SHOWNORMAL);
	return 0;
}

void CFileUploadDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO stInfo = {NULL};

	LPCITEMIDLIST pIdlst;
	TCHAR szPath[MAX_PATH];
	//stInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	stInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS; 
	stInfo.lpszTitle = "��ѡ��·��:";
	pIdlst = SHBrowseForFolder(&stInfo);
	if(!pIdlst) return ;
	if(!SHGetPathFromIDList(pIdlst, szPath)) return ;

	SETTEXT(IDC_EDIT_NEARPATH, szPath);
}


void CFileUploadDlg::OnEnKillfocusEditInterval()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString path;
	GETTEXT(IDC_EDIT_NEARPATH, path);
	if(!SetCurrentDirectory(path))
	{
		MessageBox("���鱾��·���Ƿ���д����");
	}
}
