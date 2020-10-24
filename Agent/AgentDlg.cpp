
// AgentDlg.cpp : ʵ���ļ�
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


// CAgentDlg �Ի���




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
	ON_MESSAGE(WM_UPDATE_END,OnUpDateEndMsg)				//�Զ�������Ϣ����
END_MESSAGE_MAP()


// CAgentDlg ��Ϣ�������

BOOL CAgentDlg::OnInitDialog()
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
	m_listFile.InsertColumn(0,"�ļ���",LVCFMT_LEFT,100,0);
	m_listFile.InsertColumn(1,"��С",LVCFMT_LEFT,90,0);
	m_listFile.InsertColumn(2,"����",LVCFMT_LEFT,100,0);
	m_listFile.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT
		|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_GRIDLINES);

	AfxBeginThread(ThreadUpdate,this);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAgentDlg::OnPaint()
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

	//�����ǻ�ȡ��Ҫ���µ��ļ���Ϣ�б�
	GetUpdateFile();
	
	//ˢ���б��
	m_listFile.DeleteAllItems();
	CString strUrl,strName,strTemp,strMd;
	for (map<CString,CString>::iterator beg = m_mapFile.begin();
		beg != m_mapFile.end();beg++)
	{
		strName = beg->first;
		m_listFile.InsertItem(m_listFile.GetItemCount(),"");
		m_listFile.SetItemText(m_listFile.GetItemCount()-1,0,strName);
		m_listFile.SetItemText(m_listFile.GetItemCount()-1,1,"0 kb");
		m_listFile.SetItemText(m_listFile.GetItemCount()-1,2,"������ 0 %");
	}

	//�����ļ�
	UINT nNumber = 0;
	for (map<CString,CString>::iterator beg = m_mapFile.begin();
		beg != m_mapFile.end();beg++)
	{
		strName = beg->first;
		strTemp = beg->second;

		int iPos = 0; //�ָ��λ�� 
		iPos = strTemp.Find(',');  
		strUrl = strTemp.Left(iPos); 
		strMd = strTemp.Mid(iPos + 1, strTemp.GetLength());  
		
		//�Ա��ļ���MD5
		if(!CheckFileMd5(strName,strMd)){
			if (DownloadFile(nNumber,strName,strUrl) != 0){
				m_listFile.SetItemText(nNumber,2,"����ʧ��");
				bIsSuccess = FALSE;
			}
		}else{
			m_listFile.SetItemText(nNumber,2,"�Ѿ�����");
		}
		nNumber++;
	}
	//�����ļ����سɹ�
	if (bIsSuccess){
		::PostMessage(m_hWnd,WM_UPDATE_END,0,0);	//������Ϣ �����Զ��˳�����������
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
	// ����һ��
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
	strServerIniName= pBuf;					//���������ص������ļ�·����

	if (_access(strServerIniName,0) == -1){
		AfxMessageBox("AgentVersionInfo.ini�ļ������ڣ�");
		return ;
	}

	//��ȡ�µ��ļ�����
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
0���ɹ����ر���
-1��URL����ȷ
-2��������������ʧ��
-3�������������GET����ʧ��
-4������������������
-5������һ����Ϊ��
-6������������Ϊ��
-7�������ļ�ʱ������д���ļ�ʧ��
-8��δ֪ʧ��
*/
LRESULT CAgentDlg::DownloadFile(UINT nNumber,CString &strSavePath,CString &strUrl){
	//��鴫�����������
	if (strUrl.IsEmpty())
		return -5;
	if (strSavePath.IsEmpty())
		return -6;

	//�ж��ļ�·���Ƿ���ڣ��������򴴽�
	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// ����һ��
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

	unsigned short nPort;        //���ڱ���Ŀ��HTTP����˿�
	CString strServer, strObject;    //strServer���ڱ����������ַ��strObject���ڱ����ļ���������
	DWORD dwServiceType,dwRet;        //dwServiceType���ڱ���������ͣ�dwRet���ڱ����ύGET���󷵻ص�״̬��
	//����URL����ȡ��Ϣ
	if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort)){
		return -1;//����ʧ�ܣ���Url����ȷ
	}

	//�����������Ӷ���HTTP���Ӷ���ָ������ڸ����ӵ�HttpFile�ļ�����ָ�룬ע��delete
	CInternetSession intsess;
	CHttpFile *pHtFile = NULL;
	CHttpConnection *pHtCon = NULL;

	intsess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 20);  //���ӳ�ʱ
	intsess.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);       //��������֮��ļ��ʱ��
	intsess.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);          // 2������
	intsess.SetOption(INTERNET_OPTION_SEND_TIMEOUT,6000);			//��������ĳ�ʱʱ��
	intsess.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,6000);		//�������ݵĳ�ʱʱ��

	try{		
		//������������
		pHtCon = intsess.GetHttpConnection(strServer,nPort);
		if(pHtCon == NULL){
			//������������ʧ��
			intsess.Close();
			return -2;
		}
		//����GET����
		pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET,strObject);
		if(pHtFile == NULL)
		{
			//����GET����ʧ��
			intsess.Close();
			delete pHtCon;
			pHtCon = NULL;
			return -3;
		}
		//�ύ����
		pHtFile->SendRequest();
		//��ȡ���������ص�״̬��
		pHtFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK){
			//����������������
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
	//��ȡ�ļ���С ��ˢ���б����ʾ
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
	
	//����������
	CHAR *szBuffer = new CHAR[nFileSize+1];
	TRY {
		UINT nCompletedSize = 0;
		CString strCompletedSize = "";
		int nCircle = 0;
		DWORD dwRead = 0;        //���ڱ�ʶ���˶��٣�Ϊ1��Ϊ�˽���ѭ��	
		CFile PicFile(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate);//�����ļ�
		while(nCompletedSize < nFileSize){  
			//��ջ�����
			memset(szBuffer,0,(size_t)(nFileSize+1));
			//��ȡ��������
			dwRead = pHtFile->Read(szBuffer,nFileSize); 
			//д�뵽�ļ�
			PicFile.Write(szBuffer,dwRead);

			nCompletedSize += dwRead;
			if (nCircle%10 == 0){
				int n = (int)(nCompletedSize*100/nFileSize);
				strCompletedSize.Format("������ %d",n);
				strCompletedSize = strCompletedSize + " %";
				m_listFile.SetItemText(nNumber,2,strCompletedSize);
			}
			nCircle++;
		}
		//�ر��ļ�
		PicFile.Close();
		//�ͷ��ڴ�
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//�ر���������
		intsess.Close();
		m_listFile.SetItemText(nNumber,2,"���������");
	}CATCH(CFileException,e){
		//�ͷ��ڴ�
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//�ر���������
		intsess.Close();
		return -7;            //��д�ļ��쳣
	}END_CATCH	
		return 0;
}

//�Ա��ļ���md5
BOOL CAgentDlg::CheckFileMd5(CString &strSavePath,CString &filemd){
	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// ����һ��
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
	if (PathFileExists(strFileName)){//�ļ�����
		CString fmd5 = MD5::getFileMD5(strSavePath);
		if(filemd == fmd5){
			return TRUE;
		}
	}
	return FALSE;
}

//�Թ̶��ָ����ָ�CString�ַ���
CString * CAgentDlg::SplitString(CString str, char split, int& iSubStrs)
{
	int iPos = 0; //�ָ��λ��
	int iNums = 0; //�ָ��������
	CString strTemp = str;
	CString strRight;
	//�ȼ������ַ���������
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
	if (iNums == 0) //û���ҵ��ָ��
	{
		//���ַ����������ַ�������
		iSubStrs = 1; 
		return NULL;
	}
	//���ַ�������
	iSubStrs = iNums + 1; //�Ӵ������� = �ָ������ + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//���Ӵ�
		strLeft = strTemp.Left(iPos);
		//���Ӵ�
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

LRESULT CAgentDlg::OnUpDateEndMsg(WPARAM wParam,LPARAM lParam)
{
	//���������������ļ�������°汾��д�뵽Update.ini�ļ���
	char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	// ����һ��
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
	//���±��ذ汾��
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

	//ɾ���������������ļ�AgentVersionInfo.ini
	strFileName_ini.Replace("VersionInfo.ini","AgentVersionInfo.ini");
	::DeleteFile(strFileName_ini);

	CString strExeName = strFileName_ini;
	strExeName.Replace("AgentVersionInfo.ini","AutoUpdate.exe");
	HINSTANCE hID = ShellExecuteA(NULL,"open",strExeName,NULL,NULL,SW_SHOWNORMAL);
	if ((int)hID <= 32)
	{
		AfxMessageBox("δ�ҵ�������AutoUpdate.exe����������������ΪAutoUpdate.exe��");
		return 0;
	}
	//�رձ�����
	ExitProcess(0);
	return 0;
}



