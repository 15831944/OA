#pragma once
#include "afxwin.h"
#include "staticex.hxx"
#include "afxcmn.h"


extern bool g_bVacuumSaved;

// COAVacuumView form view
typedef struct tagChV
{
	CString str;
	double  d;
}stChV;


class COAVacuumView : public CFormView
{
	DECLARE_DYNCREATE(COAVacuumView)

public:
	COAVacuumView();           // protected constructor used by dynamic creation
	virtual ~COAVacuumView();

public:
	enum { IDD = IDD_OA_VACUUM_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	stChV pORChamberVolume[16];//10->15,2008.07.14,����<����鱨���� DIM-0004>
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnDisplayVacState(WPARAM wParam,  LPARAM lParam);
	afx_msg LRESULT OnDisplayVacValue(WPARAM wParam,  LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
public:
//	afx_msg void OnBnClickedButtonTest2();
public:
//	afx_msg void OnBnClickedButtonTest1();
public:
	virtual void OnInitialUpdate();
public:
	CStaticEx m_staticVACState;
public:
	void fn_DisplayVacState(void);
	void fn_DisplayVacValue(void);
public:
	void fn_SetIDsText(void);
public:
	afx_msg void OnBnClickedBtnStartVacuumTest();
public:
	afx_msg void OnBnClickedBtnCancelVacuumTest();
public:
	void fn_CancelTest(void);
public:
	afx_msg void OnBnClickedBtnIongaugeProperty();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
//	afx_msg void OnBnClickedCheckRecordVacuum();
public:
//	afx_msg void OnBnClickedCheckLeakRate();
public:
//	afx_msg void OnBnClickedCheckExhaustRate();
public:
	afx_msg void OnCbnSelchangeComboMachineType();
public:
	afx_msg void OnBnClickedRadioLeakRate();
public:
	afx_msg void OnBnClickedRadioExhaust();
public:
	afx_msg void OnBnClickedRadioRecorder();
public:
	void fn_OnItem2VacuumThread(WPARAM wParam, LPARAM lParam);
public:
	// ������ն�ҳ�����б��е�����
	bool fn_SaveVacuumFile(void* pPara); //stVacuumTestEx* pstVacuumTestEx
public:
	void fn_ShowWindow(void);
public:
//	afx_msg void OnBnClickedStaticLeakRateGroup();
public:
	CListCtrl m_Listctrl;
	CRect     m_rtVacPic;
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
public:

	CToolTipCtrl m_ToolTips;
	// ��������ʱʹ�õ�ListCtrl,�����©�ʣ���ʱ��ListCtrl����ʹ��
	CListCtrl m_ListctrlExhaust;
	// ��ʼ����ʾ���ã����� InitDialog����
	void fn_InitView(void);
public:
	CStaticEx m_seElapsedTime;
public:
	afx_msg void OnBnClickedButtonExhaustSt();
public:
	bool fn_CheckStartCondition(void* pstVTE);
public:
//	afx_msg void OnHdnItemdblclickListVacuumContent(NMHDR *pNMHDR, LRESULT *pResult);
public:
//	afx_msg void OnNMDblclkListVacuumContent(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkListVacuumContentExhaust(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMRdblclkListVacuumContentExhaust(NMHDR *pNMHDR, LRESULT *pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnClickedCheckUsingIongauge();
public:
	afx_msg void OnBnClickedButtonClearListctrl();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	void fn_AppendHelpText(LPCTSTR lpText);
public:
	afx_msg void OnBnClickedButtonSaveListctrl();
public:
	// ��������ݱ�����ļ�,2007.12.12
	bool fn_SaveVacFile(LPCTSTR sFilename, int  iVacType);
public:
	afx_msg void OnBnClickedShiftRight();
public:
	afx_msg void OnBnClickedShiftLeft();
public:
	afx_msg void OnBnClickedBtnAddVacuumFile();
public:
	afx_msg void OnBnClickedShiftLeft2();
public:
	afx_msg void OnBnClickedShiftRight2();
public:
	// �ƶ�ͼ��
	int fn_MoveCurve(void* pZYGraph,double dMoveValue);
public:
	afx_msg void OnCbnSelchangeComboCurves();
public:
	CComboBox m_cmbCurves;
public:
	// ��ȡ��նȲ����ļ���������ȡ�� ��ն�-ʱ�� ��������ӽ� �����б�  ȥ�����򷵻ش���
	int fn_LoadVacuumPressureFile(LPCTSTR szFileName);
public:
	// ��ȡ���� 00:10:22 �ַ����е�ʱ��������pzTimeΪ�ַ���������'\t'��'\0'������
	double fn_ExtractTime(LPCTSTR pzTime);
public:
	// ���ݴ洢��ͼ�����ݣ����³�ʼ��Combobox������
	void fn_ReInitGraphCombo(void);
public:
	afx_msg void OnBnClickedBtnDeleteCurve();
public:
	// ���Ե���ն������Ƿ��Ѿ���ӽ� ��ͼ��list�У���ʼ��Ϊfalse����Ӧ��ҳ������������plc��Ϣ��������list��������true.
	bool m_bTestDataInList;
public:
	afx_msg void OnBnClickedBtnCurveProperty();
public:
	afx_msg void OnStnClickedStaticPenningValueName();
public:
	afx_msg void OnStnDblclickStaticPenningValueName();
public:
	afx_msg void OnBnClickedBtnGaugeSetup();
public:
	afx_msg void OnBnClickedButtonTest();
	// ����Ŀ�����Text,����Debug
	void fn_AddHelpText(LPCTSTR lpText,bool bAdd);
	afx_msg void OnEnChangeEditComment();
	afx_msg void OnEnChangeEditVacuumHelp();
	afx_msg void OnBnClickedButtonSaveListctrl2();
};


