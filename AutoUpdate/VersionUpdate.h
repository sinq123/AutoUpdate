#pragma once
class VersionUpdate
{
public:
	VersionUpdate(void);
	~VersionUpdate(void);

	//自动更新线程
	static UINT ThreadCheckVer(LPVOID pParam);	
	//下载包含更新版本号的配置文件
	static INT DownloadIniFile(CString strUrl);
	//下载指定地址的文件
	static INT DownloadFile(const CString strUrl,const CString strSavePath);
};

