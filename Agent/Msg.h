#pragma once
/************************************************************************/
/* 用来定义Tool::sendMessage中的消息类型                                */
/************************************************************************/
class Msg
{
public:
	Msg(void);
	~Msg(void);
	static const int typeTip = WM_USER + 0x600; //显示信息
	static const int typeBar = WM_USER + 0x601; //对进度条的控制
	static const int typeHide = WM_USER + 0x602;//隐藏掉当前窗口
	//消息码
	static const int codeDefault = 0;//默认消息码
	//获取消息
	static CString getMessageInfo(int lp);
	//发送消息到主线程
	static void sendMessage(int type, int code, CString info="");
	static void init(HWND mainThread);//初始化主线程
	static HWND getMainThread();//获取主线程
private:
	//主线程
	static HWND hMain;
	//消息数组的最大值
	static const int imaxDeal=20;
	//线程之间的交换消息, 
	static CString dealMsg[imaxDeal];
	//交换消息的下标
	static int ideal;
};

