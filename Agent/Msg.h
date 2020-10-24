#pragma once
/************************************************************************/
/* ��������Tool::sendMessage�е���Ϣ����                                */
/************************************************************************/
class Msg
{
public:
	Msg(void);
	~Msg(void);
	static const int typeTip = WM_USER + 0x600; //��ʾ��Ϣ
	static const int typeBar = WM_USER + 0x601; //�Խ������Ŀ���
	static const int typeHide = WM_USER + 0x602;//���ص���ǰ����
	//��Ϣ��
	static const int codeDefault = 0;//Ĭ����Ϣ��
	//��ȡ��Ϣ
	static CString getMessageInfo(int lp);
	//������Ϣ�����߳�
	static void sendMessage(int type, int code, CString info="");
	static void init(HWND mainThread);//��ʼ�����߳�
	static HWND getMainThread();//��ȡ���߳�
private:
	//���߳�
	static HWND hMain;
	//��Ϣ��������ֵ
	static const int imaxDeal=20;
	//�߳�֮��Ľ�����Ϣ, 
	static CString dealMsg[imaxDeal];
	//������Ϣ���±�
	static int ideal;
};

