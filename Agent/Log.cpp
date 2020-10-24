#include "StdAfx.h"
#include "Log.h"
#include  "Tool.h"

#include <stdlib.h>
#include <direct.h>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>

#define MAX_LOGBUF_SIZE 1024*4
#define CURRENT_LEVEL 1
#define LEVEL_TRACE 1
#define LEVEL_DEBUG 2
#define LEVEL_WARN 3
#define LEVEL_ERR 4
#define LEVEL_FATAL 5
Log::Log(void)
{
}


Log::~Log(void)
{
}


static HANDLE hMutex = CreateMutex(NULL, false, NULL);
void Log::log(CString level, CString msg, char* file, int line){
	//STEP 1.创建文件夹,获取系统时间
	CFile cfile ;
	CString stime;
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString logFolder;
	logFolder.Format("%04d%02d", time.wYear, time.wMonth);
	logFolder = "D:\\HDOSFingerDeviceLogs\\"+logFolder;


	WaitForSingleObject(hMutex, INFINITE);
	_mkdir("D:\\HDOSFingerDeviceLogs");
	_mkdir(logFolder);

	//STEP 2.创建文件，输出消息前面的换行
	stime.Format("%04d-%02d-%02d",time.wYear, time.wMonth, time.wDay);
	CString filename = logFolder+"\\" + stime  + ".log";
	cfile.Open (filename, CFile::modeCreate |CFile::modeWrite | CFile::modeNoTruncate );
	cfile.SeekToEnd();
	CString info = "";
	while(msg.Find("\r\n")==0){
		info += "\r\n";
		msg.Delete(0,2);
	}

	//STEP 3.输出日志信息
	stime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d\t", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	level = "[" + level + "]\t";
	CString ftag = "";
	ftag.Format("%s", file);
	int i=ftag.ReverseFind('\\');
	ftag = ftag.Right(ftag.GetLength()-i-1);
	ftag.Format("["+ftag + "(line %d)]\t| ", line);

	info += level;
	info += stime;
	info += ftag;
	info += msg;
	info += "\r\n";
	cfile.Write(info,info.GetLength());
	cfile.Flush();
	cfile.Close();

	ReleaseMutex(hMutex);
}
void Log::trace(char* file, int line, CString msg, ...){
	if(LEVEL_TRACE<CURRENT_LEVEL) return;
	char * msg_buf;
	va_list	args;   //参数对象
	va_start(args,msg); //参数列表
	msg_buf=(char *)malloc(MAX_LOGBUF_SIZE);//为参数申请空间
	ZeroMemory(msg_buf,MAX_LOGBUF_SIZE);
	va_start(args,msg);
	vsprintf( msg_buf, msg, args );  //格式化参数 
	va_end(args);
	msg = msg_buf;
	free(msg_buf); 

	log("TRACE", msg, file, line);
}
void Log::debug(char* file, int line, CString msg, ...){
	if(LEVEL_DEBUG<CURRENT_LEVEL) return;
	char * msg_buf;
	va_list	args;   //参数对象
	va_start(args,msg); //参数列表
	msg_buf=(char *)malloc(MAX_LOGBUF_SIZE);//为参数申请空间
	ZeroMemory(msg_buf,MAX_LOGBUF_SIZE);
	va_start(args,msg);
	vsprintf( msg_buf, msg, args );  //格式化参数 
	va_end(args);
	msg = msg_buf;
	free(msg_buf); 

	log("DEBUG", msg, file, line);
}
void Log::warn(char* file, int line, CString msg, ...){
	if(LEVEL_WARN<CURRENT_LEVEL) return;
	char * msg_buf;
	va_list	args;   //参数对象
	va_start(args,msg); //参数列表
	msg_buf=(char *)malloc(MAX_LOGBUF_SIZE);//为参数申请空间
	ZeroMemory(msg_buf,MAX_LOGBUF_SIZE);
	va_start(args,msg);
	vsprintf( msg_buf, msg, args );  //格式化参数 
	va_end(args);
	msg = msg_buf;
	free(msg_buf); 

	log("WARN", msg, file, line);
}
void Log::error(char* file, int line, CString msg, ...){
	if(LEVEL_ERR<CURRENT_LEVEL) return;
	char * msg_buf;
	va_list	args;   //参数对象
	va_start(args,msg); //参数列表
	msg_buf=(char *)malloc(MAX_LOGBUF_SIZE);//为参数申请空间
	ZeroMemory(msg_buf,MAX_LOGBUF_SIZE);
	va_start(args,msg);
	vsprintf( msg_buf, msg, args );  //格式化参数 
	va_end(args);
	msg = msg_buf;
	free(msg_buf); 

	log("ERROR", msg, file, line);
}
void Log::fatal(char* file, int line, CString msg, ...){
	if(LEVEL_FATAL<CURRENT_LEVEL) return;
	char * msg_buf;
	va_list	args;   //参数对象
	va_start(args,msg); //参数列表
	msg_buf=(char *)malloc(MAX_LOGBUF_SIZE);//为参数申请空间
	ZeroMemory(msg_buf,MAX_LOGBUF_SIZE);
	va_start(args,msg);
	vsprintf( msg_buf, msg, args );  //格式化参数 
	va_end(args);
	msg = msg_buf;
	free(msg_buf); 

	log("FATAL", msg, file, line);
}
