// PlcRdWrThread.cpp : implementation file
//

#include "stdafx.h"
#include "Optorun Assistant.h"
#include "PlcRdWrThread.h"
#include "OACnfgDoc.h"

//extern COACnfgDoc g_COACnfgDoc;
// CPlcRdWrThread

//�ɽ� g_hEvent_plc �� g_plc�������ڲ�������AfxBeginThread�����߳�ʱ��suspend���趨����Щ��������Resume thread.
//�����ʹ�÷�������У�����Ť��Υ������ķ�װ���ص㡣
IMPLEMENT_DYNCREATE(CPlcRdWrThread, CWinThread)

CPlcRdWrThread::CPlcRdWrThread(): m_dwLastError(0)
{
}

CPlcRdWrThread::~CPlcRdWrThread()
{
}

BOOL CPlcRdWrThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CPlcRdWrThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPlcRdWrThread, CWinThread)
	ON_THREAD_MESSAGE(WM_PLC_RD_WR_MSG,On_Plc_RW_Command)
END_MESSAGE_MAP()


// CPlcRdWrThread message handlers
void CPlcRdWrThread::On_Plc_RW_Command(WPARAM wParam,  LPARAM lParam) 
{

}

bool CPlcRdWrThread::SetPLCCIOBitEvent(WORD wAddress, WORD wBit)
{
	//WritePLC(enum _enumSysmacWrType enType, int nBeginWord, int nWriteNumber, int  nWriteArray[], char* pszSendBuf, char* pszRecvBuf)
	int nWriteValue = 1 << wBit;

	DWORD dw = WaitForSingleObject(g_hEvent_plc,_VACUUM_PROCESS_WAIT_OBJECT_TIME);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT;
		return false;
	}
	
	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	bool b = g_plc.WritePLC(enWriteCIO,wAddress,1,&nWriteValue,vt_pszSendBuf,vt_pszRecvBuf);	
	SetEvent(g_hEvent_plc);
	if(!b)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_COMM;//ͨ�ų���
	}
	return b;
}


bool CPlcRdWrThread::WritePLCCIOEvent(WORD wAddress, WORD wValue)
{
	DWORD dw = WaitForSingleObject(g_hEvent_plc,_VACUUM_PROCESS_WAIT_OBJECT_TIME);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT;
		return false;
	}
	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	int  nWriteValue = wValue;
	bool b = g_plc.WritePLC(enWriteCIO,wAddress,1,&nWriteValue,vt_pszSendBuf,vt_pszRecvBuf);	
	SetEvent(g_hEvent_plc);
	if(!b)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_COMM;//ͨ�ų���
	}
	return b;
}

//��Event�Ķ�ȡCIO
bool CPlcRdWrThread::ReadPLCCIOEvent(WORD wAddress,WORD &wReadValue)
{
	DWORD dw = WaitForSingleObject(g_hEvent_plc,_VACUUM_PROCESS_WAIT_OBJECT_TIME);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT;
		return false;
	}
	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	bool b = g_plc.ReadPLC(enReadCIO,wAddress,1,&wReadValue,vt_pszSendBuf,vt_pszRecvBuf);	
	SetEvent(g_hEvent_plc); 
	if(!b)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_COMM;//ͨ�ų���
	}
	return b;
}

//bool bDec����ʾ�Ƿ�10����
//true:  10����, Ĭ��Ϊ10����
//false: 16����
bool CPlcRdWrThread::ReadPLCDMEvent(WORD wAddress,WORD &wReadValue,bool bDec /*=true*/) //�ڶ�������Ϊ����
{
	DWORD dw = WaitForSingleObject(g_hEvent_plc,_VACUUM_PROCESS_WAIT_OBJECT_TIME);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT;
		return false;
	}
	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	bool b = g_plc.ReadPLCex(enReadDM,wAddress,1,&wReadValue,bDec,vt_pszSendBuf,vt_pszRecvBuf);	
	SetEvent(g_hEvent_plc); 
	if(!b)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_COMM;//ͨ�ų���
	}
	return b;
}
// дDM��
bool CPlcRdWrThread::WritePLCDMEvent(WORD wAddress, WORD wWriteValue)
{
	DWORD dw = WaitForSingleObject(g_hEvent_plc,_VACUUM_PROCESS_WAIT_OBJECT_TIME);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT;
		return false;
	}

	int iValue = wWriteValue;

	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	bool b = g_plc.WritePLC(enWriteDM,wAddress,1,&iValue,vt_pszSendBuf,vt_pszRecvBuf);	
	SetEvent(g_hEvent_plc); 
	if(!b)
	{
		m_dwLastError = _VACUUM_PROCESS_LAST_ERR_COMM;//ͨ�ų���
	}
	return b;
}
