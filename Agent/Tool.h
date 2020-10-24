#pragma once
/************************************************************************/
/*           ������                                                     */
/************************************************************************/
class Tool
{
private:
	static USHORT checksum(USHORT *buff,int size);
public:
	Tool(void);
	~Tool(void);
	// ���ı��ķ�ʽ�����ļ�����
	static CString loadTextFile(CString file);
	// ʮ�����ƴ�תchar����
	static int hex2Char(CString hexData, char* outData);
	// ������charתint  16���Ƶ�
	static int C2I(unsigned short c);
	//��ȡ�ַ�����
	static CString subString(CString str, int offset, int size=0);
	// �ַ���תchar�����  UTF-8����
	static int str2Char(CString str, char* cdata);
	//��Ŀ·��
	static CString exePath;
	// �ַ�����תʮ������
	static CString char2Hex(char* cdata, int offset, int size);
	// char����ת�ַ���
	static CString char2Str(char* data, int offset,const int size);
	//PING IP����������
	static boolean PING(CString ip);
	
	// �޸��ļ���
	static boolean fileRename(CString file, CString newPath);
	//ɾ���ļ�
	static boolean fileDelete(CString path);
	// ���ļ������ļ���
	static boolean mkFileDir(CString fileName);
	// ���ı��ķ�ʽд���ļ�
	static boolean writeTextFile(CString file, CString content);
	// ��ȡʱ��ڵ�yyyyMMddHHmmssSSS
	static CString getTime(CString format);
};

