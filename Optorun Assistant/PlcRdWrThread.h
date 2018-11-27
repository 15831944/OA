#pragma once

/******************************************************************
��ȡ plc �ķ����߳� ������.

*******************************************************************/
// CPlcRdWrThread
class CPlcRdWrThread : public CWinThread		//ZHOU YI ������һ��UI�߳�
{
	DECLARE_DYNCREATE(CPlcRdWrThread)

protected:
	CPlcRdWrThread();           // protected constructor used by dynamic creation
	virtual ~CPlcRdWrThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	afx_msg void On_Plc_RW_Command(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()

public:
	DWORD m_dwLastError;
public:
	//дplc CIO ĳһλ Ϊ1�� ��Ӧ����������
	bool SetPLCCIOBitEvent(WORD wAddress, WORD wBit);
	//дplc CIO
	bool WritePLCCIOEvent(WORD wAddress, WORD wValue);	
	bool ReadPLCCIOEvent(WORD wAddress,WORD &wReadValue);
	bool ReadPLCDMEvent(WORD wAddress,WORD &wReadValue,bool bDec = true);
public:
	// дDM��
	bool WritePLCDMEvent(WORD wAddress, WORD wWriteValue);
};


