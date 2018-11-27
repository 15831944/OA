// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//2007.09.12, by zhang ziye
#ifndef MAX_PAHT
#define MAX_PAHT 512
#endif

//����/©�ʲ���ʱ��plc��ȡ�˿�
#define _VACUUM_READ_PLC_ADDRESS_CNT            4
#define _VACUUM_READ_PLC_ADDRESS_SN_VGC         0
#define _VACUUM_READ_PLC_ADDRESS_SN_IONGAUGE    1
#define _VACUUM_READ_PLC_ADDRESS_SN_TP_1        2
#define _VACUUM_READ_PLC_ADDRESS_SN_TP_2        3

//iStatus����
#define _VACUUM_THREAD_START 0  //�߳̿�ʼ�ɼ�   //�ж�
#define _VACUUM_THREAD_END   1  //�߳̽����ɼ�
#define _VACUUM_NO_ANSWER    2  //����VGC023 3��ͨ�ž�û��Ӧ�� ������󣬲������ɼ�
#define _VACUUM_ACK_RIGHT    3  //�ɼ��ɹ�

struct _stVacuumThreadData
{
	//static int iTest;
	SYSTEMTIME   st;
	int    iStatus;     //״̬�����жϽ��ʹ�� ������״̬��
	DWORD  dwTickTime;  //������ʱ�� ms. ������
	WORD   wRdData[_VACUUM_READ_PLC_ADDRESS_CNT];
};

//��Hearth Thread���͵����������Ϣ LPARAM�ṹ
#define _HEARTH_READ_PLC_ADDRESS_CNT 7//6->7, 2008.07.15
#define _HEARTH_READ_PLC_ADD_1_CODER      0
#define _HEARTH_READ_PLC_ADD_1_CRUCIBLE   1 //cup
#define _HEARTH_READ_PLC_ADD_2_CODER      2
#define _HEARTH_READ_PLC_ADD_2_CRUCIBLE   3
#define _HEARTH_READ_PLC_ADD_ROTATE_CRUCIBLE   4
#define _HEARTH_READ_PLC_ADD_ROTATE_1_ANNULAR   5
#define _HEARTH_READ_PLC_ADD_ROTATE_2_ANNULAR   6

//ֱ�Ӳ��� 6 ������ʡ�ö� �������в���
//������ Hearth1 �� �������뵱ǰλ��(������), Hearth2 �� �������뵱ǰλ��(������). �Լ������� �Ƿ�������ת�ı�־λ
//������λ�ã�����λ��(�����Ƿ��Σ�����ǻ��Σ�����Ӧ�������Թ�)�Լ�ת��״̬

struct _stHearthThreadData
{
	SYSTEMTIME   st;
	int iHearth;
	enum _enHearthThreadDataState{enPlcNoAnswer,enAckRight}enStatus;
	//int    iStatus;     //״̬�����жϽ��ʹ��
	DWORD  dwTickTime;  //������ʱ�� ms. ������
	WORD wRdData[_HEARTH_READ_PLC_ADDRESS_CNT];
};

//2008.08.14
struct _stDomeThreadData
{
	SYSTEMTIME   st;
	enum _enDomeThreadDataState{enPlcNoAnswer,enAckRight,enErrNotRotation,enErrSpeed,enErrHighState,enErrStart,enErrStop,enErrDoorSwitchOff,enErrSwitchNotManual}enStatus;
	//����enAckRight��������Ϊ����״̬��
	//enPlcNoAnswer, plcͨ�Ź��ϡ�
	//enAckRight,��ȷ��ȡ����.
	//enErrStart,��һ�ν�ѭ��ʱ���ʹ���Ϣ�������̳߳�ʼ����
	//enStop,���û���������ϵ�stop��������ʱ���̹߳رռ��, �߳��ڼ�⵽ bstop���˳�ǰ���ʹ���Ϣ
	//enErrNotRotation,û���жϵ���ת��
	//enErrDoorSwitchOff,����λ��Ҫ���ϲ���High״̬ת������û�м�⵽��
	//enErrSwitchNotManual,Auto-off-manual���ز���Manual�ϡ����������Manual�ϲ���ת����
	//enErrSpeed, �ٶ���Ŀ���趨��һ�£��򳬳���Χ
	//enErrHighState, ��ǰ���ڸ���״̬��
	DWORD  dwTickTime;  //GetTickCount()
	WORD wRdData;		//��ȡ����ת������
};


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


