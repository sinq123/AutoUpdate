
// AgentDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include <vector>
#include <map>
using namespace std;

#define  WM_UPDATE_END			WM_USER+120	

// CAgentDlg �Ի���
class CAgentDlg : public CDialogEx
{
// ����
public:
	CAgentDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AGENT_DIALOG };

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
	map<CString,CString> m_mapFile;					//������Ҫ���µ��ļ���Ϣ
	CListCtrl m_listFile;

	afx_msg LRESULT OnUpDateEndMsg(WPARAM wParam, LPARAM lParam);

	static UINT ThreadUpdate(LPVOID pParam);		//�Զ������߳�
	void UpdateThread();							//�Զ������߼�
	void GetUpdateFile();							//��ȡ��Ҫ���µ��ļ���Ϣ
	BOOL CheckFileMd5(CString &strSavePath,CString &strUrl);	//�Ա��ļ���md5
	CString * SplitString(CString str, char split, int& iSubStrs);		//�Թ̶��ָ����ָ�CString�ַ���
	LRESULT DownloadFile(UINT nNumber,CString &strSavePath,CString &filemd);	
};
