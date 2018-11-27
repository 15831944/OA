#include "StdAfx.h"
#include "RotatorThread.h"
#include "OACnfgDoc.h"


//ֻ�ڡ�����cpp���õ��ı���.
#define _OA_HEARTH_THREAD_DATA_BUFFER_SIZE 20
_stHearthThreadData* GetOneHearthThreadBuffer( )
{
	static struct _stHearthThreadData g_stHearthThreadDataBuf[_OA_HEARTH_THREAD_DATA_BUFFER_SIZE];
	static int iBufPos = 0;
	iBufPos++;
	if(iBufPos >= _OA_HEARTH_THREAD_DATA_BUFFER_SIZE)
	{
		iBufPos= 0;
	}
	return &g_stHearthThreadDataBuf[iBufPos];
}


IMPLEMENT_DYNCREATE(CRotatorThread, CPlcRdWrThread)
CRotatorThread::CRotatorThread(void)
{
	m_hWnd = NULL;
}

CRotatorThread::~CRotatorThread(void)
{
}


BEGIN_MESSAGE_MAP(CRotatorThread, CWinThread)
	ON_THREAD_MESSAGE(WM_ITEM3_HEARTH_ROTATE_MONITOR,  OnHearthRotateMonitor)
END_MESSAGE_MAP()

/*************************************************************************************
�������ƣ�fn_GetHearthInfo
���������[in]  int iHearth ����ѯ��Hearth�� ,����Ϊ��0��1
          [out] bool& bHearthExist, �Ƿ����
		  [out] int&  iCrucibleNumber, ����������Ŀ
����ֵ���������Ƿ���ȷִ�У�
˵����������ѯ��Hearth �Ļ�����Ϣ
*************************************************************************************/
bool CRotatorThread::fn_GetHearthInfo(int iHearth, bool& bHearthExist, int& iCrucibleNumber)
{
	bHearthExist = false;
	iCrucibleNumber = 0;

	ASSERT( iHearth < 2 ); 

	WORD wValue;
	if( ReadPLCDMEvent( 9056, wValue, false) )
	{	
		//���� 9056.00 .01 ��Ӧ Hearth1 Hearth2��
		bHearthExist = ( wValue &  (1<<iHearth))?true:false;
		if(bHearthExist)
		{
			//9057: Hearth1 cup��, 9058: Hearth2 cup��
			if(ReadPLCDMEvent( 9057 + iHearth, wValue, true) )
			{
				iCrucibleNumber = wValue;
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

/*************************************************************************************
�������ƣ�fn_GetHearthCoderValue
���������[in]  int iHearth ����ѯ��Hearth�� ,����Ϊ��0��1
		  [out] int&  iCoderVal 
����ֵ��������(ͨ��)�Ƿ���ȷִ�У�
˵����������ѯ Hearth ����������
*************************************************************************************/
bool CRotatorThread::fn_GetHearthCoderValue(int iHearth, int& iCoderVal)
{
	ASSERT( iHearth < 2 ); 
	WORD wValue;
	if( ReadPLCDMEvent( ( iHearth==0 ? 3886 : 3906) , wValue, true) )
	{
		iCoderVal = (int)wValue;
		return true;
	}
	return false;
}

/*************************************************************************************
�������ƣ�fn_GetHearthCrucibleNumber
���������[in]  int iHearth ����ѯ��Hearth�� ,����Ϊ��0��1
		  [out] int&  iCrucibleNumber
����ֵ���������Ƿ���ȷִ�У�
˵����������ѯ Hearth ��ǰ������
*************************************************************************************/
bool CRotatorThread::fn_GetHearthCrucibleNumber(int iHearth,int& iCrucibleNumber)
{
	ASSERT( iHearth < 2 ); 
	WORD wValue;
	if( ReadPLCDMEvent( ( iHearth==0 ? 3923 : 3922) , wValue, true) )
	{	
		iCrucibleNumber = (int)wValue;
		return true;
	}
	return false;
}

/*************************************************************************************
�������ƣ�fn_GetHearthMonitorType
���������[in]  int iHearth ����ѯ��Hearth�� ,����Ϊ��0��1
		  [out] int&  iType.  1:oriental, 2: omron.
����ֵ���������Ƿ���ȷִ�У�
˵����������ѯ Hearth Monitor������
*************************************************************************************/
bool CRotatorThread::fn_GetHearthMonitorType(int iHearth, int& iType)
{
	ASSERT( iHearth < 2 ); 
	WORD wValue;
	if( ReadPLCDMEvent( ( iHearth==0 ? 9089 : 9091) , wValue, true) )
	{	
		iType = (int)wValue;
		return true;
	}
	return false;
}


// ��ʼ�� Hearth ת����ʽ�� ��������ʵ������ 0 ĳDM���� //Set ACS pos = 0
bool CRotatorThread::fn_InitHearthToPT(int iHearth)
{
	//hearth1 = 6249 (iHearth = 0)
	//hearth2 = 6248 (iHearth = 1)
	ASSERT(iHearth < 2 );
	if( WritePLCDMEvent( iHearth==0 ? 6249 : 6248 , 0 ) )
	{	
		return true;
	}
	return false;
}



//ת�������� (ģ���ֶ����������)��ָ��λ��
bool CRotatorThread::fn_RunCruciblePT(int iHearth)
{
	ASSERT(iHearth<2 && iHearth>=0);

	//<�ֶ�����>Hearth 1,2�ֱ�Ϊ 3813.01, 3814.01
	//1,<�ֶ�����> ������ ( 1 ���� ��plc������Զ��� ),CIO
	if(!WritePLCCIOEvent( 3813+iHearth, 1<<1 ) )
	{
		return false;
	}

	//ͣ�� 200ms��ȷ��PLC�Ѿ���¼����������
	Sleep(200);

	//<�ƶ�> Hearth 1,2 �ֱ�Ϊ 3813.04, 3814.05. ��ʱ<�ֶ�����>��ͬʱΪ1
	//<�ƶ�> Hearth 1,2 �ֱ�Ϊ 3813.05, 3814.05,//2008.03.20����
	//2,<�ֶ�����>��<�ƶ�>ͬʱΪ 1
	//if(!WritePLCCIOEvent( 3813+iHearth, 1<<1 | 1<<(iHearth+4) ) ) 
	if(!WritePLCCIOEvent( 3813+iHearth, 1<<1 | 1<<5 ) )
	{
		return false;
	}

	////����һ��? 08.01.04 debugging
	Sleep(200);
	if(!WritePLCCIOEvent( 3813+iHearth, 0 ) )
	{
		return false;
	}

	return true;
}

//ת��������������/��
//����ǰ��ȷ����������ֹͣ״̬.
bool CRotatorThread::fn_RunAnnularHearthPT(int iHearth,bool bPositiveRotation=true)
{
	ASSERT(iHearth<2 && iHearth>=0);

//	ASSERT(iHearth==0); //2007.12.25,��ΪĿǰ����� Hearth2����������λ��
	//���趨ת������
	//Hearth1 ת�� 3808.04
	//Hearth1 ��ת 3808.02, ��ת 3808.03
	//Hearth2 ��ת 3809.02, ��ת 3809.03 //2008.07.17
	if(!WritePLCCIOEvent(0==iHearth?3808:3809, bPositiveRotation? 1<<2 : 1<<3 ) )
	{
		return false;
	}

	//ͣ�� 300ms��ȷ��PLC�Ѿ���Ӧ
	Sleep(300);

	//�� <��>
	if(!WritePLCCIOEvent( 0==iHearth?3808:3809, 1<<4) )
	{
		return false;
	}

	return true;
}

bool CRotatorThread::fn_SwitchAnnularHearthPT(int iHearth)
{
	if(!WritePLCCIOEvent( 0==iHearth?3808:0000, 1<<4) )
	{
		return false;
	}
	return true;
}

// �趨������λ�ã�����������ʾ��λ�ã�
bool CRotatorThread::fn_SetCruciblePosPT(int iHearth, int iPos)
{
	ASSERT(iHearth<2 && iHearth>=0);
	//��Ϊ 3940�ڲ��� BCD������ʽ��������Ҫ�� iPosαװ�� ���� ���� iPos = 10���� iPos2 = 16.ʵ�ʶ�Ӧ "0010"
	//int iPos2 = 16*(iPos/10) + iPos%10
	if(iHearth==1)
	{//Hearth2��Ŀǰ�в�֪ Hearth1���趨λ�õ�ַ�� 2007.12.18//2008.03.20���Hearth1
		if( WritePLCDMEvent( 3940, 16*(iPos/10) + iPos%10 ) )
		{	
			return true;
		}
		return false;
	}
	else if(iHearth==0)
	{
		if( WritePLCDMEvent( 3995, 16*(iPos/10) + iPos%10 ) )//2008.03.20���Hearth1
		{	
			return true;
		}
		return false;
	}
	return false;
}


/*
struct _stHearthThreadData
{
	SYSTEMTIME   st;
	enum _enHearthThreadDataState{enPlcNoAnswer,enAckRight}enStatus;
	//int    iStatus;     //״̬�����жϽ��ʹ��
	DWORD  dwTickTime;  //������ʱ�� ms. ������
	WORD wRdData[_HEARTH_READ_PLC_ADDRESS_CNT];
};
*/
void CRotatorThread::OnHearthRotateMonitor(WPARAM wParam, LPARAM lParam)
{
	/*
	#define _HEARTH_READ_PLC_ADD_1_CODER      0
#define _HEARTH_READ_PLC_ADD_1_CRUCIBLE   1 //cup
#define _HEARTH_READ_PLC_ADD_2_CODER      2
#define _HEARTH_READ_PLC_ADD_2_CRUCIBLE   3
#define _HEARTH_READ_PLC_ADD_ROTATE_CRUCIBLE   4
#define _HEARTH_READ_PLC_ADD_ROTATE_1_ANNULAR   5
	//2007.12.27���� 3884 ,���ڻ�������1�� 3884.08, ȷ���Ƿ�������ת
	*/
	static enum _enumSysmacRdType enRdType[_HEARTH_READ_PLC_ADDRESS_CNT] = {enReadDM,enReadDM,enReadDM,enReadDM,enReadCIO,enReadCIO,enReadCIO};
	//������ Hearth1 �� �������뵱ǰλ��(������), Hearth2 �� �������뵱ǰλ��(������). �Լ������� �Ƿ�������ת�ı�־λ
	static WORD wHearthRdAddress[_HEARTH_READ_PLC_ADDRESS_CNT] = {3886,3923,3906,3922,10,3884,3885};
	//���Խ������óɹ�������,�ѷ��㶯̬����. 2007.12.29 

	int iHearth = (int)lParam; 
	bool bTest = true;
	bool bPlc  = true;
	_stHearthThreadData* pst;// = GetOneHearthThreadBuffer();
//	PostMessage(m_hWnd,WM_ITEM3_HEARTH_ROTATE_DATA,0,(LPARAM)pst);
	while(bTest)
	{
		Sleep(400);
		EnterCriticalSection(&gCriticalSection_HearthState);
			bTest = g_bTestHearthRotation;
		LeaveCriticalSection(&gCriticalSection_HearthState);
		if(!bTest)
		{
			//AfxMessageBox(TEXT("DEBUG:g_bTestHearthRotation = false. Stop"));
			break;
		}

		bPlc = true;
		pst = GetOneHearthThreadBuffer();
		pst->dwTickTime = GetTickCount();
		GetLocalTime(&pst->st);
		for(int i=0;i<_HEARTH_READ_PLC_ADDRESS_CNT;i++)
		{
			if(enRdType[i] == enReadDM)
			{
				bPlc &= ReadPLCDMEvent(wHearthRdAddress[i],pst->wRdData[i],true);//ȫ�� 10���ƣ�ʡ���ٿ��ռ�.
			}
			else
			{//enReadCIO
				bPlc &= ReadPLCCIOEvent(wHearthRdAddress[i],pst->wRdData[i]);
			}
		}

		if(bPlc)
		{
			pst->enStatus = _stHearthThreadData::enAckRight;
		}
		else
		{//ͨ�Ų���
			pst->enStatus = _stHearthThreadData::enPlcNoAnswer;
			break;
		}

		PostMessage(m_hWnd,WM_ITEM3_HEARTH_ROTATE_DATA,0,(LPARAM)pst);
	}
	return;
}

//��������
bool CRotatorThread::fn_GetHearthSpeed(int iHearth, int& iSpeed)
{
	ASSERT( iHearth < 2 ); 
	WORD wValue;
	if( ReadPLCDMEvent( ( iHearth==0 ? 6238 : 6239) , wValue, true) )
	{	
		iSpeed = (int)wValue;
		return true;
	}
	return false;
}

//��������
bool CRotatorThread::fn_SetHearthSpeed(int iHearth, int iSpeed)
{
	ASSERT( iHearth < 2 ); 
	ASSERT(iSpeed>=0 && iSpeed<1000);
	//�� 3 λ 10������ת�� 16���� ���ɹ� WritePLCDMEvent.
	//WORD wValue = 16*16*(iSpeed/100) + 16*((iSpeed/10)%10)+iSpeed%10;
	WORD wValue = ((iSpeed/100)<<8) + (((iSpeed/10)%10)<<4) +iSpeed%10;
	if( WritePLCDMEvent ( iHearth==0 ? 6238 : 6239, wValue) )
	{	
		iSpeed = (int)wValue;
		return true;
	}
	return false;
}

//���Ի��������Ƿ���ת����.
bool CRotatorThread::fn_GetHearthRunState(int iHearth, bool& bRunning, bool bAnnular = true)
{
	//�������� 1���Ƿ�����ת���� SERIAL:3884.08
	//�������� 2 ��3885.08 ? //2008.07.14
	WORD wVal;
	if( bAnnular)
	{//��������
		if( ReadPLCCIOEvent(0==iHearth?3884:3885,wVal) )
		{
			if( wVal & (1<<8))
			{
				bRunning = true;
			}
			else
			{
				bRunning = false;
			}
			return true;
		}
	}
	else
	{//������
		bRunning = false;
		return true;
	}
	return false;
}

/****************************************************************************
�������ƣ�fn_GetAnnularHearthCntrl
˵��:  ��ȡ���������Ŀ���״̬,FIX or ROTATE
       ������ģ�´�����ROTATEģʽ�¶���,��Ҫ״̬ȷ��.
�������: int iHearth, Hearth��, 0ΪHearth1, 1:Hearth2.
          int& iControlState, ���ض�ȡ���Ŀ���״̬, 0:Fix, 1:Rotate
����ֵ:   ��ȡ�ɹ�?
Hearth1	FIX		ROTATE
	д	3808.00	3808.01
	��	3884.14	3884,15
Hearth2	FIX		ROTATE
	д	3809.00	3809.01
	��	3885.14	3885,15
д��������,�������ȡ״̬
****************************************************************************/
bool CRotatorThread::fn_GetAnnularHearthCntrl(int iHearth, int& iControlState)
{
	ASSERT(iHearth==0||iHearth==1);
	WORD wVal;
	if( ReadPLCCIOEvent(0==iHearth?3884:3885,wVal) )
	{
		if( wVal & (1<<14))
		{
			iControlState = 0;
		}
		else
		{
			iControlState = 1;
		}
		return true;
	}
	return false;
}


