#pragma once
// "ZzyWaitingThread.h"

// ���ڽϳ�ʱ��ȴ�.  ʱ�䲢����ȷ, ׼ȷ��ͨ��Ϊ�뼶.
// �ȴ�����
// ����������͵ȴ���Ϣ(��λbWaiting)�� ���߳���Ӧ��Ϣ�����еȴ�ָ��ʱ�䣬����ñ���(bWaiting)���˳���

// ����������͵���ϢӦ��Ϊ���½ṹ _stZzyWaiting.

// ��������� ����ýṹ�����ά����������ʼ�� CRITICAL_SECTION cs_ZzyWaiting��

// ��������� Ӧ��ʱ����ĳ��ѭ���еȴ��ñ��������

#define WM_ZZY_WAITING_THREAD_WORK (WM_USER + 400)

struct _stZzyWaiting{
	bool bWaitingStart;             //�����������߳�ʹ��, ����ͬ������. ��ʼ�� Ϊfalse. 
	                                //��Ҫ�ȴ�ʱ�� �ó� true.

	bool bWaiting;                  //�������߳����� true, ���߳��ڵȴ�ʱ��ﵽ�������ó� false. ��Ҫͬ������.
	                                //�������߳������ú�, ����ĳ�ֻ��ƽ���ȷ��(ѭ��/��ʱ/��Ϣ��Ӧ��)
	
	bool bCancelWaiting;            //�������̷߳�����Ϣʱ�������ó� false, ��;�����Ҫ ȡ��,���� �� true.
	                                //���߳�ֻ �����ȡ, ��Ҫ����ͬ������
	
	CRITICAL_SECTION cs_ZzyWaiting; //ͬ����������
	
	DWORD dwWaitingMilliSeconds;    //�ȴ�ʱ��, ��λΪms. ���Բ�ͬ������.

	DWORD dwStartWaitingTickCount;  //�����������߳�ʹ�ã���¼������Ϣʱ�� ϵͳTickCount,�����ڽ����ʱ���ж�.
	                            
	                                //��ʵ���߳̿��Խ��� dwStartWaitingTickCount��dwWaitingMilliSeconds,�Լ�bWaiting���ɽ���ʱ���ж�.������ô�鷳. ��ʼʱbWaiting = false;
};

// CZzyWaitingThread
class CZzyWaitingThread : public CWinThread
{
	DECLARE_DYNCREATE(CZzyWaitingThread)

protected:
	CZzyWaitingThread();           // protected constructor used by dynamic creation
	virtual ~CZzyWaitingThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	afx_msg void OnZzyWaiting(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


