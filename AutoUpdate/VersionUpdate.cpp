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

//自动更新线程
UINT VersionUpdate::ThreadCheckVer(LPVOID lpParam)
{
	CString strFileName_ini,strControlName;
	//从配置文件读入当前版本号
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

	//获取当前配置文件的版本号
	::GetPrivateProfileString("MAINVER","mainver",NULL,strControlName.GetBuffer(128),128,strFileName_ini);
	strControlName.ReleaseBuffer();
	double flCurVer = atof(strControlName);			//保存当前版本号

	//获取下载配置文件的地址
	strControlName = "";
	::GetPrivateProfileString("INIURL","iniurl",NULL,strControlName.GetBuffer(MAX_PATH),MAX_PATH,strFileName_ini);
	strControlName.ReleaseBuffer();
	CString strUrl = strControlName;
	if (strUrl.GetLength() == 0){
		AfxMessageBox("VersionInfo.ini文件有误，请手动下载新版本！");
		return 0;
	}

	//下载包含更新版本号的配置文件
	INT DownloadFileResult =  DownloadIniFile(strUrl);
	if(DownloadFileResult != 0){
		//AfxMessageBox("检测新版本失败！如果软件不能使用请手动下载更新!");
		return 0;
	}

	//从下载来的配置文件读入最新版本号
	strFileName_ini.Replace("VersionInfo.ini","AgentVersionInfo.ini");
	::GetPrivateProfileString("MAINVER","mainver",NULL,strControlName.GetBuffer(128),128,strFileName_ini);
	strControlName.ReleaseBuffer();
	double flNewVer = atof(strControlName);

	//对比版本号	如果大于本地版本就启动更新程序进行更新  更新成功就写入新版本到pz.ini里
	if( flNewVer > flCurVer){
		//启动UpDate.exe来更新Dll
		char pBuf[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,pBuf);
		strcat_s(pBuf,"\\Agent.exe");
		CString strPath = pBuf;
		HINSTANCE hID = ShellExecuteA(NULL,"open",strPath,NULL,NULL,SW_SHOWNORMAL);
		if ((int)hID <= 32)
		{
			AfxMessageBox("程序目录下缺少升级程序Agent.exe！，请重新下载完整版本！");
			return 0;
		}
		//关闭本程序
		ExitProcess(0);
	}else{
		::DeleteFile(strFileName_ini);						//删除下载来的配置文件
	}
	return 0;		
}

//下载包含更新版本号的配置文件
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

//下载指定地址的文件
/*
0：成功下载保存
-1：URL不正确
-2：建立网络连接失败
-3：向服务器发起GET请求失败
-4：服务器不接受请求
-5：参数一不能为空
-6：参数二不能为空
-7：保存文件时创建或写入文件失败
*/
INT VersionUpdate::DownloadFile(const CString strUrl,const CString strSavePath)
{
	//检查传入的两个参数
	if (strUrl.IsEmpty())
		return -5;
	if (strSavePath.IsEmpty())
		return -6;

	unsigned short nPort;        //用于保存目标HTTP服务端口
	CString strServer, strObject;    //strServer用于保存服务器地址，strObject用于保存文件对象名称
	DWORD dwServiceType,dwRet;        //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号

	//解析URL，获取信息
	if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort)){
		//解析失败，该Url不正确
		return -1;
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

	//获取文件大小
	UINT nFileLen = (UINT)pHtFile->GetLength();
	DWORD dwRead = 1;        //用于标识读了多少，为1是为了进入循环
	//创建缓冲区
	CHAR *szBuffer = new CHAR[nFileLen+1];
	TRY {
		//创建文件
		CFile PicFile(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);
		while(dwRead>0)
		{  
			//清空缓冲区
			memset(szBuffer,0,(size_t)(nFileLen+1));
			//读取到缓冲区
			dwRead = pHtFile->Read(szBuffer,nFileLen); 
			//写入到文件
			PicFile.Write(szBuffer,dwRead);
		}
		//关闭文件
		PicFile.Close();
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
	}
	CATCH(CFileException,e)
	{
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
		return -7;            //读写文件异常
	}
	END_CATCH
		return 0;
}