#pragma once
class VersionUpdate
{
public:
	VersionUpdate(void);
	~VersionUpdate(void);

	//�Զ������߳�
	static UINT ThreadCheckVer(LPVOID pParam);	
	//���ذ������°汾�ŵ������ļ�
	static INT DownloadIniFile(CString strUrl);
	//����ָ����ַ���ļ�
	static INT DownloadFile(const CString strUrl,const CString strSavePath);
};

