/*********************************************************************************************************
�ļ���:		SyncCom.cpp
Comment:    ͬ�������շ��� CSyncCom ��ʵ���ļ�.
Author:     cn_zhangziye@yahoo.com.cn
Last time:  2007.09.05
��Ȩ����:   cn_zhangziye@yahoo.com.cn

˵��1��������ν��ͬ���շ���������ָ���ڵ�ͬ��ִ�С�����ָ������ǰ�̡߳�
(ͬ��IO��������ǰ�ĵ����̣߳����첽IO��������IO����ĵ����̼߳���ִ�У��ȵ�IO���󱻴���󣬻�֪ͨ�����̡߳�)
��ʵ�ϣ����۶���ͬ��IO�������첽IO,��IO�����͵�device driver��device driver��ִ�������첽�ġ�
       ���������ֱ�ӵ��ñ���ʱ����Ϊ��������ǰ�̣߳����Եý���������Լ�ʹ���޷���
	   �Ը���һ�㣬�ɿ��߳����С� �����򣨽��棩���߳�֮��ͨ����Ϣ����ͨ�š�
˵��2��������ͨ��ͨ������Ӧ��ʽ��
       ����λ��(pc)��������󣬵ȴ���λ��(����)�Ե�ǰ�������Ӧ��pc�����ٶ���Ӧ���ݽ��з������Ա���һ��������
	   ������Եľ�������Ӧ��ʽ��
	   ����Ϊһ��һ����������λ���Ƚ����ױ�̡�
˵��3������� InitComm() �����õ�ͨ�ŷ�ʽû��Ӳ���������ã������� 3�� ���ӡ�
       ʹ���߿ɸ���������Ҫ��������ͨ�ŷ�ʽ��
˵��4������ͻ����һ����� ������ ֡��β �жϡ�

�� CSyncCom ʹ�ò���:
1, ���� �����ͷ�ļ� 
   #include "SyncCom.h"
2, ��ӳ�Ա����  CSyncCom m_SyncCom. (���������Զ�)
3, �趨 �ж�֡��β�ķ�ʽ��COM�˿ںš��˿ڲ����ȡ� �μ� InitComm()����.
   �� InitComm������ֱ�������� OMRON SYSMAC CS/CJ ϵ��PLCͨ��(���˶˿ں���). 
4, �򿪴��� OpenCom
  
5, ��������  SendCommand  //�����������ַ�����ʽ������λ����Э��������
6, ��������  ReciveData   

ѭ�� 5~6

//����ĺ������������ԡ�

���ó�Ա�������ý���֡��β��ʽ
m_SyncCom.m_enumCommEndType = CSyncCom::enCommNone;

ע��1�����಻���̰߳�ȫ�͡����߳�Ӧ�����Ӧ����ע�⣬��������� Event �� ͬ���ֶΡ�
ע��2���ڲ�֡��������СΪ512 ( _COM_PORT_BUF_LENGTH )����Ĭ��ʹ���ڲ�������ʱ������û���������飬���Կɽ��յ�֡���ȱ���С��512�ֽڡ�
       
bug 1: ��������ĳ���������� CSyncCom myCom �����շ������붨���ں������⣬�����Ա��
       �� SetCommState ��������LastError = 87, ����Ϊ "��������ȷ"��
       ԭ������δ�����
*********************************************************************************************************/

#include "stdafx.h"
#include "SyncCom.h"

CSyncCom::CSyncCom(void):m_dwBytesRead(0)
{
	m_dwLastError = _SYNCCOM_ERROR_NONE_;
	*m_pszAPIErrStrBuf = _T('\0');
	*m_pszAPIErrFunBuf = _T('\0');
	*m_pszRecvBuf = _T('\0');
	*m_pszSendBuf = _T('\0');
	InitComm();
}

CSyncCom::~CSyncCom(void)
{
	CloseCom();
}


//���������践��ֵ����Ϊ������ʧ��.
void CSyncCom::CloseCom(void)
{
	if( IsOpen() )
	{
		::CloseHandle(m_hCom);	//����ǰ���رմ򿪵� COM ��.
	}
	m_hCom = INVALID_HANDLE_VALUE;
}

void CSyncCom::InitComm(void)
{
	//�ȴ�ʱ�䣬�����÷��μ� ReciveData����
	m_dwWaitTime = 200; //200ms //�ܵ���ȴ�ʱ��
	m_dwSleepTime = 10; //10ms  //���β�ѯ���

	//���²���Ĭ�Ͽɶ�ȡ OMRON PLC
	//֡��β��ʽ
	m_enumCommEndType = enCommSubString;
	//֡��β�����ַ���
	*m_pszRecEnd = '*';
	*(m_pszRecEnd+1) = 0x0D;
	*(m_pszRecEnd+2) = 0;
	
	m_strCom = TEXT("COM1");
	m_hCom  = INVALID_HANDLE_VALUE;
	
	m_dcb.DCBlength = sizeof(DCB);
	m_dcb.BaudRate  = 38400; 
	m_dcb.ByteSize  = 7; 
	m_dcb.Parity    = EVENPARITY;
	m_dcb.StopBits  = TWOSTOPBITS; 
	m_dcb.fParity   = true; 
	m_dcb.fBinary   = true;

	m_CommTimeouts.ReadIntervalTimeout = 100; //ms
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 100; 
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000; 
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 100;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;
}

bool CSyncCom::SetComPort(unsigned int  iPort)
{
	if( IsOpen() )
	{
		m_dwLastError = _SYNCCOM_ERROR_RENAME_COM_;
		return false;
	}
	else
	{

		if( iPort >= 256 )
		{
			m_dwLastError = _SYNCCOM_ERROR_COM_OVER_255_;
			return false;
		}
		else
		{
			m_strCom.Format(TEXT("COM%d"),iPort);
			return true;
		}
	}
}

bool CSyncCom::SetComPort(LPCTSTR strComPort)
{
	//�˿ڴ򿪵�����£��������޸Ķ˿����ƣ�
	if(IsOpen())
	{
		m_dwLastError = _SYNCCOM_ERROR_RENAME_COM_;
		return false;
	}
	else
	{
		m_strCom = strComPort;
		return true;
	}
}

bool CSyncCom::SetComPortDCB(DCB& dcb)
{
	memcpy(&m_dcb,&dcb,sizeof(DCB));
	if( IsOpen() )
	{//�������£�
		return ::SetCommState(m_hCom,&m_dcb);
	}
	return true;	
}

bool CSyncCom::SetWaitTime(DWORD dwNewWaitTime)
{
	m_dwWaitTime = dwNewWaitTime;
	return true;
}

bool CSyncCom::SetSleepTime(DWORD dwNewSleepTime)
{
	m_dwSleepTime = dwNewSleepTime;
	return true;
}

bool CSyncCom::OpenCom(void)
{
	if( INVALID_HANDLE_VALUE != m_hCom ) 
	{
		CloseCom(); //�ȹرգ������´򿪣�
		//return true;
	}
	
	m_hCom = ::CreateFile( (LPCTSTR)m_strCom ,
		GENERIC_READ | GENERIC_WRITE, 
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(m_hCom != INVALID_HANDLE_VALUE)
	{
		if( ::SetCommMask(m_hCom, EV_ERR|EV_RXCHAR) )
		{
			if(::SetCommTimeouts( m_hCom, &m_CommTimeouts ))
			{
				if(::SetCommState(m_hCom,&m_dcb))
				{
					::SetupComm( m_hCom, 512,512) ; 
					::PurgeComm( m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR	| PURGE_RXCLEAR ); 
					return true;
				}
				else
				{
					_tcscpy(m_pszAPIErrFunBuf, _T("CSyncCom::OpenCom -> ::SetCommState"));					
				}
			}
			else
			{
				_tcscpy(m_pszAPIErrFunBuf, _T("CSyncCom::OpenCom -> ::SetCommTimeouts"));					
			}
		}
		else
		{
			_tcscpy(m_pszAPIErrFunBuf, _T("CSyncCom::OpenCom -> ::SetCommMask"));
		}
		m_dwLastError = _SYNCCOM_ERROR_SYS_API_ ;
		m_dwLastSysApiError = ::GetLastError();
		CloseCom();
	}
	else
	{
		m_dwLastError = _SYNCCOM_ERROR_OPEN_COM_ ;
		m_dwLastSysApiError = ::GetLastError();
	}
	return false;
}

//�˺�����unicode ���͵��ַ���������unicodeת��Ϊ��unicode����char���ͣ�����Ϊ����ֻ�ܴ���char��������
BOOL CSyncCom::SendCommand(const wchar_t* strCmd, unsigned int iLength, char* pcBuf)
{
	if(m_hCom == INVALID_HANDLE_VALUE)
	{
		m_dwLastError = _SYNCCOM_ERROR_HANDLE_ ;
		return false;
	}

	char* pCmd = ( ( NULL != pcBuf ) ? pcBuf : m_pszSendBuf );    //ת��Ϊchar���ͻ������Ƿ���ڣ��������Լ�����һ��������
	DWORD dwBytesToWrite =  (( 0!=iLength) ?  iLength : wcslen(strCmd)  );		//����������ݳ����Ƿ������ã�δ���ó����Զ�����Ϊ�������ݵĴ�С
	
	DWORD dwBytesWritten;
	::WideCharToMultiByte(CP_ACP,            // ANSI Code Page
			0,                 // no flags
			strCmd,                 // source widechar string
			-1,                // assume NUL-terminated
			pCmd,                 // target buffer
			_COM_PORT_BUF_LENGTH, // target buffer length
			NULL,              // use system default char
			NULL);             // don''t care if default used

	//����շ�����������
	PurgeComm(m_hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);
	return WriteFile(m_hCom,pCmd,dwBytesToWrite,&dwBytesWritten,NULL); 
}

BOOL CSyncCom::SendCommand(const char* pCmd ,unsigned int iLength)
{
	if(m_hCom == INVALID_HANDLE_VALUE)
	{
		m_dwLastError = _SYNCCOM_ERROR_HANDLE_ ;
		return false;
	}

	DWORD dwBytesToWrite = ( ( 0 != iLength ) ? iLength : strlen(pCmd) );
	DWORD dwBytesWritten;
	//����շ�����������
	PurgeComm(m_hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);
	if(WriteFile(m_hCom,pCmd,dwBytesToWrite,&dwBytesWritten,NULL))
	{
		return true;
	}
	else
	{
		_tcscpy(m_pszAPIErrFunBuf, _T("CSyncCom::SendCommand -> ::WriteFile"));
		m_dwLastError = _SYNCCOM_ERROR_SYS_API_;
		m_dwLastSysApiError = ::GetLastError();
		return false;
	}
}

/*******************************************************************************************
�������ƣ�CSyncCom::SetFrameEndStr(const char* pszEnd)
���������const char* pstrEnd���µ� ����ͨ�� ������־�ַ���
���ز�������
comment:  ���ĳ��������������ñ��������н�β�����趨�� ������ȷ���ա�
          �ɽ�һ���ֳ� ���ַ�( <CR=0x0D>, VGC023 )�� һ���ַ���(*,<CR=0x0D> PLC )��
		  ���������ַ�����(<ACK=0x06> �� <NAK=0x15>)��
********************************************************************************************/
void CSyncCom::SetFrameEndStr(const char* pszEnd)
{
	strcpy(m_pszRecEnd, pszEnd);
}

/*******************************************************************************************
��������:bool CSyncCom::ReceiveData(DWORD& dwBytesRead, unsigned int iBufSize, char* pcBuf )
�������:DWORD& dwBytesRead ,���� ���յ��� �ַ���
		 unsigned int iBufSize, ���ջ�����pcBuf������
         char* pcBuf, ���ջ�����. ��Ϊ NULL,���� ���� ���ջ�����
         ��Ϊ�ڲ��趨���������ֽ���Ϊ 512, Զ����ͨ����֡����, ����û��������������쳣����.
���ز���:�������״̬
         ���������֡��β���ţ�����յ�֮�󷵻� true������ʱ����false��
Comment: ֡���ձ�־
	enCommNone,       //û�������ַ�(��). ����ֻҪ���յ��ַ���,�������˳�.  SleepTime�㹻���Ļ�,�ܽ���������֡.
	enCommOneChar,    //һ�������ַ�,ͨ��Ϊ <CR> ,�� 0x0D
	enCommSubString,  //�����ַ���������2���ַ����ϵ��ַ�����,���� OMRON PLCҪ�� "*\x0D"
	enCommEitherChar  //������ĸ�е�����һ��(2���ַ��е���һ��). //�е������������ɹ�ʱ����һ��ȷ���ַ������򷵻���һ���ַ���ʾʧ�ܡ�
˵��:    һ�����շ���������޶� m_dwWaitTime ���Բ������Ļ���������ͨ����ҪΪ�������㹻��ʱ��.
         ����Ӧ�õķ���֮�����ڱ�������
		 ͨ������SendCommand֮��Ϳ���������ReceiveData������
*******************************************************************************************/
bool CSyncCom::ReceiveData(DWORD& dwBytesRead, unsigned int iBufSize, char* pcBuf )
{
	dwBytesRead = 0;
	if( m_hCom == INVALID_HANDLE_VALUE ) //
	{
		m_dwLastError = _SYNCCOM_ERROR_HANDLE_ ;
		return false;
	}
	
	char* lpszRecBuf = ( (NULL==pcBuf) ? m_pszRecvBuf : pcBuf ); //�����������ʱû��ָ������buffer,�����ڲ�buffer
	char* lpRec = lpszRecBuf; //����ָ��

	*lpRec = '\0'; //(�൱��)��ս��ջ�����

	DWORD dwErrorFlags = 0;
	DWORD dwOneRead = 0; //���ν��յ������ݸ���

	SetCommMask( m_hCom , EV_RXCHAR );
	DWORD dwStart = GetTickCount();
	COMSTAT ComStat ;
	char* ptemp = NULL;
	BOOL bEnd = false;  //������־
	while( GetTickCount() - dwStart < m_dwWaitTime )
	{ 
		//�߳��ó���ǰCPUʱ��Ƭ, ��ȴ���Ӧ
		Sleep(m_dwSleepTime); 
		//�쿴������������
		ClearCommError(m_hCom, &dwErrorFlags, &ComStat );
		if( 0 != ComStat.cbInQue )
		{				
			if(ReadFile(m_hCom,lpRec,ComStat.cbInQue,&dwOneRead,NULL))
			{
				lpRec += dwOneRead;
				*lpRec = '\0' ; //��������ַ�����β��

				/*���ҵ���β���ţ����˳���*/
				switch(m_enumCommEndType)
				{
				case enCommOneChar: 
					//���β��ҽ��յ������ݣ�ֱ�����ҵ� ��β��־���š� 
					//������ĩβ�ַ����enCommOneCharEx,2007.11.17
					//�е���λ��(����)�������ͼ�֡���ݣ����� VGC023���˴����յ�1����֡��־���˳���
					//��RecevieData���÷�����ʵ��֡���жϡ�
					for( ptemp = lpRec-dwOneRead; *ptemp != '\0'; ptemp++ )
					{
						if( *ptemp == m_pszRecEnd[0] )
						{
							bEnd = true;
							break;
						}
					}
					break;					

				case enCommSubString:		//���ַ���
					if( NULL != strstr(lpszRecBuf,m_pszRecEnd))
					{
						bEnd = true;
					}
					break;

				case enCommEitherChar:      //��ѡһ�ַ�����Ӧ DM-1(�������������Ͷ�֡)
					if( *(lpRec-1)==m_pszRecEnd[0] || *(lpRec-1)==m_pszRecEnd[1] )
					{
						bEnd = true;
					}
					break;
					/*
					for(ptemp = lpRec-dwOneRead-1; *ptemp!='\0'; ptemp++)
					{
						if( *ptemp==m_pszRecEnd[0] || *ptemp==m_pszRecEnd[1] )
						{
							bEnd = true;
							break;
						}
					}
					break;
					*/
				case enCommOneCharEx:					
					//������β�ַ��������һ���ַ��Ƿ���ϣ�
					if(*(lpRec-1) ==  m_pszRecEnd[0] )
					{
						bEnd = true;
					}
					break;					
					
				case enCommNone:
				default:
					bEnd = true;
					break;					
				}

				dwBytesRead += dwOneRead;
				//�б���
				if(bEnd)
				{
					return true;
				}
				else
				{
					continue; //û���ҵ�,����� while ѭ��,ֱ����ʱ����!
				}
			}
		}
	}

	if(dwBytesRead==0)
	{
		m_dwLastError = _SYNCCOM_ERROR_RECV_TIMEOUT_ ; 
	}
	else
	{
		m_dwLastError = _SYNCCOM_ERROR_RECV_TIMEOUT_NO_END ; 
	}
	return false;
}


/***********************************************************************************
�������ƣ�GetApiLastErrorStr
���������DWORD dwApiErrCode������� Windows API �����Ĵ������
          LPTSTR lpszBuf�������ַ�����buffer�������߸�buffer
		  int iBufSize��buffer�Ĵ�С
�����������������ͬ LPTSTR lpszBuf�����ؽ����ַ���
comment:  ��ȡ API ��������Ľ��ͣ��ɵ��÷��������ַ����Ļ����������С
************************************************************************************/
bool CSyncCom::GetApiLastErrorStr(DWORD dwApiErrCode, LPTSTR lpszBuf, int iBufSize)
{
	if(iBufSize==0)
		return false;

	*lpszBuf = _T('\0');

	if(0!=::FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL, 
		dwApiErrCode, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
		lpszBuf, 
		iBufSize, 
		NULL 
		) )
		return true;	   
	else
	{
		_tcscpy(lpszBuf,_T("CSyncCom::FormatMessage Error: Unknown API Error Code"));
	}
	    return false;	
}