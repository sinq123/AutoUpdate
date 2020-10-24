#pragma once
/************************************************************************/
/* 日志记录的类                                                         */
/************************************************************************/
class Log
{
public:
	static void trace(char* file, int line, CString msg, ...);
	static void debug(char* file, int line, CString msg, ...);
	static void warn(char* file, int line, CString msg, ...);
	static void error(char* file, int line, CString msg, ...);
	static void fatal(char* file, int line, CString msg, ...);
	Log(void);
	~Log(void);

private:
	static void log(CString level, CString msg, char* file, int line);
};

