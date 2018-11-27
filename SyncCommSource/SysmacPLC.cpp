#include "StdAfx.h"
#include "SysmacPLC.h"

CSysmacPLC::CSysmacPLC(void)
: m_nUnitNumber(0)
{
	InitComm();
}

CSysmacPLC::~CSysmacPLC(void)
{

}
/********************************************************************************************
Function name: CreateFCS
Parameters:   const char* pstr, ������У��֡���ַ���
			  int   iCount,     pstr�ַ�����У��ĳ���
			  char* pFCS,       У�����洢��
����ֵ��      ָ��pFCS�׵�ַ
comment:      OMRON SYSMAC ϵ��PLC C mode ͨ��У��֡����
���÷�ʽ:     �ɽ� pstr ָ���У���ַ��������ַ��� pFCSָ���У�鴮��FCS����(���ֽ�)����iCount���趨Ϊ
              pFCS-pstr����������������ɺ�pstr���������Ŀɷ��͸�PLC���ַ�����
********************************************************************************************/
char* CSysmacPLC::CreateFCS(const char* pstr, int iCount, char* pFCS)
{
	if(pstr==NULL || pFCS==NULL)
		return NULL;
    char ch = *pstr;

	//�����ַ��������
	for(int i=1;i<iCount;i++)
	{ 
		ch ^= *(pstr+i);
	}	

	//��4λ���һ���ַ� (16���ƣ� 0~F)
	char ct = ch >> 4; 
	if(ct>9) 
		ct += 'A'-10;
	else 
		ct += '0';
    *pFCS = ct; 

	//��4λ���һ���ַ�(16���ƣ� 0~F)
    ct = ch & 0x0F;
	if(ct>9) 
		ct += 'A'-10;
	else 
		ct += '0';

    *(pFCS+1) = ct;
	
	return pFCS;
}

//�����ĸ�ʽ��μ����� CreateFCS
BOOL CSysmacPLC::IsFCSOK(const char* pstr, int iCount, char* pFCS)
{
	char cTmp[2];
	CreateFCS(pstr,iCount,&cTmp[0]);
	if(*pFCS == cTmp[0] && *(pFCS+1)== cTmp[1])
		return true;
	else
		return false;

}
void CSysmacPLC::InitComm(void)
{
	//�ȴ�ʱ�䣬�����÷��μ� ReciveData����
	m_dwWaitTime = 200; //200ms
	m_dwSleepTime = 10; //10ms

	//֡��β��ʽ
	m_enumCommEndType = enCommSubString;
	//֡��β�����ַ���
	*m_pszRecEnd = '*';
	*(m_pszRecEnd+1) = 0x0D;
	*(m_pszRecEnd+2) = 0;
	
	//���²���Ĭ�Ͽɶ�ȡ OMRON PLC
	m_strCom = TEXT("COM1");
	m_hCom  = INVALID_HANDLE_VALUE;
	
	m_dcb.DCBlength = sizeof(DCB);
	m_dcb.BaudRate  = 38400; 
	m_dcb.ByteSize  = 7; 
	m_dcb.Parity    = EVENPARITY;
	m_dcb.StopBits  = TWOSTOPBITS; 
	m_dcb.fParity   = true; 
	m_dcb.fBinary   = TRUE;

	m_CommTimeouts.ReadIntervalTimeout = 100; //ms
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 100; 
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000; 
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 100;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;
}

BOOL CSysmacPLC::SetUnitNumber(unsigned int nUnitNumber)
{
	if( nUnitNumber <= 99 ) //������2λ����
	{
		m_nUnitNumber = nUnitNumber;
		return true;
	}
	else
	{
		return 0;
	}	
}

/***************************************************************************************************
�������ƣ�CreateFCSChar
          ����һ��FCS�ַ�
���������const char* pstr, �������ַ���
          iCount, ָ���������ɵĳ���
����ֵ��  ���ɵ�FCS�ַ�
��ע��    ����������ʱ����һ���м��ַ���������ӳ��ַ���������FCS������Դ��м��ַ���ʼ���Լӿ���������FCS���ٶȡ�
          �ƺ����������������� RD��WD�������У�����Ϊ ��˼��ٵĻ���Ч�ʿ�������ȥһ�㣬����������Խ�����½���
          ��Ϊ����������һ��FCS�������õ���ʱ��ʵ�ʷǳ��̣������������ط���������û���á�
***************************************************************************************************/
char CSysmacPLC::CreateFCSChar(const char* pstr , int iCount)
{
	char chTmp = *pstr;
	for(int i=1;i<iCount;i++)
	{
		chTmp ^= *(pstr+i);
	}
	return chTmp;
}


/******************************************************************************************************
�������ƣ�FormatCmdBufferRD
��ʽ�����������
���������
����ֵ��
comment:  �������ɷ��͸�plc�Ķ�����
note:     ��֧�� RE�����ȡEM������ΪRE��ȡ�����ʽ����ͨ�Ķ�ȡ�������һ��Bank No. ��������Դ��ɡ�
******************************************************************************************************/
BOOL CSysmacPLC::FormatCmdBufferRD(char* lpszBuffer, enum _enumSysmacRdType enType, int nBeginWord, int nReadNumber)
{
	switch (enType)
	{
	case enReadCIO:
		sprintf(lpszBuffer,"@%02dRR%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	case enReadLR:
		sprintf(lpszBuffer,"@%02dRL%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	case enReadHR:
		sprintf(lpszBuffer,"@%02dRH%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	case enReadTC:
		sprintf(lpszBuffer,"@%02dRC%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	case enReadTCStatus:
		sprintf(lpszBuffer,"@%02dRG%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	case enReadAR:
		//nBeginWord: 0~959
		sprintf(lpszBuffer,"@%02dRJ%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	case enReadDM:		
	default:	
		sprintf(lpszBuffer,"@%02dRD%04d%04dXX*\x0D",m_nUnitNumber,nBeginWord,nReadNumber);
		break;
	}
	
	CreateFCS(lpszBuffer,ciRW_FCS_Pos,lpszBuffer + ciRW_FCS_Pos);  //Zhou yi :���任���ַ�������δ�任�ĺ��棬��ַ������13word
	return true;
}

/******************************************************************************************************
�������ƣ�FormatCmdBufferWR
          ��ʽ��д�������
���������char* lpszBuffer�� �������
          enum _enumSysmacRdType enType, ��������
		  int  nBeginWord, ��ʼ�ֵ�ַ
		  int  iWriteNumber, ��д�ֽ���
		  int  nWriteArray[], ��д�������
����ֵ��  
comment:  �������ɷ��͸�plc��д����
note:     1, ��֧�� WE���дEM������ΪWE�����ʽ����ͨ�Ķ�ȡ�������һ��Bank No. ��������Դ��ɡ�
          2, �������÷�����ָ���� nWriteArray, �����������������ֵת�� �� nWriteArray �������
		  3, ����,��λ CIO 3248 �ĵ� n (n:0~15)λ,  nWriteArray��ֵӦ����  1<<n
******************************************************************************************************/
BOOL CSysmacPLC::FormatCmdBufferWR(char* lpszBuffer, enum _enumSysmacWrType enType, int nBeginWord, int nWriteNumber,int nWriteArray[])
{
	int nByteWrite = 0;
	switch (enType)
	{
	case enWriteCIO:
		nByteWrite = sprintf(lpszBuffer,"@%02dWR%04d",m_nUnitNumber, nBeginWord);
		break;
	case enWriteLR:
		nByteWrite = sprintf(lpszBuffer,"@%02dWL%04d",m_nUnitNumber, nBeginWord);
		break;
	case enWriteHR:
		nByteWrite = sprintf(lpszBuffer,"@%02dWH%04d",m_nUnitNumber, nBeginWord);
		break;
	case enWriteTC:
		nByteWrite = sprintf(lpszBuffer,"@%02dWC%04d",m_nUnitNumber, nBeginWord);
		break;
	case enWriteAR:
		nByteWrite = sprintf(lpszBuffer,"@%02dWJ%04d",m_nUnitNumber, nBeginWord);
		break;
	case enWriteDM:
		nByteWrite = sprintf(lpszBuffer,"@%02dWD%04d",m_nUnitNumber, nBeginWord);
	default:
		break;
	}

	for( int i = 0; i < nWriteNumber; i++)
	{
		nByteWrite += sprintf( lpszBuffer + nByteWrite, "%04X", *(nWriteArray+i));
	}
	CreateFCS(lpszBuffer,nByteWrite,lpszBuffer + nByteWrite);	
	
	nByteWrite += 2; //�ܿ�FCS��
	*(lpszBuffer + nByteWrite)	    = '*';
	*(lpszBuffer + nByteWrite + 1)  = '\x0D';  //<CR>
	*(lpszBuffer + nByteWrite + 2)  = '\0';    //����ַ�����β��

	return true;
}

/**********************************************************************************
�������ƣ�DecString2Int
          ��ָ���ַ���ָ������ת���� Int����//������ atoi ���������ֱ�Ӵ���.
���������const char* ps, ��ת����Դ�ַ���
          int iLength,    ��ת���ĳ���
����ֵ��  ת����� int ����.
**********************************************************************************/
int CSysmacPLC::DecString2Int(const char* ps, int iLength)
{
	int sum = 0;
	for(int i=0;i<iLength;i++)
	{
		if(*(ps+i)>='0' && *(ps+i)<='9')
		{
			sum += *(ps+i) - '0' ;
			sum *=10;
		}
	}
	return sum/10;
}

/********************************************************************************************************
�������ƣ�HexString2Int
          �� 16���� �ַ���ת���� int ��
���������const char* ps, Դ�ַ���
          iLength, ��ת���ַ�������
����ֵ��  ת����� int ����ֵ
********************************************************************************************************/
int CSysmacPLC::HexString2Int(const char* ps, int iLength)
{
	int sum = 0;
	for(int i=0; i<iLength; i++)
	{
		if(*(ps+i)>='0' && *(ps+i)<='9')
		{
			sum += *(ps+i) - '0' ;
		}
		else if( *(ps+i)>='A' && *(ps+i)<='F' )
		{
			sum += *(ps+i) - 'A' + 10;
		}
		else if( *(ps+i)>='a' && *(ps+i)<='f' )
		{
			sum += *(ps+i) - 'a' + 10;
		}
		sum <<= 4; //sum = sum*16;				
	}
	return sum >> 4 ;
}
WORD CSysmacPLC::HexString2Word(const char* ps) //iLength = 4
{
	DWORD sum=0; //����� WORD��������.
	for(int i=0; i<4; i++)
	{
		if(*(ps+i)>='0' && *(ps+i)<='9')
		{
			sum += *(ps+i) - '0' ;
		}
		else if( *(ps+i)>='A' && *(ps+i)<='F' )
		{
			sum += *(ps+i) - 'A' + 10;
		}
		else if( *(ps+i)>='a' && *(ps+i)<='f' )
		{
			sum += *(ps+i) - 'a' + 10;
		}
		sum <<= 4; //sum = sum*16;				
	}
	return (WORD)(sum >> 4) ;

}

/********************************************************************************************************
�������ƣ�HexString2TCHARstring
          �� 16�����ַ���ʾ�� �ַ���ת���� �ַ�����
		  ���� "4F54" --> TEXT("OT").
		  ���� O �� ASCII��Ϊ 4F
			   T �� ASCII��Ϊ 54
���������const char* ps, Դ�ַ���
          iLength, ��ת���ַ�������
���������TCHAR* pD
����ֵ��  ת����� int ����ֵ
˵����    iLength������ż����ÿ2��Դ�ַ����һ��ASCII TCHAR.
********************************************************************************************************/
BOOL CSysmacPLC::HexString2TCHARstring(const char* ps, int iLength, TCHAR* pD)
{
	if( 0!=iLength%2 || NULL==ps || NULL==pD )
	{
		return false;
	}

	char* pt = (char*)pD;
	int iTimes = iLength>>1;
	for(int i=0; i< iTimes; i++)
	{
#ifdef _UNICODE
		*pt++ = HexString2Int( ps + i*2 , 2 ); //little endian, ���ֽ���ǰ�����ֽ��ں�.
		*pt++ = 0;
#else
		*pt++ = HexString2Int( ps + i*2 , 2 );
#endif
	}
	return true;
}
/********************************************************************************************************
�������ƣ�IsEndCodeOK
          ���ջ�����Endcode�Ƿ� normal completion
������������ջ�����ָ��
����ֵ��  �жϽ��
��ʾ��    End code ��  plc����֡��״̬����Ϊ "00" ʱ��Ϊ normal completion.
          ciEndCodePos �ǳ���������� "SysmacPLC.h" �ļ�. ָ�� End code�� ���ջ���������ʼλ�á�
********************************************************************************************************/
bool CSysmacPLC::IsEndCodeOK(char* lpszRecvBuf)
{
	if( *(lpszRecvBuf + ciEndCodePos) == '0' &&  *(lpszRecvBuf + ciEndCodePos + 1) == '0' )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**********************************************************************************************
�������ƣ�GetEndCodeMeaning
          ��ȡ ��plc���յ���֡ �� End code�ĺ���
���������const char* lpszRecvBuf��ָ���plc���յ�����֡��
          int iEndCode
����ֵ��  �����ַ�����
note��
          End code �� �����ַ��� ��μ� P63 Section 4-2
**********************************************************************************************/
LPCTSTR CSysmacPLC::GetEndCodeMeaning(int iEndCode)//const char* lpszRecvBuf)
{
	//int iEndCode = HexString2Int(lpszRecvBuf + ciEndCodePos, 2);
	switch( iEndCode )
	{
	case 0x00:
		return _T("Normal completion");
	case 0x01:
		return _T("Not executable in RUN mode");
	case 0x02:
		return _T("Not executalbe in MONITOR mode");
	case 0x03:
		return _T("UM write-protected");
	case 0x04:
		return _T("Address over");
	case 0x0B:
		return _T("Not executable in PROGRAM mode");
	case 0x13:
		return _T("FCS error");
	case 0x14:
		return _T("Format error");
	case 0x15:
		return _T("Entry number data error");
	case 0x16:
		return _T("Command not supported");
	case 0x18:
		return _T("Frame length error");
	case 0x19:
		return _T("Not executable");
	case 0x20:
		return _T("Could not create I/O table");
	case 0x21:
		return _T("Not executable due to CPU Unit error");
	case 0x23:
		return _T("User memory protected");
	case 0xA3:
		return _T("Aborted due to FCS error in transmission data");
	case 0xA4:
		return _T("Aborted due to format error in trnasimission data");
	case 0xA5:
		return _T("Aborted due to entry number error in trnasimission data");
	case 0xA8:
		return _T("Aborted due to Frame length error in trnasimission data");
	default:
		return _T("Unknown End code!");
	}
}

/*********************************************************************************************************
�������� ReadPLC
         ��ȡ PLC �ڲ�ĳ��ַ����
���������
         enum _enumSysmacRdType enRdType, ��ȡ�������� 
		 int  nBeginWord,                 ��ȡ�Ŀ�ʼ��ַ
		 int  nReadNumber                 ����ȡ��word����. 
		 int  nReadArray[]                ��ȡ���ݴ�ŵ�ַ
		 char* pszSendBuf				  �����������ɻ�����
		 char* pszRecvBuf				  �������ݻ�����
����ֵ�� ��ȡ�ɹ����
��ע��   nRdNumWords >= 1
         nReadArray[] Ϊ���ն�ȡ���ݵ����飬���������㹻�Ŀռ䡣 �ռ���� >= nRdNumWords.
		 �� nRdNumWords == 1 , nReadArray����Ϊĳint���� int&���ɣ�
		 ���շ�������ָ��ΪNULLʱ������������ڲ�������
���̣�   1����������������������ַ���
         2������������ plc
		 3������ plc���ݣ� ��ת���� ���� nReadArray��.
note:    nRdNumberWords ����� < 30; //ʡ�ö�֡���������鷳.
         Read EM ���ڴ���
*********************************************************************************************************/
bool CSysmacPLC::ReadPLC(enum _enumSysmacRdType enRdType, int nBeginWord, int nReadNumber, 
							WORD wReadArray[]/*int  nReadArray[]*/,char* pszSendBuf, char* pszRecvBuf)
{
	char* lpszBuffer = ( (NULL == pszSendBuf )? m_pszSendBuf: pszSendBuf); //ָ�����������
	FormatCmdBufferRD(lpszBuffer,enRdType,nBeginWord,nReadNumber);

	if(	!SendCommand(lpszBuffer,ciRD_Len))
	{//SendCommand�ڲ��Ѿ���¼������룬�������ڷ�����
		return false;
	}

	DWORD& dwBytesRead = m_dwBytesRead;
	lpszBuffer       = ( (NULL == pszRecvBuf )? m_pszRecvBuf: pszRecvBuf); //ָ��������ݻ�����
	if(ReceiveData(dwBytesRead, 512, lpszBuffer))
	{
		if(IsEndCodeOK(lpszBuffer))
		{
			if(IsFCSOK( lpszBuffer, dwBytesRead-4, lpszBuffer + dwBytesRead - 4 )) //FCS����֡��β�� 4���ֽ�(2��FCS�ַ����Լ�*\x0D)
			{
				int iBegin = ciEndCodePos + 2;
				if(enReadTC == enRdType)
				{//Ω�� Read TC ����ص���10���ƣ�����Ϊ16����.
					for(int i = 0;i<nReadNumber;i++,iBegin += 4)
					{
						//��10���Ʊ�ʾ���ַ���ת����int���ݡ� 
						wReadArray[i] = DecString2Int(lpszBuffer + iBegin,4); //�̶�Ϊ 4 ���ַ����� 1 �� words.
					}
				}
				else
				{
					for(int i = 0;i<nReadNumber;i++,iBegin += 4)
					{
						//��16���Ʊ�ʾ���ַ���ת���� WORD���ݡ� //RC��10���ƣ���RC������ʱ���ã�
						wReadArray[i] = HexString2Word(lpszBuffer + iBegin); //�̶�Ϊ 4 ���ַ����� 1 �� word.
					}					
				}
				return true;
			}
			else
			{
				m_dwLastError = _SYSMAC_PLC_ERROR_FCS_;
			}
		}
		else
		{
			m_dwLastError	  = _SYSMAC_PLC_ERROR_END_CODE_;
			m_iLastErrEndCode = HexString2Int( lpszBuffer + ciEndCodePos, 2);
		}
	}
	//ReceiveData�ڲ��Ѿ���¼������룬�������ڷ�����
	return false;
}

/*********************************************************************************************************
�μ� ReadPLC
DM�������ݣ��� OMRON <SYSMAC CS/CJ Series Communications Commands REFERENCE MANUAL> ���У�˵���� 16���ƣ�ʵ��
����ȴ��10���ƣ��ʴ����г�����
*********************************************************************************************************/
bool CSysmacPLC::ReadPLCex(enum _enumSysmacRdType enRdType, int nBeginWord, int nReadNumber, WORD wReadArray[], bool bDecimal,char* pszSendBuf , char* pszRecvBuf)
{
	char* lpszBuffer = ( (NULL == pszSendBuf )? m_pszSendBuf: pszSendBuf); //ָ�����������
	FormatCmdBufferRD(lpszBuffer,enRdType,nBeginWord,nReadNumber);

	if(	!SendCommand(lpszBuffer,ciRD_Len))
	{//SendCommand�ڲ��Ѿ���¼������룬�������ڷ�����
		return false;
	}

	DWORD& dwBytesRead = m_dwBytesRead;
	lpszBuffer       = ( (NULL == pszRecvBuf )? m_pszRecvBuf: pszRecvBuf); //ָ��������ݻ�����
	if(ReceiveData(dwBytesRead, 512, lpszBuffer))
	{
		if(IsEndCodeOK(lpszBuffer))
		{
			if(IsFCSOK( lpszBuffer, dwBytesRead-4, lpszBuffer + dwBytesRead - 4 )) //FCS����֡��β�� 4���ֽ�(2��FCS�ַ����Լ�*\x0D)
			{
				int iBegin = ciEndCodePos + 2;
				if(bDecimal)
				{//ָ��10��������
					for(int i = 0;i<nReadNumber;i++,iBegin += 4)
					{
						//��10���Ʊ�ʾ���ַ���ת����int���ݡ� 
						wReadArray[i] = DecString2Int(lpszBuffer + iBegin,4); //�̶�Ϊ 4 ���ַ����� 1 �� words.
					}
				}
				else
				{//�� bDecimal == false����Ϊ 16����
					for(int i = 0;i<nReadNumber;i++,iBegin += 4)
					{
						wReadArray[i] = HexString2Word(lpszBuffer + iBegin); //�̶�Ϊ 4 ���ַ����� 1 �� word.
					}					
				}
				return true;
			}
			else
			{
				m_dwLastError = _SYSMAC_PLC_ERROR_FCS_;
			}
		}
		else
		{
			m_dwLastError	  = _SYSMAC_PLC_ERROR_END_CODE_;
			m_iLastErrEndCode = HexString2Int( lpszBuffer + ciEndCodePos, 2);
		}
	}
	//ReceiveData�ڲ��Ѿ���¼������룬�������ڷ�����
	return false;
}

/*********************************************************************************************************
�������ƣ�	GetDMstr
���������	nBeginWord, ��ʼ��ַ
            nReadNumber����ȡ��WORD����ע��DM�ǰ�WORD�洢���ݵġ�һ��WORDչ������2���ַ���
����ֵ��    ת�������ַ�����ע���ʱ�Ѿ��� ���� unicode�Ĵ���
����˵����  ��DM����ָ����ʼ��ַ��ȡָ���������ݣ�������ת�����ַ���
            ��ΪDM������ַ�����ȡ��������16���������ַ�����ʽ�� ��Ҫ���������ַ�����ת�����������ַ���
			���� : "4F544643" �ֱ��� "OTFC"��ASCII��.
			������������ReadPLC�ķ��档
			nReadNumber < 31
			��������Ӧ�����߳���ʹ�á�
**********************************************************************************************************/
CString CSysmacPLC::GetDMstr(int nBeginWord, int nReadNumber)
{
	ASSERT( nReadNumber < 31 );  //ȷ��ֻ��һ֡��ɣ�

	CString strR;
	char* lpszBuffer = m_pszSendBuf; //ָ�����������
	FormatCmdBufferRD(lpszBuffer,enReadDM,nBeginWord,nReadNumber);

	if(	!SendCommand(lpszBuffer,ciRD_Len))
	{//SendCommand�ڲ��Ѿ���¼������룬�������ٷ�����
		return strR;
	}

	DWORD& dwBytesRead = m_dwBytesRead;
	lpszBuffer       = m_pszRecvBuf; //ָ��������ݻ�����
	if(ReceiveData(dwBytesRead, 512, lpszBuffer))
	{
		if(IsEndCodeOK(lpszBuffer))
		{
			if(IsFCSOK( lpszBuffer, dwBytesRead-4, lpszBuffer + dwBytesRead - 4 )) //FCS����֡��β�� 4���ֽ�(2��FCS�ַ����Լ�*\x0D)
			{
				ASSERT((dwBytesRead-ciEndCodePos-2-4) == (nReadNumber << 2) );

				TCHAR*   pcht = new TCHAR[(nReadNumber<<1)+1]; //��Ϊ�������õĴ����٣�����ֱ���� new and delete.
				pcht[nReadNumber<<1] = _T('\0'); //�ַ���ĩβ����
				//ASSERT
				HexString2TCHARstring(lpszBuffer+ciEndCodePos+2,nReadNumber<<2,pcht);
				strR.Format(TEXT("%s"),pcht);
				delete  [] pcht;
				return strR;
			}
			else
			{
				m_dwLastError = _SYSMAC_PLC_ERROR_FCS_;
			}
		}
		else
		{
			m_dwLastError	  = _SYSMAC_PLC_ERROR_END_CODE_;
			m_iLastErrEndCode = HexString2Int( lpszBuffer + ciEndCodePos, 2);
		}
	}
	//ReceiveData�ڲ��Ѿ���¼������룬������������
	return strR;	
}

/*********************************************************************************************************
�������� WritePLC
         д�� PLC �ڲ�ĳ��ַ����
���������
         enum _enumSysmacWrType enType, ��д�������� 
		 int  nBeginWord,                 ��ʼ��ַ
		 int  nWriteNumber                дword����. 
		 int  nWriteArray[]               ���ݴ�ŵ�ַ
		 char* pszSendBuf				  �����������ɻ�����
		 char* pszRecvBuf				  �������ݻ�����
����ֵ�� �ɹ����
��ע��   nWriteNumber >= 1
         nWriteArray[] Ϊ���ն�ȡ����
		 �����飬���������㹻�Ŀռ䡣 �ռ���� >= nRdNumWords.
		 �� nWriteNumber == 1 , nWriteArray����Ϊĳint���� int&���ɣ�
		 ���շ�������ָ��ΪNULLʱ������������ڲ�������
���̣�   1����������������������ַ���
         2������������ plc
		 3��Write ����ͺ������Ļ���EndCode= 00��������OK.
note:    nWriteNumber ����� < 30; //ʡ�ö�֡���������鷳.
         Write EM ���ڴ���
*********************************************************************************************************/
bool CSysmacPLC::WritePLC(enum _enumSysmacWrType enType, int nBeginWord, int nWriteNumber, int  nWriteArray[], char* pszSendBuf, char* pszRecvBuf)
{
	char* lpszBuffer = ( (NULL == pszSendBuf )? m_pszSendBuf: pszSendBuf); //ָ�����������
	FormatCmdBufferWR(lpszBuffer,enType, nBeginWord, nWriteNumber,nWriteArray);

	if(	!SendCommand(lpszBuffer,strlen(lpszBuffer)))
	{//SendCommand�ڲ��Ѿ���¼������룬�������ٷ�����
		return false;
	}

	DWORD& dwBytesRead = m_dwBytesRead;
	lpszBuffer       = ( (NULL == pszRecvBuf )? m_pszRecvBuf: pszRecvBuf); //ָ��������ݻ�����
	if(ReceiveData(dwBytesRead, 512, lpszBuffer))
	{
		if(IsEndCodeOK(lpszBuffer))
		{
			if(IsFCSOK( lpszBuffer, dwBytesRead - 4, lpszBuffer + dwBytesRead - 4 )) //FCS����֡��β�� 4���ֽ�(2��FCS�ַ����Լ�*\x0D)
			{
				return true;
			}
			else
			{
				m_dwLastError = _SYSMAC_PLC_ERROR_FCS_;
			}
		}
		else
		{
			m_dwLastError	  = _SYSMAC_PLC_ERROR_END_CODE_;
			m_iLastErrEndCode = HexString2Int( lpszBuffer + ciEndCodePos, 2);
		}
	}
	//ReceiveData�ڲ��Ѿ���¼������룬�������ٷ�����
	return false;
}