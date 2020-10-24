#pragma once
/************************************************************************/
/*           工具类                                                     */
/************************************************************************/
class Tool
{
private:
	static USHORT checksum(USHORT *buff,int size);
public:
	Tool(void);
	~Tool(void);
	// 以文本的方式加载文件内容
	static CString loadTextFile(CString file);
	// 十六进制串转char数组
	static int hex2Char(CString hexData, char* outData);
	// 单个的char转int  16进制的
	static int C2I(unsigned short c);
	//截取字符串的
	static CString subString(CString str, int offset, int size=0);
	// 字符串转char数组的  UTF-8编码
	static int str2Char(CString str, char* cdata);
	//项目路径
	static CString exePath;
	// 字符数组转十六进制
	static CString char2Hex(char* cdata, int offset, int size);
	// char数组转字符串
	static CString char2Str(char* data, int offset,const int size);
	//PING IP，测试网络
	static boolean PING(CString ip);
	
	// 修改文件名
	static boolean fileRename(CString file, CString newPath);
	//删除文件
	static boolean fileDelete(CString path);
	// 给文件创建文件夹
	static boolean mkFileDir(CString fileName);
	// 以文本的方式写入文件
	static boolean writeTextFile(CString file, CString content);
	// 获取时间节点yyyyMMddHHmmssSSS
	static CString getTime(CString format);
};

