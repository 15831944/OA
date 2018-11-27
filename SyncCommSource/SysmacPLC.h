/*******************************************************************************
�ļ�����"SysmacPLC.h"
������   �� CSysmacPLC ��ͷ�ļ���
         CSysmacPLC�������� ͬ OMRON  SYSMAC CS/CJϵ�� PLC ���д���ͨ�ŵ��ࡣ
		 ���� CSynCom �� ͬ������ͨ���࣬����ͨ�š�
		 ������Ҫ����ͨ��Э��Ľ����
		 Э����μ� OMRON Cat.No.W342-E1-08 
		 <SYSMAC CS/CJ Series Communications Commands REFERENCE MANUAL>
Author:  Zhang ziye,2007.09.05
********************************************************************************/

#pragma once
#include "synccom.h"

#define  _SYSMAC_PLC_ERROR_END_CODE_    ( _SYNCCOM_DERIVE_ERROR_ + 1 )
#define  _SYSMAC_PLC_ERROR_FCS_		    ( _SYSMAC_PLC_ERROR_END_CODE_ +1 )

/******************************************************************************
ŷķ��PLC�ڴ���˵����	
AR:���⸨���̵�����														 (δʹ��)
HR:���̵ּ�����,��Դ�Ͽ�ʱ�ܼ�סON/OFF��״̬							 (δʹ�ã�
IO/CIO:CIO������������I/O������ݣ�Ҳ���������ڲ�����ʹ洢���ݵĹ���λ
LR:���Ӽ̵�������������Ϊ�ڲ������̵���ʹ��								 (δʹ��)
DM:��̬���ݻ�����
TC:����̵��������������⹦��	
TCStatus:																 (δʹ��)
*******************************************************************************/
enum _enumSysmacRdType
{
	enReadCIO,enReadLR,enReadHR,enReadTC,enReadTCStatus,enReadDM,enReadAR
};

enum _enumSysmacWrType
{
	enWriteCIO,enWriteLR,enWriteHR,enWriteTC,enWriteDM,enWriteAR
};


const int ciRW_FCS_Pos = 13; //FCS�ڶ�дPLC�������ַ����е���ʼλ��.( �� 1 ��λ��Ϊ 0 )
const int ciEndCodePos = 5;  //PLC���ش��� End code����ʼλ��( ��һ��λ��Ϊ 0.).
                             //End code Ϊ 2 �ֽڣ���plc���������Ӧ״̬���롣
const int ciRD_Len = 17;     //ͨ���Ķ������

class CSysmacPLC :	public CSyncCom
{
public:
	CSysmacPLC(void);
public:
	~CSysmacPLC(void);
public:
	static char* CreateFCS(const char* pstr, int iCount, char* pFCS);
public:
	static BOOL IsFCSOK(const char* pstr, int iCount, char* pFCS);
public:
	virtual void InitComm(void);

	int    m_iLastErrEndCode;  //���һ��ͨ�ż�����ʱ��EndCode.
private:
	// PLC Unit numer
	unsigned int m_nUnitNumber;

public:
	BOOL   SetUnitNumber(unsigned int nUnitNumber);
public:
	//���� "@00RD" ���ַ������� Ϊ5
	char   CreateFCSChar(const char* pstr , int iCount=5);

public:
	BOOL FormatCmdBufferRD(char* lpszBuffer, enum _enumSysmacRdType enType, int nBeginWord, int nReadNumber);
	BOOL FormatCmdBufferWR(char* lpszBuffer, enum _enumSysmacWrType enType, int nBeginWord, int nWriteNumber,int nWriteArray[]);
	//BOOL FormatCmdBufferWR(char* lpszBuffer, enum _enumSysmacRdType enType, int nBeginWord, int iWriteNumber,int nWriteArray[]);
public:
	static int  HexString2Int(const char* ps, int iLength);
	static WORD HexString2Word(const char* ps); //iLength = 4
public:
	static int  DecString2Int(const char* ps, int iLength);
public:
	bool IsEndCodeOK(char* lpszRecvBuf);
public:
	LPCTSTR GetEndCodeMeaning(int iEndCode);//const char* lpszRecvBuf);
public:
	bool ReadPLC(enum _enumSysmacRdType enRdType, int nBeginWord, int nReadNumber, WORD wReadArray[], char* pszSendBuf = NULL, char* pszRecvBuf = NULL);
	bool WritePLC(enum _enumSysmacWrType enType, int nBeginWord, int nWriteNumber, int  nWriteArray[], char* pszSendBuf = NULL, char* pszRecvBuf = NULL);

	bool ReadPLCex(enum _enumSysmacRdType enRdType, int nBeginWord, int nReadNumber, WORD wReadArray[], bool bDecimal = true,char* pszSendBuf = NULL, char* pszRecvBuf = NULL);
	CString GetDMstr(int nBeginWord, int nReadNumber);
public:
	BOOL HexString2TCHARstring(const char* ps, int iLength, TCHAR* pD);
};
