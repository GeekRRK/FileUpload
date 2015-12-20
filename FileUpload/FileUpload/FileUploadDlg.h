
// FileUploadDlg.h : 头文件
//

#pragma once
#include "Inifile.h"
#include "afxinet.h"
#include "Config.h"

// CFileUploadDlg 对话框
class CFileUploadDlg : public CDialogEx
{
// 构造
public:
	CFileUploadDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILEUPLOAD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void InitNotifyIcon(void);	//创建系统托盘图标
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);	//添加右键弹出菜单选项
	void OnMenuExit(void);	//点击退出选项的处理函数
	void OnMenuShow(void);  //点击显示/隐藏选的处理函数
	virtual BOOL PreTranslateMessage(MSG* pMsg);	//屏蔽ESC,RETURN,ALT+F4按键
	void SetAutoRun(BOOL bAutoRun);	//设置开机自启动
	void OnMenuUpdate(void);	//点击检查更新选项消息映射函数
	void LoadConfig(void);	//加载config.ini中的信息到配置界面上的各个控件中
	static UINT Update(void);	//从服务器上下载更新

	static UINT UploadFile(LPVOID);	//上传文件的线程
	afx_msg void OnBnClickedButton2();	//点击“提交”按钮的处理函数，负责把各个换件中的值写入到config.ini中
	afx_msg void OnTimer(UINT_PTR nIDEvent);	//定时器响应函数
	
	NOTIFYICONDATA m_NotifyIcon;	//托盘图标结构
	static CInternetSession *m_pInetSession;	//网络会话指针
	static CFtpConnection *m_pFtpConnection;	//FTP连接指针
	static CIniFile m_IniFile;	//操作config.ini文件的类对象，主要用来获得和设置config.ini中的各属性值
	static Config m_config;	//记录config.ini中各属性的值的类对象
	static BOOL ConnectRemote(void);	//建立FTP连接
	static void DisConnect(void);	//断开FTP连接
	static CString m_exePath;	//当前程序所在的路径
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnKillfocusEditInterval();
	afx_msg void OnEnKillfocusEditBefore();
	static void GetExePath(CString &path);
	afx_msg void OnEnKillfocusEditNearpath();

	static BOOL UploadFinished;
};
