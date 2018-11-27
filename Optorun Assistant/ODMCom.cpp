#include "StdAfx.h"
#include "ODMCom.h"

CODMCom::CODMCom(void)
{
	InitComm();
}

CODMCom::~CODMCom(void)
{
}

void CODMCom::InitComm(void)
{
	//�ȴ�ʱ�䣬�����÷��μ� ReciveData����
	m_dwWaitTime = 200; //200ms
	m_dwSleepTime = 10; //10ms

	//֡��β��ʽ
	m_enumCommEndType = enCommEitherChar;
	//֡��β�����ַ���
	*m_pszRecEnd     = 0x06;
	*(m_pszRecEnd+1) = 0x15;
	*(m_pszRecEnd+2) = 0;
	
	m_strCom = TEXT("COM2");
	m_hCom  = INVALID_HANDLE_VALUE;

	m_dcb.DCBlength = sizeof(DCB);
	m_dcb.BaudRate = 9600; 
	m_dcb.ByteSize =8; 
	m_dcb.Parity = NOPARITY; 
	m_dcb.StopBits = ONESTOPBIT ;
	m_dcb.fBinary = TRUE; 
	m_dcb.fParity = false;

	m_CommTimeouts.ReadIntervalTimeout = 100; //ms
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 100; 
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000; 
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 100;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;
}
