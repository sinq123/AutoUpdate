#include "StdAfx.h"
#include "VersionUpdate.h"
#include <WinBase.h>
#include <afxinet.h>

#define BUF_SIZE	2048  
#define WM_UPDATE		WM_USER+100

VersionUpdate::VersionUpdate(void)
{
}


VersionUpdate::~VersionUpdate(void)
{
}

//�Զ������߳�
UINT VersionUpdate::ThreadCheckVer(LPVOID lpParam)
{
	CString strFileName_ini,strControlName;
	//�������ļ����뵱ǰ�汾��
	char str[MAX_PATH];
	GetModuleFileName(NULL,str,128);
	CString exePath;
	exePath = str;
	CString programFolder;
	programFolder = exePath.Mid(0,exePath.ReverseFind('\\'))+"\\";

	//char pBuf[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,pBuf);
	//strcat_s(pBuf,"\\VersionInfo.ini");
	strFileName_ini = programFolder + "VersionInfo.ini";

	//��ȡ��ǰ�����ļ��İ汾��
	::GetPrivateProfileString("MAINVER","mainver",NULL,strControlName.GetBuffer(128),128,strFileName_ini);
	strControlName.ReleaseBuffer();
	double flCurVer = atof(strControlName);			//���浱ǰ�汾��

	//��ȡ���������ļ��ĵ�ַ
	strControlName = "";
	::GetPrivateProfileString("INIURL","iniurl",NULL,strControlName.GetBuffer(MAX_PATH),MAX_PATH,strFileName_ini);
	strControlName.ReleaseBuffer();
	CString strUrl = strControlName;
	if (strUrl.GetLength() == 0){
		AfxMessageBox("VersionInfo.ini�ļ��������ֶ������°汾��");
		return 0;
	}

	//���ذ������°汾�ŵ������ļ�
	INT DownloadFileResult =  DownloadIniFile(strUrl);
	if(DownloadFileResult != 0){
		//AfxMessageBox("����°汾ʧ�ܣ�����������ʹ�����ֶ����ظ���!");
		return 0;
	}

	//���������������ļ��������°汾��
	strFileName_ini.Replace("VersionInfo.ini","AgentVersionInfo.ini");
	::GetPrivateProfileString("MAINVER","mainver",NULL,strControlName.GetBuffer(128),128,strFileName_ini);
	strControlName.ReleaseBuffer();
	double flNewVer = atof(strControlName);

	//�ԱȰ汾��	������ڱ��ذ汾���������³�����и���  ���³ɹ���д���°汾��pz.ini��
	if( flNewVer > flCurVer){
		//����UpDate.exe������Dll
		char pBuf[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,pBuf);
		strcat_s(pBuf,"\\Agent.exe");
		CString strPath = pBuf;
		HINSTANCE hID = ShellExecuteA(NULL,"open",strPath,NULL,NULL,SW_SHOWNORMAL);
		if ((int)hID <= 32)
		{
			AfxMessageBox("����Ŀ¼��ȱ����������Agent.exe�������������������汾��");
			return 0;
		}
		//�رձ�����
		ExitProcess(0);
	}else{
		::DeleteFile(strFileName_ini);						//ɾ���������������ļ�
	}
	return 0;		
}

//���ذ������°汾�ŵ������ļ�
INT  VersionUpdate::DownloadIniFile(CString strUrl)
{
	CString strFileName_ini;

	char pBuf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,pBuf);
	strcat_s(pBuf,"\\AgentVersionInfo.ini");
	strFileName_ini = pBuf;

	char buf[BUF_SIZE] = {0};  
	char url[MAX_PATH];
	strcat_s(url,strUrl);

	return DownloadFile(url,strFileName_ini);
}

//����ָ����ַ���ļ�
/*
0���ɹ����ر���
-1��URL����ȷ
-2��������������ʧ��
-3�������������GET����ʧ��
-4������������������
-5������һ����Ϊ��
-6������������Ϊ��
-7�������ļ�ʱ������д���ļ�ʧ��
*/
INT VersionUpdate::DownloadFile(const CString strUrl,const CString strSavePath)
{
	//��鴫�����������
	if (strUrl.IsEmpty())
		return -5;
	if (strSavePath.IsEmpty())
		return -6;

	unsigned short nPort;        //���ڱ���Ŀ��HTTP����˿�
	CString strServer, strObject;    //strServer���ڱ����������ַ��strObject���ڱ����ļ���������
	DWORD dwServiceType,dwRet;        //dwServiceType���ڱ���������ͣ�dwRet���ڱ����ύGET���󷵻ص�״̬��

	//����URL����ȡ��Ϣ
	if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort)){
		//����ʧ�ܣ���Url����ȷ
		return -1;
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

	//��ȡ�ļ���С
	UINT nFileLen = (UINT)pHtFile->GetLength();
	DWORD dwRead = 1;        //���ڱ�ʶ���˶��٣�Ϊ1��Ϊ�˽���ѭ��
	//����������
	CHAR *szBuffer = new CHAR[nFileLen+1];
	TRY {
		//�����ļ�
		CFile PicFile(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);
		while(dwRead>0)
		{  
			//��ջ�����
			memset(szBuffer,0,(size_t)(nFileLen+1));
			//��ȡ��������
			dwRead = pHtFile->Read(szBuffer,nFileLen); 
			//д�뵽�ļ�
			PicFile.Write(szBuffer,dwRead);
		}
		//�ر��ļ�
		PicFile.Close();
		//�ͷ��ڴ�
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//�ر���������
		intsess.Close();
	}
	CATCH(CFileException,e)
	{
		//�ͷ��ڴ�
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//�ر���������
		intsess.Close();
		return -7;            //��д�ļ��쳣
	}
	END_CATCH
		return 0;
}