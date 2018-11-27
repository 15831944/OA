#pragma once
#include "plcrdwrthread.h"

class CRotatorThread :public CPlcRdWrThread
{
	DECLARE_DYNCREATE(CRotatorThread)
public:
	CRotatorThread(void);
public:
	virtual ~CRotatorThread(void);

public:
	HWND  m_hWnd; //��������Ϣ�Ĵ���, ���߳�����ʱ��ʼ��
protected:
	afx_msg void OnHearthRotateMonitor(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	bool fn_GetHearthInfo(int iHearth, bool& bHearthExist, int& iCrucibleNumber);
public:
	bool fn_GetHearthCoderValue(int iHearth, int& iCoderVal);
public:
	bool fn_GetHearthCrucibleNumber(int iHearth, int& iCrucibleNumber);
public:
	// ��ʼ�� Hearth ת����ʽ�� ��������ʵ������ 0 ĳDM����
	bool fn_InitHearthToPT(int iHearth);
public:
	bool fn_RunCruciblePT(int iHearth);
	bool fn_RunAnnularHearthPT(int iHearth,bool bPositiveRotation);
public:
	// �趨������λ�ã�����������ʾ��λ�ã�
	bool fn_SetCruciblePosPT(int iHearth, int iPos);

public:
	bool fn_GetHearthSpeed(int iHearth, int& iSpeed);
public:
	bool fn_SetHearthSpeed(int iHearth, int iSpeed);
public:
	bool fn_GetHearthRunState(int iHearth, bool& bRunning, bool bAnnular);
public:
	bool fn_SwitchAnnularHearthPT(int iHearth);
public:
	// ��ȡ��ǰ���������Ŀ���״̬,FIX or  ROTATE
	bool fn_GetAnnularHearthCntrl(int iHearth, int& iControlState);
public:
	bool fn_GetHearthMonitorType(int iHearth, int& iType);
};