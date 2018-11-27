#pragma once

#define _DLG_INIT_VAC_TST_RT_OVERTIME   0 //�Ⱥ�ʱ����
#define _DLG_INIT_VAC_TST_RT_TIMERERR   1 //Timer����������
#define _DLG_INIT_VAC_TST_RT_CANCEL     2 //�û�ȡ��
#define _DLG_INIT_VAC_TST_RT_COMM_ERR   3 //ͨ�ų���
#define _DLG_INIT_VAC_TST_RT_PRESS_ERR  4 //��ʼ��ն�û�дﵽ����������û�д򿪣�
#define _DLG_INIT_VAC_TST_RT_MANUAL_ERR 5 //10������δ�ܽ��� <�ֶ�����>��ģʽ
#define _DLG_INIT_VAC_TST_RT_MV_ERR     6 //��նȴﵽ������ر��������������ڣ��������ڲ�û�йرգ�

#define _DLG_INIT_VAC_TST_RT_OK         10

// CDlgInitVacTst dialog

class CDlgInitVacTst : public CDialog
{
	DECLARE_DYNAMIC(CDlgInitVacTst)

public:
	CDlgInitVacTst(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInitVacTst();

// Dialog Data
	enum { IDD = IDD_DIALOG_INIT_VACUUM_TEST };



	UINT m_unCurrentTimeEvent;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	DWORD m_dwMaxWaitingTime; //��ȴ�ʱ�䣨���ݽ�����ֵ��,��λΪ ��
	DWORD m_dwInitTick;       //��ʼ��ʱ��TickCount
	DWORD m_dwPressureKeepTime;  //�ﵽ��ʼ��նȺ���ȶ�ʱ�䣬ĬȻ���� ����
	
	double  m_dStartPressure; //��ʼ��ն�ֵ�����ڼ��ɣ�
	int     m_nRetValue;      //�˳�ֵ������� _DLG_INIT_VAC_TST_RT_OK��

	bool    m_bFirstTimer;
	
public:
//	afx_msg void OnClose();
};
