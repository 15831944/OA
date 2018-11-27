#pragma once

#include "PlcRdWrThread.h"

extern char  vt_pszSendBuf[512]; //vacuum thread buffer
extern char  vt_pszRecvBuf[512];

//û�д���
#define _VACUUM_PROCESS_LAST_ERR_NONE         0 
//�ڹ涨ʱ����δ�ȵ� SingleObject
#define _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT  1 
//plc ͨ�ų�������ݳ������һ�����
#define _VACUUM_PROCESS_LAST_ERR_COMM         2 

//�ȴ�object��ʱ��, ms
#define _VACUUM_PROCESS_WAIT_OBJECT_TIME      1000

/***********************************2018-8-29 Zhou Yi(�̺߳��������˶�ε���������ʱ��Ϊû�б�Ҫ)********************************************************/
VOID CALLBACK gfn_VacuumTimerProc(
	HWND hwnd,     // handle of window for timer messages
	UINT uMsg,     // WM_TIMER message
	UINT idEvent,  // timer identifier
	DWORD dwTime   // current system time
);

// CVacuumProcessThread
class CVacuumProcessThread : public CPlcRdWrThread
{
	DECLARE_DYNCREATE(CVacuumProcessThread)

protected:
	CVacuumProcessThread();           // protected constructor used by dynamic creation
	virtual ~CVacuumProcessThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	HWND  m_hWnd; //��������Ϣ�Ĵ���, ���߳�����ʱ��ʼ��
protected:
	afx_msg void OnVacuumThreadTimerEx(WPARAM wParam, LPARAM lParam);
	afx_msg void OnItem2VacuumThreadQA(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
/*
public:
	//дplc CIO ĳһλ Ϊ1�� ��Ӧ����������
	bool SetPLCCIOBitEvent(WORD wAddress, WORD wBit);

	//дplc CIO
	bool WritePLCCIOEvent(WORD wAddress, WORD wValue);
	
	bool ReadPLCCIOEvent(WORD wAddress,WORD &wReadValue);
	bool ReadPLCDMEvent(WORD wAddress,WORD &wReadValue,bool bDec = true);
	
public:
	DWORD m_dwLastError;
	*/
public:
	// ȡ�� VGC023����ն�
	bool fn_GetVGCch3Pressure(double& dPressure);
public:
	// ��������Ƿ�ر�
	bool fn_CheckMainValveStatus(bool& bOpen);
public:
	bool fn_SwitchMainValve(void);
public:
	bool fn_CheckExhaustMode(bool& bExhaustModeManual);
public:
	bool fn_SetExhaustMode(bool bManual);
public:
	// ��ѯ�Ƿ���ɢ��
	bool fn_IsDiffusionPump(bool& bDiffusionPump);
public:
	bool fn_IsIonGaugeRemoteControl(bool& bRemoteControl);
};



