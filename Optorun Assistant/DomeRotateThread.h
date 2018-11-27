#pragma once

#include "plcrdwrthread.h"

#define _ZZY_DOME_ROTATE_LOW	1
#define _ZZY_DOME_ROTATE_HIGH	0

#define _ZZY_DOME_SWITCH_AUTO   0
#define _ZZY_DOME_SWITCH_OFF    1
#define _ZZY_DOME_SWITCH_MANUAL 2

#define _ZZY_DOME_ROTATE_DATA_WPARAM_START		0 //�����׶�
#define _ZZY_DOME_ROTATE_DATA_WPARAM_READ		1 //���ݶ�ȡ�׶�
// CDomeRotateThread //#include "DomeRotateThread"
/********************************************************
��Dome��ת���м�ʱ����Ҫ���̡߳�
���߳�ʵ�ʽ�����ʱ�ɼ����ݣ����͸������̡߳�
Low: 10-15    High:10-30, Ҳ�е�OTFC 10-50
High �±������������������1,����Źر�; 2,ˮ����ȴˮ�򿪡�
AUTO ״̬Ϊ��Ĥר��
OFF  ״̬�¿�ͨ����ס���ϵĿ��ƺа�ť��ת��Dome
MANUAL ״̬��ת��. //High����������������.
*********************************************************/

class CDomeRotateThread : public CPlcRdWrThread
{
	DECLARE_DYNCREATE(CDomeRotateThread)

protected:
	CDomeRotateThread();           // protected constructor used by dynamic creation
	virtual ~CDomeRotateThread();
public:
	HWND  m_hWnd; //��������Ϣ�Ĵ���, ���߳�����ʱ��ʼ��
bool fn_ToggleDomeSwitch(int iAUTO_OFF_MANUAL);		   //AUTO, OFF, MANUAL: ת��
bool fn_GetDomeRotationControl(int &iAUTO_OFF_MANUAL); //��ȡ��ǰDome Toggle Switch״̬
bool fn_SetDomeRotationState(int iSpeed, int iLow);    //���õ�ǰLOW\HIGHת�������ת��
bool fn_GetDomeRotationState(int& iSpeed, int& iLow);  //��ȡ��ǰ���ת����� Low or high�Լ���ǰ��ת��.
bool fn_GetDomeHighSpeed(int& iSpeed);
bool fn_SetDomeHighSpeed(int iSpeed);
bool fn_IsDomeRotation(bool& bRunning);                //��ǰ�Ƿ���ת���У��жϴ������ϵ�
bool fn_IsDoorSwitchClosed(bool &bClosed);			   //���ϵ���λ�����Ƿ�رգ� High�±������㣬��Ҫ��Ϊ���ϡ� ����ʾ
bool fn_ReadDomeRotationCounts(WORD& wRotationCnt);    //��ȡ��ǰת������.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	afx_msg void OnDomeRotateMonitor(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


