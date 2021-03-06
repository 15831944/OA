// OAVacuumView.cpp : implementation file
//

#include "stdafx.h"
#include "Optorun Assistant.h"
#include "OAVacuumView.h"
#include "OACnfgDoc.h"
#include "DlgSetIonGaugeMode.h"
#include "DlgInitVacTst.h"
#include "DlgVacExhaustEdit.h"

#include "DlgVacCurveProperty.h"
#include "DlgVacBkPro.h"
#include "DlgVacGaugeSetup.h"
#include <fstream>

//适用于漏率测试以及定时记录（漏率测试数据也属于定时记录，所以二者兼容）wang
#define _VACUUM_LEAK_CTRL_LIST_COL_TIME_OTHER  0  
#define _VACUUM_LEAK_CTRL_LIST_COL_VGC_GAUGE   1 // ch3 Penning
#define _VACUUM_LEAK_CTRL_LIST_COL_ION_GAUGE   2 // ION GAUGE(如果有的话)

#define _VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME 0
#define _VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME 1 //ION GAUGE(如果有的话)
#define _VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME 2 //当前压力
#define _VACUUM_EXHAUST_CTRL_LIST_COMMENT      3 //备注
#define _VACUUM_EXHAUST_CTRL_LIST_APPEND      4 //补充，用于用户双击备注栏时自动添加时间。
#define _VACUUM_EXHAUST_CTRL_LIST_DM_HeaterTemp_TIME 5 //201205

// COAVacuumView
extern COACnfgDoc g_COACnfgDoc;

extern TCHAR g_buffer[];
extern char  g_chBuffer[];

DWORD g_dwStartTime = 0; //测试开始时间

bool g_bVacuumSaved = true;

extern CZYGraphPlot g_CZYGraphPlot;

CString   star;
IMPLEMENT_DYNCREATE(COAVacuumView, CFormView)

COAVacuumView::COAVacuumView()
	: CFormView(COAVacuumView::IDD)
	, m_bTestDataInList(false)
{

	m_rtVacPic.SetRectEmpty();

	
	//<设计情报文书 DIM-0004>,2008.07.14
	pORChamberVolume[0].str = TEXT("GENER-1300");
	pORChamberVolume[0].d   = 2.66; 
	pORChamberVolume[1].str = TEXT("OTFC-1300D/C");
	pORChamberVolume[1].d   = 3.51;//3.46，旧值
	pORChamberVolume[2].str = TEXT("OTFC-1300D(特)");
	pORChamberVolume[2].d   = 2.81;
	pORChamberVolume[3].str = TEXT("OTFC-1550D/C");
	pORChamberVolume[3].d   = 5.06;//4.83
	pORChamberVolume[4].str = TEXT("OTFC-1800D");
	pORChamberVolume[4].d   = 6.46;//6.37
	pORChamberVolume[5].str = TEXT("OTFC-1800C");
	pORChamberVolume[5].d   = 6.69;
	pORChamberVolume[6].str = TEXT("AR-1300D");
	pORChamberVolume[6].d   = 2.84;//2.82
	pORChamberVolume[7].str = TEXT("AR-1300D ﾛﾝｸﾞ");
	pORChamberVolume[7].d   = 3.11;//3.20
	pORChamberVolume[8].str = TEXT("OTFC-900D");
	pORChamberVolume[8].d   = 1.38;
	pORChamberVolume[9].str = TEXT("OTFC-1100DA/CA");
	pORChamberVolume[9].d   = 1.42;
	pORChamberVolume[10].str = TEXT("OTFC-1100DB/CB");
	pORChamberVolume[10].d  = 2.63;//2.6
	pORChamberVolume[11].str = TEXT("OTFC-600C(S014)");
	pORChamberVolume[11].d  = 0.38;
	pORChamberVolume[12].str = TEXT("LED-600");
	pORChamberVolume[12].d  = 0.38;
	pORChamberVolume[13].str = TEXT("STAR-1300");
	pORChamberVolume[13].d  = 2.58;
	pORChamberVolume[14].str = TEXT("OTFC-2350");				//201606
	pORChamberVolume[14].d  = 8.4;
	pORChamberVolume[15].str = TEXT("MTFC-900CAI");				//20160704
	pORChamberVolume[15].d  = 0.63;
	/*  8.4
	pORChamberVolume[0].str = TEXT("GENER");
	pORChamberVolume[0].d   = 2.60;
	pORChamberVolume[1].str = TEXT("AR+");
	pORChamberVolume[1].d   = 3.08;
	pORChamberVolume[2].str = TEXT("AR");
	pORChamberVolume[2].d   = 2.82;
	pORChamberVolume[3].str = TEXT("1800");
	pORChamberVolume[3].d   = 6.37;
	pORChamberVolume[4].str = TEXT("1550");
	pORChamberVolume[4].d   = 4.83;
	pORChamberVolume[5].str = TEXT("1300");
	pORChamberVolume[5].d   = 3.46;
	pORChamberVolume[6].str = TEXT("1100");
	pORChamberVolume[6].d   = 2.63;
	pORChamberVolume[7].str = TEXT("900");
	pORChamberVolume[7].d   = 1.38;
	pORChamberVolume[8].str = TEXT("1300");
	pORChamberVolume[8].d   = 2.58;
	*/
}

COAVacuumView::~COAVacuumView()
{
}

void COAVacuumView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PLC_VAC_STATE, m_staticVACState);
	DDX_Control(pDX, IDC_LIST_VACUUM_CONTENT, m_Listctrl);
	DDX_Control(pDX, IDC_LIST_VACUUM_CONTENT_EXHAUST, m_ListctrlExhaust);
	DDX_Control(pDX, IDC_STATIC_ELAPSED_TIME, m_seElapsedTime);
	DDX_Control(pDX, IDC_COMBO_CURVES, m_cmbCurves);
}

BEGIN_MESSAGE_MAP(COAVacuumView, CFormView)
	ON_MESSAGE(WM_ITEM2_DISPLAY_VAC_STATE,OnDisplayVacState)
	ON_MESSAGE(WM_ITEM2_DISPLAY_VAC_VALUE,OnDisplayVacValue)
//	ON_BN_CLICKED(IDC_BUTTON_TEST2, &COAVacuumView::OnBnClickedButtonTest2)
//	ON_BN_CLICKED(IDC_BUTTON_TEST1, &COAVacuumView::OnBnClickedButtonTest1)
	ON_BN_CLICKED(IDC_BTN_START_VACUUM_TEST, &COAVacuumView::OnBnClickedBtnStartVacuumTest)
	ON_BN_CLICKED(IDC_BTN_CANCEL_VACUUM_TEST, &COAVacuumView::OnBnClickedBtnCancelVacuumTest)
	ON_BN_CLICKED(IDC_BTN_IONGAUGE_PROPERTY, &COAVacuumView::OnBnClickedBtnIongaugeProperty)
//	ON_BN_CLICKED(IDC_CHECK_RECORD_VACUUM, &COAVacuumView::OnBnClickedCheckRecordVacuum)
//	ON_BN_CLICKED(IDC_CHECK_LEAK_RATE, &COAVacuumView::OnBnClickedCheckLeakRate)
//	ON_BN_CLICKED(IDC_CHECK_EXHAUST_RATE, &COAVacuumView::OnBnClickedCheckExhaustRate)
	ON_CBN_SELCHANGE(IDC_COMBO_MACHINE_TYPE, &COAVacuumView::OnCbnSelchangeComboMachineType)
	ON_BN_CLICKED(IDC_RADIO_LEAK_RATE, &COAVacuumView::OnBnClickedRadioLeakRate)
	ON_BN_CLICKED(IDC_RADIO_EXHAUST, &COAVacuumView::OnBnClickedRadioExhaust)
	ON_BN_CLICKED(IDC_RADIO_RECORDER, &COAVacuumView::OnBnClickedRadioRecorder)
//	ON_BN_CLICKED(IDC_STATIC_LEAK_RATE_GROUP, &COAVacuumView::OnBnClickedStaticLeakRateGroup)
//ON_WM_SIZE()
ON_BN_CLICKED(IDC_BUTTON_EXHAUST_ST, &COAVacuumView::OnBnClickedButtonExhaustSt)
//ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &COAVacuumView::OnHdnItemdblclickListVacuumContent)
//ON_NOTIFY(NM_DBLCLK, IDC_LIST_VACUUM_CONTENT, &COAVacuumView::OnNMDblclkListVacuumContent)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_VACUUM_CONTENT_EXHAUST, &COAVacuumView::OnNMDblclkListVacuumContentExhaust)
ON_NOTIFY(NM_RDBLCLK, IDC_LIST_VACUUM_CONTENT_EXHAUST, &COAVacuumView::OnNMRdblclkListVacuumContentExhaust)
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_CHECK_USING_IONGAUGE, &COAVacuumView::OnBnClickedCheckUsingIongauge)
ON_BN_CLICKED(IDC_BUTTON_CLEAR_LISTCTRL, &COAVacuumView::OnBnClickedButtonClearListctrl)
ON_WM_SIZE()
ON_BN_CLICKED(IDC_BUTTON_SAVE_LISTCTRL, &COAVacuumView::OnBnClickedButtonSaveListctrl)
ON_BN_CLICKED(IDC_SHIFT_RIGHT, &COAVacuumView::OnBnClickedShiftRight)
ON_BN_CLICKED(IDC_SHIFT_LEFT, &COAVacuumView::OnBnClickedShiftLeft)
ON_BN_CLICKED(IDC_BTN_ADD_VACUUM_FILE, &COAVacuumView::OnBnClickedBtnAddVacuumFile)
ON_BN_CLICKED(IDC_SHIFT_LEFT2, &COAVacuumView::OnBnClickedShiftLeft2)
ON_BN_CLICKED(IDC_SHIFT_RIGHT2, &COAVacuumView::OnBnClickedShiftRight2)
ON_CBN_SELCHANGE(IDC_COMBO_CURVES, &COAVacuumView::OnCbnSelchangeComboCurves)
ON_BN_CLICKED(IDC_BTN_DELETE_CURVE, &COAVacuumView::OnBnClickedBtnDeleteCurve)
ON_BN_CLICKED(IDC_BTN_CURVE_PROPERTY, &COAVacuumView::OnBnClickedBtnCurveProperty)
ON_STN_CLICKED(IDC_STATIC_PENNING_VALUE_NAME, &COAVacuumView::OnStnClickedStaticPenningValueName)
ON_STN_DBLCLK(IDC_STATIC_PENNING_VALUE_NAME, &COAVacuumView::OnStnDblclickStaticPenningValueName)
ON_BN_CLICKED(IDC_BTN_GAUGE_SETUP, &COAVacuumView::OnBnClickedBtnGaugeSetup)
ON_BN_CLICKED(IDC_BUTTON_TEST, &COAVacuumView::OnBnClickedButtonTest)
ON_EN_CHANGE(IDC_EDIT_COMMENT, &COAVacuumView::OnEnChangeEditComment)
ON_EN_CHANGE(IDC_EDIT_VACUUM_HELP, &COAVacuumView::OnEnChangeEditVacuumHelp)
ON_BN_CLICKED(IDC_BUTTON_SAVE_LISTCTRL2, &COAVacuumView::OnBnClickedButtonSaveListctrl2)
END_MESSAGE_MAP()

// COAVacuumView diagnostics

#ifdef _DEBUG
void COAVacuumView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void COAVacuumView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// COAVacuumView message handlers
#include <math.h>
BOOL COAVacuumView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
/*
	DWORD dwStart = GetTickCount();
	double x = 2;
	double y = 3;
	double z;
	for(int i=0;i<1000000;i++) 
	{
		//10M double 乘法，40ms. 1M log 后乘法，200ms.
		//z = x*y;	
		z = log10(x) * log10(y);
	}
	dwStart = GetTickCount()-dwStart;
	TRACE1("%d\r\n",dwStart);
*/
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}




//Read CIO
#include "DlgVacErr.h"

//Test
//void COAVacuumView::OnBnClickedButtonTest1()
//{			
//	if(OpenClipboard())
//	{
//		HGLOBAL clipbuffer;
//		TCHAR * buffer;
//		EmptyClipboard();
//		clipbuffer = GlobalAlloc(GMEM_DDESHARE, 3*sizeof(TCHAR)); 
//		if(clipbuffer)
//		{
//			memset(clipbuffer, 0, 3*sizeof(TCHAR));
//			buffer = (TCHAR*)GlobalLock(clipbuffer);
//			buffer[0] = _T('U');
//			buffer[1] = _T('F');
//			buffer[2] = _T('\0');
//
//			GlobalUnlock(clipbuffer);
//#ifdef _UNICODE
//			::SetClipboardData(CF_UNICODETEXT,clipbuffer);
//#else
//			::SetClipboardData(CF_TEXT,clipbuffer);
//#endif
//		}
//		CloseClipboard();
//	}
//
//
//	/*
//	//Cdm1Com g_dm1; //放在这里不行，原因不明
//	//g_dm1.SetComPort(1);
//	g_dm1.OpenCom();
//	g_dm1.SendCommand("H\x06",2);
//	DWORD wReadBytes;
//	g_dm1.ReceiveData(wReadBytes);
//	MultiByteToWideChar(CP_UTF8,0,g_dm1.m_pszRecvBuf,strlen(g_dm1.m_pszRecvBuf) + 1,g_strBuffer,1024); // strlen(..) + 1，保证连字符串的结束符号一并转化 
//	AfxMessageBox(g_strBuffer);
//	*/
//
//	// TODO: Add your control notification handler code here
//AfxMessageBox(g_item2_String[_ITEM2_CANCEL_WARNING_TEXT],MB_YESNO | MB_ICONQUESTION|MB_DEFBUTTON2);
//	CDlgVacErr dlg;
//	dlg.m_strReason = _T("No Reason");
//	dlg.DoModal();	
//	/*
//	double pa = g_COACnfgDoc.fn_TransPenningVoltPa(4199);
//	pa = g_COACnfgDoc.fn_TransPenningVoltPa(176);
//
//	g_IonGauge.SetGaugeType(4);
//	pa = g_IonGauge.TransVal2Pascal(4199);
//	pa = g_IonGauge.TransVal2Pascal(500);
//*/
///*
//	g_IonGauge.SetGaugeType(1);
//	double pa = g_IonGauge.TransVal2Pascal(1817); //计算出来: 5.42E-4, 实际显示约 3.4E-4
//	pa = g_IonGauge.TransVal2Pascal(4000); //100.00
//	pa = g_IonGauge.TransVal2Pascal(0);    //0.0
//	pa = g_IonGauge.TransVal2Pascal(4200); //316
//
//	*/
//
//	CString str;
//	GetDlgItemText(IDC_EDIT_ADDRESS,str);
//	WORD wAddress = _ttoi(str);
//
//	WORD wValue = 0xffff;
//	static int nReadTimes = 0;
//	if(g_pVacProThread->ReadPLCCIOEvent(wAddress,wValue))
//	{
//		// char  vt_pszSendBuf[512]; //vacuum thread buffer
//        // char  vt_pszRecvBuf[512];
//		nReadTimes++;
//		TCHAR tcTemp[512];
//		int nFileByteLen = strlen(vt_pszRecvBuf);
//		tcTemp[14] = _T('\0');
//		tcTemp[15] = _T('\0');
//		tcTemp[16] = _T('\0');
//
//		MultiByteToWideChar(CP_UTF8,0,vt_pszRecvBuf,nFileByteLen,tcTemp,512-1); 
//		str.Format(_T("ReadTimes=%d,WordReadnumber=%d,\r\n%s,ReadWord = %x"),nReadTimes,nFileByteLen,tcTemp,wValue);
//		AfxMessageBox(str);
//
//		//str.Format(_T("ReadTimes=%d,WordReadnumber=%d,\r\n ReadWord = %x"),nReadTimes,nFileByteLen,wValue);
//		//AfxMessageBox(str);
//	}	
//}

//Write CIO
//void COAVacuumView::OnBnClickedButtonTest2()
//{
//	CString str;
//	GetDlgItemText(IDC_EDIT_ADDRESS,str);
//	WORD wAddress = _ttoi(str);
//	
//	GetDlgItemText(IDC_EDIT_BIT,str);
//	WORD wBit = _ttoi(str);
//
//	/*
//	if(g_pVacProThread->SetPLCCIOBitEvent(wAddress,wBit))
//	{
//		// char  vt_pszSendBuf[512]; //vacuum thread buffer
//        // char  vt_pszRecvBuf[512];
//		TCHAR tcTemp[512];
//		int nFileByteLen = strlen(vt_pszRecvBuf);
//		MultiByteToWideChar(CP_UTF8,0,vt_pszRecvBuf,nFileByteLen,tcTemp,512); 
//		str.Format(_T("%s"),tcTemp);
//		AfxMessageBox(str);
//	}
//	*/
//}


//必须在MainFrame里自行指定，否则不会执行此函数OnInitialUpdate
void COAVacuumView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_ToolTips.Create(this);
	//bool b = m_ToolTips.AddTool(GetDlgItem(IDC_BTN_START_VACUUM_TEST),TEXT("This is tooltip"));

	CComboBox* pComboChV = ( CComboBox*) GetDlgItem(IDC_COMBO_MACHINE_TYPE);
    for(int i=0;i<=15;i++)
	{//7->13,2008.07.14
		pComboChV->AddString(pORChamberVolume[i].str);
	}

	pComboChV->SetCurSel(g_iMachineType);
	CString str;
	str.Format(TEXT("%.2f"),pORChamberVolume[g_iMachineType].d);
	SetDlgItemText(IDC_EDIT_CHAMBER_VOLUME,str);

	//CButton*   pwnd = (CButton*)GetDlgItem( IDC_CHECK_IONGAUGE );
	//pwnd->SetCheck(gb_UsingIonGauge?BST_CHECKED:BST_UNCHECKED);

//	fn_DisplayVacState(); //item1启动后会发送显示消息，此处可忽略
	((CButton*)GetDlgItem(IDC_BTN_IONGAUGE_PROPERTY)) ->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_PROPERTY));
	fn_SetIDsText();

	//debugging，放到 Vacuum View中启动
	//开启真空度相关监测线程
	g_pVacProThread = (CVacuumProcessThread*)AfxBeginThread(RUNTIME_CLASS(CVacuumProcessThread), THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	g_pVacProThread->m_hWnd = GetSafeHwnd();
	g_pVacProThread->ResumeThread();

	/*
#ifdef _DEBUG
	//将测试数据添加进 list中，供显示之用.
	CZYGraphData* pGraphData = new CZYGraphData;
	pGraphData->iDataNumber  = 0;
	pGraphData->pxData = g_dVacuumTime;
	pGraphData->pyData = g_dVacuumVGCCh3;
	pGraphData->bCanDelete = false;
	pGraphData->strName = TEXT("APC");
	//显示测试数据 VGCCh3
	//pGraphData->iGraphStyle = _ZYGRAPH_STYLE_DISCRETE_DOT;
	pGraphData->iGraphStyle = _ZYGRAPH_STYLE_DOT_LINE;
	pGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_DIMOND;
	pGraphData->dxDispShift = 480;
	pGraphData->iDataNumber = 4;
	g_dVacuumTime[0] = 1;
	g_dVacuumTime[1] = 10;
	g_dVacuumTime[2] = 60;
	g_dVacuumTime[3] = 120;
	g_dVacuumVGCCh3[0] = 10;
	g_dVacuumVGCCh3[1] = 100;
	g_dVacuumVGCCh3[2] = 1E+5;
	g_dVacuumVGCCh3[3] = 1E-2;
	//显示测试结束
	POSITION pos = g_CZYGraphPlot.listData.AddHead(pGraphData);

	pGraphData = new CZYGraphData;
	pGraphData->iDataNumber  = 0;
	pGraphData->pxData = g_dVacuumTime;
	pGraphData->pyData = g_dVacuumIonGauge;
	pGraphData->bCanDelete = false;
	pGraphData->strName = TEXT("ION");
	
	//显示测试数据 IonGauge
	pGraphData->iGraphStyle = _ZYGRAPH_STYLE_DOT_LINE;//_ZYGRAPH_STYLE_DISCRETE_DOT;
	pGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_TRIANGLE;//_ZYGRAPH_STYLE_DISCRETE_X_1_SHAPLE;//_ZYGRAPH_STYLE_DISCRETE_DIMOND;//_ZYGRAPH_STYLE_DISCRETE_CROSS;//;//;//_ZYGRAPH_STYLE_DISCRETE_X_SHAPLE;//_ZYGRAPH_STYLE_DISCRETE_TRIANGLE;//;//_ZYGRAPH_STYLE_DISCRETE_RECT;//;
	pGraphData->crColor = RGB(0,255,0);
	pGraphData->penLine.DeleteObject();
	pGraphData->penLine.CreatePen(pGraphData->nPenStyle,1,pGraphData->crColor);

	pGraphData->dxDispShift = 120;
	pGraphData->iDataNumber = 4;

	g_dVacuumIonGauge[0] = 1;
	g_dVacuumIonGauge[1] = 10;
	g_dVacuumIonGauge[2] = 1E+2;
	g_dVacuumIonGauge[3] = 1E-3;
	//显示测试结束	

	g_CZYGraphPlot.listData.InsertAfter(pos,pGraphData);//AddTail(pGraphData);
	//int iCount = g_CZYGraphPlot.listData.GetCount();
#endif
	*/
	g_CZYGraphPlot.m_strTitle = TEXT("Vacuum Pressure - minutes");


	CEdit* editHelp = (CEdit*)GetDlgItem(IDC_EDIT_VACUUM_HELP);
	editHelp->SetLimitText(100*1000000);

	fn_InitView();
#ifdef _DEBUG
	GetDlgItem(IDC_BUTTON_TEST)->ShowWindow(SW_SHOW);
#else
	GetDlgItem(IDC_BUTTON_TEST)->ShowWindow(SW_HIDE);
#endif
}


LRESULT COAVacuumView::OnDisplayVacValue(WPARAM wParam,  LPARAM lParam)
{
	fn_DisplayVacValue();
	return 0;
}
BOOL setDM_HeaterTemp(unsigned int DM_HeaterTemp)  //201205
{
	if(DM_HeaterTemp <=30)
	{
		DM_HeaterTemp = DM_HeaterTemp;
		return true;
	}
	else 
	{
		return 0;
	}
}  //201205
void COAVacuumView::fn_DisplayVacValue(void)
{
	if(gs_VacuumPressure.dVacuumPenningPa > gc_dMaxPenningPa)
	{
		SetDlgItemText(IDC_STATIC_PENNING_VALUE,_T("*********Pa"));
	}
	else if(gs_VacuumPressure.dVacuumPenningPa < gc_dMinPenningPa)
	{
		SetDlgItemText(IDC_STATIC_PENNING_VALUE,_T("---------Pa"));
	}
	else	
	{
		_stprintf(g_strBuffer,TEXT("%.2EPa"),gs_VacuumPressure.dVacuumPenningPa);
		ASSERT(GetDlgItem(IDC_STATIC_PENNING_VALUE)->GetSafeHwnd()!=NULL);
		SetDlgItemText(IDC_STATIC_PENNING_VALUE,g_strBuffer);
	}
	
	if( g_IonGauge.HasIonGauge() )
	{
		if(gs_VacuumPressure.dVacuumIonGaugePa > gc_dMaxIonGaugePa)
		{
			SetDlgItemText(IDC_STATIC_IONGAUGE_VALUE,_T("*********Pa"));
		}
		else if(gs_VacuumPressure.dVacuumIonGaugePa < gc_dMinIonGaugePa)
		{
			SetDlgItemText(IDC_STATIC_IONGAUGE_VALUE,_T("---------Pa"));
		}
		else	
		{
			_stprintf(g_strBuffer,TEXT("%.2EPa"),gs_VacuumPressure.dVacuumIonGaugePa);
			ASSERT(GetDlgItem(IDC_STATIC_IONGAUGE_VALUE)->GetSafeHwnd()!=NULL);
			SetDlgItemText(IDC_STATIC_IONGAUGE_VALUE,g_strBuffer);
		}
	}
}

/*
#define _ITEM2_VAC_STATE_NOT_CONNECT 0
#define _ITEM2_VAC_STATE_PLC_OK      1
#define _ITEM2_VAC_STATE_TESTING     2
*/
//响应从 SysInfoView 发送的消息 WM_ITEM2_DISPLAY_VAC_STATE
LRESULT COAVacuumView::OnDisplayVacState(WPARAM wParam,  LPARAM lParam)
{
	EnterCriticalSection(&gCriticalSection_State);
		int iPlcState = g_COACnfgDoc.m_iPlcState;
	LeaveCriticalSection(&gCriticalSection_State);

	if( iPlcState == _COM_PLC_STATE_COMM_NORMAL)
	{
		//如果正常响应，则更改 IonGauge项标题
		//抽速
		CString str = g_IonGauge.GetGaugeTypeName();
		HDITEM   htItem   =   {0};   
		htItem.mask   =   HDI_TEXT;   
		htItem.pszText   =   (LPTSTR)(LPCTSTR)str;
		m_ListctrlExhaust.GetHeaderCtrl()->SetItem(_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME, &htItem);		

		//漏率及记录表
		m_Listctrl.GetHeaderCtrl()->SetItem(_VACUUM_LEAK_CTRL_LIST_COL_ION_GAUGE, &htItem);
		
	}

	fn_DisplayVacState();
	return 0;
}

//需要继续添加测试状态
void COAVacuumView::fn_DisplayVacState(void)
{
	CWnd* pwnd;

	EnterCriticalSection(&gCriticalSection_State);
		int iPlcState = g_COACnfgDoc.m_iPlcState;
	LeaveCriticalSection(&gCriticalSection_State);

	if( iPlcState != _COM_PLC_STATE_COMM_NORMAL)
	{
		m_staticVACState.textColor(gc_clrBlack);
		m_staticVACState.SetWindowText( g_item2_Vac_state[_ITEM2_VAC_STATE_NOT_CONNECT] );

		pwnd = GetDlgItem(IDC_STATIC_PENNING_VALUE);
		pwnd->ShowWindow(SW_HIDE);

		pwnd = GetDlgItem(IDC_STATIC_IONGAUGE_VALUE);
		pwnd->ShowWindow(SW_HIDE);

		//debugging
		//pwnd = GetDlgItem(IDC_BTN_START_VACUUM_TEST);
		//	pwnd->EnableWindow(false);

		pwnd = GetDlgItem(IDC_BTN_CANCEL_VACUUM_TEST);
		pwnd->EnableWindow(false);
	}
	else
	{
		//经常需要到的时候，并且并不会影响主要流程，可不添加 critcial section保护
		if(gi_stVacuumTestState.bTest)
		{
			m_staticVACState.textColor(gc_clrAlert);
			m_staticVACState.SetWindowText( g_item2_Vac_state[_ITEM2_VAC_STATE_TESTING] );
			pwnd = GetDlgItem(IDC_BTN_START_VACUUM_TEST);
			pwnd->EnableWindow(false);
			pwnd = GetDlgItem(IDC_BTN_CANCEL_VACUUM_TEST);
			pwnd->EnableWindow(true);
		}
		else
		{
			pwnd = GetDlgItem(IDC_BTN_START_VACUUM_TEST);
			pwnd->EnableWindow(true);
			pwnd = GetDlgItem(IDC_BTN_CANCEL_VACUUM_TEST);
			pwnd->EnableWindow(false);

			m_staticVACState.textColor(gc_clrOK);
			m_staticVACState.SetWindowText( g_item2_Vac_state[_ITEM2_VAC_STATE_PLC_OK] );
		}
		pwnd = GetDlgItem(IDC_STATIC_PENNING_VALUE);
		pwnd->ShowWindow(SW_SHOW);

		if(g_IonGauge.HasIonGauge())
		{
			pwnd = GetDlgItem(IDC_STATIC_IONGAUGE_VALUE);
			pwnd->ShowWindow(SW_SHOW);
		}

		if(!m_bTestDataInList)
		{//m_bTestDataInList保证只执行一次初始化动作。
			CZYGraphData* pGraphData = new CZYGraphData;
			pGraphData->iDataNumber  = 0;
			pGraphData->pxData = g_dVacuumTime;
			pGraphData->pyData = g_dVacuumVGCCh3;
			pGraphData->bCanDelete = false;
			pGraphData->strName = TEXT("APC");
			//显示测试数据 VGCCh3
			//pGraphData->iGraphStyle = _ZYGRAPH_STYLE_DISCRETE_DOT;
			pGraphData->iGraphStyle = _ZYGRAPH_STYLE_LINE;
			pGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_DIMOND;
			pGraphData->dxDispShift = 0;
			pGraphData->iDataNumber = 0;
			g_dVacuumTime[0] = 1;
			g_dVacuumTime[1] = 10;
			g_dVacuumTime[2] = 60;
			g_dVacuumTime[3] = 120;
			g_dVacuumVGCCh3[0] = 10;
			g_dVacuumVGCCh3[1] = 100;
			g_dVacuumVGCCh3[2] = 1E+5;
			g_dVacuumVGCCh3[3] = 1E-2;
			//显示测试结束
			POSITION pos = g_CZYGraphPlot.listData.AddHead(pGraphData); //保持第一位
			if(g_IonGauge.HasIonGauge())
			{
				pGraphData = new CZYGraphData;
				pGraphData->iDataNumber  = 0;
				pGraphData->pxData = g_dVacuumTime;
				pGraphData->pyData = g_dVacuumIonGauge;
				pGraphData->bCanDelete = false;
				pGraphData->strName = TEXT("ION");

				//显示测试数据 IonGauge
				pGraphData->iGraphStyle = _ZYGRAPH_STYLE_LINE;//_ZYGRAPH_STYLE_DISCRETE_DOT;
				pGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_TRIANGLE;//_ZYGRAPH_STYLE_DISCRETE_X_1_SHAPLE;//_ZYGRAPH_STYLE_DISCRETE_DIMOND;//_ZYGRAPH_STYLE_DISCRETE_CROSS;//;//;//_ZYGRAPH_STYLE_DISCRETE_X_SHAPLE;//_ZYGRAPH_STYLE_DISCRETE_TRIANGLE;//;//_ZYGRAPH_STYLE_DISCRETE_RECT;//;
				pGraphData->crColor = RGB(0,255,0);
				pGraphData->penLine.DeleteObject();
				pGraphData->penLine.CreatePen(pGraphData->nPenStyle,1,pGraphData->crColor);

				pGraphData->dxDispShift = 0;
				pGraphData->iDataNumber = 0;

				g_dVacuumIonGauge[0] = 1;
				g_dVacuumIonGauge[1] = 10;
				g_dVacuumIonGauge[2] = 1E+2;
				g_dVacuumIonGauge[3] = 1E-3;
				//显示测试结束	

				g_CZYGraphPlot.listData.InsertAfter(pos,pGraphData); //保持第二位
			}
			//int iCount = g_CZYGraphPlot.listData.GetCount();
			g_CZYGraphPlot.m_strTitle = TEXT("Vacuum Pressure - minutes");
			m_bTestDataInList = true;
			fn_ReInitGraphCombo();
		}
	}
	m_staticVACState.ShowWindow(SW_SHOW);

	//电离规显示
	pwnd = GetDlgItem( IDC_STATIC_IONGAUGE_TEXT );
	ASSERT( NULL != pwnd->GetSafeHwnd() );
	if(g_IonGauge.HasIonGauge())
	{
		//_stprintf(g_strBuffer,TEXT("%s%s"),g_item2_String[_ITEM2_USING_ION_GAUGE],g_IonGauge.GetGaugeTypeName());
		pwnd->SetWindowText(g_IonGauge.GetGaugeTypeName());
		pwnd->ShowWindow(SW_SHOW);

		if(gb_ProgrammerDebugNormal && g_IonGauge.GetGaugeType()!=5) //不是BPG402,2008.02.14
			GetDlgItem(IDC_BTN_IONGAUGE_PROPERTY)->ShowWindow(SW_SHOW);
	}
	else
	{
		pwnd->ShowWindow(SW_HIDE);
		//debugging
		GetDlgItem(IDC_BTN_IONGAUGE_PROPERTY)->ShowWindow(SW_HIDE);
	}

}

void COAVacuumView::fn_SetIDsText(void)
{
	for(int i=0;i<gc_item2_Text_ID_CNT;i++)
	{
		if(!g_item2_ID_Text[i].IsEmpty())
			SetDlgItemText(g_item2_ID[i],g_item2_ID_Text[i]);
	}
}

/************************************************************************
函数名称：OnBnClickedBtnStartVacuumTest
参数：    无
说明：    用户点击了界面上的 "开始" 按钮，此为响应函数。
*************************************************************************/
void COAVacuumView::OnBnClickedBtnStartVacuumTest()
{

	//.SetColumn()

	//WORD  x = 1<<6;
	// TODO: Add your control notification handler code here
	//1, 确认初始条件！
	//double x = _tstof(TEXT("3.3E-4"));

	static stVacuumTestEx lstTestEx;

	//线程锁临界函数
	EnterCriticalSection(&gCriSec_Vacuum);
		memcpy(&lstTestEx,&g_stVacuumTestEx,sizeof(stVacuumTestEx));  //从缓冲区之间复制字节。第一个参量为新缓存区地址，第二个为旧缓存区地址，第三个为要复制的大小
	LeaveCriticalSection(&gCriSec_Vacuum);

	if(	fn_CheckStartCondition((void*)&lstTestEx))	//此函数包括一个检测PLC通信状态的函数，弹出对话框“PLC通讯是否正常”
	{
		if(lstTestEx.iCurTestType == _VACUUM_TESTTYPE_EXHAUST)
		{
			m_ListctrlExhaust.DeleteAllItems();
			ASSERT(m_ListctrlExhaust.GetItemCount() == 0);

			for(int i=0;i<g_iVacuumExhaustDataItems;i++)
			{
				m_ListctrlExhaust.InsertItem(i,TEXT(""));
				if(g_pstVacuumExhaustData[i].bPressureExist)
				{					
					_stprintf(g_buffer,TEXT("%.2E"),g_pstVacuumExhaustData[i].dPressure);
					m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME,g_buffer);
				}
				if(g_pstVacuumExhaustData[i].enRecordType == stVacuumExhaustData::_enumAction 
					|| g_pstVacuumExhaustData[i].enRecordType == stVacuumExhaustData::_enumAction_CH2)
				{
					m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COMMENT,g_pstVacuumExhaustData[i].sActionName);	//写下动作在控件中
				}
			}
		}
		else if(lstTestEx.iCurTestType == _VACUUM_TESTTYPE_LEAKRATE)
		{
			CDlgInitVacTst dlg;
			dlg.m_dwMaxWaitingTime = lstTestEx.sLeakRate.iWaitTimeSeconds;
			dlg.m_dStartPressure   = lstTestEx.sLeakRate.dStartPressure;
			dlg.m_dwPressureKeepTime = lstTestEx.sLeakRate.iPressureKeepTime;
			dlg.DoModal();  
/*
"DlgInitVacTst.h"
#define _DLG_INIT_VAC_TST_RT_OVERTIME   0 //等候超时出错
#define _DLG_INIT_VAC_TST_RT_TIMERERR   1 //Timer　启动出错
#define _DLG_INIT_VAC_TST_RT_CANCEL     2 //用户取消
#define _DLG_INIT_VAC_TST_RT_COMM_ERR   3 //通信出错
#define _DLG_INIT_VAC_TST_RT_PRESS_ERR  4 //起始真空度没有达到，而且主阀没有打开．
#define _DLG_INIT_VAC_TST_RT_MANUAL_ERR 5 //10秒内仍未能进入 <手动排气>　模式
#define _DLG_INIT_VAC_TST_RT_MV_ERR     6 //真空度达到，程序关闭主阀，但主阀在１０秒钟内并没有关闭．
#define _DLG_INIT_VAC_TST_RT_OK         10
*/
			if(_DLG_INIT_VAC_TST_RT_OK != dlg.m_nRetValue )
			{ 
				return;
			}
		}
	}
	else
	{
		return;
	}

	EnterCriticalSection(&gCriSec_Vacuum);
		g_stVacuumTestEx.bTesting = true;
	LeaveCriticalSection(&gCriSec_Vacuum);

}

//取消按钮
void COAVacuumView::OnBnClickedBtnCancelVacuumTest()
{
	// TODO: Add your control notification handler code here
	bool bVacuumTest;
	EnterCriticalSection(&gCriSec_Vacuum);
	bVacuumTest = g_stVacuumTestEx.bTesting;
	LeaveCriticalSection(&gCriSec_Vacuum);

	if(bVacuumTest)
	{
		if(IDYES == AfxMessageBox(g_item2_String[_ITEM2_CANCEL_WARNING_TEXT].IsEmpty()?TEXT("当前正在进行测试。你真的想取消吗？\r\nIt's testing now, are you sure to cancel?"):g_item2_String[_ITEM2_CANCEL_WARNING_TEXT],
			MB_YESNO | MB_ICONQUESTION|MB_DEFBUTTON2))
		{
			fn_CancelTest();
		}
	}
}

//测试进行中取消测试。
void COAVacuumView::fn_CancelTest(void)
{
	EnterCriticalSection(&gCriSec_Vacuum);
		g_stVacuumTestEx.bTesting = false;
	LeaveCriticalSection(&gCriSec_Vacuum);
}

//设置IonGauge属性，另外
void COAVacuumView::OnBnClickedBtnIongaugeProperty()
{
	// TODO: Add your control notification handler code here
	//debugging
	//g_IonGauge.SetGaugeType(1);
	ASSERT(::g_IonGauge.HasIonGauge());
	
	CDlgSetIonGaugeMode dlg;
	//初始化 dlg 的参数
	_stprintf(dlg.m_tszGroup,TEXT("Setup %s Recorder mode"),g_IonGauge.GetGaugeTypeName());
	dlg.m_nGaugeRecorderMode = g_IonGauge.m_nOutputMode;
	dlg.m_nGaugeType = g_IonGauge.GetGaugeType();
	if(IDOK == dlg.DoModal())
	{
		if(-1 != g_IonGauge.SetOutputMode(dlg.m_nGaugeRecorderMode))
		{			
			//Debugging 2007.12.20 need cancel
			CString str;
			str.Format(TEXT("IONGAUGE MODE = %d "),dlg.m_nGaugeRecorderMode);
			AfxMessageBox(str);

			AfxGetApp()->WriteProfileInt(TEXT("DEBUG"),TEXT("IONGAUGE_RECORDER_MODE"),g_IonGauge.m_nOutputMode);
		}
	}
}

LRESULT COAVacuumView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_ITEM2_TEST:
		//debugging
		//AfxMessageBox(_T("Test OK!\r\nWM_ITEM2_TEST"));
		break;
	case WM_VACUUM_THREAD_TO_ITEM2:
		fn_OnItem2VacuumThread(wParam, lParam);
		break;
	case WM_VACUUM_THREAD_ITEM2_QA:
		break;
	}

	return CFormView::WindowProc(message, wParam, lParam);
}


void COAVacuumView::OnCbnSelchangeComboMachineType()
{
	// TODO: Add your control notification handler code here
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_MACHINE_TYPE);
	CString str;
	str.Format(TEXT("%.2f"),pORChamberVolume[pCombo->GetCurSel()].d);
	SetDlgItemText(IDC_EDIT_CHAMBER_VOLUME,str);
}

void COAVacuumView::OnBnClickedRadioLeakRate()
{
	// TODO: Add your control notification handler code here
	ASSERT(g_stVacuumTestEx.bTesting == false);
	g_stVacuumTestEx.iCurTestType = _VACUUM_TESTTYPE_LEAKRATE;
	fn_ShowWindow();
	SetDlgItemText(IDC_EDIT_VACUUM_HELP,g_item2_String[_ITEM2_STR_LEAKRATE_HELP]);
}




void COAVacuumView::OnBnClickedRadioExhaust()
{
	// TODO: Add your control notification handler code here
	ASSERT(g_stVacuumTestEx.bTesting == false);
	g_stVacuumTestEx.iCurTestType = _VACUUM_TESTTYPE_EXHAUST;
	fn_ShowWindow();
	SetDlgItemText(IDC_EDIT_VACUUM_HELP,g_item2_String[_ITEM2_STR_EXHUAST_HELP]);
}

void COAVacuumView::OnBnClickedRadioRecorder()
{
	// TODO: Add your control notification handler code here
	ASSERT(g_stVacuumTestEx.bTesting == false);
	g_stVacuumTestEx.iCurTestType = _VACUUM_TESTTYPE_RECORDER;
	fn_ShowWindow();
	SetDlgItemText(IDC_EDIT_VACUUM_HELP,g_item2_String[_ITEM2_STR_RECORDER_HELP]);
}
int iTemp;
void COAVacuumView::fn_OnItem2VacuumThread(WPARAM wParam, LPARAM lParam)

{
	static bool b;
	static int i;
	static double dVGCCh3Rate;
	static double dIonGaugeRate;
	static stVacuumTestEx lstTestEx;
	static WORD wWord1,wWord2;
	static double d1;

	EnterCriticalSection(&gCriSec_Vacuum);
		memcpy(&lstTestEx,&g_stVacuumTestEx,sizeof(stVacuumTestEx));
	LeaveCriticalSection(&gCriSec_Vacuum);

	//static 
	_stVacuumThreadData* pst = (_stVacuumThreadData*) lParam;
	switch(pst->iStatus)
	{
	case _VACUUM_THREAD_START:
		//GetDlgItemText(IDC_BTN_START_VACUUM_TEST, g_buffer, 1024-1);
		if( _VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType )
		{
			i = m_ListctrlExhaust.GetItemCount();	//检索获得listctrl控件的行数
			m_ListctrlExhaust.InsertItem(i,TEXT("Start"));
			//m_ListctrlExhaust.SetItemText(i,1,g_buffer);//20140214				2018-9-13 Zhou Yi abolish
			_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
			m_ListctrlExhaust.SetItemText(i, 1, g_buffer);							//2018 - 9 - 13 Zhou Yi
		}
		else
		{
			m_Listctrl.InsertItem(0,TEXT("Start"));
			//m_Listctrl.SetItemText(0,1,g_buffer);//20140214						2018-9-13 Zhou Yi abolish
			_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
			m_Listctrl.SetItemText(0, 1, g_buffer);									//2018 - 9 - 13 Zhou Yi
		}
		g_dwStartTime = pst->dwTickTime;  //开始计时（检测到start按钮点击）
		
		break;
	case _VACUUM_THREAD_END:
		//GetDlgItemText(IDC_BTN_CANCEL_VACUUM_TEST, g_buffer, 1024-1);
		if( _VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType )
		{
			i = m_ListctrlExhaust.GetItemCount();
			m_ListctrlExhaust.InsertItem(i,TEXT("End"));
			m_ListctrlExhaust.SetItemText(i,1,g_buffer);//20140214
			_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
			
		}
		else
		{
			m_Listctrl.InsertItem(0,TEXT("End"));
			m_Listctrl.SetItemText(0,1,g_buffer);//20140214
			_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
			
		}

		//有数据发生，则自动记录. 记录仪状态不自动记录.
		if(!g_bVacuumSaved && lstTestEx.iCurTestType!=_VACUUM_TESTTYPE_RECORDER)
		{			
			fn_SaveVacuumFile(&lstTestEx);
		}
		break;

	case _VACUUM_NO_ANSWER:
		if( _VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType )
		{
			i = m_ListctrlExhaust.GetItemCount();
			m_ListctrlExhaust.InsertItem(i,TEXT("ERROR： PLC not answer, Stop."));
			m_ListctrlExhaust.SetItemText(i,1,g_buffer);//20140214
			_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
			
		}
		else
		{
			m_Listctrl.InsertItem(0,TEXT("ERROR： PLC not answer, Stop."));
			m_Listctrl.SetItemText(0,1,g_buffer);//20140214
			_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
			
		}


		//AfxMessageBox(_T("错误： PLC 没有正常响应.测试停止"),MB_OK|MB_ICONSTOP);
		AfxMessageBox(g_item2_String[_ITEM2_STR_PLC_COMM_ERR_STOP].IsEmpty()?
						TEXT("错误： PLC 没有正常响应.测试停止！\r\n plc communication failed."):g_item2_String[_ITEM2_STR_PLC_COMM_ERR_STOP]);

		if(!g_bVacuumSaved && lstTestEx.iCurTestType!=_VACUUM_TESTTYPE_RECORDER)
		{			
			fn_SaveVacuumFile(&lstTestEx);
		}
		break;

	case _VACUUM_ACK_RIGHT:
		g_bVacuumSaved = false; //有数据进来

		//添加流逝时间（显示的是界面上的红字）。
		iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
		_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,0);//20140214   iTemp%60
		SetDlgItemText(IDC_STATIC_ELAPSED_TIME,g_buffer);
		//m_Listctrl.InsertItem(0,g_buffer);
	
		//#define _VACUUM_READ_PLC_ADDRESS_SN_TP_1        2
		//#define _VACUUM_READ_PLC_ADDRESS_SN_TP_2        3
		//记录真空度 数据并显示出来
		//g_dVacuumVGCCh3[g_iVacuumPressurePos] = g_COACnfgDoc.fn_TransPenningVoltPa((double)pst->wRdData[_VACUUM_READ_PLC_ADDRESS_SN_VGC]);//2008.05.19
		g_dVacuumVGCCh3[g_iVacuumPressurePos] = g_APCGauge.TransVal2Pascal(pst->wRdData[_VACUUM_READ_PLC_ADDRESS_SN_VGC]);  //将每组结构体中的数据(voltage)->（pascal）
		
		//2008.09.12 .G1284发生LeakRate中途不记录数据，2次。 16:00->19:38 , 14:00->21:56,22:56
		//if(0 == g_iVacuumTestCnt % lstTestEx.iRecordInterval)//定时将数据显示进列表框
		//_stprintf(g_buffer,TEXT("%.2E"),g_dVacuumVGCCh3[g_iVacuumPressurePos]); //08.09.12之前
		_stprintf(g_buffer,TEXT("%.2E,cnt=%d,int=%d,Tick=%d\r\n"),g_dVacuumVGCCh3[g_iVacuumPressurePos],g_iVacuumTestCnt,lstTestEx.iRecordInterval,GetTickCount()/1000);
		SetDlgItemText(IDC_STATIC_PENNING_VALUE,g_buffer);  //笔记:这个函数似乎是画出来（或许是显示与help界面）
		fn_AddHelpText(g_buffer,true);


		if(g_IonGauge.HasIonGauge())
		{
			g_dVacuumIonGauge[g_iVacuumPressurePos] = g_IonGauge.TransVal2Pascal(pst->wRdData[_VACUUM_READ_PLC_ADDRESS_SN_IONGAUGE]);
			if( g_dVacuumIonGauge[g_iVacuumPressurePos] < 1.0E-8 )
			{//防止没有打开!
				g_dVacuumIonGauge[g_iVacuumPressurePos] = 1.0E-8;
			}

			_stprintf(g_buffer,TEXT("%.2E"),g_dVacuumIonGauge[g_iVacuumPressurePos]);
			SetDlgItemText(IDC_STATIC_IONGAUGE_VALUE,g_buffer);
		}

		if( _VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType )
		{
#define _EXHAUST_ACTION_MV_OP    6
#define _EXHAUST_ACTION_S_RV_CL  5
#define _EXHAUST_ACTION_MBP_ON   3
#define _EXHAUST_ACTION_RV_OP    1
#define _EXHAUST_ACTION_SRV_OP   0

//#define _VACUUM_READ_PLC_ADDRESS_SN_TP_1        2
//#define _VACUUM_READ_PLC_ADDRESS_SN_TP_2        3

			wWord1 = stVacuumExhaustData::wTP_1_Mask & pst->wRdData[_VACUUM_READ_PLC_ADDRESS_SN_TP_1];
			wWord2 = stVacuumExhaustData::wTP_2_Mask & pst->wRdData[_VACUUM_READ_PLC_ADDRESS_SN_TP_2];
			for( i=0; i<=_EXHAUST_ACTION_MV_OP; i++)
			{
				if(g_pstVacuumExhaustData[i].bRecord) //如果本栏已经记录，则跳过后续的判断.
				{
					continue;
				}

				if( stVacuumExhaustData::_enumAction == g_pstVacuumExhaustData[i].enRecordType 
					|| (stVacuumExhaustData::_enumAction_CH2 == g_pstVacuumExhaustData[i].enRecordType && g_IonGauge.GetGaugeType()!= _TYPE_ION_GAUGE_BPG402 ))
				{
					if( g_pstVacuumExhaustData[i].wTP1_OK_Value == wWord1 && g_pstVacuumExhaustData[i].wTP2_OK_Value == wWord2)
					{//有动作发生
						iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
						if(iTemp < 10 && _EXHAUST_ACTION_S_RV_CL==i)
							continue; //跳过初始阶段的 RV,SRV CLOSE状态, 2007.12.20

						_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);

						if( g_IonGauge.GetGaugeType() == _TYPE_ION_GAUGE_BPG402)
						{//如果是 BPG402，则记录进 BPG402列(Ion Gauge列). 2008.06.20
							//因为BPG402接的是ch2，而动作在有BPG402时是依靠它内部的皮拉尼规读数来进行的。
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME,g_buffer);
						}
						else
						{
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME,g_buffer); //2008.03.31						
						}

						g_pstVacuumExhaustData[i].bRecord = true;
					}
				}
				else if( stVacuumExhaustData::_enumAction_CH2 == g_pstVacuumExhaustData[i].enRecordType && g_IonGauge.GetGaugeType()== _TYPE_ION_GAUGE_BPG402)
				{//初始列表中只有动作没有真空度, 如果是VGC403，则记录下该动作时刻的时间以及当前的真空度数据.2008.03.27
					if( g_pstVacuumExhaustData[i].wTP1_OK_Value == wWord1 && g_pstVacuumExhaustData[i].wTP2_OK_Value == wWord2)
					{	
						//记录动作发生时间
						iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
						_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
						if( g_IonGauge.GetGaugeType() == _TYPE_ION_GAUGE_BPG402)
						{//2008.06.20，解释见上一条
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME,g_buffer); //_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME
						}
						else
						{
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME,g_buffer); //2008.03.31
						}
						
						//额外记录真空度
						if( g_IonGauge.GetGaugeType() == _TYPE_ION_GAUGE_BPG402)
						{//2008.06.20，解释见上一条
							_stprintf(g_buffer,TEXT("%.1E[CH2]"),g_dVacuumIonGauge[g_iVacuumPressurePos]); //2008.06.19//有BPG402的时候，低真空数据取自BPG402
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME,g_buffer); 
						}
						else if(g_IonGauge.GetGaugeType() != _TYPE_ION_GAUGE_BPG402 && _TYPE_APC_GAUGE_MPG400 == g_APCGauge.GetGaugeType())
						{ //MPG400在没有BPG402时，而且使用了 MPG400，此时也可以记录真空度!
							_stprintf(g_buffer,TEXT("%.1E[CH3]"),g_dVacuumVGCCh3[g_iVacuumPressurePos]); //2008.06.19//有BPG402的时候，低真空数据取自BPG402							
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME,g_buffer); 
						}
						
						g_pstVacuumExhaustData[i].bRecord = true;
					}					
				}
				else if(stVacuumExhaustData::_enumNotRecord_CH2 == g_pstVacuumExhaustData[i].enRecordType 
					&& (g_IonGauge.GetGaugeType() == _TYPE_ION_GAUGE_BPG402 || _TYPE_APC_GAUGE_MPG400 == g_APCGauge.GetGaugeType() )
					)
				{ //对应初始列表中只有 真空度数据.2008.03.27
					//如果有MPG400而没有BPG402，则判断 ch3(MPG400) 的真空度
					//如果有BPG402，则判断 ch2(BPG402)的真空度
					//其它情况不关心.
					if( g_IonGauge.GetGaugeType() == _TYPE_ION_GAUGE_BPG402)
					{
						if(	g_dVacuumIonGauge[g_iVacuumPressurePos] < g_pstVacuumExhaustData[i].dPressure )
						{
							iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
							_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME,g_buffer); //2008.03.31
							g_pstVacuumExhaustData[i].bRecord = true;
						}
					}
					else if(g_IonGauge.GetGaugeType() != _TYPE_ION_GAUGE_BPG402 && _TYPE_APC_GAUGE_MPG400 == g_APCGauge.GetGaugeType())
					{
						if(	g_dVacuumVGCCh3[g_iVacuumPressurePos] < g_pstVacuumExhaustData[i].dPressure )
						{
							iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
							_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME,g_buffer); //2008.03.31
							g_pstVacuumExhaustData[i].bRecord = true;
						}
					}
				}
			}
			

			if( b || g_dVacuumVGCCh3[g_iVacuumPressurePos] < 1.0E-2 )
			{
				if( g_iVacuumPressurePos > 3)
				{
					//3秒钟平均
					d1 = (g_dVacuumVGCCh3[g_iVacuumPressurePos] + g_dVacuumVGCCh3[g_iVacuumPressurePos-1] + g_dVacuumVGCCh3[g_iVacuumPressurePos-2])/3.0;
				}
				else
				{
					d1 = g_dVacuumVGCCh3[g_iVacuumPressurePos];
				}

				for( i = 7;i<g_iVacuumExhaustDataItems;i++)
				{
					if( !g_pstVacuumExhaustData[i].bRecord )
					{
						if( d1 < g_pstVacuumExhaustData[i].dPressure)
						{
							g_pstVacuumExhaustData[i].bRecord = true; //记录此时的数据.
							iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
							_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
							m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME,g_buffer);
						}
					}					
				}

				if(g_IonGauge.HasIonGauge())
				{
					if( g_iVacuumPressurePos > 3)
					{
						//3秒钟平均
						d1 = (g_dVacuumIonGauge[g_iVacuumPressurePos] + g_dVacuumIonGauge[g_iVacuumPressurePos-1] + g_dVacuumIonGauge[g_iVacuumPressurePos-2])/3.0;
					}
					else
					{
						d1 = g_dVacuumIonGauge[g_iVacuumPressurePos];
					}
					for(int i=8;i<g_iVacuumExhaustDataItems;i++)
					{
						if( !g_pstVacuumExhaustData[i].bIonGaugeRecord )
						{
							if( d1 < g_pstVacuumExhaustData[i].dPressure)
							{
								g_pstVacuumExhaustData[i].bIonGaugeRecord = true; //记录此时的数据.
								iTemp = (pst->dwTickTime-g_dwStartTime)/1000;
								_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
								m_ListctrlExhaust.SetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME,g_buffer);
							}
						}
					}
				}

				b = false;
				//判断是否全部结束
				if(	g_IonGauge.HasIonGauge() && g_bUseIongauge )
				{
					if( g_pstVacuumExhaustData[g_iVacuumExhaustDataItems-1].bIonGaugeRecord && g_pstVacuumExhaustData[g_iVacuumExhaustDataItems-1].bRecord )
					{
						b = true;
					}
				}
				else
				{
					if( g_pstVacuumExhaustData[g_iVacuumExhaustDataItems-1].bRecord )
					{
						b = true;
					}
				}

				if(b)
				{//测试完全结束
					EnterCriticalSection(&gCriSec_Vacuum);
					g_stVacuumTestEx.bTesting = false;       //停止测试
					LeaveCriticalSection(&gCriSec_Vacuum);
				}
			}
		}
		else
		{
			//定时将数据显示进列表框
			//2018-9-10 ZHOU YI: g_iVacuumTestCnt改为(g_iVacuumTestCnt+1)，并且加了一个判断 || g_iVacuumTestCnt==0
		//2018-9-29Zhou	if ((0 == (g_iVacuumTestCnt - 1) % (lstTestEx.iRecordInterval - 1)) || g_iVacuumTestCnt == 0)  //（iRecordInterval=60，以为这每过一分钟将数据写入ClistCtrl中一次）
			iTemp = (pst->dwTickTime - g_dwStartTime) / 1000;
			if (iTemp%lstTestEx.iRecordInterval == 0)		//2018-9-29由定间隔改成定时写入数据
			{
					//iTemp = (pst->dwTickTime - g_dwStartTime) / 1000;	/2018-9-29Zhou
					_stprintf(g_buffer, TEXT("%02d:%02d:%02d"), iTemp / 3600, (iTemp / 60) % 60, iTemp % 60);
					//_stprintf(g_buffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
					m_Listctrl.InsertItem(0, g_buffer);

					_stprintf(g_buffer, TEXT("%.2E"), g_dVacuumVGCCh3[g_iVacuumPressurePos]);
					m_Listctrl.SetItemText(0, _VACUUM_LEAK_CTRL_LIST_COL_VGC_GAUGE, g_buffer);
					if (g_IonGauge.HasIonGauge())
					{
						_stprintf(g_buffer, TEXT("%.2E"), g_dVacuumIonGauge[g_iVacuumPressurePos]);
						m_Listctrl.SetItemText(0, _VACUUM_LEAK_CTRL_LIST_COL_ION_GAUGE, g_buffer);
					}
			}

			//第一次进数据,记录初始数据
			if(0 == g_iVacuumTestCnt)
			{
				g_stVacuumTestEx.sLeakRate.dRecordedStartPressure = g_dVacuumVGCCh3[g_iVacuumPressurePos];
				g_stVacuumTestEx.sLeakRate.dRecordedStartIonGauge = g_dVacuumIonGauge[g_iVacuumPressurePos];
			}
			else //不是第一次进数据（指中间记录的数据，这里的数据应该是记录到每秒都会显示的HELP控件中）
			{
				g_stVacuumTestEx.sLeakRate.iTimeElapseSeconds   = g_iVacuumTestCnt;								
				g_stVacuumTestEx.sLeakRate.dRecordedEndPressure = g_dVacuumVGCCh3[g_iVacuumPressurePos];
				g_stVacuumTestEx.sLeakRate.dRecordedEndIonGauge = g_dVacuumIonGauge[g_iVacuumPressurePos];
				dVGCCh3Rate = g_stVacuumTestEx.sLeakRate.fnLeakRate();  //漏率的计算公式
				if(g_IonGauge.HasIonGauge())
				{
					dIonGaugeRate = g_stVacuumTestEx.sLeakRate.fnIonGaugeLeakRate();
				}
			}

			//Times up,正常计时完成，　输出漏率结果(漏率检测完成后的一些时间显示)
			if( g_iVacuumTestCnt == lstTestEx.iRecordTime)
			{
				EnterCriticalSection(&gCriSec_Vacuum);
				     g_stVacuumTestEx.bTesting = false;       //停止测试
				LeaveCriticalSection(&gCriSec_Vacuum);

				_stprintf(g_buffer, TEXT("Rate %4d-%02d-%02d %02d:%02d:%02d"), pst->st.wYear,pst->st.wMonth,pst->st.wDay,pst->st.wHour,pst->st.wMinute,pst->st.wSecond);
				m_Listctrl.InsertItem(0,g_buffer);

				_stprintf(g_buffer,TEXT("%.1E"),dVGCCh3Rate);
				m_Listctrl.SetItemText(0,_VACUUM_LEAK_CTRL_LIST_COL_VGC_GAUGE,g_buffer);
				if(g_IonGauge.HasIonGauge())
				{
					_stprintf(g_buffer,TEXT("%.1E"),dIonGaugeRate);
					m_Listctrl.SetItemText(0,_VACUUM_LEAK_CTRL_LIST_COL_ION_GAUGE,g_buffer);
				}
			}
		}

		//真空度指针位置调整，待下次记录之用(也就是下一组数据开始转存)
		g_iVacuumPressurePos++;
		if(g_iVacuumPressurePos>=_VACUUM_PRESSURE_MAX)
		{
			g_iVacuumPressurePos = 0;
		}


		//显示
		CZYGraphData* pGraphData = g_CZYGraphPlot.listData.GetHead();
		pGraphData->iDataNumber = g_iVacuumTestCnt;//g_iVacuumPressurePos;
		pGraphData->pxData[pGraphData->iDataNumber-1] = (pst->dwTickTime-g_dwStartTime)/1000; //同时影响2个

		if(g_IonGauge.HasIonGauge())
		{
			pGraphData = g_CZYGraphPlot.listData.GetAt(g_CZYGraphPlot.listData.FindIndex(1)); 
			pGraphData->iDataNumber = g_iVacuumTestCnt;//g_iVacuumPressurePos;
		}

		//图形快到右端时，自动更改显示范围。
		if( g_iVacuumPressurePos > (g_CZYGraphPlot.xRight - g_CZYGraphPlot.xRight/g_CZYGraphPlot.ixGridNum ) 
			&& g_iVacuumPressurePos < g_CZYGraphPlot.xRight)
		{
			g_CZYGraphPlot.xRight += 60;
			g_CZYGraphPlot.xLeft  += 60;
		}
		else if( g_iVacuumPressurePos > g_CZYGraphPlot.xRight )
		{//如果本身由用户调整达到右端，则等待10秒后再自动调整回来.
			

		}

		InvalidateRect(&m_rtVacPic,false);

		g_iVacuumTestCnt++;
		break;
	}
	fn_ShowWindow();
}

// 自动生成文件名,保存真空度页面内列表中的内容   
bool COAVacuumView::fn_SaveVacuumFile(void* pPara)		//ZHOU YI:将计算机的名字赋给文件名
{
	stVacuumTestEx* pstVacuumTestEx = (stVacuumTestEx*)pPara;
	
//	int iLength;	
	TCHAR  lptchTmp[256];  //不可重入
	DWORD bufsize = 512;
	int iCnt;
	if( _VACUUM_TESTTYPE_EXHAUST == pstVacuumTestEx->iCurTestType )
	{
		iCnt = m_ListctrlExhaust.GetItemCount();
	}
	else
	{
		iCnt = m_Listctrl.GetItemCount();
	}
	
//	int ibuf;	
	TCHAR cbuf[512];
//	TCHAR cbuf2[512];
	
	if(iCnt==0)
	{	
		SYSTEMTIME    st;   
		CString str;
		GetLocalTime(&st);   
		str.Format(TEXT("Error: no data! \r\n%4d-%02d-%02d %02d:%02d:%02d\r\n"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

		fn_AppendHelpText(str);
		return false;
	}

	::GetModuleFileName(NULL,lptchTmp,512);
	CString str(lptchTmp);
	int ibackslash = str.ReverseFind(TEXT('\\'));
	lptchTmp[ibackslash + 1]= TEXT('\0');

	::GetComputerName(cbuf,&bufsize);

	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	str.Format(TEXT("-OA-VAC-%4d%02d%02d %02d%02d%02d.log"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	_tcscat(cbuf,str);
	_tcscat(lptchTmp,cbuf);

	return fn_SaveVacFile(lptchTmp, pstVacuumTestEx->iCurTestType);

}

//根据当前状态，显示当前的各控件窗口
void COAVacuumView::fn_ShowWindow(void)
{
	static stVacuumTestEx lstTestEx;

	EnterCriticalSection(&gCriSec_Vacuum);
		memcpy(&lstTestEx,&g_stVacuumTestEx,sizeof(stVacuumTestEx));
	LeaveCriticalSection(&gCriSec_Vacuum);
	
	m_Listctrl.ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_HIDE:SW_SHOW);
	m_ListctrlExhaust.ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);

//	GetDlgItem(IDC_STATIC_INTERVAL_TEXT)->ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_HIDE:SW_SHOW);
//	GetDlgItem(IDC_COMBO_INTERVAL)->ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_HIDE:SW_SHOW);
//	GetDlgItem(IDC_STATIC_RECORD_TIME_TEXT)->ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_HIDE:SW_SHOW);
//	GetDlgItem(IDC_EDIT_RECORD_TIME)->ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_HIDE:SW_SHOW);

	GetDlgItem(IDC_COMBO_INTERVAL)->EnableWindow( (_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType || lstTestEx.bTesting) ? false:true); //只有排气模式不用
	GetDlgItem(IDC_COMBO_RECORD_TIME)->EnableWindow( (_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType || lstTestEx.bTesting) ? false:true); //只有排气模式不用	

	GetDlgItem(IDC_RADIO_LEAK_RATE)->EnableWindow(lstTestEx.bTesting ? false:true);
	GetDlgItem(IDC_RADIO_EXHAUST)->EnableWindow(lstTestEx.bTesting ? false:true);
	GetDlgItem(IDC_RADIO_RECORDER)->EnableWindow(lstTestEx.bTesting ? false:true);

//	GetDlgItem(IDC_BUTTON_EXHAUST_ST)->ShowWindow(_VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);
//	GetDlgItem(IDC_BUTTON_EXHAUST_ST)->EnableWindow( lstTestEx.bTesting ? false:true);
	
	m_seElapsedTime.textColor(lstTestEx.bTesting?gc_clrRunning:gc_clrBlack);

	GetDlgItem(IDC_COMBO_MACHINE_TYPE)->EnableWindow(lstTestEx.bTesting ? false:true);
	GetDlgItem(IDC_EDIT_CHAMBER_VOLUME)->EnableWindow(lstTestEx.bTesting ? false:true);

	GetDlgItem(IDC_COMBO_LEAKRATE_START_PRESSURE)->EnableWindow( ((_VACUUM_TESTTYPE_LEAKRATE==lstTestEx.iCurTestType) && !lstTestEx.bTesting) ? true:false );
	GetDlgItem(IDC_COMBO_VACUUM_WAIT_TIME)->EnableWindow(((_VACUUM_TESTTYPE_LEAKRATE==lstTestEx.iCurTestType) && !lstTestEx.bTesting) ? true:false ); //leakrate
	GetDlgItem(IDC_COMBO_LEAKRATE_KEEP_TIME)->EnableWindow(((_VACUUM_TESTTYPE_LEAKRATE==lstTestEx.iCurTestType) && !lstTestEx.bTesting) ? true:false ); //leakrate
	//	GetDlgItem(IDC_EDIT_LEAKRATE_START_PRESSURE)->ShowWindow(_VACUUM_TESTTYPE_LEAKRATE == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);
//	GetDlgItem(IDC_EDIT_VACUUM_WAIT_TIME)->ShowWindow(_VACUUM_TESTTYPE_LEAKRATE == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);
//	GetDlgItem(IDC_STATIC_LEAKRATE_START_PRESSURE_TEXT)->ShowWindow(_VACUUM_TESTTYPE_LEAKRATE == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);
//	GetDlgItem(IDC_STATIC_LEAKRATE_KEEP_TIME_TEXT)->ShowWindow(_VACUUM_TESTTYPE_LEAKRATE == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);
//	GetDlgItem(IDC_EDIT_LEAKRATE_KEEP_TIME)->ShowWindow(_VACUUM_TESTTYPE_LEAKRATE == lstTestEx.iCurTestType ? SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_BUTTON_CLEAR_LISTCTRL)->EnableWindow( ((_VACUUM_TESTTYPE_EXHAUST!=lstTestEx.iCurTestType) && !lstTestEx.bTesting) ? true:false );
	//GetDlgItem(IDC_BUTTON_SAVE_LISTCTRL)->EnableWindow( ((_VACUUM_TESTTYPE_EXHAUST!=lstTestEx.iCurTestType) && !lstTestEx.bTesting) ? true:false );
	GetDlgItem(IDC_BUTTON_SAVE_LISTCTRL)->EnableWindow( !lstTestEx.bTesting ? true:false );

	GetDlgItem(IDC_CHECK_USING_IONGAUGE)->EnableWindow( ( g_IonGauge.HasIonGauge() && (_VACUUM_TESTTYPE_EXHAUST==lstTestEx.iCurTestType) && !lstTestEx.bTesting) ? true:false );

	//debugging
	//g_bOffLineDebug = true;

	if(g_bOffLineDebug)
	{
		GetDlgItem(IDC_BTN_START_VACUUM_TEST)->EnableWindow(lstTestEx.bTesting ? false:true);
		GetDlgItem(IDC_BTN_CANCEL_VACUUM_TEST)->EnableWindow(lstTestEx.bTesting ? true:false);	
	}
	else
	{
		if(g_plc.IsOpen())
		{
			GetDlgItem(IDC_BTN_START_VACUUM_TEST)->EnableWindow(lstTestEx.bTesting ? false:true);
			GetDlgItem(IDC_BTN_CANCEL_VACUUM_TEST)->EnableWindow(lstTestEx.bTesting ? true:false);		
		}
		else
		{
			GetDlgItem(IDC_BTN_START_VACUUM_TEST)->EnableWindow(false); 
			GetDlgItem(IDC_BTN_CANCEL_VACUUM_TEST)->EnableWindow(false);
		}
	}

#ifdef _DEBUG
	GetDlgItem(IDC_BTN_START_VACUUM_TEST)->EnableWindow(lstTestEx.bTesting ? false:true);
#endif

}

// 初始化显示设置，仅供 InitDialog调用
void COAVacuumView::fn_InitView(void)
{
	//定位列表框位置
	CRect rt1,rt2,rt3;
	CWnd* pwnd = GetDlgItem( IDC_BUTTON_CLEAR_LISTCTRL ); //按钮
	if( pwnd->GetSafeHwnd() && m_Listctrl.GetSafeHwnd() && m_ListctrlExhaust.GetSafeHwnd() )
	{
		GetWindowRect(&rt1); //页面窗口
		pwnd->GetWindowRect(&rt2); //按钮 的位置
		rt3.bottom = rt1.bottom - 3;
		rt3.right  = rt1.right - 3;
		rt3.left   = rt2.right + 3;
		rt3.top    = rt1.bottom - rt1.Height()/2 + 3;
		ScreenToClient(&rt3);
		m_Listctrl.MoveWindow(&rt3);
		m_ListctrlExhaust.MoveWindow(&rt3);

		//图形区域
		GetDlgItem(IDC_BTN_IONGAUGE_PROPERTY)->GetWindowRect(&m_rtVacPic);
		ScreenToClient(&m_rtVacPic);
		m_rtVacPic.top = m_rtVacPic.bottom + 3;
		
//		m_rtVacPic.top = rt1.top + 3; 
		m_rtVacPic.bottom = rt3.top - 6;
		m_rtVacPic.left = rt3.left ;
		m_rtVacPic.right = rt3.right;
	}
	((CButton*)GetDlgItem(IDC_CHECK_USING_IONGAUGE))->SetCheck(g_bUseIongauge ? BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_LEAK_RATE))->SetCheck(_VACUUM_TESTTYPE_LEAKRATE == g_stVacuumTestEx.iCurTestType ? BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_EXHAUST))->SetCheck(_VACUUM_TESTTYPE_EXHAUST == g_stVacuumTestEx.iCurTestType ? BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_RECORDER))->SetCheck(_VACUUM_TESTTYPE_RECORDER == g_stVacuumTestEx.iCurTestType ? BST_CHECKED:BST_UNCHECKED);	

	CDC* pdc = m_Listctrl.GetDC();
	CSize sizet = pdc->GetTextExtent(CString(TEXT("  11:30:20 ")));
	CSize sizec = pdc->GetTextExtent(CString(TEXT("   2.46E-01   ")));
	m_Listctrl.ReleaseDC(pdc);

	m_Listctrl.SetExtendedStyle(m_Listctrl.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	CRect rect;
	m_Listctrl.GetClientRect(rect);	
	m_Listctrl.InsertColumn(_VACUUM_LEAK_CTRL_LIST_COL_TIME_OTHER, TEXT("Time"),LVCFMT_LEFT,sizet.cx);
//	m_Listctrl.InsertColumn(_VACUUM_LEAK_CTRL_LIST_COL_VGC_GAUGE, TEXT("Penning"),LVCFMT_LEFT,sizec.cx << 1);
	m_Listctrl.InsertColumn(_VACUUM_LEAK_CTRL_LIST_COL_VGC_GAUGE, TEXT("VGC ch3"),LVCFMT_LEFT,sizec.cx << 1); //2008.02.14
	m_Listctrl.InsertColumn(_VACUUM_LEAK_CTRL_LIST_COL_ION_GAUGE, TEXT("Ion Gauge"),LVCFMT_LEFT,sizec.cx);

	m_ListctrlExhaust.SetExtendedStyle(m_Listctrl.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ListctrlExhaust.InsertColumn(_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME, TEXT("VGC"),LVCFMT_LEFT,sizet.cx);
	m_ListctrlExhaust.InsertColumn(_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME, TEXT("Ion Gauge"),LVCFMT_LEFT,sizet.cx << 1);
	m_ListctrlExhaust.InsertColumn(_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME, TEXT("压力(Pa)"),LVCFMT_LEFT,sizec.cx);
	m_ListctrlExhaust.InsertColumn(_VACUUM_EXHAUST_CTRL_LIST_COMMENT, TEXT("Comment"),LVCFMT_LEFT,sizec.cx);
	m_ListctrlExhaust.InsertColumn(_VACUUM_EXHAUST_CTRL_LIST_APPEND, TEXT("Extra Time"),LVCFMT_LEFT,sizec.cx);
	
	SetDlgItemText(IDC_COMBO_INTERVAL,_T("60"));
	SetDlgItemText(IDC_COMBO_RECORD_TIME,_T("20"));
	SetDlgItemText(IDC_COMBO_VACUUM_WAIT_TIME,_T("600"));
	SetDlgItemText(IDC_COMBO_LEAKRATE_START_PRESSURE,_T("1.0E-4"));
	SetDlgItemText(IDC_COMBO_LEAKRATE_KEEP_TIME,_T("10"));
	

	//填写曲线选择框m_cmbCurves
	fn_ReInitGraphCombo();
/*
	CZYGraphData* pZYGraphData;
//	ASSERT(g_CZYGraphPlot.listData.GetCount() == 2); //此时应该有两项，ch3，ch2
	POSITION pos = g_CZYGraphPlot.listData.GetHeadPosition();
	for(int i=0;i<g_CZYGraphPlot.listData.GetCount();i++)
	{
		pZYGraphData = g_CZYGraphPlot.listData.GetNext(pos);
		m_cmbCurves.AddString(pZYGraphData->strName);
	}
	*/

	//补充说明框
	GetDlgItem(IDC_EDIT_COMMENT)->GetWindowRect(&rt2);//
	pwnd = GetDlgItem(IDC_EDIT_VACUUM_HELP);
	pwnd->GetWindowRect(&rect);
	rect.bottom = rt1.bottom-3;
	rect.right  = rt2.right;
	ScreenToClient(&rect);
	pwnd->MoveWindow(&rect);

	fn_ShowWindow();
}

//设置 抽速需要测量的内容！
void COAVacuumView::OnBnClickedButtonExhaustSt()
{
	ASSERT(g_stVacuumTestEx.bTesting == false);
}

//设置与检查开始条件
bool COAVacuumView::fn_CheckStartCondition(void* pstVTE)
{
	stVacuumTestEx* p = (stVacuumTestEx*)pstVTE;
	
	CString str;
	int i;
	
	if( (_VACUUM_TESTTYPE_LEAKRATE == p->iCurTestType ) || (_VACUUM_TESTTYPE_RECORDER == p->iCurTestType))
	{
		//记录总长度
		GetDlgItemText(IDC_COMBO_RECORD_TIME,str);
		i = _ttoi(str);
		if( i <= 0)
		{
			//AfxMessageBox(TEXT("Record time is invalid"));
			AfxMessageBox(g_item2_String[_ITEM2_STR_TESTTIME_INVALID_ERR].IsEmpty()?
						TEXT("错误： 记录时间无效！\r\n Record time is invalid."):g_item2_String[_ITEM2_STR_TESTTIME_INVALID_ERR]);
			return false;
		}
		else
		{
			p->iRecordTime = i * 60; //由分钟 转成 秒钟
		}

		//记录总时间
		GetDlgItemText(IDC_COMBO_INTERVAL,str);
		i = _ttoi(str);
		if( i <= 0)
		{
			//AfxMessageBox(TEXT("Interval time is invalid"));
			AfxMessageBox(g_item2_String[_ITEM2_STR_INTERVAL_TIME_ERR].IsEmpty()?
				TEXT("错误： 测试间隔时间无效！\r\nError:Interval time is invalid."):g_item2_String[_ITEM2_STR_INTERVAL_TIME_ERR]);
			return false;
		}
		else
		{
			p->iRecordInterval = i;	
		}

		if( _VACUUM_TESTTYPE_LEAKRATE == p->iCurTestType )
		{
			GetDlgItemText(IDC_COMBO_LEAKRATE_START_PRESSURE,str);
			p->sLeakRate.dStartPressure = _tstof(str);

			if( p->sLeakRate.dStartPressure <= 1.0E-7 )
			{//
				//AfxMessageBox(TEXT("错误：要求的起始真空度无效！"));
				AfxMessageBox(g_item2_String[_ITEM2_STR_START_PRESSURE_INVALID ].IsEmpty()?
				TEXT("错误：要求的起始真空度无效！\r\nError:Starting pressure is invalid."):g_item2_String[_ITEM2_STR_START_PRESSURE_INVALID]);


			//	((CEdit*)GetDlgItem(IDC_COMBO_LEAKRATE_START_PRESSURE))->SetSel(0,-1);
				GetDlgItem(IDC_COMBO_LEAKRATE_START_PRESSURE)->SetFocus();
				return false;
			}

			GetDlgItemText(IDC_COMBO_VACUUM_WAIT_TIME,str);
			p->sLeakRate.iWaitTimeSeconds = _ttoi(str) * 60;

			if( p->sLeakRate.iWaitTimeSeconds <= 0 )
			{//
				//AfxMessageBox(TEXT("等待真空度到达的时间必须大于０"));				
				AfxMessageBox(g_item2_String[_ITEM2_STR_WAITINT_TIME_MIN_ERR].IsEmpty()?
				TEXT("错误：等待真空度到达的时间必须大于０！\r\nError:Starting pressure waiting time should greater than 0."):g_item2_String[_ITEM2_STR_WAITINT_TIME_MIN_ERR]);

				//((CEdit*)GetDlgItem(IDC_COMBO_VACUUM_WAIT_TIME))->SetSel(0,-1);
				GetDlgItem(IDC_COMBO_VACUUM_WAIT_TIME)->SetFocus();
				return false;
			}
		
			GetDlgItemText(IDC_COMBO_LEAKRATE_KEEP_TIME,str);
			p->sLeakRate.iPressureKeepTime = _ttoi(str);
			if( p->sLeakRate.iPressureKeepTime < 0 )
			{//
				//AfxMessageBox(TEXT("稳定时间必须大于０"));				
				AfxMessageBox(g_item2_String[_ITEM2_STR_PRESSURE_STABLE_TIME_MIN_ERR].IsEmpty()?
				TEXT("错误：稳定时间必须大于０！\r\nError:Stable time should greater than 0."):g_item2_String[_ITEM2_STR_PRESSURE_STABLE_TIME_MIN_ERR]);

				//((CEdit*)GetDlgItem(IDC_EDIT_LEAKRATE_KEEP_TIME))->SetSel(0,-1);
				GetDlgItem(IDC_COMBO_LEAKRATE_KEEP_TIME)->SetFocus();
				return false;
			}

			bool bDiffusionPump;
			if(g_pVacProThread->fn_IsDiffusionPump(bDiffusionPump))
			{
				if(!bDiffusionPump)
				{
					//AfxMessageBox(TEXT("错误：主泵必须是扩散泵！\r\n提示：转为记录仪模式吧."),MB_OK|MB_ICONSTOP);
					AfxMessageBox(g_item2_String[_ITEM2_STR_NOT_DP_MACHINE_ERR].IsEmpty()?
				TEXT("错误：漏率测试暂时只支持带扩散泵的机器。请转为记录模式！\r\nNow, only coaters with diffusion pump are supported by this program."):g_item2_String[_ITEM2_STR_NOT_DP_MACHINE_ERR]);

					return false;
				}
			}
			else
			{
				//AfxMessageBox(TEXT("错误：plc没有通信"));				
				AfxMessageBox(g_item2_String[_ITEM2_STR_PLC_COMM_ERR].IsEmpty()?
				TEXT("出错：plc通信不正常！\r\n plc communication failed."):g_item2_String[_ITEM2_STR_PLC_COMM_ERR]);
				return false;
			}
		}
	//	IDC_EDIT_VACUUM_WAIT_TIME		
	}
	else 
	{//抽速
			for(int i=0; i< g_iVacuumExhaustDataItems; i++)
			{
				g_pstVacuumExhaustData[i].bRecord = false;
				g_pstVacuumExhaustData[i].bIonGaugeRecord = false;
			}

			//2008.02.18, 只支持扩散泵
			bool bDiffusionPump;
			if(g_pVacProThread->fn_IsDiffusionPump(bDiffusionPump))
			{
				if(!bDiffusionPump)
				{
					//AfxMessageBox(TEXT("错误：主泵必须是扩散泵！\r\n提示：转为记录仪模式吧."),MB_OK|MB_ICONSTOP);
					AfxMessageBox(g_item2_String[_ITEM2_STR_NOT_DP_MACHINE_ERR].IsEmpty()?
				TEXT("错误：只支持带扩散泵的机器。请转为记录模式！\r\nNow, only coaters with diffusion pump are supported by this program."):g_item2_String[_ITEM2_STR_NOT_DP_MACHINE_ERR]);

					return false;
				}
			}

	  //增加 GI-M2 由程序自动确认。2007.12.20
			if(g_bUseIongauge && g_IonGauge.HasIonGauge() && g_IonGauge.GetGaugeType()!=5 ) //2008.02.18,增加g_IonGauge.GetGaugeType()!=5 BPG402
			{
				bool bRemoteControl;
				if( g_pVacProThread->fn_IsIonGaugeRemoteControl(bRemoteControl))
				{
					if(!bRemoteControl)
					{
						//if( IDYES != AfxMessageBox(TEXT("Ion Gauge 非 Remote 模式，Ion Gauge的数据将可能不正常，继续吗？"),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
						if(  IDYES != AfxMessageBox(g_item2_String[_ITEM2_STR_IONGAUGE_NOT_REMOTE_ERR].IsEmpty()?
						TEXT("Ion Gauge 不在 Remote 模式，Ion Gauge的数据将不正常，继续吗？\r\n Ion Gauge is not in remote mode, Are you sure to continue?") : g_item2_String[_ITEM2_STR_IONGAUGE_NOT_REMOTE_ERR]))
						{
							return false;
						}
					}
				}
				else
				{
					//AfxMessageBox(TEXT("错误：plc没有通信"));				
					AfxMessageBox(g_item2_String[_ITEM2_STR_PLC_COMM_ERR].IsEmpty()?
						TEXT("出错：plc通信不正常！\r\n plc communication failed."):g_item2_String[_ITEM2_STR_PLC_COMM_ERR]);
					return false;
				}

			}
	}

	//
	GetDlgItemText(IDC_EDIT_CHAMBER_VOLUME,str);
	p->sLeakRate.dChamberVolume = _tstof(str);
	if( p->sLeakRate.dChamberVolume < 0.00001)
	{
		AfxMessageBox(TEXT("Chamber volume is invalid"));
		return false;
	}

	//拷贝进去
	EnterCriticalSection(&gCriSec_Vacuum);
		memcpy(&g_stVacuumTestEx,p,sizeof(stVacuumTestEx));
	LeaveCriticalSection(&gCriSec_Vacuum);	
	
	g_iVacuumTestCnt = 0;     //每次测试开始后数据到来的次数. //同秒数 
	
	//g_iVacuumStartPos = g_iVacuumPressurePos;2008.05.22
	g_iVacuumStartPos = 0;
	g_iVacuumPressurePos = 0; //每次重新清0，否则出现指针不能对齐。
	                          //画图中的指针始终指向 0。
	
	//pstVTE
	return true;
}

void COAVacuumView::OnNMDblclkListVacuumContentExhaust(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	bool bTesting;
	EnterCriticalSection(&gCriSec_Vacuum);
	bTesting = g_stVacuumTestEx.bTesting;
	LeaveCriticalSection(&gCriSec_Vacuum);
	//if(bTesting)
	{
		CPoint cspoint;
		if(GetCursorPos(&cspoint))
		{
			m_ListctrlExhaust.ScreenToClient(&cspoint);
			LVHITTESTINFO lvhti;
			lvhti.pt = cspoint;
			if(-1 != m_ListctrlExhaust.SubItemHitTest( &lvhti ))
			{
				if(	_VACUUM_EXHAUST_CTRL_LIST_APPEND == lvhti.iSubItem)
				{
					if(bTesting)
					{//测量时只能进行　填写时间
						DWORD dwElapsed =( GetTickCount() - g_dwStartTime )/1000;
						_stprintf(g_buffer,TEXT("%02d:%02d:%02d"),dwElapsed/3600,(dwElapsed/60)%60 ,dwElapsed%60);
						m_ListctrlExhaust.SetItemText(lvhti.iItem,lvhti.iSubItem,g_buffer);
					}
					else
					{//不在测量时，可进行更改
						int iColumWidth = 0;
						for(int i=0;i<_VACUUM_EXHAUST_CTRL_LIST_APPEND;i++)
						{
							iColumWidth += m_ListctrlExhaust.GetColumnWidth(i);
						}

						CDlgVacExhaustEdit dlg;
						m_ListctrlExhaust.GetItemPosition(lvhti.iItem,&dlg.m_point);
						dlg.m_point.x += iColumWidth;
						m_ListctrlExhaust.ClientToScreen(&dlg.m_point);

						dlg.m_strEdit = m_ListctrlExhaust.GetItemText(lvhti.iItem,lvhti.iSubItem);
						
						if(IDOK==dlg.DoModal())
						{
							m_ListctrlExhaust.SetItemText(lvhti.iItem,lvhti.iSubItem,dlg.m_strEdit);
						}
					}
				}
				else if(_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME == lvhti.iSubItem)
				{//真空度列
					if(lvhti.iItem<g_iVacuumExhaustDataHighItems)
					{
						int iColumWidth = 0;
						for(int i=0;i<_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME;i++)
						{
							iColumWidth += m_ListctrlExhaust.GetColumnWidth(i);
						}

						CDlgVacExhaustEdit dlg;
						m_ListctrlExhaust.GetItemPosition(lvhti.iItem,&dlg.m_point);
						dlg.m_point.x += iColumWidth;
						m_ListctrlExhaust.ClientToScreen(&dlg.m_point);

						dlg.m_strEdit = m_ListctrlExhaust.GetItemText(lvhti.iItem,lvhti.iSubItem);
						
						if(IDOK==dlg.DoModal())
						{
							m_ListctrlExhaust.SetItemText(lvhti.iItem,lvhti.iSubItem,dlg.m_strEdit);
						}
					}
				}
			}
		}
	}


	*pResult = 0;
}

void COAVacuumView::OnNMRdblclkListVacuumContentExhaust(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	bool bTesting;
	EnterCriticalSection(&gCriSec_Vacuum);
	bTesting = g_stVacuumTestEx.bTesting;
	LeaveCriticalSection(&gCriSec_Vacuum);

	if(bTesting)
	{
		CPoint cspoint;
		if(GetCursorPos(&cspoint))
		{
			m_ListctrlExhaust.ScreenToClient(&cspoint);
			LVHITTESTINFO lvhti;
			lvhti.pt = cspoint;
			if(-1 != m_ListctrlExhaust.SubItemHitTest( &lvhti ))
			{
				if(	_VACUUM_EXHAUST_CTRL_LIST_APPEND == lvhti.iSubItem)
				{
					m_ListctrlExhaust.SetItemText(lvhti.iItem,lvhti.iSubItem,TEXT(""));
				}				
			}
		}
	}
	*pResult = 0;
}

BOOL COAVacuumView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
		m_ToolTips.RelayEvent(pMsg);
		break;
	}
	return CFormView::PreTranslateMessage(pMsg);
}

HBRUSH COAVacuumView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	//为什么不起作用呢？ 2007.12.06 debugging
	if( pWnd->GetDlgCtrlID() == IDC_BTN_START_VACUUM_TEST )
	{
		pDC->SetTextColor(RGB(255,0,0));
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
//#include <assert.h>
//assert(g_IonGauge.HasIonGauge() && TEXT("ZHEN DE"));
void COAVacuumView::OnBnClickedCheckUsingIongauge()
{
	// TODO: Add your control notification handler code here
	//是否在抽速策略过程中使用 IonGauge. 如果系统存在IonGauge，而选择不使用，则开始测量时进行提示.

	//ASSERT(g_IonGauge.HasIonGauge());
	//ASSERT(g_IonGauge.HasIonGauge() && ("ZHEN DE"));
	g_bUseIongauge = (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_USING_IONGAUGE))->GetCheck()) ? true : false;
	//bool   
}

#include <math.h>
void COAVacuumView::OnBnClickedButtonClearListctrl()
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox(TEXT("no"));
	m_Listctrl.DeleteAllItems();

	
	/*
	//Test 2008.02.13
	double dVolt = 862;
	double p = pow(10.0, dVolt * 7.0 / 4000.0 -7.0 );

	
	//D6587 = 1422 --> 2.6E-4 . BPG402SD D9085 = 3 ?
	p = pow( 10, ( dVolt/400.0 - 7.75 ) / 0.75 + 2 ) ;// c = 2 , pa

	//p = 10^(1.667U-9.333)
	p = (double) 923/400;
	p = pow( 10, 1.667 * p - 9.333  ) ;


	//p = 10^[U/(10/11) - 8]
	p = 923;
	p = pow(10, p/400*11/10 -6);

	// 923 : 3.4E-4
	double U = 1* ( 5.6 + 0.6 * log10 ( 2.3E-3 )); //2.5 = 10000/4000.
	*/
	
}

void COAVacuumView::OnSize(UINT nType, int cx, int cy)
{//debugging 2007.12.09
	CFormView::OnSize(nType, cx, cy);
	if(GetSafeHwnd() )
	{
		if(cx==0)
		{
			int h = 0;
		}
		if(nType==SIZE_MAXHIDE || nType==SIZE_MINIMIZED)
		{
			int y =8;
		}
		else
		{
		int x = 9;
		}
		if(nType!=0)
		{
			AfxMessageBox(_T("nType!=0"));
		}
	}
	// TODO: Add your message handler code here
}

void COAVacuumView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
	CDC* pdc = GetDC();
	/*
	//pdc->FillSolidRect(&m_rtVacPic,RGB(0,0,0));
	pdc->SetBkMode(TRANSPARENT);
	//pdc->Ellipse(&m_rtVacPic);
	pdc->SetTextColor(RGB(80,80,80));

	//在日文操作系统下，直接DrawText将显示不正确. S561, 2007.12.20
	CFont* oldFont;
	CFont* pFont = GetFont();
	oldFont = pdc->SelectObject(pFont);
	pdc->DrawText(CString(TEXT("这里将显示真空度图形,to be continued...")),&m_rtVacPic,DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	pdc->SelectObject(oldFont);
	*/

	g_CZYGraphPlot.PlotGraph(pdc,m_rtVacPic);
	
	ReleaseDC(pdc);
}

void COAVacuumView::fn_AppendHelpText(LPCTSTR lpText)
{
	CEdit* pwnd = (CEdit*)GetDlgItem(IDC_EDIT_VACUUM_HELP);
	if(pwnd->GetSafeHwnd())
	{
		int nLen=pwnd->GetWindowTextLength (); 
		pwnd->SetSel (nLen, nLen); 
		pwnd->ReplaceSel (lpText);
	}
}
#include "atlrx.h"
void COAVacuumView::OnBnClickedButtonSaveListctrl()
{
	//TEST begin

	/*

    CAtlRegExp<> reUrl;
    // Five match groups: scheme, authority, path, query, fragment
    REParseError status = reUrl.Parse(
        TEXT("({[^:/?#]+}:)?(//{[^/?#]*})?{[^?#]*}(?{[^#]*})?(#{.*})?" ));

    if (REPARSE_ERROR_OK != status)
    {
        // Unexpected error.
        return ;
    }

    CAtlREMatchContext<> mcUrl;
    if (!reUrl.Match(TEXT("http://search.microsoft.com/us/Search.asp?qu=atl&boolean=ALL#results"),&mcUrl))
    {
        // Unexpected error.
        return;
    }

	CString str1;
    for (UINT nGroupIndex = 0; nGroupIndex < mcUrl.m_uNumGroups;
         ++nGroupIndex)
    {
        const CAtlREMatchContext<>::RECHAR* szStart = 0;
        const CAtlREMatchContext<>::RECHAR* szEnd = 0;
        mcUrl.GetMatch(nGroupIndex, &szStart, &szEnd);

        ptrdiff_t nLength = szEnd - szStart;
		str1.Format(_T("%d: \"%.*s\"\n"),nGroupIndex, nLength, szStart); //很有趣的用法,还是我以前不知道. %.*s可指定待输出的字符数字.
		AfxMessageBox(str1);

        //printf_s("%d: \"%.*s\"\n", nGroupIndex, nLength, szStart);
    }
	*/


	//TEST end
	// TODO: Add your control notification handler code here
	static stVacuumTestEx lstTestEx;
	EnterCriticalSection(&gCriSec_Vacuum);
		memcpy(&lstTestEx,&g_stVacuumTestEx,sizeof(stVacuumTestEx));
	LeaveCriticalSection(&gCriSec_Vacuum);

	

//	int iLength;	
//	TCHAR  lptchTmp[256];  //不可重入
	DWORD bufsize = 512;
	int iCnt;
	if( _VACUUM_TESTTYPE_EXHAUST == lstTestEx.iCurTestType )
	{
		iCnt = m_ListctrlExhaust.GetItemCount();
	}
	else
	{
		iCnt = m_Listctrl.GetItemCount();
	}
	
//	int ibuf;	
	TCHAR cbuf[512];
//	TCHAR cbuf2[512];
	
	if(iCnt==0)
	{	
		SYSTEMTIME   st;   
		CString str;
		GetLocalTime(&st);   
		str.Format(TEXT("Error: no data! \r\n%4d-%02d-%02d %02d:%02d:%02d\r\n"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

		fn_AppendHelpText(str);

		return ; 
	}

	::GetModuleFileName(NULL,g_buffer,512);
	CString str(g_buffer);
	int ibackslash = str.ReverseFind(TEXT('\\'));
	g_buffer[ibackslash + 1]= TEXT('\0');

	bufsize = 512-2;
	::GetComputerName(cbuf,&bufsize);

	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	str.Format(TEXT("-OA-VAC-%4d%02d%02d %02d%02d%02d.log"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	_tcscat(cbuf,str);
	_tcscat(g_buffer,cbuf);
		CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrTitle = _T("Save Vacuum Data into File");
	dlg.m_ofn.lpstrFilter = _T("Optorun Assitant log(*.log)\0*.log\0");
	dlg.m_ofn.lpstrInitialDir = g_buffer; 
	dlg.m_ofn.lpstrFile = g_buffer;

	if(dlg.DoModal()==IDOK)
	{
		str = dlg.GetPathName();
		if(-1==str.Find(_T(".log")))
		{str +=_T(".log");}			
		fn_SaveVacFile(str,lstTestEx.iCurTestType);
	}
}

// 将真空数据保存进文件,2007.12.12
bool COAVacuumView::fn_SaveVacFile(LPCTSTR sFilename, int  iVacType)
{
	CFile fFile;
	if(!fFile.Open(sFilename,CFile::modeCreate|CFile::modeWrite))
	{
		return false;
	}

	CString str;


	fFile.SeekToBegin();
	//Save unicode file head
#ifdef UNICODE
	g_buffer[0] = 0xFEFF; //FF在低 FE在高
	fFile.Write(g_buffer,1*sizeof(TCHAR)); 
#endif

    TCHAR* p = TEXT("OPTORUN ASSITANT TEST FILE\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	if(iVacType==_VACUUM_TESTTYPE_EXHAUST)
	{
		p = TEXT("FILE TYPE: VACUUM EXHAUST(测抽速)\x0d\x0a");			//2018-7-30	ZhouYi
		fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
	}
	else if(iVacType==_VACUUM_TESTTYPE_LEAKRATE)
	{
		p = TEXT("FILE TYPE: VACUUM LEAKRATE(测漏率)\x0d\x0a");			//2018-7-30 ZhouYi
		fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
	}
	else
	{
		p = TEXT("FILE TYPE: VACUUM PRESSURE RECORDER（真空压力记录仪）\x0d\x0a");		//2018-7-30 ZhouYi
		fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
	}

	TCHAR cbuf[256];

	int iLength;
	iLength = _stprintf(g_buffer,TEXT("PROGRAM VERSION: %.3f\x0d\x0a"),g_dProgramVersion);
	if(iLength)
		fFile.Write( g_buffer,iLength * sizeof(TCHAR) );

	MultiByteToWideChar(CP_UTF8,0,__DATE__,strlen(__DATE__) + 1,cbuf,1024); // strlen(..) + 1，保证连字符串的结束符号一并转化 
	iLength = _stprintf(g_buffer,TEXT("PROGRAM DATE: %s\x0d\x0a"),cbuf);
	if(iLength)
		fFile.Write( g_buffer,iLength * sizeof(TCHAR) );
	
	iLength = _stprintf(g_buffer,TEXT("PROGRAM DATA VERSION: %.3f\x0d\x0a"),g_dDataVersion);
	if(iLength)
		fFile.Write(g_buffer, iLength * sizeof(TCHAR));

	//文件保存时刻
	DWORD bufsize = 256;
	::GetComputerName(cbuf,&bufsize);
	iLength = wsprintf(g_buffer,TEXT("%s%s\x0d\x0a"),TEXT("Computer Name: "),cbuf);
	fFile.Write(g_buffer,iLength*sizeof(TCHAR));

	SYSTEMTIME   st;   
	GetLocalTime(&st); 
	str.Format(TEXT("Save Time: %4d-%02d-%02d %02d:%02d:%02d\x0d\x0a"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	fFile.Write(str,sizeof(TCHAR) * str.GetLength()); 

	iLength = _stprintf(g_buffer,TEXT("Ion Gauge: %s\r\n"),g_IonGauge.GetGaugeTypeName());
	if(iLength)
		fFile.Write(g_buffer, iLength * sizeof(TCHAR));
	
	
	//存储说明栏内容
	p = TEXT("<COMMENT>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
	GetDlgItemText(IDC_EDIT_COMMENT,str);
	if(str.GetLength()>0)
	{
		if( TEXT('\x0a') != str.GetAt(str.GetLength()-1))
		{
			str.Append(TEXT("\x0d\x0a"));
		}		
	}
	fFile.Write(str,str.GetLength()*sizeof(TCHAR));
	p = TEXT("</COMMENT>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	//数据起始标志
	p=TEXT("<DATA>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	int iCnt;
	if( _VACUUM_TESTTYPE_EXHAUST == iVacType )
	{
		iCnt = m_ListctrlExhaust.GetItemCount();
	}
	else
	{
		iCnt = m_Listctrl.GetItemCount();
	}

	if(iVacType==_VACUUM_TESTTYPE_EXHAUST)
	{//
		HDITEM hdi;
		CHeaderCtrl* pmyHeaderCtrl = m_ListctrlExhaust.GetHeaderCtrl();

		//列表头
		str.Empty();
		hdi.mask = HDI_TEXT;
		hdi.pszText = cbuf;
		hdi.cchTextMax = 256;
		for(int i = 0 ; i< pmyHeaderCtrl->GetItemCount();i++)
		{
			pmyHeaderCtrl->GetItem(i,&hdi);
			str += hdi.pszText;
			str += TEXT('\t');
		}
		str += TEXT("\x0d\x0a");
		fFile.Write(str, str.GetLength() * sizeof(TCHAR));
		
		//列表内容
		for(int i=0;i<iCnt;i++)
		{
			iLength = _stprintf(g_buffer,TEXT("%s\t%s\t%s\t%s\t%s\r\n"),m_ListctrlExhaust.GetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_VGC_TIME),
				m_ListctrlExhaust.GetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COL_ION_TIME),
				m_ListctrlExhaust.GetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_PRESSURE_TIME),
				m_ListctrlExhaust.GetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_COMMENT),
				m_ListctrlExhaust.GetItemText(i,_VACUUM_EXHAUST_CTRL_LIST_APPEND));
			fFile.Write(g_buffer,iLength*sizeof(TCHAR));
		}
	}
	else
	{//漏率和记录仪
		//列表头
		iLength = _stprintf(g_buffer,TEXT("Time\tVGC-ch3\t%s\r\n"),g_IonGauge.GetGaugeTypeName());
		if(iLength)
			fFile.Write(g_buffer, iLength * sizeof(TCHAR));

		//fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
		//列表内容
		for(int i=iCnt-1;i>=0;i--)
		{ 
			
			iLength = _stprintf(g_buffer,TEXT("%s\t%s\t%s\r\n"),m_Listctrl.GetItemText(i,_VACUUM_LEAK_CTRL_LIST_COL_TIME_OTHER),
				m_Listctrl.GetItemText(i,_VACUUM_LEAK_CTRL_LIST_COL_VGC_GAUGE),
				m_Listctrl.GetItemText(i,_VACUUM_LEAK_CTRL_LIST_COL_ION_GAUGE));

			fFile.Write(g_buffer,iLength*sizeof(TCHAR));
		}
	}

	p=TEXT("</DATA>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	_stprintf(g_buffer,TEXT("File %s Saved!\x0d\x0a"),fFile.GetFileName()),
	fFile.Close();
	g_bVacuumSaved = true; //存储标志

	fn_AppendHelpText(g_buffer);

	return true;
}

//整体移动图形相对位置
int COAVacuumView::fn_MoveCurve(void* pZYGraph,double dMoveValue)
{
	ASSERT(pZYGraph!=NULL);
	CZYGraphData* p = (CZYGraphData*)pZYGraph;
	if(p==NULL || p->iDataNumber ==0)
	{
		return 0;
	}
	p->dxDispShift += dMoveValue;
	return 1;
}

//向右移动当前选择的曲线，每次移动1秒
//若是背景，则移动1格
void COAVacuumView::OnBnClickedShiftRight()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_cmbCurves.GetCurSel();
	if( nIndex > 0 )
	{
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);
		if(pos)
		{
			CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
			fn_MoveCurve(p,1.0);
			InvalidateRect(&m_rtVacPic,false);
		}
	}
	else if(nIndex == 0)
	{
		double xGrid = (g_CZYGraphPlot.xRight - g_CZYGraphPlot.xLeft)/g_CZYGraphPlot.ixGridNum;
		g_CZYGraphPlot.xRight += xGrid;
		g_CZYGraphPlot.xLeft  += xGrid;
		InvalidateRect(&m_rtVacPic,false);
	}
}
//向左移动当前选择的曲线，每次1秒
//若是背景，则每次 1 格
void COAVacuumView::OnBnClickedShiftLeft()
{
	int nIndex = m_cmbCurves.GetCurSel();
	if( nIndex > 0 )
	{
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);
		if(pos)
		{
			CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
			fn_MoveCurve(p,-1.0);
			InvalidateRect(&m_rtVacPic,false);
		}
	}
	else if(nIndex == 0)
	{
		double xGrid = (g_CZYGraphPlot.xRight - g_CZYGraphPlot.xLeft)/g_CZYGraphPlot.ixGridNum;
		if(  g_CZYGraphPlot.xLeft >= xGrid)
		{
			g_CZYGraphPlot.xLeft -= xGrid ;	
			g_CZYGraphPlot.xRight -= xGrid ;
		}
		else
		{
			g_CZYGraphPlot.xRight -= g_CZYGraphPlot.xLeft;
			g_CZYGraphPlot.xLeft   = 0;
		}
		InvalidateRect(&m_rtVacPic,false);
	}
}
//向左移动当前选择曲线，每次1格
//若是背景，则每次 1/4 总格数
void COAVacuumView::OnBnClickedShiftLeft2()
{
	int nIndex = m_cmbCurves.GetCurSel();
	double xGrid = (g_CZYGraphPlot.xRight - g_CZYGraphPlot.xLeft)/g_CZYGraphPlot.ixGridNum;
	if( nIndex > 0 )
	{
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);
		if(pos)
		{
			CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
			fn_MoveCurve(p,-xGrid);
			InvalidateRect(&m_rtVacPic,false);
		}
	}
	else if( nIndex == 0 )
	{
		if(  g_CZYGraphPlot.xLeft >= xGrid * (g_CZYGraphPlot.ixGridNum>>2) )
		{
			g_CZYGraphPlot.xLeft -= xGrid * (g_CZYGraphPlot.ixGridNum>>2) ;	
			g_CZYGraphPlot.xRight -= xGrid * (g_CZYGraphPlot.ixGridNum>>2) ;
		}
		else
		{
			g_CZYGraphPlot.xRight -= g_CZYGraphPlot.xLeft;
			g_CZYGraphPlot.xLeft   = 0;
		}
		InvalidateRect(&m_rtVacPic,false);
	}
}
//向右移动当前选择曲线，每次1格
//若是背景，则每次 1/4 总格数
void COAVacuumView::OnBnClickedShiftRight2()
{
	int nIndex = m_cmbCurves.GetCurSel();
	double xGrid = (g_CZYGraphPlot.xRight - g_CZYGraphPlot.xLeft)/g_CZYGraphPlot.ixGridNum;
	if( nIndex > 0 )
	{
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);
		if(pos)
		{
			CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
			fn_MoveCurve(p,xGrid);
			InvalidateRect(&m_rtVacPic,false);
		}
	}
	else if( nIndex == 0 )
	{
		g_CZYGraphPlot.xRight += xGrid * (g_CZYGraphPlot.ixGridNum>>2) ;
		g_CZYGraphPlot.xLeft  += xGrid * (g_CZYGraphPlot.ixGridNum>>2);
		InvalidateRect(&m_rtVacPic,false);
	}
}
//选择变了
void COAVacuumView::OnCbnSelchangeComboCurves()
{
	// TODO: Add your control notification handler code here
	bool bEnable = false;//IDC_BTN_DELETE_CURVE
	int nIndex = m_cmbCurves.GetCurSel();
	if( nIndex > 0 )  //0为BackGround
	{   //-1，表示没有选择. 第0项代表为ch3，不可删除. 
		//第1项若是 ch2-ion，则不可删除
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);
		CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
		if(p->bCanDelete)
		{
			bEnable = true;
		}
	}
	GetDlgItem(IDC_BTN_DELETE_CURVE)->EnableWindow(bEnable);
}

//添加曲线
#define MAXFILE 65536
void COAVacuumView::OnBnClickedBtnAddVacuumFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,_T("log"),NULL,OFN_EXPLORER |OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT); 
	dlg.m_ofn.lpstrTitle = _T("Open vacuum pressure data files");
	dlg.m_ofn.lpstrFilter= _T("Vacuum pressure log file(*.log)\0*.log;*.asc\0");
	//dlg.m_ofn.lpstrInitialDir = //GetCurrentPath()pDoc->m_strSpectrPath;

	static	TCHAR pc[MAXFILE];
	dlg.m_ofn.nMaxFile = MAXFILE;//MAXFILE;
	dlg.m_ofn.lpstrFile = pc;
	dlg.m_ofn.lpstrFile[0] = NULL;

	if(dlg.DoModal() ==IDOK )
	{		
		CString strFile;
		//CMyListCtrl& listctrl = ((CBottomTabFormView*)m_wndSplitter.GetPane(1,0))->m_ListPage.m_MyListCtrl;
		int iOpen=0; //files open number
		POSITION pos = dlg.GetStartPosition(); 
		while (pos != NULL)
		{ 
			strFile = dlg.GetNextPathName(pos);	
			int ix = fn_LoadVacuumPressureFile(strFile);
			if(ix == 0)
			{
				iOpen++;
			}
			else
			{
				CString str;
				str.Format(TEXT("%s \r\nRet = %d"),(LPCTSTR)strFile,ix);
				AfxMessageBox(str,MB_OK|MB_ICONSTOP);
			}
		}

		//此处需添加未正确读取的数据报告.
		//ASSERT(false);
		if(iOpen)
		{
			InvalidateRect(&m_rtVacPic,false);

			fn_ReInitGraphCombo();
			m_cmbCurves.SetCurSel(m_cmbCurves.GetCount()-1);
		}
		
	}

}


/*************************************************************************************
读取真空度测试结果数据文件，正常读取到 真空度-时间 数据则添加进 数据列表  去，否则返回错误
函数名：  fn_LoadVacuumPressureFile
输入参数：LPCTSTR szFileName; 含有真空度-时间数据的文件名
输出参数：无
返回值：  读取过程的错误代码
          0：没有出错，正常读取并已经添加进
		  1：文件读取操作本身出错
		  2：文件不是Unicode文件
		  3：文件不是可识别的 真空度-时间数据文件
		  4：文件是真空度测试文件，但没有完整的数据
		  5：内存申请出错
		  6：Data版本高，不能读取错误。
		  7：暂不支持的错误
注意：    文件必须是 Unicode 类型
          文件要求是由本软件保存的，否则不能通过文件识别。
例如：
OPTORUN ASSITANT TEST FILE
FILE TYPE: VACUUM EXHAUST
PROGRAM VERSION: 1.010
PROGRAM DATE: Dec 28 2007
PROGRAM DATA VERSION: 1.010
Computer Name: G1265
Save Time: 2007-12-28 16:37:16
Ion Gauge: GI-M2
<COMMENT>
No Heater
</COMMENT>
<DATA>
VGC	Ion Gauge	压力(Pa)	Comment	Extra Time	
	00:00:07	1.00E+005	SRV OP	
	00:01:09	4.00E+004	RV OP	
		1.00E+004		
	00:04:02	1.30E+003	MBP ON	
		1.00E+001		
	00:04:57	6.60E+000	SRV RV CL	
	00:06:04		MV OP	
00:06:39		3.00E-003		
00:07:04	00:07:10	2.00E-003		
00:08:00	00:07:41	1.30E-003		
00:08:48	00:08:23	1.00E-003		
00:15:16	00:15:11	5.00E-004		
		1.50E-004		
		1.00E-004		
Start	2007-12-28 16:18:21			
End	2007-12-28 16:37:16			
</DATA>
**************************************************************************************/
int COAVacuumView::fn_LoadVacuumPressureFile(LPCTSTR szFileName)
{
	CFile fFile;
	int   iret = 3;   //返回值
	byte* pszCurFile = NULL; //临时指针，分配内容给

	double dFileDataVersion;  //文件数据版本
	//以下定义仅在本函数内部有效
#define _VAC_FILE_TYPE_EXHAUST  1
#define _VAC_FILE_TYPE_LEAKRATE 2
#define _VAC_FILE_TYPE_RECORDER 3
	int   iFileType = 0;  //记录文件内容类型
	//int   iIonGauge = -1;  //有没有Iongauge或新的VGC402的ch2
	CString strIonGaugeName; //ionGaugeName 或新的VGC402的ch2. 如果为空，说明没有。

	if(!fFile.Open(szFileName,CFile::modeRead))
	{
		iret = 1;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	DWORD dwsize = fFile.GetLength();
	if(dwsize<=4)
	{//文件太短错误
		iret = 3;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	pszCurFile = new byte[dwsize+2];
	if( NULL == pszCurFile )
	{
		iret = 5;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}
	fFile.Read(pszCurFile,dwsize);

	if( 0xFF!= *(pszCurFile) || 0xFE!=*(pszCurFile+1))
	{//查找unicode 文件头.进入此处表明没有unicode文件头出错.
		iret = 2;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	WCHAR* pwFile = (WCHAR*)(pszCurFile+2);  //文件有效内容起始指针
	WCHAR* pwsz   = pwFile; //当前指针
	WCHAR* pwTmp  = NULL;
	pwsz = wcsstr(pwFile,TEXT("OPTORUN ASSITANT TEST FILE"));
	if(!pwsz)
	{//没有找到本软件的特征字符串.
		iret = 3;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	//查找文件类型
	iFileType = -1;
	if( pwsz = wcsstr(pwFile,TEXT("FILE TYPE: VACUUM EXHAUST")) )
	{
		iFileType = _VAC_FILE_TYPE_EXHAUST;
	}
	else if(pwsz = wcsstr(pwFile,TEXT("FILE TYPE: VACUUM LEAKRATE")) )
	{
		iFileType = _VAC_FILE_TYPE_LEAKRATE;
	}
	else if(pwsz = wcsstr(pwFile,TEXT("FILE TYPE: VACUUM PRESSURE RECORDER")) )
	{
		iFileType = _VAC_FILE_TYPE_RECORDER;
	}
	//判断文件类型查找结果
	if( iFileType == -1)
	{
		//没有找到本软件的特征字符串.
		iret = 3;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	dFileDataVersion = -1;
	if( pwsz = wcsstr(pwFile,TEXT("PROGRAM DATA VERSION")) )
	{
		pwsz = wcsstr(pwsz,_T(":"));
		if( pwsz )
		{
			pwsz ++; //跳过找到的符号 ":"
			dFileDataVersion = _wtof(pwsz);
		}
	}
	//判断版本查找结果
	if( dFileDataVersion <= 0 )
	{
		iret = 3;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}
	else if( dFileDataVersion > (g_dDataVersion + 1E-10) )
	{//文件版本高过本软件能处理的文件版本，错误
		//后面的 1E-10，用于转化中可能存在的精度问题。
		iret = 6;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	//查找有没有 IonGauge
	//"Ion Gauge"到 换行符号之间有没有字母. //目前类型中必然含有这几个字符 "GMB"
	if( pwsz = wcsstr(pwsz,TEXT("Ion Gauge:")) )
	{
		pwsz = wcsstr(pwsz,_T(":"));
		if( pwsz )
		{
			pwsz++; //跳过找到的符号 ":"
			pwTmp = wcsstr( pwsz,TEXT("\r\n") ); //向后找 回车符号。
			if(!pwTmp)
			{//没有找到行尾符号，退出
				iret = 3;
				goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
			}

			for(;pwsz!=pwTmp;pwsz++)
			{//在 :与回车符号之间查找字符. 要求IonGauge名必须含有 字母。
				if( *pwsz<=L'Z' && *pwsz >= L'A')
				{//
					//iIonGauge = 1; //查找到 Ion Gauge名称.
					*pwTmp = L'\0'; //先写 字符串结尾符号
					strIonGaugeName = pwsz; //拷贝IonGauge名称
					*pwTmp = L'\r'; //还原.
					break;
				}
			}			
		}
	}

	//pwsz已经指向 IonGauge行尾.
	//向后查找 <DATA> </DATA>对
	if( pwTmp = wcsstr(pwsz,TEXT("<DATA>")))
	{
		pwFile = pwTmp;//指向<DATA>起始处
		pwTmp = wcsstr(pwTmp,TEXT("</DATA>"));
	}

	//只要没有找到 <DATA> </DATA>对，则退出
	if(!pwTmp)
	{
		iret = 3;
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	int   iLines; //行数
	int   iTabCounts = 0;// '\t'数，应有 iTabCount = iLines*3，进行有效性验证
	//查找 <DATA> </DATA>对 之间的可能数据行数
	//此时pwFile 指向<DATA>起始处
	pwsz = pwTmp; //让pwsz指向</DATA>处
	for(iLines = 0,pwTmp=pwFile; pwTmp!=pwsz; pwTmp++)
	{
		if( L'\r' == *pwTmp)
		{//找到一个 结尾符号
			iLines++;  //次数递增
			if( 2 == iLines )
			{//剔出 前两行可能发现的 '\t'
				iTabCounts = 0;
			}
		}
		else if( L'\t' == *pwTmp)
		{
			iTabCounts++;
		}
	}

	if( iLines <= 5 ) //没有数据或数据量不足，退出
	{
		iret = 4; //数据量不足,错.
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	iLines -= 2; //扣除 <DATA>行以及标题行
	if( (( _VAC_FILE_TYPE_EXHAUST == iFileType) &&  (iTabCounts != iLines*4 ) )
		|| ((_VAC_FILE_TYPE_EXHAUST != iFileType) &&  (iTabCounts != iLines*2 )) )
	{
		iret = 3;//数据不完整.
		goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;
	}

	double* pxIon = NULL;
	double* pyIon = NULL;
	double* px = new double[iLines];
	double* py = new double[iLines];
	CZYGraphData* pZYGraphData = new CZYGraphData;
	CZYGraphData* pZYGraphIon  = NULL;
	int     iGraphData = 0; //ch3有效时间-真空度数
	int     iGraphIon  = 0; //ion或ch2(VGC403)有效时间-真空度数

	if( !strIonGaugeName.IsEmpty() )
	{
		pxIon = new double[iLines];
		pyIon = new double[iLines];
		pZYGraphIon  = new CZYGraphData;
		if( !pxIon || !pyIon || !pZYGraphIon)
		{
			iret = 5;
			goto _LABEL_LOAD_VAC_READ_DATA_ERR;
		}
	}

	if( !px || !py || !pZYGraphData)
	{
		iret = 5;
		goto _LABEL_LOAD_VAC_READ_DATA_ERR;
	}

	//pwsz指向</DATA>处
	//此时pwFile 指向<DATA>起始处.
	//以下，向后移动2行，将其指向数据开始处.
	int iTmp;//临时对象
	for( iTmp=0,pwTmp=pwFile;pwTmp!=pwsz;pwTmp++)
	{//以下肯定能通过，因为前面已经判断过 至少超过5行.
		if( L'\n' == *pwTmp)
		{
			iTmp++;
			if( 2==iTmp)
				break;
		}
	}

	/*
	ch3    ch2/iongauge 真空度
	00:00:07	1.00E+005	SRV OP	
	00:01:09	4.00E+004	RV OP	
	1.00E+004		
	00:04:02	1.30E+003	MBP ON	
	1.00E+001		
	00:04:57	6.60E+000	SRV RV CL	
	00:06:04		MV OP	
	00:06:39		3.00E-003		
	00:07:04	00:07:10	2.00E-003		
	00:08:00	00:07:41	1.30E-003		
	00:08:48	00:08:23	1.00E-003		
	00:15:16	00:15:11	5.00E-004	
	*/

	TCHAR* pszCur; //临时指针
	bool bCh3DataExist; //临时变量，每行有没有检测到有效的数据
	bool bIonDataExist; //临时变量，每行有没有检测到有效的数据
	bool bReadDataErr = false;
	bool bFoundTmp = false; //临时变量
	
	//pwTmp,pwFile 指向可能的数据 第一行.
	//pwsz指向</DATA>处(所有有效数据的下一行)
	pwFile = pwTmp+1;
	for( iTmp=0; iTmp<iLines; iTmp++)
	{	     
		//一直找到每行的起始与终止位置
		bFoundTmp = false;
		for(pwTmp = pwFile; pwTmp<=pwsz; pwTmp++)
		{
			if( L'\n' == *pwTmp)
			{
				bFoundTmp = true; //因为前面已经确认过 iTmp 个 '\n'，所以此处可不添加
				break;
			}
		}
		if( !bFoundTmp)
		{
			iret = 3;//数据不完整.
			goto _LABEL_LOAD_VAC_READ_DATA_ERR;
		}

		//此时行首pwFile, 行尾 pwTmp
		pszCur = pwFile; //指向行首
		//每行 ch3\t ion\t pascal\t ..\t. \r\n
		bFoundTmp = false;
		while(pszCur < pwTmp)
		{
			if( L'\t' == *pszCur )
			{
				bFoundTmp = true;
				pszCur++;//再向后移动一位
				break;
			}
			pszCur++;
		}

		if( !bFoundTmp)
		{
			iret = 3;//数据不完整.因为竟然在行内没有找到 '\t'
			goto _LABEL_LOAD_VAC_READ_DATA_ERR;
		}

		if( _VAC_FILE_TYPE_EXHAUST == iFileType )
		{
			//有时间，有真空度，才能算完整的一个数据
			bCh3DataExist = false;
			bIonDataExist = false;
			//找到第一个'\t'
			if( pszCur - pwFile >= 8 )
			{//00:15:16 至少有 8 个字符,存储可能的 px
				px[iGraphData] = fn_ExtractTime(pwFile);
				bCh3DataExist  = true; //读取到可疑的ch3数值
			}
			pwFile = pszCur;   //指向第一个'\t'的下一个字符

			bFoundTmp = false; //查找前清一次.
			//开始找第二个 '\t'
			while( pszCur < pwTmp )
			{
				if( L'\t' == *pszCur)
				{
					bFoundTmp = true;
					pszCur++; //移动一次.
					break;
				}
				pszCur++;
			}
			if( !bFoundTmp)
			{
				iret = 3;//数据不完整.因为竟然在行内没有找到 '\t'
				goto _LABEL_LOAD_VAC_READ_DATA_ERR;
			}

			if( !strIonGaugeName.IsEmpty())
			{ //如果有 IonGauge,则查找可能的数据
				if( pszCur - pwFile > 8)
				{
					pxIon[iGraphIon] = fn_ExtractTime(pwFile);
					bIonDataExist = true;
				}
			}
			pwFile = pszCur;   //指向第2个'\t'的下一个字符

			//查找第三个'\t'. 可能的真空度数据.
			bFoundTmp = false; //查找前清一次.
			while( pszCur < pwTmp )
			{
				if( L'\t' == *pszCur)
				{
					bFoundTmp = true;
					pszCur++; //移动一次.
					break;
				}
				pszCur++;
			}

			if( !bFoundTmp)
			{
				iret = 3;//数据不完整.因为竟然在行内没有找到 '\t'
				goto _LABEL_LOAD_VAC_READ_DATA_ERR;
			}

			if( pszCur - pwFile > 3 ) //存在真空度数据
			{
				if(bCh3DataExist)
				{
					py[iGraphData] = _wtof(pwFile);
					iGraphData++;
				}
				if( bIonDataExist)
				{
					pyIon[iGraphIon] = _wtof(pwFile);
					iGraphIon++;
				}
			}
		}
		else
		{//漏率或记录仪
			//找到第一个'\t' . 第一个'\t'之前的是 时间
			if( pszCur - pwFile == 8 || pszCur - pwFile == 9)
			{//00:15:16 至少有 8 个字符,存储可能的 px
				px[iGraphData] = fn_ExtractTime(pwFile);
			}
			else 
			{
				//让pwFile指向下一行，此处准备好跳过本行.
				bFoundTmp = false;
				for(; pszCur<=pwsz; pszCur++)
				{
					if( L'\n' == *pszCur)
					{
						bFoundTmp = true; //因为前面已经确认过 iTmp 个 '\n'，所以此处可不添加
						pwFile = pszCur+1; //指向下一行首
						break;
					}
				}
				continue; //问题
			}

			pwFile = pszCur;   //指向第一个'\t'的下一个字符

			bFoundTmp = false; //查找前清一次.
			//开始找第二个 '\t'. 第二个't'之前的是 ch3.
			while( pszCur < pwTmp )
			{
				if( L'\t' == *pszCur)
				{
					bFoundTmp = true;
					pszCur++; //再移动一次.
					break;
				}
				pszCur++;
			}
			if( !bFoundTmp)
			{
				iret = 3;//数据不完整.因为竟然在行内没有找到 '\t'
				goto _LABEL_LOAD_VAC_READ_DATA_ERR;
			}

			if( pszCur - pwFile > 3 ) //存在真空度数据.没有则有问题.
			{
				py[iGraphData] = _wtof(pwFile);
			}
			else
			{
				iret = 3;//数据不完整.因为竟然在行内没有找到 '\t'
				goto _LABEL_LOAD_VAC_READ_DATA_ERR;
			}

			if( !strIonGaugeName.IsEmpty())
			{ //如果有 IonGauge,则查找可能的数据
				pxIon[iGraphData] = px[iGraphData]; //漏率与记录仪数据中，ch3与Ion的数据量是一致的。
				//pszCur指向第二个'\t'下一个字符					
				//行尾 pwTmp
				if(pwTmp-pszCur>3) //二者相差多于 3个字符才说明有可能是真空度时间.否则说明数据出问题了
				{
					pyIon[iGraphData] = _wtof(pszCur);
				}
				else
				{
					iGraphData--;
				}
				iGraphIon = iGraphData+1;
			}
			iGraphData++;
		}
		pwFile = pwTmp + 1;//pwFile指向下一行开始处.
	}

	//记录进
	if( iGraphData>0 )
	{ 
		iret = 0; //此处写正常返回.
		pZYGraphData->iDataNumber = iGraphData; //数量
		pZYGraphData->pxData = px;
		pZYGraphData->pyData = py;
		
		if(_VAC_FILE_TYPE_EXHAUST == iFileType)
		{
			pZYGraphData->iGraphStyle = _ZYGRAPH_STYLE_DOT_LINE;
			pZYGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_RECT;
		}
		else
		{
			pZYGraphData->iGraphStyle = _ZYGRAPH_STYLE_LINE;
			pZYGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_RECT;
		}

		pZYGraphData->strName.Format(TEXT("%d-APC"),g_CZYGraphPlot.listData.GetCount());

		pZYGraphData->crColor = g_CZYGraphPlot.m_clrSet[g_CZYGraphPlot.listData.GetCount()%16];
		pZYGraphData->penLine.DeleteObject();
		pZYGraphData->penLine.CreatePen(pZYGraphData->nPenStyle,1,pZYGraphData->crColor);
		g_CZYGraphPlot.listData.AddTail(pZYGraphData);
	}
	else
	{//没有读取到数据，则删除已经分配的空间
		delete [] px;
		px = NULL;
		delete [] py;
		py = NULL;
	}

	if( iGraphIon>0 )
	{ 
		pZYGraphIon->iDataNumber = iGraphIon; //数量
		pZYGraphIon->pxData = pxIon;
		pZYGraphIon->pyData = pyIon;

		if(_VAC_FILE_TYPE_EXHAUST == iFileType)
		{
			pZYGraphData->iGraphStyle = _ZYGRAPH_STYLE_DOT_LINE;
			pZYGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_RECT;
		}
		else
		{
			pZYGraphData->iGraphStyle = _ZYGRAPH_STYLE_LINE;
			pZYGraphData->nDotStyle   = _ZYGRAPH_STYLE_DISCRETE_DIMOND;
		}

		pZYGraphIon->strName.Format(TEXT("%d-ION"),g_CZYGraphPlot.listData.GetCount());
		pZYGraphIon->crColor = g_CZYGraphPlot.m_clrSet[g_CZYGraphPlot.listData.GetCount()%16];
		pZYGraphIon->penLine.DeleteObject();
		pZYGraphIon->penLine.CreatePen(pZYGraphIon->nPenStyle,1,pZYGraphIon->crColor);
		g_CZYGraphPlot.listData.AddTail(pZYGraphIon);
	}
	else
	{
		if(!strIonGaugeName.IsEmpty())
		{//没有读取到数据，则删除已经分配的空间
			delete [] pxIon;
			pxIon = NULL;
			delete [] pyIon;
			pyIon = NULL;
		}
	}

	//正常退出
	goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;

_LABEL_LOAD_VAC_READ_DATA_ERR:
	if(pxIon)
		delete [] pxIon;
	if(pyIon)
		delete [] pyIon;
	if(pZYGraphIon)
		delete pZYGraphIon;
	if(px)
		delete [] px;
	if(py)
		delete [] py;
	if(pZYGraphData)
		delete pZYGraphData;
	goto _LABEL_LOAD_VACCUMM_PRESSURE_RET;


	//函数退出点
_LABEL_LOAD_VACCUMM_PRESSURE_RET:
	if(pszCurFile)
	{//释放分配过的空间
		delete [] pszCurFile;
	}
	fFile.Close();
	return iret;
}

// 提取形如 00:10:22 字符串中的时间秒数。pzTime为字符串，遇到'\t'或'\0'结束。
double COAVacuumView::fn_ExtractTime(LPCTSTR pzTime)
{
	TCHAR* p = (TCHAR*)pzTime;

	int iTime = _ttoi(p); //提取小时数

	//p仍指向字符串首
	for(int i=0;i<4;i++,p++)
	{//找小时后面的冒号 ':'
		if( *p == _T(':') )
		{
			p++; //再向后移一位，
			iTime = iTime*60 + _ttoi(p);//提取分钟数，目前为 iTime 为分钟
			break;
		}
	}
	//p已指向分钟首，则 p+3指向 秒
	iTime = iTime*60 + _ttoi(p+3);//再提取秒，目前 iTime已经转化为秒.
	return (double)iTime;
}

// 根据存储的图形内容，重新初始化Combobox的内容
void COAVacuumView::fn_ReInitGraphCombo(void)
{
	m_cmbCurves.ResetContent();
	m_cmbCurves.AddString(TEXT("BackGround"));
	POSITION pos = g_CZYGraphPlot.listData.GetHeadPosition();
	for(int i=0;i<g_CZYGraphPlot.listData.GetCount();i++)
	{
		CZYGraphData* pZYGraphData = g_CZYGraphPlot.listData.GetNext(pos);
		m_cmbCurves.AddString(pZYGraphData->strName);
	}
}

//删除内容
//似乎双击Dialog资源，添加了响应函数后，输入法发生改变，中文不再可用？待确认。
void COAVacuumView::OnBnClickedBtnDeleteCurve()
{
//TEST
	/*
	int*c = new int[100];
	int d = *(c + 10);//错误,可能访问出错 
	int e = c[10];//正确
	delete [] c;
	*/
	//
	//SetDlgItemText(IDC_BTN_DELETE_CURVE,L"&&");

	// TODO: Add your control notification handler code here
	int nIndex = m_cmbCurves.GetCurSel();
	if( nIndex > 0 )  //0为BackGround
	{   //-1，表示没有选择. 第0项代表为ch3，不可删除. 
		//第1项若是 ch2-ion，则不可删除
		//     若不是，则可删除.
		//是否第二项
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);

		CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
		if(p->bCanDelete)
		{
			g_CZYGraphPlot.listData.RemoveAt(pos);
			delete p;
			fn_ReInitGraphCombo();
			InvalidateRect(&m_rtVacPic,false);
		}
	}
}
void COAVacuumView::OnBnClickedBtnCurveProperty()
{
	// TODO: Add your control notification handler code here #include "DlgVacCurveProperty.h"
	int nIndex = m_cmbCurves.GetCurSel();
	
	if( nIndex == 0 )
	{//0为BackGround
		CDlgVacBkPro dlg;
		dlg.m_crBk = g_CZYGraphPlot.m_clrBk;
		dlg.m_crGrid = g_CZYGraphPlot.m_clrGrid;
		dlg.m_crScale = g_CZYGraphPlot.m_clrYScale;
		dlg.m_uiXGridNumber = g_CZYGraphPlot.ixGridNum;
		dlg.m_ixMinutesPerGrid = g_CZYGraphPlot.ixMinutesPerGrid;
		dlg.m_iYUpperExp = log10(g_CZYGraphPlot.yTop);
		dlg.m_iYLowerExp = log10(g_CZYGraphPlot.yBot);
		dlg.m_bShowXGrids = g_CZYGraphPlot.bxGridDisp;
		
		if(IDOK==dlg.DoModal())
		{
			g_CZYGraphPlot.m_clrBk = dlg.m_crBk;
			g_CZYGraphPlot.m_clrGrid = dlg.m_crGrid ;
			g_CZYGraphPlot.m_clrYScale = dlg.m_crScale;

			g_CZYGraphPlot.bxGridDisp = dlg.m_bShowXGrids;

			if(dlg.m_iYUpperExp>dlg.m_iYLowerExp)
			{
				g_CZYGraphPlot.iyGridNum = dlg.m_iYUpperExp-dlg.m_iYLowerExp;
				g_CZYGraphPlot.yTop = pow(10.0,dlg.m_iYUpperExp);
				g_CZYGraphPlot.yBot = pow(10.0,dlg.m_iYLowerExp);				
			}

			g_CZYGraphPlot.ixMinutesPerGrid = dlg.m_ixMinutesPerGrid;
			g_CZYGraphPlot.ixGridNum = dlg.m_uiXGridNumber;
			g_CZYGraphPlot.xRight = g_CZYGraphPlot.xLeft + 60*g_CZYGraphPlot.ixGridNum*g_CZYGraphPlot.ixMinutesPerGrid;

			InvalidateRect(&m_rtVacPic,false);
		}


	}
	else if( nIndex > 0 )  //0为BackGround
	{
		CDlgVacCurveProperty dlg;
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);
		CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);
		dlg.m_strName = p->strName;
		dlg.m_cr      = p->crColor;
		dlg.m_nDotStyle = p->nDotStyle;
		dlg.m_nGraphStyle = p->iGraphStyle;
		dlg.m_nPenStyle   = p->nPenStyle;
		if(IDOK==dlg.DoModal())
		{
			if(p->strName!=dlg.m_strName)
			{
				p->strName = dlg.m_strName;
				m_cmbCurves.InsertString(nIndex+1,p->strName); //下一位加入
				m_cmbCurves.DeleteString(nIndex); //删除当前，此时 nIndex已经指向 加入项
				m_cmbCurves.SetCurSel(nIndex);    //选择为当前
			}
			p->fn_ChangeColor(dlg.m_cr);		
			p->nDotStyle = dlg.m_nDotStyle;
			p->iGraphStyle = dlg.m_nGraphStyle;
			p->nPenStyle = dlg.m_nPenStyle;
			InvalidateRect(&m_rtVacPic,false);
		}
	}
}

//隐藏的设置问题,2008.05.20追加，对应真空计不能正确解析
//此处强制更改设置，注意，每次软件启动都需要重新设置。
//因为本设置是对应有的机台plc配置相关内容不正确而额外添加的。
//正常机台，不需从此处更改设置，每次连接plc时程序会自动配置。

void COAVacuumView::OnStnClickedStaticPenningValueName()
{
	CDlgVacGaugeSetup dlg;
	if(IDOK == dlg.DoModal())
	{
		g_APCGauge.SetGaugeType(dlg.m_iAPCGauge);
		g_IonGauge.SetGaugeType(dlg.m_iIonGauge);
		fn_DisplayVacState();
	}
}

void COAVacuumView::OnStnDblclickStaticPenningValueName()
{
	CDlgVacGaugeSetup dlg;
	if(IDOK == dlg.DoModal())
	{
		g_APCGauge.SetGaugeType(dlg.m_iAPCGauge);
		g_IonGauge.SetGaugeType(dlg.m_iIonGauge);
		fn_DisplayVacState();
	}
}

void COAVacuumView::OnBnClickedBtnGaugeSetup()
{
	// TODO: Add your control notification handler code here
		CDlgVacGaugeSetup dlg;
	if(IDOK == dlg.DoModal())
	{
		g_APCGauge.SetGaugeType(dlg.m_iAPCGauge);
		g_IonGauge.SetGaugeType(dlg.m_iIonGauge);
		fn_DisplayVacState();
//			gc_dMaxPenningPa = g_APCGauge.TransVal2Pascal(4199-1);
//			gc_dMinPenningPa = g_APCGauge.TransVal2Pascal(176+1);
	}
}


//测试用按钮.仅提供给Debug版本.
//即Debug版本下，本按钮才会显现
//功能：将选择的曲线数据发送到内存中去，
void COAVacuumView::OnBnClickedButtonTest()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_cmbCurves.GetCurSel();
	
	int x = sizeof(_T("00:10:20\t1.00E-003\r\n")); //sizeof包含了字符串结尾符号

	if( nIndex > 0 )  //0为BackGround
	{   //-1，表示没有选择. 第0项代表为ch3，不可删除. 
		//第1项若是 ch2-ion，则不可删除
		//     若不是，则可删除.
		//是否第二项
		POSITION pos = g_CZYGraphPlot.listData.FindIndex(nIndex-1);

		CZYGraphData* p = g_CZYGraphPlot.listData.GetAt(pos);

		if(OpenClipboard())
		{
			HGLOBAL clipbuffer;
			TCHAR * buffer;
			EmptyClipboard();

			int imemsize = p->iDataNumber*sizeof(_T("00:10:20\t1.00E-003\r\n"));

			clipbuffer = GlobalAlloc(GMEM_DDESHARE, imemsize);
			if(clipbuffer)
			{
				memset(clipbuffer,0,imemsize); //全部置空
				buffer = (TCHAR*)GlobalLock(clipbuffer);
				int    iTime;
				int    iprinted = 0;
				for(int i=0;i<p->iDataNumber;i++)
				{
					//_stprintf(g_buffer,TEXT(""),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
					iTime = p->pxData[i];
					buffer += iprinted;
					iprinted = _stprintf(buffer,TEXT("%02d:%02d:%02d\t%.2E\r\n"),iTime/3600,
						(iTime/60)%60 ,iTime%60, p->pyData[i]);
				}
				GlobalUnlock(clipbuffer);
#ifdef _UNICODE
				::SetClipboardData(CF_UNICODETEXT,clipbuffer);
#else
				::SetClipboardData(CF_TEXT,clipbuffer);
#endif
				AfxMessageBox(_T("Selected Vac Data was sent into clipboard."));

			}
			/*

			clipbuffer = GlobalAlloc(GMEM_DDESHARE, 3*sizeof(TCHAR)); 
			if(clipbuffer)
			{
				memset(clipbuffer, 0, 3*sizeof(TCHAR));
				buffer = (TCHAR*)GlobalLock(clipbuffer);
				buffer[0] = _T('U');
				buffer[1] = _T('F');
				buffer[2] = _T('\0');
				GlobalUnlock(clipbuffer);
#ifdef _UNICODE
				::SetClipboardData(CF_UNICODETEXT,clipbuffer);
#else
				::SetClipboardData(CF_TEXT,clipbuffer);
#endif
				AfxMessageBox(_T("Selected Vac Data was sent into clipboard."));
			}
			*/
			else
			{
				AfxMessageBox(_T("Strangous, Cannot alloc memory for clipbuffer!"));
			}
			CloseClipboard();
		}

	}
	else
	{
		AfxMessageBox(_T("Please select a curve which you want to output."));
	}


}

// 向栏目中添加Text,用于Debug
void COAVacuumView::fn_AddHelpText(LPCTSTR lpText,bool bAdd)
{
	CEdit* pwnd = (CEdit*)GetDlgItem(IDC_EDIT_VACUUM_HELP);
	if(!pwnd->GetSafeHwnd())
		return;
	if(bAdd)
	{
		int nLen=pwnd->GetWindowTextLength(); 
		pwnd->SetSel (nLen, nLen); 
		pwnd->ReplaceSel (lpText);
	}
	else
	{
		pwnd->SetWindowText(lpText);
	}
}


void COAVacuumView::OnEnChangeEditComment()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void COAVacuumView::OnEnChangeEditVacuumHelp()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

/*数据存入excel表格 2018 9 -13*/
void COAVacuumView::OnBnClickedButtonSaveListctrl2()
{	
	std::ofstream oFile;
	TCHAR wBuffer[50];
	char bufferOut[50];
	oFile.open("test.csv", std::ios_base::out | std::ios_base::trunc);
	if (g_stVacuumTestEx.iCurTestType == _VACUUM_TESTTYPE_EXHAUST)
	{
		
		int irow = m_ListctrlExhaust.GetItemCount();
		int ilist = m_ListctrlExhaust.GetHeaderCtrl()->GetItemCount();
		for (int i = 0; i < irow; i++)
		{
			for (int j = 0; j < ilist; j++)
			{
				memset(wBuffer, 0, sizeof(wBuffer));
				memset(bufferOut, 0, sizeof(bufferOut));
				m_ListctrlExhaust.GetItemText(i, j, wBuffer, 50);
				WideCharToMultiByte(CP_ACP, 0, wBuffer, 50, bufferOut, 50, NULL, NULL);
				oFile << bufferOut;
				oFile << ",";
			}
			oFile << std::endl;
		}
	
	}
	else
	{

		int irow = m_Listctrl.GetItemCount();
		int ilist = m_Listctrl.GetHeaderCtrl()->GetItemCount();
		for (int i = irow-1; i >= 0; i--)
		{
			for (int j = 0; j < ilist; j++)
			{
				memset(wBuffer, 0, sizeof(wBuffer));
				memset(bufferOut, 0, sizeof(bufferOut));
				m_Listctrl.GetItemText(i, j, wBuffer, 50);
				WideCharToMultiByte(CP_ACP, 0, wBuffer, 50, bufferOut, 50, NULL, NULL);
				oFile << bufferOut;
				oFile << ",";
			}
			oFile << std::endl;
		}
	}
	oFile.close();
}
