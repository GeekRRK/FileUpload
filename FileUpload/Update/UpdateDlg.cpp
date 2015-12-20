
// UpdateDlg.cpp : ʵ���ļ�
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


// CUpdateDlg �Ի���




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


// CUpdateDlg ��Ϣ�������

BOOL CUpdateDlg::OnInitDialog()
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
	Update();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUpdateDlg::OnPaint()
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
HCURSOR CUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpdateDlg::Update()
{
	
	//��ȡconfig.ini�е����ϵͳ����FTP�����������°汾FileUpload.exe
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
	//����½���ɹ����׳��쳣������������쳣�Ĵ���
	catch(CInternetException *pEx){
		/*TCHAR szError[1024] = {0};
		if(pEx->GetErrorMessage(szError,1024))
			AfxMessageBox(szError);
		else
			AfxMessageBox(_T("There was an exception"));*/
		MessageBox("�޷����ӵ������������������Ƿ��������������ý�������Ϣ�Ƿ����󣬻��߷�����û�п�����ط���");
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

	//����FTP���Ӳ��������°汾�����Ϣ
	if(!m_pFtpConnection->GetFile(strUpdatepath + "/update.ini", path + "/update.ini"), FALSE)
	{
		//MessageBox("update.ini����ʧ�ܣ�������ý��������������·���Ƿ����󣬻���Ȩ�޲���");
	}
	else
	{
		//������Ӧ�ó���ͬĿ¼�µ�update.iniΪ�����ļ�����ø���·�������°汾��Ϣ
		m_IniFile.SetFileName(path + "/update.ini");
		CString newVersion;
		m_IniFile.GetProfileString("Update", "version", newVersion);
		DeleteFile(path + "/update.ini");
		
		//��ǰ�汾�����°汾��Ϣ���бȽϣ������һ�¾ͽ��и��£��������°汾������Ӧ�ó���ǰ·����Ϊ�������ݹ�ȥ
		if(newVersion != oldVersion)
		{
			m_IniFile.SetFileName(path + "/config.ini");

			KillProcessFromName("FileUpload.exe");
			CFile::Rename(path + "/FileUpload.exe", path + "/FileUploadX.exe");
			
			if(!m_pFtpConnection->GetFile(strUpdatepath + "/FileUpload.exe", path + "/FileUpload.exe"), FALSE)
			{
				MessageBox("����ʧ�ܣ�������ý��������·���Ƿ�����");
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
	//�������̿���(TH32CS_SNAPPROCESS��ʾ�������н��̵Ŀ���)  
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
  
    //PROCESSENTRY32���̿��յĽṹ��  
    PROCESSENTRY32 pe;  
  
    //ʵ������ʹ��Process32First��ȡ��һ�����յĽ���ǰ�����ĳ�ʼ������  
    pe.dwSize = sizeof(PROCESSENTRY32);  
  
  
    //�����IFЧ��ͬ:  
    //if(hProcessSnap == INVALID_HANDLE_VALUE)   ��Ч�ľ��  
    if(!Process32First(hSnapShot,&pe))  
    {  
        return FALSE;  
    }  
  
    //���ַ���ת��ΪСд  
    strProcessName.MakeLower();  
  
    //��������Ч  ��һֱ��ȡ��һ�����ѭ����ȥ  
    while (Process32Next(hSnapShot,&pe))  
    {  
  
        //pe.szExeFile��ȡ��ǰ���̵Ŀ�ִ���ļ�����  
        CString scTmp = pe.szExeFile;  
  
  
        //����ִ���ļ���������Ӣ����ĸ�޸�ΪСд  
        scTmp.MakeLower();  
  
        //�Ƚϵ�ǰ���̵Ŀ�ִ���ļ����ƺʹ��ݽ������ļ������Ƿ���ͬ  
        //��ͬ�Ļ�Compare����0  
        if(!scTmp.Compare(strProcessName))  
        {  
  
            //�ӿ��ս����л�ȡ�ý��̵�PID(������������е�PID)  
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
