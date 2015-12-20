
// FileUploadDlg.h : ͷ�ļ�
//

#pragma once
#include "Inifile.h"
#include "afxinet.h"
#include "Config.h"

// CFileUploadDlg �Ի���
class CFileUploadDlg : public CDialogEx
{
// ����
public:
	CFileUploadDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILEUPLOAD_DIALOG };

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
	void InitNotifyIcon(void);	//����ϵͳ����ͼ��
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);	//����Ҽ������˵�ѡ��
	void OnMenuExit(void);	//����˳�ѡ��Ĵ�����
	void OnMenuShow(void);  //�����ʾ/����ѡ�Ĵ�����
	virtual BOOL PreTranslateMessage(MSG* pMsg);	//����ESC,RETURN,ALT+F4����
	void SetAutoRun(BOOL bAutoRun);	//���ÿ���������
	void OnMenuUpdate(void);	//���������ѡ����Ϣӳ�亯��
	void LoadConfig(void);	//����config.ini�е���Ϣ�����ý����ϵĸ����ؼ���
	static UINT Update(void);	//�ӷ����������ظ���

	static UINT UploadFile(LPVOID);	//�ϴ��ļ����߳�
	afx_msg void OnBnClickedButton2();	//������ύ����ť�Ĵ�����������Ѹ��������е�ֵд�뵽config.ini��
	afx_msg void OnTimer(UINT_PTR nIDEvent);	//��ʱ����Ӧ����
	
	NOTIFYICONDATA m_NotifyIcon;	//����ͼ��ṹ
	static CInternetSession *m_pInetSession;	//����Ựָ��
	static CFtpConnection *m_pFtpConnection;	//FTP����ָ��
	static CIniFile m_IniFile;	//����config.ini�ļ����������Ҫ������ú�����config.ini�еĸ�����ֵ
	static Config m_config;	//��¼config.ini�и����Ե�ֵ�������
	static BOOL ConnectRemote(void);	//����FTP����
	static void DisConnect(void);	//�Ͽ�FTP����
	static CString m_exePath;	//��ǰ�������ڵ�·��
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnKillfocusEditInterval();
	afx_msg void OnEnKillfocusEditBefore();
	static void GetExePath(CString &path);
	afx_msg void OnEnKillfocusEditNearpath();

	static BOOL UploadFinished;
};
