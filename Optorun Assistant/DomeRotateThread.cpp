#include "stdafx.h"
#include "Optorun Assistant.h"
#include "DomeRotateThread.h"

#include "OACnfgDoc.h"
// CDomeRotateThread

//ֻ�ڡ�����cpp���õ��ı���.
#define _OA_DOME_THREAD_DATA_BUFFER_SIZE 20
_stDomeThreadData* GetOneDomeThreadBuffer( )
{
	static struct _stDomeThreadData g_stDomeThreadDataBuf[_OA_DOME_THREAD_DATA_BUFFER_SIZE];
	static int iBufPos = 0;
	iBufPos++;
	if(iBufPos >= _OA_DOME_THREAD_DATA_BUFFER_SIZE)
	{
		iBufPos= 0;
	}
	return &g_stDomeThreadDataBuf[iBufPos];
}

IMPLEMENT_DYNCREATE(CDomeRotateThread, CWinThread)

CDomeRotateThread::CDomeRotateThread()
{
	m_hWnd = NULL;
}

CDomeRotateThread::~CDomeRotateThread()
{
}

BOOL CDomeRotateThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CDomeRotateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDomeRotateThread, CWinThread)
	ON_THREAD_MESSAGE(WM_ITEM3_DOME_ROTATE_MONITOR,  OnDomeRotateMonitor)
END_MESSAGE_MAP()

// CDomeRotateThread message handlers
/******************************************************************
AUTO, OFF, MANUAL: ת��
��������fn_ToggleDomeSwitch
���������int iAUTO_OFF_MANUAL
			#define _ZZY_DOME_SWITCH_AUTO   0
			#define _ZZY_DOME_SWITCH_OFF    1
			#define _ZZY_DOME_SWITCH_MANUAL 2
����ֵ����ȡplc�Ƿ���ȷ
˵�����趨��ǰ Toggle Switch -> Dome Rotation��״̬
��������
		AUTO	OFF	MANUAL
write:	3820.00	.01	.02
display:3873.00	.01	.02
******************************************************************/
bool CDomeRotateThread::fn_ToggleDomeSwitch(int iAUTO_OFF_MANUAL)
{
	ASSERT(iAUTO_OFF_MANUAL< 3 );
	if( WritePLCCIOEvent(3820, 1<<iAUTO_OFF_MANUAL ))
	{
		return true;
	}
	return false;
}

//��ȡ��ǰDome Toggle Switch״̬
bool CDomeRotateThread::fn_GetDomeRotationControl(int &iAUTO_OFF_MANUAL)
{
	WORD wWord;
	if( ReadPLCCIOEvent(3873,wWord) )
	{
		iAUTO_OFF_MANUAL = -1;

		if( wWord & 1<<_ZZY_DOME_SWITCH_AUTO)
			iAUTO_OFF_MANUAL = _ZZY_DOME_SWITCH_AUTO;
		else if(wWord & 1<<_ZZY_DOME_SWITCH_OFF)
			iAUTO_OFF_MANUAL = _ZZY_DOME_SWITCH_OFF;
		else if(wWord & 1<<_ZZY_DOME_SWITCH_MANUAL)
			iAUTO_OFF_MANUAL = _ZZY_DOME_SWITCH_MANUAL;

		ASSERT(iAUTO_OFF_MANUAL!=-1);		
		return true;
	}
	return false;
}

/******************************************************************
��������fn_GetDomeRotationState
���������	int& iLow,
			int& iSpeed, ��ǰ״̬��Ӧ����ֵ
����ֵ����ȡplc�Ƿ���ȷ
˵������ȡ��ǰ���ת����� Low or high�Լ���ǰ��ת��.
��������
Speed   High	Low	
SERIALA:DM03636	 SERIALA:DM03639  //BCD2
state	High	 Low
write:	3810.00	.01	 //SERIALA:03810.01
display:3877.13	.14

#define _ZZY_DOME_ROTATE_LOW	1
#define _ZZY_DOME_ROTATE_HIGH	0
******************************************************************/
bool CDomeRotateThread::fn_GetDomeRotationState(int& iSpeed, int& iLow)
{
	WORD wWord;
	if( ReadPLCCIOEvent(3877,wWord) )
	{	
		iLow = -1;
		if(wWord & 1<< 13)
		{
			iLow = _ZZY_DOME_ROTATE_HIGH;
		}
		else if(wWord & 1<<14)
		{
			iLow = _ZZY_DOME_ROTATE_LOW;
		}
		ASSERT(iLow!=-1);

		if( ReadPLCDMEvent( iLow==_ZZY_DOME_ROTATE_LOW ? 3639 : 3636, wWord, true) )
		{	
			iSpeed = (int)wWord;
			return true;
		}
	}
	return false;
}
bool CDomeRotateThread::fn_GetDomeHighSpeed(int& iSpeed)
{
	WORD wWord;
	if( ReadPLCDMEvent(3636,wWord, true) )
	{	
		iSpeed = (int)wWord;
		return true;
	}
	return false;
}
bool CDomeRotateThread::fn_SetDomeHighSpeed(int iSpeed)
{
	//10���� --> 16����
	WORD wValue = ((iSpeed/100)<<8) + (((iSpeed/10)%10)<<4) +iSpeed%10;
	if( WritePLCDMEvent(3636, wValue))
	{
		return true;
	}
	return false;
}

/*
//���õ�ǰLOW\HIGHת�������ת��
//Low: 10-15    High:10-30, Ҳ�е�OTFC 10-50
��������
Speed   High	Low	
SERIALA:DM03636	 SERIALA:DM03639  //BCD2
state	High	 Low
write:	3810.00	.01	 //SERIALA
display:3877.13	.14
*/
bool CDomeRotateThread::fn_SetDomeRotationState(int iSpeed, int iLow)
{
	ASSERT(iLow == _ZZY_DOME_ROTATE_HIGH || iLow ==_ZZY_DOME_ROTATE_LOW);
	ASSERT(iSpeed>=10 && iSpeed<=50 );

	if( WritePLCCIOEvent( 3810, iLow==_ZZY_DOME_ROTATE_LOW ? 1<<01 : 1<<00 ) )
	{
		//10���� --> 16����
		WORD wValue = ((iSpeed/100)<<8) + (((iSpeed/10)%10)<<4) +iSpeed%10;
		if( WritePLCDMEvent( iLow==_ZZY_DOME_ROTATE_LOW ? 3639 : 3636, wValue))
		{
			return true;
		}
	}
	return false;
}



//��ǰ�Ƿ���ת���У�����Ϊ��ȡ�������ϵ���ʾ��ַ
//SERIALA:03876.04
bool CDomeRotateThread::fn_IsDomeRotation(bool& bRunning)
{
	WORD wWord;
	if( ReadPLCCIOEvent(3876,wWord) )
	{
		bRunning = ( 0 != (wWord & ((WORD)1<<4)));
		return true;
	}
	return false;
}

//���ϵ���λ�����Ƿ�رգ� High�±������㣬��Ҫ��Ϊ���ϡ� ����ʾ
//SERIALA:03876.00
bool CDomeRotateThread::fn_IsDoorSwitchClosed(bool &bClosed)
{
	WORD wWord;
	if( ReadPLCCIOEvent(3876,wWord) )
	{
		bClosed = ( 0 != (wWord & (1<<0)));
		return true;
	}
	return false;
}

bool CDomeRotateThread::fn_ReadDomeRotationCounts(WORD& wRotationCnt)
{
	return ReadPLCDMEvent(9999,wRotationCnt);
}

/****************************************************************
//������ÿ��ʼһ���µ�item���ͷ�һ�α���Ϣ.��OnDomeRotateMonitor��Ӧ
1�����Door Switch������ On.	��������Ϣ.�����˳����� ���ⲿ��g_bTestDomeRotation���ó�false.
2���趨��ǰ���ٶ�,�趨High
3���趨Manual
4������ѭ�� 
	{
		Sleep(400); //�ȴ�400ms.
		1>, ���g_bTestDomeRotation״̬, true����2>��false������Ϣ
		2>, ���Rotation(run)״̬
			a, running,����3>
			b, not running
			{
				��� Door Switch,
				��� Manual.
				������Ϣ���˳�
			}
		3>, ��ȡ��ǰ�� ��������������Ϣ
	}
*****************************************************************/
void CDomeRotateThread::OnDomeRotateMonitor(WPARAM wParam, LPARAM lParam)
{
	int  iSpeed = (int)lParam;

	bool bTest = true; //�Ƿ��ڲ�����
	bool bPlc  = true; //ͨ���Ƿ�����
	_stDomeThreadData* pst=NULL;
	bool bRunning = false;//�Ƿ���ת��
	bool bDoorClosed  = false;//�ſ����Ƿ����
	int  iAuto_off_manual = _ZZY_DOME_SWITCH_AUTO;
	
	int  iCurSpeed   = 5;//����������ѭ�����á�
	int  iCurHighLow = _ZZY_DOME_ROTATE_LOW;
	

	//�����߳���Ӧ��Ϣ
	pst = GetOneDomeThreadBuffer();
	pst->dwTickTime = GetTickCount();
	pst->enStatus   = _stDomeThreadData::enErrStart; //��δ�ؽ�
	PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_START,(LPARAM)pst);


	//��һ�ν���ʱ����ǰ���趨����.
	while(bTest)
	{
		Sleep(300);
		EnterCriticalSection(&gCriticalSection_DomeState);
			bTest = g_bTestDomeRotation;
		LeaveCriticalSection(&gCriticalSection_DomeState);
		if(!bTest)
		{	
			pst = GetOneDomeThreadBuffer();
			pst->dwTickTime = GetTickCount();
			pst->enStatus   = _stDomeThreadData::enErrStop; 
			PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_START,(LPARAM)pst);
			break;				
		}

		bPlc = true; //ÿ��ѭ����ʼ�����¼ٶ�ͨ������.

		
				
		//1,���Door Switch������ On.
		bDoorClosed = false;
		bPlc &= fn_IsDoorSwitchClosed(bDoorClosed);
		if(bPlc && !bDoorClosed)
		{
			pst = GetOneDomeThreadBuffer();
			pst->dwTickTime = GetTickCount();
			pst->enStatus   = _stDomeThreadData::enErrDoorSwitchOff; //��δ�ؽ�
			PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_START,(LPARAM)pst);
			continue;
		}

		//2,�趨��ǰ���ٶȣ������ó�High.
		if(iSpeed<5 || iSpeed>50)
		{//���iSpeed������Χ���򲻽����趨��
			pst = GetOneDomeThreadBuffer();
			pst->dwTickTime = GetTickCount();
			pst->enStatus   = _stDomeThreadData::enErrSpeed; //Speed������Χ
			PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_START,(LPARAM)pst);
		}
		else
			bPlc &= fn_SetDomeRotationState(iSpeed,	_ZZY_DOME_ROTATE_HIGH);

		//3,�趨Dome switchΪManualģʽ
		bPlc &= fn_ToggleDomeSwitch(_ZZY_DOME_SWITCH_MANUAL);
		
		if(!bPlc)
		{
			pst = GetOneDomeThreadBuffer();
			pst->enStatus = _stDomeThreadData::enPlcNoAnswer;
			pst->dwTickTime = GetTickCount();
			PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_START,(LPARAM)pst);
			continue;
		}
		else
		{
			break;
		}
	}
	
	while(bTest)
	{
		Sleep(200);
		EnterCriticalSection(&gCriticalSection_DomeState);
			bTest = g_bTestDomeRotation;
		LeaveCriticalSection(&gCriticalSection_DomeState);

		if(!bTest)
		{	
			pst = GetOneDomeThreadBuffer();
			pst->dwTickTime = GetTickCount();
			pst->enStatus   = _stDomeThreadData::enErrStop; 
			PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_START,(LPARAM)pst);
			break;				
		}
		
		bPlc = true; //ÿ��ѭ����ʼ�����¼ٶ�ͨ������.

		//ȷ�ϴ���ת��״̬.
		bRunning = false;
		bPlc &= fn_IsDomeRotation(bRunning);
		if(bPlc && !bRunning)
		{//����ת��״̬
			pst = GetOneDomeThreadBuffer();
			pst->dwTickTime = GetTickCount();
			pst->enStatus   = _stDomeThreadData::enErrNotRotation; //����ת����
			PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_READ,(LPARAM)pst);
		
			bDoorClosed = false;
			bPlc &= fn_IsDoorSwitchClosed(bDoorClosed);
			if(bPlc && !bDoorClosed)
			{
				pst = GetOneDomeThreadBuffer();
				pst->dwTickTime = GetTickCount();
				pst->enStatus   = _stDomeThreadData::enErrDoorSwitchOff; 
				PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_READ,(LPARAM)pst);
			}
			else if(bPlc && bDoorClosed)
			{
				bPlc &= fn_GetDomeRotationControl(iAuto_off_manual);
				pst = GetOneDomeThreadBuffer();
				pst->dwTickTime = GetTickCount();
				pst->enStatus   = _stDomeThreadData::enErrSwitchNotManual;
				PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_READ,(LPARAM)pst);
			}
			continue; //����������������һ�ּ��!
		}

		//ȷ�ϵ�ǰ���ٶ���������ͬ�����Ѵ��ڸ���״̬
		bPlc &= this->fn_GetDomeRotationState(iCurSpeed,iCurHighLow);
		if(bPlc)
		{
			if(iCurSpeed!=iSpeed)
			{
				pst = GetOneDomeThreadBuffer();
				pst->dwTickTime = GetTickCount();
				pst->enStatus   = _stDomeThreadData::enErrSpeed; 
				PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_READ,(LPARAM)pst);
				continue;
			}
			else
			{
				if( iCurHighLow != _ZZY_DOME_ROTATE_HIGH)
				{
					pst = GetOneDomeThreadBuffer();
					pst->dwTickTime = GetTickCount();
					pst->enStatus   = _stDomeThreadData::enErrHighState; 
					PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_READ,(LPARAM)pst);
					continue;
				}
			}
			
		}

		pst = GetOneDomeThreadBuffer();
		pst->dwTickTime = GetTickCount();
		GetLocalTime(&pst->st);
		bPlc &= fn_ReadDomeRotationCounts(pst->wRdData); //��ȡ����������
		if(bPlc)
		{
			pst->enStatus = _stDomeThreadData::enAckRight;
		}
		else
		{//ͨ�Ų���
			pst->enStatus = _stDomeThreadData::enPlcNoAnswer;
			break;
		}

		PostMessage(m_hWnd,WM_ITEM3_DOME_ROTATE_DATA,_ZZY_DOME_ROTATE_DATA_WPARAM_READ,(LPARAM)pst);
	}
	return;
}