
// UpdateDlg.h : 头文件
//

#pragma once

#include "afxinet.h"
#include "IniFile.h"

// CUpdateDlg 对话框
class CUpdateDlg : public CDialogEx
{
// 构造
public:
	CUpdateDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UPDATE_DIALOG };

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
	static CInternetSession *m_pInetSession;
	static CFtpConnection *m_pFtpConnection;
	void CUpdateDlg::Update();
private:
	CIniFile m_IniFile;
public:
	BOOL KillProcessFromName(CString strProcessName);
	static void GetExePath(CString &path);
};
