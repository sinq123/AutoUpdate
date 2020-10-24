#include "StdAfx.h"
#include "Tool.h"
#include <stdlib.h>

#include <direct.h>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <Shlwapi.h>
#include "Log.h"

//--PINGר��
#include<winsock2.h>
#include<conio.h>
#pragma comment(lib,"WS2_32")
//����ICMPͷ�����ݽṹ
typedef struct icmp_hdr
{
	unsigned char icmp_type;  //��Ϣ����
	unsigned char inmp_code;  //����
	unsigned short icmp_checksum; //У���

	//����ͷ
	unsigned short icmp_id;  //��־�����ID��
	unsigned short icmp_sequence; //���к�
	unsigned long icmp_timestamp; //ʱ���
} ICMP_HDR,*PICMP_HDR;

#define ICMP_ECHOREPLY 0
#define ICMP_ECHOREQ 8
#define REQ_DATASIZE 32

typedef struct tagIPHDR
{
	u_char VIHL;
	u_char TOS;
	short TotLen;
	short ID;
	short FlagOff;
	u_char TTL;
	u_char Protocol;
	u_short Checksum;
	struct in_addr iaSrc;
	struct in_addr iaDst;


}IPHDR,*PIPHDR;

typedef struct tagICMPHDR
{
	u_char Type;
	u_char Code;
	u_short Checksum;
	u_short ID;
	u_short Seq;
	char Data;
}ICMPHDR,*PICMPHDR;

typedef struct tagECHOREQUEST
{
	ICMPHDR icmpHdr;
	DWORD dwTime;
	char cData[REQ_DATASIZE];
}ECHOREQUEST,*PECHOREQUEST;

typedef struct tagECHOREPLY
{
	IPHDR ipHdr;
	ECHOREQUEST echoRequest;
	char cFiller[256];
}ECHOREPLY,*PECHOREPLY;



typedef struct IPHeader {
	UCHAR  iph_verlen;  // Version and length 
	UCHAR  iph_tos;  // Type of service 
	USHORT  iph_length;  // Total datagram length 
	USHORT  iph_id;  // Identification 
	USHORT  iph_offset;  // Flags, fragment offset 
	UCHAR  iph_ttl;  // Time to live 
	UCHAR  iph_protocol;  // Protocol 
	USHORT  iph_xsum;  // Header checksum 
	ULONG  iph_src;  // Source address 
	ULONG  iph_dest;  // Destination address 
} IPHeader; 
//--PINGר��

CString Tool::exePath;
Tool::Tool(void)
{
}


Tool::~Tool(void)
{
}


// ���ı��ķ�ʽ�����ļ�����
CString Tool::loadTextFile(CString file)
{
	CString str;
	CFile myFile;
	if(!myFile.Open(file,CFile::modeRead))
		return("");
	char buff1[8192];
	char buff2[1024];
	int rlen=0, alen=0;
	while((rlen=myFile.Read(buff2,1024))>0){
		for(int i=0; i<rlen; i++)
			buff1[i+alen] = buff2[i];
		alen += rlen;
	}
	buff1[alen]=0;
	myFile.Close();
	str.Format("%s",buff1);
	return(str);
}


// ʮ�����ƴ�תchar����
int Tool::hex2Char(CString hexData, char* outData)
{
	int isize = hexData.GetLength() / 2;
	for(int i=0; i< isize; i++)
		outData[i] = C2I(hexData[i * 2]) * 16 + C2I(hexData[i * 2+1]);
	return isize;
}


// ������charתint  16���Ƶ�
int Tool::C2I(unsigned short c)
{
	switch(c)
	{case '0':return(0);
	case '1':return(1);
	case '2':return(2);
	case '3':return(3);
	case '4':return(4);
	case '5':return(5);
	case '6':return(6);
	case '7':return(7);
	case '8':return(8);
	case '9':return(9);
	case 'A':return(10);
	case 'B':return(11);
	case 'C':return(12);
	case 'D':return(13);
	case 'E':return(14);
	case 'F':return(15);
	case 'a':return(10);
	case 'b':return(11);
	case 'c':return(12);
	case 'd':return(13);
	case 'e':return(14);
	case 'f':return(15);
	}
	return(0);
}

//��ȡ�ַ�����
CString Tool::subString(CString str, int offset, int size)
{
	if(offset>=str.GetLength())
		return "";
	if(offset+size>=str.GetLength())
		return str.Right(str.GetLength()-offset);
	if(offset>0)
		str = str.Right(str.GetLength()-offset);
	if(size>0)
		return str.Left(size);
	return str;
}


// �ַ���תchar�����  UTF-8����
int Tool::str2Char(CString str, char* cdata)
{
#define charSize 4096
	wchar_t d[charSize]; 
	memset(d ,0,charSize);
	MultiByteToWideChar( CP_ACP , 0 , str , str.GetLength() , d , charSize);
	int textlen;
	//	msg = d;
	textlen = WideCharToMultiByte( CP_UTF8,    0,    d,    -1,    NULL, 0, NULL, NULL );
	int len = (textlen)*sizeof(char);
	memset(cdata, 0, len);
	WideCharToMultiByte( CP_UTF8, 0, d, -1, cdata, textlen, NULL, NULL );
	return len-1;
}

// �ַ�����תʮ������
CString Tool::char2Hex(char* cdata, int offset, int size)
{
	CString strtem = "";
	CString astrtem = "";
	for(int i =0; i < size; i++)
	{
		strtem.Format("%.2X", (byte)(cdata[i+offset]));
		astrtem += strtem;
	}
	return(astrtem);
}


// char����ת�ַ���
CString Tool::char2Str(char* data, int offset,const int size)
{
	char temp[1024];
	for(int i=0; i<size; i++)
		temp[i] = data[i+offset];
	temp[size] = 0;
	CString ret;
	ret.Format("%s", temp);
	return ret;
}
//��PING�������õ��ڲ�����
USHORT Tool::checksum(USHORT *buff,int size)
{
	unsigned long cksum = 0;
	//���������ֵ�λ�ۼӵ�cksum ��
	while(size > 1)
	{
		cksum += *buff++;
		size -= sizeof(USHORT);
	}
	//���Ϊ�����������һ���ֽ���չΪ˫�֣����ۼӵ�cksum��
	if(size)
	{
		cksum += *(UCHAR*) buff;
	}
	//��cksum�ĸ�16λ�͵�16λ��ӣ�ȥ����õ�У���
	cksum = (cksum >> 16)+(cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}

boolean Tool::PING(CString ip)
{
	try{
		Log::trace(__FILE__, __LINE__, "�˴ε�IP�ǣ�[%s]", ip);
		if(ip.Find(":")!=-1) ip = ip.Left(ip.Find(":"));
		if(ip.GetLength()<5) return true;
		char szDestIp[20];
		memset(szDestIp, 0, 20);
		str2Char(ip,szDestIp);

		//	SOCKET sRaw = ::socket(PF_INET,SOCK_RAW,IPPROTO_IP);
		Log::trace(__FILE__, __LINE__, "IP�������");
		WSADATA wsaData;
		Log::trace( __FILE__, __LINE__, "׼��PING:"+ip);
		if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
		{
			Log::warn( __FILE__, __LINE__,"��ʼ������ʧ��");
			WSACleanup();
			return false;
		}
		//����ԭʼ�׽���
		SOCKET sRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,WSA_FLAG_OVERLAPPED);
		if(sRaw == INVALID_SOCKET){
			Log::warn( __FILE__, __LINE__,"�����׽���ʧ��");

			int ierror = WSAGetLastError();
			CString ss;
			ss.Format("%d|%x",ierror, ierror);
			WSACleanup();
			//AfxMessageBox(ss);
			return false;
		}
		//���ý��ճ�ʱ
		int timeout=2000;
		if(setsockopt(sRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))== SOCKET_ERROR) 
		{
			Log::warn( __FILE__, __LINE__, "�����������ʧ��");
			WSACleanup();
			return false;
		}
		//����Ŀ�ĵ�ַ
		SOCKADDR_IN dest;
		dest.sin_family = AF_INET;
		dest.sin_port = htons(0);
		if(INADDR_NONE==(dest.sin_addr.S_un.S_addr = inet_addr(szDestIp)))
		{
			Log::fatal( __FILE__, __LINE__, "�����IP��ַ");
			return false;
		}

		//����һ�� ICMP ���
		char buff[sizeof(ICMP_HDR)+32];
		ICMP_HDR*pIcmp = (ICMP_HDR*)buff;
		pIcmp->icmp_type = 8;
		pIcmp->inmp_code = 0;
		pIcmp->icmp_id = (USHORT)::GetCurrentProcessId();
		pIcmp->icmp_checksum = 0;
		pIcmp->icmp_sequence = 0;

		memset(&buff[sizeof(ICMP_HDR)],'E',32); //������ݲ��֣���������  void *memset( void *dest, int c, size_t count );Sets buffers to a specified character.

		//��ʼ���ͺͽ���ICMP���
		USHORT nSeq = 0;
		char recvBuf[1024];
		SOCKADDR_IN from; //һ���ṹ��
		int nLen = sizeof(from);

		int nCount = 0;
		while (TRUE)
		{
			int nRet;
			nCount++;
			if(nCount == 4) break;
			pIcmp->icmp_checksum = 0;
			pIcmp->icmp_timestamp = ::GetTickCount();
			pIcmp->icmp_sequence  = nSeq++;
			pIcmp->icmp_checksum = checksum((USHORT*)buff,sizeof(ICMP_HDR)+32);
			nRet = ::sendto (sRaw,buff,sizeof(ICMP_HDR)+32,0,(SOCKADDR*)&dest,sizeof(dest));

			if(nRet == SOCKET_ERROR)
			{
				Log::warn(__FILE__, __LINE__,"��������ʧ��");
				WSACleanup();
				return false;
			}

			nRet = ::recvfrom(sRaw,recvBuf,1024,0,(sockaddr*)&from,&nLen);

			if(nRet== SOCKET_ERROR)
			{
				if(WSAGetLastError()==10060)//10060��WSAGetLastError()��ϵͳ������
				{
					Log::warn( __FILE__, __LINE__, "�������ݳ�ʱ");
					WSACleanup();
					return false;
				}
				Log::warn(__FILE__, __LINE__,"��������ʧ��");
				WSACleanup();
				return false;
			}
			Log::trace(__FILE__, __LINE__, "�ɹ��յ���������");
			::Sleep(1000);

		}
		return true;
	}
	catch(...){
		Log::trace(__FILE__, __LINE__, "���ִ�����");
		return false;
	}
}


// �޸��ļ���
boolean Tool::fileRename(CString file, CString newPath)
{
	if(PathFileExists(newPath))
		CFile::Remove(newPath);
	CFile::Rename(file, newPath);
	return true;
}
//ɾ���ļ�
boolean Tool::fileDelete(CString path)
{
	if(PathFileExists(path))
		CFile::Remove(path);
	return true;
}


// ���ļ������ļ���
boolean Tool::mkFileDir(CString fileName)
{
	CString path = fileName.Left(fileName.ReverseFind('\\'));
	if(!PathFileExists(path)){
		if(mkFileDir(path))
			return CreateDirectory(path, NULL);
	}
	return true;
}


// ���ı��ķ�ʽд���ļ�
boolean Tool::writeTextFile(CString file, CString content)
{
	CFile cfile ;
	CString logFolder;
	boolean isDaemon = false;
	CString info;
	cfile.Open (file, CFile::modeCreate |CFile::modeWrite);
	cfile.Write(content, content.GetLength());
	cfile.Flush();
	cfile.Close();
	return true;
}


// ��ȡʱ��ڵ�yyyyMMddHHmmssSSS
CString Tool::getTime(CString format)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString yyyy,MM,dd,HH,mm,ss,SSS;
	yyyy.Format("%04d", time.wYear);
	MM.Format("%02d", time.wMonth);
	dd.Format("%02d", time.wDay);
	HH.Format("%02d", time.wHour);
	mm.Format("%02d", time.wMinute);
	ss.Format("%02d", time.wSecond);
	SSS.Format("%03d", time.wMilliseconds);
	format.Replace("yyyy", yyyy);
	format.Replace("MM", MM);
	format.Replace("dd", dd);
	format.Replace("HH", HH);
	format.Replace("mm", mm);
	format.Replace("ss", ss);
	format.Replace("SSS", SSS);
	return format;
}
