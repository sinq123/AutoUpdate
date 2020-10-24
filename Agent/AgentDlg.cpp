
// AgentDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Agent.h"
#include "AgentDlg.h"
#include "afxdialogex.h"

#include <WinBase.h>
#include <afxinet.h>
#include "io.h"
#include "md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CAgentDlg 对话框




CAgentDlg::CAgentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAgentDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_listFile);
}

BEGIN_MESSAGE_MAP(CAgentDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_END,OnUpDateEndMsg)				//自动更新消息处理
END_MESSAGE_MAP()


// CAgentDlg 消息处理程序

BOOL CAgentDlg::OnInitDialog()
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
	m_listFile.InsertColumn(0,"文件名",LVCFMT_LEFT,100,0);
	m_listFile.InsertColumn(1,"大小",LVCFMT_LEFT,90,0);
	m_listFile.InsertColumn(2,"进度",LVCFMT_LEFT,100,0);
	m_listFile.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT
		|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_GRIDLINES);

	AfxBeginThread(ThreadUpdate,this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAgentDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAgentDlg::OnPaint()
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
HCURSOR CAgentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CAgentDlg::ThreadUpdate(LPVOID pParam)
{
	CAgentDlg* pDlg = (CAgentDlg*) pParam;
	ASSERT(pDlg);
	//ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CUpdaterDlg)));
	pDlg->UpdateThread();
	return 0;
}

void CAgentDlg::UpdateThread()
{
	BOOL bIsSuccess = TRUE;

	//首先是获取需要更新的文件信息列表
	GetUpdateFile();
	
	//刷新列表框
	m_listFile.DeleteAllItems();
	CString strUrl,strName,strTemp,strMd;
	for (map<CString,CString>::iterator beg = m_mapFile.begin();
		beg != m_mapFile.end();beg++)
	{
		strName = beg->first;
		m_listFile.InsertItem(m_listFile.GetItemCount(),"");
		m_listFile.SetItemText(m_listFile.GetItemCount()-1,0,strName);
		m_listFile.SetItemText(m_listFile.GetItemCount()-1,1,"0 kb");
		m_listFile.SetItemText(m_listFile.GetItemCount()-1,2,"已下载 0 %");
	}

	//下载文件
	UINT nNumber = 0;
	for (map<CString,CString>::iterator beg = m_mapFile.begin();
		beg != m_mapFile.end();beg++)
	{
		strName = beg->first;
		strTemp = beg->second;

		int iPos = 0; //分割符位置 
		iPos = strTemp.Find(',');  
		strUrl = strTemp.Left(iPos); 
		strMd = strTemp.Mid(iPos + 1, strTemp.GetLength());  
		
		//对比文件的MD5
		if(!CheckFileMd5(strName,strMd)){
			if (DownloadFile(nNumber,strName,strUrl) != 0){
				m_listFile.SetItemText(nNumber,2,"下载失败");
				bIsSuccess = FALSE;
			}
		}else{
			m_listFile.SetItemText(nNumber,2,"已经最新");
		}
		nNumber++;
	}
	//所有文件下载成功
	if (bIsSuccess){
		::PostMessage(m_hWnd,WM_UPDATE_END,0,0);	//发送消息 进行自动退出并打开主程序
	}
}

void CAgentDlg::GetUpdateFile()
{
	CString strServerIniName,strControlName,strKeyName;
	CString str1,str2,str3;
	int nCount = 0;

	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// 向上一层
	for (int i=(int)(strlen(pBuf)-1); i>=0; i--)
	{
		if ('\\' != pBuf[i])
		{
			pBuf[i] = '\0';
		}
		else
		{
			pBuf[i] = '\0';
			break;
		}
	}
	strcat_s(pBuf,"\\AgentVersionInfo.ini");
	strServerIniName= pBuf;					//服务器下载的配置文件路径名

	if (_access(strServerIniName,0) == -1){
		AfxMessageBox("AgentVersionInfo.ini文件不存在！");
		return ;
	}

	//获取新的文件总数
	nCount = ::GetPrivateProfileIntA("FILECOUNT","filecount",0,strServerIniName);

	for (int i=1;i<nCount+1;i++)
	{
		strKeyName.Format("filename%d",i);
		::GetPrivateProfileStringA("FILENAME",strKeyName,NULL,
			strControlName.GetBuffer(MAX_PATH),MAX_PATH,strServerIniName);
		strControlName.ReleaseBuffer();
		str1 = strControlName;

		strKeyName.Format("fileurl%d",i);
		::GetPrivateProfileString("FILEURL",strKeyName,NULL,
			strControlName.GetBuffer(MAX_PATH),MAX_PATH,strServerIniName);
		strControlName.ReleaseBuffer();
		str2 = strControlName;
	
		m_mapFile[str1] = str2;
	}
}
/*
0：成功下载保存
-1：URL不正确
-2：建立网络连接失败
-3：向服务器发起GET请求失败
-4：服务器不接受请求
-5：参数一不能为空
-6：参数二不能为空
-7：保存文件时创建或写入文件失败
-8：未知失败
*/
LRESULT CAgentDlg::DownloadFile(UINT nNumber,CString &strSavePath,CString &strUrl){
	//检查传入的两个参数
	if (strUrl.IsEmpty())
		return -5;
	if (strSavePath.IsEmpty())
		return -6;

	//判断文件路径是否存在，不存在则创建
	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// 向上一层
	for (int i=(int)(strlen(pBuf)-1); i>=0; i--)
	{
		if ('\\' != pBuf[i])
		{
			pBuf[i] = '\0';
		}
		else
		{
			pBuf[i] = '\0';
			break;
		}
	}
	strcat_s(pBuf,"\\"+strSavePath.Left(strSavePath.ReverseFind('/')));
	CString fileurl = pBuf;
	if (!PathIsDirectory(fileurl)){
		CreateDirectory(fileurl,NULL);
	}

	unsigned short nPort;        //用于保存目标HTTP服务端口
	CString strServer, strObject;    //strServer用于保存服务器地址，strObject用于保存文件对象名称
	DWORD dwServiceType,dwRet;        //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号
	//解析URL，获取信息
	if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort)){
		return -1;//解析失败，该Url不正确
	}

	//创建网络连接对象，HTTP连接对象指针和用于该连接的HttpFile文件对象指针，注意delete
	CInternetSession intsess;
	CHttpFile *pHtFile = NULL;
	CHttpConnection *pHtCon = NULL;

	intsess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 20);  //连接超时
	intsess.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);       //两次重试之间的间隔时间
	intsess.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);          // 2次重试
	intsess.SetOption(INTERNET_OPTION_SEND_TIMEOUT,6000);			//发送请求的超时时间
	intsess.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,6000);		//接受数据的超时时间

	try{		
		//建立网络连接
		pHtCon = intsess.GetHttpConnection(strServer,nPort);
		if(pHtCon == NULL){
			//建立网络连接失败
			intsess.Close();
			return -2;
		}
		//发起GET请求
		pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET,strObject);
		if(pHtFile == NULL)
		{
			//发起GET请求失败
			intsess.Close();
			delete pHtCon;
			pHtCon = NULL;
			return -3;
		}
		//提交请求
		pHtFile->SendRequest();
		//获取服务器返回的状态号
		pHtFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK){
			//服务器不接受请求
			intsess.Close();
			delete pHtCon;pHtCon = NULL;
			delete pHtFile;pHtFile = NULL;
			return -4;
		}
	}catch (CInternetException* e){
		e->Delete();
		intsess.Close();
		delete pHtCon;pHtCon = NULL;
		delete pHtFile;pHtFile = NULL;
		return -2;
	}
	//获取文件大小 并刷新列表框显示
	CString szSize;
	pHtFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szSize);
	//UINT nFileSize = (UINT)pHtFile->GetLength();
	UINT nFileSize = _ttoi(szSize);

	CString strFileSize = "";
	if (nFileSize <1024){		
		strFileSize.Format("%d b",nFileSize);
		m_listFile.SetItemText(nNumber,1,strFileSize);
	}else if (nFileSize > 1024 && nFileSize <1024*1024){		
		strFileSize.Format("%d kb",nFileSize/1024);
		m_listFile.SetItemText(nNumber,1,strFileSize);
	}else{
		double dbFileSize = (double)nFileSize;
		strFileSize.Format("%.2lf MB",dbFileSize/(1024*1024));
		m_listFile.SetItemText(nNumber,1,strFileSize);
	}
	
	//创建缓冲区
	CHAR *szBuffer = new CHAR[nFileSize+1];
	TRY {
		UINT nCompletedSize = 0;
		CString strCompletedSize = "";
		int nCircle = 0;
		DWORD dwRead = 0;        //用于标识读了多少，为1是为了进入循环	
		CFile PicFile(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate);//创建文件
		while(nCompletedSize < nFileSize){  
			//清空缓冲区
			memset(szBuffer,0,(size_t)(nFileSize+1));
			//读取到缓冲区
			dwRead = pHtFile->Read(szBuffer,nFileSize); 
			//写入到文件
			PicFile.Write(szBuffer,dwRead);

			nCompletedSize += dwRead;
			if (nCircle%10 == 0){
				int n = (int)(nCompletedSize*100/nFileSize);
				strCompletedSize.Format("已下载 %d",n);
				strCompletedSize = strCompletedSize + " %";
				m_listFile.SetItemText(nNumber,2,strCompletedSize);
			}
			nCircle++;
		}
		//关闭文件
		PicFile.Close();
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
		m_listFile.SetItemText(nNumber,2,"已完成下载");
	}CATCH(CFileException,e){
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
		return -7;            //读写文件异常
	}END_CATCH	
		return 0;
}

//对比文件的md5
BOOL CAgentDlg::CheckFileMd5(CString &strSavePath,CString &filemd){
	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// 向上一层
	for (int i=(int)(strlen(pBuf)-1); i>=0; i--)
	{
		if ('\\' != pBuf[i])
		{
			pBuf[i] = '\0';
		}
		else
		{
			pBuf[i] = '\0';
			break;
		}
	}
	strcat_s(pBuf,"\\"+strSavePath);
	CString strFileName = pBuf;
	if (PathFileExists(strFileName)){//文件存在
		CString fmd5 = MD5::getFileMD5(strSavePath);
		if(filemd == fmd5){
			return TRUE;
		}
	}
	return FALSE;
}

//以固定分隔符分割CString字符串
CString * CAgentDlg::SplitString(CString str, char split, int& iSubStrs)
{
	int iPos = 0; //分割符位置
	int iNums = 0; //分割符的总数
	CString strTemp = str;
	CString strRight;
	//先计算子字符串的数量
	while (iPos != -1)
	{
		iPos = strTemp.Find(split);
		if (iPos == -1)
		{
			break;
		}
		strRight = strTemp.Mid(iPos + 1, str.GetLength());
		strTemp = strRight;
		iNums++;
	}
	if (iNums == 0) //没有找到分割符
	{
		//子字符串数就是字符串本身
		iSubStrs = 1; 
		return NULL;
	}
	//子字符串数组
	iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//左子串
		strLeft = strTemp.Left(iPos);
		//右子串
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

LRESULT CAgentDlg::OnUpDateEndMsg(WPARAM wParam,LPARAM lParam)
{
	//将下载来的配置文件里的最新版本号写入到Update.ini文件里
	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// 向上一层
	for (int i=(int)(strlen(pBuf)-1); i>=0; i--)
	{
		if ('\\' != pBuf[i])
		{
			pBuf[i] = '\0';
		}
		else
		{
			pBuf[i] = '\0';
			break;
		}
	}
	strcat_s(pBuf,"\\AgentVersionInfo.ini");
	CString strFileName_ini = pBuf;

	CString mainver,verdate,author;
	//更新本地版本号
	::GetPrivateProfileString("MAINVER","mainver",NULL,mainver.GetBuffer(128),128,strFileName_ini);
	mainver.ReleaseBuffer();
	::GetPrivateProfileString("VERDATE","verdate",NULL,verdate.GetBuffer(128),128,strFileName_ini);
	verdate.ReleaseBuffer();
	::GetPrivateProfileString("AUTHOR","author",NULL,author.GetBuffer(128),128,strFileName_ini);
	verdate.ReleaseBuffer();

	strFileName_ini.Replace("AgentVersionInfo.ini","VersionInfo.ini");
	::WritePrivateProfileString("MAINVER","mainver",mainver,strFileName_ini);
	::WritePrivateProfileString("VERDATE","verdate",verdate,strFileName_ini);
	::WritePrivateProfileString("AUTHOR","author",author,strFileName_ini);

	//删除下载来的配置文件AgentVersionInfo.ini
	strFileName_ini.Replace("VersionInfo.ini","AgentVersionInfo.ini");
	::DeleteFile(strFileName_ini);

	CString strExeName = strFileName_ini;
	strExeName.Replace("AgentVersionInfo.ini","AutoUpdate.exe");
	HINSTANCE hID = ShellExecuteA(NULL,"open",strExeName,NULL,NULL,SW_SHOWNORMAL);
	if ((int)hID <= 32)
	{
		AfxMessageBox("未找到主程序AutoUpdate.exe！请重命名主程序为AutoUpdate.exe！");
		return 0;
	}
	//关闭本程序
	ExitProcess(0);
	return 0;
}



