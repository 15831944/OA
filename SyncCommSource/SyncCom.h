#pragma once
#define _COM_PORT_BUF_LENGTH         512
#define _SYN_COM_END_STR_LENGTH      12

//�洢ϵͳ���������ַ����Ļ�����������ⲿ����
#define _SYNC_COM_API_ERR_STR_LENGTH 512  

//�������  Error ���ͣ����� _SYNCCOM_DERIVE_ERROR_ ֮�϶���. 100~65535
#define _SYNCCOM_DERIVE_ERROR_					100 

#define _SYNCCOM_ERROR_NONE_					0
#define _SYNCCOM_ERROR_SYS_API_					1  //ϵͳAPI����������ʱm_dwLastSysApiError����¼ϵͳ�� lasterror.
#define _SYNCCOM_ERROR_HANDLE_					2  //COM �˿ڵ� Handle �Ƿ�(COM��δ��)
#define _SYNCCOM_ERROR_OPEN_COM_				3  //COM�ڴ�ʧ�ܣ�����ԭ�� 1���ÿڱ���������ռ�ã�2���ÿڲ�����
#define _SYNCCOM_ERROR_RENAME_COM_				4  //���Ĵ��򿪴��ڳ�����ǰ���ڴ�״̬�������ȹرա�
#define _SYNCCOM_ERROR_COM_OVER_255_			5  //���ĵĴ��ڶ˿ںų��� 255������

#define _SYNCCOM_ERROR_RECV_TIMEOUT_			20  //���մ������ݳ�ʱ��û�н��յ��ַ�
#define _SYNCCOM_ERROR_RECV_TIMEOUT_NO_END		21  //���մ������ݳ�ʱ�����յ��ַ��������޶�ʱ����û�м�⵽֡��β����


class CSyncCom
{
public:
	CSyncCom(void);
public:
	virtual ~CSyncCom(void);
public:
	//������ز���
	HANDLE		 m_hCom;
	CString		 m_strCom;
	COMMTIMEOUTS m_CommTimeouts;
	DCB          m_dcb;

	//������ѭ�������е����ȴ�ʱ��
	DWORD        m_dwWaitTime;
	//������ѭ����������δ�� �ַ� ��ͨ��δ��� �ı����߳�ʱ�䡣
	DWORD		 m_dwSleepTime;

	//���һ�γ������//û�д�����ʱ��������¼�������
	DWORD        m_dwLastError;       
	//���һ�γ��������API��ϵͳ����ԭ�����¼������롣�������������ԭ���򱾴�����������塣
	DWORD        m_dwLastSysApiError;

	//�շ��������������豸ʹ�õ��� char
	char  m_pszSendBuf[_COM_PORT_BUF_LENGTH];
	char  m_pszRecvBuf[_COM_PORT_BUF_LENGTH];
	DWORD m_dwBytesRead;

	TCHAR m_pszAPIErrStrBuf[_SYNC_COM_API_ERR_STR_LENGTH];//ȡ��API�������������
	TCHAR m_pszAPIErrFunBuf[_SYNC_COM_API_ERR_STR_LENGTH];//����API������¼

/************************************************************************************
	֡β��־�ַ�(��):
    ��Ӧ��ʽ(һ��һ��)����ͨ����, ÿ��ͨ�ŵ�����Ϊһ֡.
		�е�ͨ�Ź���, ��֡ͷ��֡β����ʶ������һ֡;
		�е�ֻ��֡β��־�ַ�(��).
	�����ṩ��� ֡β��־�ַ�(��). �����һ�ζ�ȡ֡�Ķ���������֡ͷ���жϣ�������÷����С�
**************************************************************************************/
enum _enumCommFrameEndType
{
	enCommNone,       //û�������ַ�(��). ����ֻҪ���յ��ַ���,�������˳�.  //SleepTime�㹻���Ļ�,�ܽ���������֡.
	enCommOneChar,    //һ�������ַ�,ͨ��Ϊ <CR> ,�� 0x0D������ͷ�ң��ҵ������ַ����˳���
	enCommSubString,  //�����ַ���������2���ַ����ϵ��ַ�����,���� OMRON PLCҪ�� "*\x0D"
	enCommEitherChar,  //������ĸ�е�����һ��(2���ַ��е���һ��)
	enCommOneCharEx   //һ�������ַ�,�������ҽ�β�ַ������� enCommOneChar ����չ��
};

	_enumCommFrameEndType m_enumCommEndType;
	char m_pszRecEnd[_SYN_COM_END_STR_LENGTH]; //����֡�ַ�����β��־����

public:
	inline BOOL IsOpen(void) {return (m_hCom != INVALID_HANDLE_VALUE); }
public:
	void CloseCom(void);
public:
	virtual void InitComm(void);
public:
	bool SetComPort(unsigned int  iPort);
public:
	bool SetComPort(LPCTSTR strComPort);
public:
	bool SetComPortDCB(DCB& dcb);
public:
	bool SetWaitTime(DWORD dwNewWaitTime);				//2018-8-30 δʹ��
public:
	bool SetSleepTime(DWORD dwNewSleepTime);
public:
	bool OpenCom(void);
public:
	BOOL SendCommand(const wchar_t* strCmd, unsigned int iLength = 0, char* pcBuf=NULL);
public:
	BOOL SendCommand(const char* pCmd , unsigned int iLength = 0);
public:
	void SetFrameEndStr(const char* pszEnd);
public:
	bool ReceiveData(DWORD& dwBytesRead, unsigned int iBufSize = 0, char* pcBuf = NULL);
public:

	DWORD GetLastError(void)
	{
		return m_dwLastError;
	}
public:
	bool GetApiLastErrorStr(DWORD dwApiErrCode, LPTSTR lpszBuf = NULL, int iBufSize = 0);
public:

	// are there any characters read last time
	bool IsLastReadAnyChar(void)
	{
		return m_dwBytesRead!=0;
	}
};
