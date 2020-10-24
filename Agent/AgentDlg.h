
// AgentDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include <vector>
#include <map>
using namespace std;

#define  WM_UPDATE_END			WM_USER+120	

// CAgentDlg 对话框
class CAgentDlg : public CDialogEx
{
// 构造
public:
	CAgentDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AGENT_DIALOG };

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
	map<CString,CString> m_mapFile;					//保存需要更新的文件信息
	CListCtrl m_listFile;

	afx_msg LRESULT OnUpDateEndMsg(WPARAM wParam, LPARAM lParam);

	static UINT ThreadUpdate(LPVOID pParam);		//自动更新线程
	void UpdateThread();							//自动更新逻辑
	void GetUpdateFile();							//获取需要更新的文件信息
	BOOL CheckFileMd5(CString &strSavePath,CString &strUrl);	//对比文件的md5
	CString * SplitString(CString str, char split, int& iSubStrs);		//以固定分隔符分割CString字符串
	LRESULT DownloadFile(UINT nNumber,CString &strSavePath,CString &filemd);	
};
