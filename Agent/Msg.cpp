#include "StdAfx.h"
#include "Msg.h"
#include "Log.h"

HWND Msg::hMain;
CString Msg::dealMsg[20];
int Msg::ideal=0;
Msg::Msg(void)
{
}


Msg::~Msg(void)
{
}


// //发送消息到主线程
void Msg::sendMessage(int type, int code, CString info)
{
//	CString log;
//	log.Format("%d,%d,%s", type, code, info);
//	Log::trace(log, __FILE__, __LINE__);
	dealMsg[ideal] = info;
	PostMessage(hMain, type, code, ideal);
	if(++ideal == imaxDeal) ideal = 0;
	Sleep(300);
}
//初始化，设置消息
void Msg::init(HWND mainThread){
	hMain = mainThread;
}
HWND Msg::getMainThread(){
	return hMain;
}
//获取线程信息
CString Msg::getMessageInfo(int lp){
	if(lp<0 || lp>=imaxDeal) return NULL;
	return dealMsg[lp];
}
