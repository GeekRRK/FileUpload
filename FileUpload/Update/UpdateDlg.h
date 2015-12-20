
// UpdateDlg.h : ͷ�ļ�
//

#pragma once

#include "afxinet.h"
#include "IniFile.h"

// CUpdateDlg �Ի���
class CUpdateDlg : public CDialogEx
{
// ����
public:
	CUpdateDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
