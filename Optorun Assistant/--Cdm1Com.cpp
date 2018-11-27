#include "StdAfx.h"
#include "Cdm1Com.h"

Cdm1Com::Cdm1Com(void)
{
	InitComm();
}

Cdm1Com::~Cdm1Com(void)
{
}

void Cdm1Com::InitComm(void)
{
	m_strCom = TEXT("COM1");
	m_hCom   = INVALID_HANDLE_VALUE;

	//�ȴ�ʱ�䣬�����÷��μ� ReciveData����
	m_dwWaitTime = 500; //200ms
	m_dwSleepTime = 10; //10ms

	//���²������ڶ�ȡ DM-1������XTC/2
	//֡��β��ʽ��Ϊ2���ֽ�ѡ1��
	m_enumCommEndType = enCommEitherChar; 
	m_pszRecEnd[0] = 0x06;
	m_pszRecEnd[1] = 0x15;
	*(m_pszRecEnd+2) = 0;

	m_dcb.DCBlength = sizeof(DCB);
	m_dcb.BaudRate  = 9600; 
	m_dcb.ByteSize  = 8; 
	m_dcb.Parity    = NOPARITY;
	m_dcb.StopBits  = ONESTOPBIT; 
	m_dcb.fParity   = true; 
	m_dcb.fBinary   = false;

	m_CommTimeouts.ReadIntervalTimeout = 100; //ms
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 100; 
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000; 
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 100;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

}
