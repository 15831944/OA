// OASysInfoView.cpp : implementation of the COASysInfoView class
//

#include "stdafx.h"
#include "Optorun Assistant.h"

#include "Optorun AssistantDoc.h"
#include "OASysInfoView.h"

#include "MainFrm.h"
//class CMainFrame;
#include "OACnfgDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern COACnfgDoc g_COACnfgDoc;

char pCom5TestString[1024];
char pComTestStringForGL[1024]; //GL �� IBS�ķ����ַ���.
char pComTestStringForGasCont[1024];

TCHAR g_buffer[1024];
char  g_chBuffer[1024];
TCHAR g_tchBuffer2[1024];

//#define WM_ITEM1_ADD_RUN_HISTORY (WM_USER+701)

// COASysInfoView


IMPLEMENT_DYNCREATE(COASysInfoView, CFormView)

BEGIN_MESSAGE_MAP(COASysInfoView, CFormView)
	ON_MESSAGE(WM_ITEM1_DISPLAY_PLC_STATE,OnDisplayPlcState)
	ON_MESSAGE(WM_ITEM1_ADD_RUN_HISTORY,OnAddRunHistroy)
	ON_MESSAGE(WM_ITEM1_GET_NAMES,OnGetNames)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_HELP_LEFT, &COASysInfoView::OnBnClickedButtonHelpLeft)
	ON_BN_CLICKED(IDC_BUTTON_HELP_RIGHT, &COASysInfoView::OnBnClickedButtonHelpRight)
	ON_WM_TIMER()
//	ON_WM_CLOSE()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTON_CLOSE_CONNECTION, &COASysInfoView::OnBnClickedButtonCloseConnection)
ON_BN_CLICKED(IDC_BUTTON_CONNECT_PLC, &COASysInfoView::OnBnClickedButtonConnectPlc)
//ON_BN_CLICKED(IDC_STATIC_, &COASysInfoView::OnBnClickedStatic)
//ON_BN_CLICKED(IDC_STATIC_SERIAL_PORT_TEST, &COASysInfoView::OnBnClickedStaticTest)
ON_BN_CLICKED(IDC_BUTTON_COM2_OPEN, &COASysInfoView::OnBnClickedButtonCom2Open)
ON_BN_CLICKED(IDC_BUTTON_COM2_CLOSE, &COASysInfoView::OnBnClickedButtonCom2Close)
ON_BN_CLICKED(IDC_BUTTON_COM2_TEST, &COASysInfoView::OnBnClickedButtonCom2Test)
ON_BN_CLICKED(IDC_BUTTON_COM2_PROPERTY, &COASysInfoView::OnBnClickedButtonCom2Property)
ON_BN_CLICKED(IDC_BUTTON_COM3_OPEN, &COASysInfoView::OnBnClickedButtonCom3Open)
ON_BN_CLICKED(IDC_BUTTON_COM3_CLOSE, &COASysInfoView::OnBnClickedButtonCom3Close)
ON_BN_CLICKED(IDC_BUTTON_COM3_TEST, &COASysInfoView::OnBnClickedButtonCom3Test)
ON_BN_CLICKED(IDC_BUTTON_COM3_PROPERTY, &COASysInfoView::OnBnClickedButtonCom3Property)
ON_BN_CLICKED(IDC_BUTTON_COM4_OPEN, &COASysInfoView::OnBnClickedButtonCom4Open)
ON_BN_CLICKED(IDC_BUTTON_COM4_CLOSE, &COASysInfoView::OnBnClickedButtonCom4Close)
ON_BN_CLICKED(IDC_BUTTON_COM4_TEST, &COASysInfoView::OnBnClickedButtonCom4Test)
ON_BN_CLICKED(IDC_BUTTON_COM4_PROPERTY, &COASysInfoView::OnBnClickedButtonCom4Property)
ON_BN_CLICKED(IDC_BUTTON_COM5_OPEN, &COASysInfoView::OnBnClickedButtonCom5Open)
ON_BN_CLICKED(IDC_BUTTON_COM5_CLOSE, &COASysInfoView::OnBnClickedButtonCom5Close)
ON_BN_CLICKED(IDC_BUTTON_COM5_TEST, &COASysInfoView::OnBnClickedButtonCom5Test)
ON_BN_CLICKED(IDC_BUTTON_COM5_PROPERTY, &COASysInfoView::OnBnClickedButtonCom5Property)
//ON_BN_DOUBLECLICKED(IDC_STATIC_SERIAL_PORT_TEST, &COASysInfoView::OnBnDoubleclickedStaticSerialPortTest)
//ON_BN_CLICKED(IDC_STATIC_SERIAL_PORT_TEST, &COASysInfoView::OnBnClickedStaticSerialPortTest)
ON_STN_CLICKED(IDC_STATIC_COM2_TEXT, &COASysInfoView::OnStnClickedStaticCom2Text)
//ON_BN_CLICKED(IDC_STATIC_SERIAL_PORT_TEST, &COASysInfoView::OnBnClickedStaticSerialPortTest)
ON_STN_CLICKED(IDC_STATIC_COM3_TEXT, &COASysInfoView::OnStnClickedStaticCom3Text)
ON_STN_CLICKED(IDC_STATIC_COM4_TEXT, &COASysInfoView::OnStnClickedStaticCom4Text)
ON_STN_CLICKED(IDC_STATIC_COM5_TEXT, &COASysInfoView::OnStnClickedStaticCom5Text)
ON_BN_CLICKED(IDC_CHECK_RECORD_VACUUM, &COASysInfoView::OnBnClickedCheckRecordVacuum)
//ON_WM_ERASEBKGND()
ON_STN_CLICKED(IDC_STATIC_HELP_TITLE, &COASysInfoView::OnStnClickedStaticHelpTitle)
END_MESSAGE_MAP()

//FindCommPort������ע�����COM�����ã��������� comboBox��
void FindCommPort( CComboBox *pComboBox ) 
{ 
	HKEY hKey;
#ifdef _DEBUG 
	ASSERT( pComboBox != NULL );
	pComboBox->AssertValid();
	pComboBox->ResetContent(); //����ִ�����
#endif
	//��ע����еĴ�����Ϣ
	if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,TEXT("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{ 
		int i=0;
		TCHAR portName[256],commName[256];
		DWORD dwLong,dwSize;
		while(1)
		{
			dwLong = dwSize = sizeof(portName)/sizeof(TCHAR);
			//ö�ٴ��ڲ���ӽ� pComboBox��
			if( ::RegEnumValue( hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize ) == ERROR_NO_MORE_ITEMS ) //
				break;
			pComboBox->AddString( commName );// commName
			i++;
		}
		if( pComboBox->GetCount() == 0 )
		{
			::AfxMessageBox( TEXT("Cannot Find any available COM port!") );
		}
		RegCloseKey(hKey);
	}
}


// COASysInfoView construction/destruction
COASysInfoView::COASysInfoView()
	: CFormView(COASysInfoView::IDD)
{
	// TODO: add construction code here
	m_uiHelpNumber = 0;
	m_unSysOneSecondTimer = 1;

//	gb_IonGauge = false;

	//ASSERT(0);
}

COASysInfoView::~COASysInfoView()
{

}

void COASysInfoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HISTORY, m_editHistory);
	DDX_Control(pDX, IDC_EDIT_HELP_TEXT, m_editHelp);
	DDX_Control(pDX, IDC_STATIC_PLC_STATE, m_staticPLCstate);
	DDX_Control(pDX, IDC_STATIC_HELP_TITLE, m_staticHelpTilte);
}

BOOL COASysInfoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void COASysInfoView::OnInitialUpdate()
{
	/*
	double y = 1923;
	double x = g_COACnfgDoc.fn_TransPenningVoltPa(10*y);
	x = g_COACnfgDoc.fn_TransPenningVoltPa(y);
	char*  p = "4f54";
	//TCHAR* x = L"OT";
	*/

	COptorunAssistantDoc* pd =(COptorunAssistantDoc*) ((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveDocument();
	
	CFormView::OnInitialUpdate();

	((CMainFrame*)AfxGetApp()->m_pMainWnd)->AddView(g_COACnfgDoc.m_sTabLabel[_OA_FORM_VIEW_SYSTEM],this,g_COACnfgDoc.m_sTabTooltip[_OA_FORM_VIEW_SYSTEM]);
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->SetCurView(_OA_FORM_VIEW_SYSTEM);

	//��xml��ȡ���ı����˴�����ҳ��ID���ı�
	fn_SetIDsText();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_editHistory.bkColor( RGB(255,255,255) );
	m_editHelp.bkColor( RGB(255,255,225) );
	m_staticHelpTilte.textColor(RGB(0,73,214));

	m_editHelp.SetWindowText(g_item1_Help_Text[0]);




	EnterCriticalSection(&gCriticalSection_State);
		g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_NOT_CONNECT;
	LeaveCriticalSection(&gCriticalSection_State);

	


	fn_DisplayPlcState();
	fn_ShowWindow();

	DWORD bufsize = 512;
	::GetComputerName(g_strBuffer,&bufsize);
	SetDlgItemText(IDC_STATIC_PC_NAME_AREA,g_strBuffer);

	//���м�¼�������ʼʱ��
	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	_stprintf(g_strBuffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d  Optorun Assistant start up\r\n"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	fn_AppendHistoryText(g_strBuffer);

	//������ʱ��
	m_unSysOneSecondTimer = SetTimer(m_unSysOneSecondTimer,1000,NULL);

	UINT uLimit = m_editHistory.GetLimitText();
	//debug
#ifdef _DEBUG
	//SendMessage(WM_ITEM1_ADD_RUN_HISTORY,0,(LPARAM)TEXT("\r\nFrom send message\r\n"));
	//PostMessage(WM_ITEM1_ADD_RUN_HISTORY,0,(LPARAM)TEXT("\r\nFrom post message\r\n"));
	//fn_AppendHistoryText(TEXT("\r\nHere test "));

#endif
	::SetFocus(GetDlgItem(IDC_BUTTON_CONNECT_PLC)->GetSafeHwnd());

	SetDlgItemText(IDC_STATIC_PRODUCT_VERSION_AREA,g_strProductVersion);
	SetDlgItemText(IDC_STATIC_MACHINE_TYPE_AREA,TEXT(" "));
	SetDlgItemText(IDC_STATIC_MACHINE_SN_AREA,TEXT(" "));
	SetDlgItemText(IDC_STATIC_TOUCHPANEL_VER_AREA,TEXT(" "));

	//1���������ƣ�
	m_editHistory.SetLimitText(100*1000000);
	/*
	for(int i = 0; i<100000; i++)
	{
		_stprintf(g_strBuffer,TEXT("% 5d: 0123456789012345678901234567890123456789012345678901234567890123456789\r\n"),i);
		fn_AppendHistoryText(g_strBuffer);
	}*/

	//��Ӳ��Դ��ڵĴ���
	for(int i=IDC_COMBO_COM2;i<=IDC_COMBO_COM5;i++)
	{
		CComboBox* pCom = (CComboBox*)GetDlgItem(i);
		FindCommPort(pCom);
	}
	//property icon
	for(int i = IDC_BUTTON_COM2_PROPERTY ;i<=IDC_BUTTON_COM5_PROPERTY ;i++)
	{
		((CButton*)GetDlgItem(i))->SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_PROPERTY) ); 
	}

	memcpy(	pCom5TestString, "VER?\r\n\0", sizeof("VER?\r\n\0"));	
	memcpy(	pComTestStringForGL, "IBSQID\x0D", sizeof("IBSQID\x0D")); //2010.06.10
	memcpy(	pComTestStringForGasCont, "RS232C\x0D", sizeof("RS232C\x0D")); //2010.06.10
	
}

// COASysInfoView diagnostics
#ifdef _DEBUG
void COASysInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

void COASysInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

COptorunAssistantDoc* COASysInfoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COptorunAssistantDoc)));
	return (COptorunAssistantDoc*)m_pDocument;
}
#endif //_DEBUG


// �趨ҳ���ı�����Щ�ı��Ǵ� xml�ļ��ж�ȡ����

bool COASysInfoView::fn_SetIDsText(void)
{
	//SetDlgItemText(1001,g_item1_ID_Text[0]);	
	//g_item1_ID_Text[0] = TEXT("Write");
	for(int i=0;i<_ITEM1_NEED_TEXT_ID_CNT;i++)
	{
		//SetDlgItemText(g_item1_ID[i],g_item1_ID_Text[i]);
		if(!g_item1_ID_Text[i].IsEmpty())
			SetDlgItemText(g_item1_ID[i],g_item1_ID_Text[i]);
	}
	return false;
}

void COASysInfoView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if(::IsWindow(GetSafeHwnd()))
	{//
		CWnd* pwnd = GetDlgItem(IDC_ITEM1_SYS_GROUP);
		if(::IsWindow(pwnd->GetSafeHwnd()))
		{
			//ȡ��sys group ����
			CRect rt1,rt2,rt3,rt4,rect;
			pwnd->GetWindowRect(&rt1); //screen 
			ScreenToClient(&rt1);      //ת������ҳ������

			GetClientRect(&rect);      //��ҳ������

			//-----���� run History �༭��λ��
			//��� sys group�������س��� ��ҳ�淶Χ�� 2/3����� �����ؿ�ʼ������� ҳ����� 1/3����ʼ��
			rt2 = rect;
			rt2.top     = 2*rect.Height()/3 > rt1.bottom ? 2*rect.Height()/3 : rt1.bottom+20;
			rt2.left   += 2;
			rt2.right  -= 2;
			rt2.bottom -= 2;

			//ҳ��ȫ���
			//Move history group window
			pwnd = GetDlgItem(IDC_STATIC_HISTORY);
			pwnd->MoveWindow(&rt2); //MoveWindow����ڸ�����(��ҳ��)����

			//move History edit 
			rt2.top +=20;
			pwnd = GetDlgItem(IDC_EDIT_HISTORY);
			pwnd->MoveWindow(&rt2);

			//-----���� Help �༭��λ��
			//IDC_BUTTON_HELP_LEFT IDC_EDIT_HELP_TEXT IDC_STATIC_HELP_TITLE IDC_BUTTON_HELP_RIGHT
			GetClientRect(&rect);      //�ٴ�ȡ�ñ�ҳ������
		    pwnd = GetDlgItem(IDC_BUTTON_HELP_RIGHT);
			pwnd->GetWindowRect(&rt2);
			ScreenToClient(&rt2);
			rt3.top    = rt1.top;
			rt3.bottom = rt3.top + rt2.Height();
			rt3.right  = rect.Width() -2 ;
			rt3.left   = rt3.right - rt2.Width();
			pwnd->MoveWindow(&rt3);

			pwnd = GetDlgItem(IDC_BUTTON_HELP_LEFT);
			rt2.top = rt3.top ;
			rt2.bottom = rt3.bottom;
			rt2.left = rect.Width() > 2 * rt1.Width() ? rect.right - 4 -rt1.Width() : rt1.right +2;
			rt2.right = rt2.left + rt3.Width();
			pwnd->MoveWindow(&rt2);

			pwnd  = GetDlgItem(IDC_STATIC_HELP_TITLE);
			rt4.top = rt2.top;
			rt4.bottom = rt2.bottom;
			rt4.left = rt2.right + 2;
			rt4.right = rt3.left -2;
			pwnd->MoveWindow(&rt4);
			
			pwnd  = GetDlgItem(IDC_EDIT_HELP_TEXT);
			rt4.left = rt2.left;
			rt4.right = rt3.right;
			rt4.top =   rt2.bottom + 2;
			rt4.bottom = rt1.bottom - 1;
			pwnd->MoveWindow(&rt4);
		}
	}
	// TODO: Add your message handler code here
}

//����ʷ�༭��������ı���
void COASysInfoView::fn_AppendHistoryText(LPCTSTR lpText)
{
	CEdit* pwnd = (CEdit*)GetDlgItem(IDC_EDIT_HISTORY);
	if(pwnd->GetSafeHwnd())
	{
		int nLen=pwnd->GetWindowTextLength (); 
		pwnd->SetSel (nLen, nLen); 
		pwnd->ReplaceSel (lpText);
	}
}

void COASysInfoView::OnBnClickedButtonHelpLeft()
{
	// TODO: Add your control notification handler code here
	m_uiHelpNumber--;

	if( m_uiHelpNumber < 0 )
	{
		//��ֹ giHelpItems = 0�����;
		if(giHelpItems)
			m_uiHelpNumber = giHelpItems - 1;		
		else
			m_uiHelpNumber = 0;		
	}

	if(m_uiHelpNumber>=giHelpItems)
	{
		m_uiHelpNumber = 0;
	}

	m_editHelp.SetWindowText(g_item1_Help_Text[m_uiHelpNumber]);
}

void COASysInfoView::OnBnClickedButtonHelpRight()
{
	// TODO: Add your control notification handler code here
	m_uiHelpNumber++;
	if(m_uiHelpNumber>=giHelpItems)
	{
		m_uiHelpNumber = 0;
	}
	if( m_uiHelpNumber < 0 )
	{
		m_uiHelpNumber = 0;
	}
	m_editHelp.SetWindowText(g_item1_Help_Text[m_uiHelpNumber]);
}

LRESULT COASysInfoView::OnDisplayPlcState(WPARAM wParam, LPARAM lParam)
{
	fn_DisplayPlcState();
	return 0;
}


CString COASysInfoView::GetDMstrEvent(int nBeginWord, int nReadNumber)
{
	CString str;
	DWORD dw = WaitForSingleObject(g_hEvent_plc,500);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
#ifdef _DEBUG
		fn_AppendHistoryText(TEXT("Error Connect PLC: OnGetNames->WaitForSingleObject(g_hEvent_plc,500) ,time out;\r\n"));
		return str;
#endif //_DEBUG
	}
	
#ifdef _ZHANGZIYE_DEBUG	
	DWORD dwStart = GetTickCount();
#endif// _ZHANGZIYE_DEBUG	

	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	str = g_plc.GetDMstr(nBeginWord, nReadNumber);
	SetEvent(g_hEvent_plc);  //��ɺ���

#ifdef _ZHANGZIYE_DEBUG	
	_stprintf(g_strBuffer,TEXT("GetDMstrEvent: %d ms\r\n"),GetTickCount()-dwStart);	
	fn_AppendHistoryText(g_strBuffer);
#endif// _ZHANGZIYE_DEBUG	
	return str;
}

//��Ϣ��Ӧ����
LRESULT COASysInfoView::OnGetNames(WPARAM wParam, LPARAM lParam)
{
	HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));

	_stprintf(g_strBuffer,TEXT("%s  Error: OnGetNames Error, dwLastError = %d \r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.GetLastError());
	CString str = GetDMstrEvent(gc_DM_machinetype,10);	
	if(0==str.GetLength())
	{
		_stprintf(g_strBuffer,TEXT("%s  Error: Get machine type Error, dwLastError = %d \r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.GetLastError());
		goto end_OnGetNames_Error;
	}	
	SetDlgItemText(IDC_STATIC_MACHINE_TYPE_AREA,str);

	str = GetDMstrEvent(gc_DM_sequencer,10);
	if( 0==str.GetLength())
	{
		_stprintf(g_strBuffer,TEXT("%s  Error: Get sequencer version Error, dwLastError = %d \r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.GetLastError());
		goto end_OnGetNames_Error;
	}
	SetDlgItemText(IDC_STATIC_MACHINE_SN_AREA,str);

	str = GetDMstrEvent(gc_DM_touchpanel,10);
	if( 0==str.GetLength())
	{
		_stprintf(g_strBuffer,TEXT("%s  Error: Get Touch Panel version Error, dwLastError = %d \r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.GetLastError());
		goto end_OnGetNames_Error;
	}
	SetDlgItemText(IDC_STATIC_TOUCHPANEL_VER_AREA,str);
    

	//2008.05.12���
	WORD wRdWordAPCGauge = 0;
	g_plc.ReadPLCex(enReadDM,gc_DM_APCGaugeType,1,&wRdWordAPCGauge,true);
	g_APCGauge.SetGaugeType(wRdWordAPCGauge);
	gc_dMaxPenningPa = g_APCGauge.TransVal2Pascal(4199-1);
	gc_dMinPenningPa = g_APCGauge.TransVal2Pascal(176+1);

	WORD wRdWordIonGauge = 0;
	//�趨���ӹ�����.����еĻ���wRdWord!=0������=0.
	if( g_plc.ReadPLCex(enReadDM,gc_DM_IonGaugeType,1,&wRdWordIonGauge,true) )
	{
		//g_IonGauge.SetGaugeType((int)wRdWordIonGauge); 
		/*
		bool bIonGaugeExsit;
		//2008.05.12, 9088:03 && 9085:05����Ӧ VGC403 Ch2 Ϊpsd������û�е�����չ�//�������G1275
		if( 0 == wRdWordIonGauge || ( 3 == (int)wRdWordAPCGauge && 5 == (int)wRdWordIonGauge) )
		{
			bIonGaugeExsit = false;
		}
		else 
			bIonGaugeExsit = true;
		g_IonGauge.SetGaugeTypeEx((int)wRdWordIonGauge,bIonGaugeExsit);
		*/

		g_IonGauge.SetGaugeTypeEx((int)wRdWordIonGauge, wRdWordIonGauge!=0);	

#ifdef _ZHANGZIYE_DEBUG
		_stprintf(g_strBuffer,TEXT("Ion Gauge type = %s\r\n"),g_IonGauge.GetGaugeTypeName());
		fn_AppendHistoryText(g_strBuffer);
#endif
	}



	//temp ��ַ����
	if(g_plc.ReadPLCex(enReadDM,gc_DM_AddressHeaterTempSVPV,_TEMP_HEATER_NUMBER-2,gcp_DM_HeaterTemp,true))
	{//_TEMP_HEATER_NUMBER-2����Ϊ����ڵ�ַָ��λ������
		_stprintf(g_strBuffer,TEXT("Address of MonitorHeater PV =%d, Dome PV=%d, Monitor SV=%d, Dome SV=%d\r\n"),gcp_DM_HeaterTemp[0],gcp_DM_HeaterTemp[1],gcp_DM_HeaterTemp[2],gcp_DM_HeaterTemp[3]);
		fn_AppendHistoryText(g_strBuffer);
		//��Ӳ�ڵ�ַ
		if(g_plc.ReadPLCex(enReadDM,gc_DM_AddressDome2TempSVPV,2,&gcp_DM_HeaterTemp[_TEMP_HEATER_NUMBER-2],true))
		{
			_stprintf(g_strBuffer,TEXT("Address of DomeHeater2 PV =%d, DomeHeater2 SV=%d\r\n"),gcp_DM_HeaterTemp[4],gcp_DM_HeaterTemp[5]);
			fn_AppendHistoryText(g_strBuffer);
		}
	}
	

	/*
#ifdef _ZHANGZIYE_DEBUG
	g_plc.ReadPLC(enReadDM,gc_DM_PenningValue,1,&wRdWord);

	_stprintf(g_strBuffer,TEXT("Penning gauge Volt = %d\r\n"),(int)wRdWord);
	fn_AppendHistoryText(g_strBuffer);
	
	if(g_IonGauge.GetGaugeType())
	{
		g_plc.ReadPLC(enReadDM,gc_DM_IonGaugeValue,1,&wRdWord);
		_stprintf(g_strBuffer,TEXT("Ion Gauge Volt = %d\r\n"),(int)wRdWord);
		fn_AppendHistoryText(g_strBuffer);
	}
#endif 
	*/
	//debug end
		
	EnterCriticalSection(&gCriticalSection_State);
	g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_COMM_NORMAL;
	LeaveCriticalSection(&gCriticalSection_State);

	goto end_OnGetNames;

end_OnGetNames_Error:
	fn_AppendHistoryText(g_strBuffer);
	if( 0 != g_plc.m_dwBytesRead )
	{
		//g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_COMM_CONTENT_ERROR;
		EnterCriticalSection(&gCriticalSection_State);
		g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_COMM_CONTENT_ERROR;
		LeaveCriticalSection(&gCriticalSection_State);
	}
	else
	{
		//g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_COMM_NORESPOND;
		EnterCriticalSection(&gCriticalSection_State);
		g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_COMM_NORESPOND;
		LeaveCriticalSection(&gCriticalSection_State);
	}
end_OnGetNames:
	fn_DisplayPlcState();

	SetCursor(hcur);
	return 0;
}

LRESULT COASysInfoView::OnAddRunHistroy(WPARAM wParam, LPARAM lParam)
{
	fn_AppendHistoryText( (LPCTSTR) lParam);
	return 0;
}

void COASysInfoView::fn_DisplayPlcState(void)
{
	EnterCriticalSection(&gCriticalSection_State);
		int iPlcState = g_COACnfgDoc.m_iPlcState;
	LeaveCriticalSection(&gCriticalSection_State);
	switch(iPlcState)
	{
	case _COM_PLC_STATE_NOT_CONNECT:
		m_staticPLCstate.textColor(gc_clrBlack);
		break;
	case _COM_PLC_STATE_TRY_COMM:
		m_staticPLCstate.textColor(gc_clrTrying);
		break;
	case _COM_PLC_STATE_COMM_NORMAL:
		m_staticPLCstate.textColor(gc_clrOK);
		break;
	case _COM_PLC_STATE_OPEN_COM_ERROR:
	case _COM_PLC_STATE_COMM_CONTENT_ERROR:
	case _COM_PLC_STATE_COMM_NORESPOND:
		m_staticPLCstate.textColor(gc_clrAlert);
		break;
	}
	m_staticPLCstate.ShowWindow(SW_SHOW);
	m_staticPLCstate.SetWindowText(g_item1_plc_state[iPlcState]);

	static int iLastState = 0xFFFF;
	if(iLastState != iPlcState)
	{
		iLastState = iPlcState;
		AfxGetMainWnd()->PostMessage(MAIN_FRM_ITEMx_2_ITEMy,MAKEWPARAM(1,0),WM_ITEM2_DISPLAY_VAC_STATE);
	}

}

void COASysInfoView::fn_ShowWindow(void)
{

	EnterCriticalSection(&gCriticalSection_State);
		int iPlcState = g_COACnfgDoc.m_iPlcState;
	LeaveCriticalSection(&gCriticalSection_State);
	

	bool b = ( _COM_PLC_STATE_NOT_CONNECT == iPlcState || _COM_PLC_STATE_OPEN_COM_ERROR==iPlcState);
	GetDlgItem(IDC_BUTTON_CONNECT_PLC)->EnableWindow( b);
	GetDlgItem(IDC_BUTTON_CLOSE_CONNECTION)->EnableWindow(!b);
}

//�κ���ÿ���ӵ���һ��
void COASysInfoView::OnTimer(UINT_PTR nIDEvent)  
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == m_unSysOneSecondTimer)
	{

	EnterCriticalSection(&gCriticalSection_State);
		int iPlcState = g_COACnfgDoc.m_iPlcState;
	LeaveCriticalSection(&gCriticalSection_State);

		if(_COM_PLC_STATE_COMM_NORESPOND==iPlcState  || _COM_PLC_STATE_COMM_CONTENT_ERROR ==iPlcState  || _COM_PLC_STATE_OPEN_COM_ERROR==iPlcState )
		{
			static BOOL bFlickerOn = true;
			bFlickerOn = !bFlickerOn;
			m_staticPLCstate.ShowWindow(bFlickerOn ? SW_SHOW:SW_HIDE); 			
			if(_COM_PLC_STATE_OPEN_COM_ERROR != iPlcState)
			{ //���ݴ��û����Ӧ��������������
				static unsigned int uTest = 0;
				uTest++;
				if(uTest>3)   
				{
					uTest = 0;
					PostMessage(WM_ITEM1_GET_NAMES,0,0);//ÿ��3���ٳ���һ��
				}
			}
		}

/*

*/
	}
	CFormView::OnTimer(nIDEvent);
}

void COASysInfoView::OnDestroy()
{
	if(m_unSysOneSecondTimer)
	{
		KillTimer(m_unSysOneSecondTimer);
	}
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
}

void COASysInfoView::OnBnClickedButtonConnectPlc()
{
	// TODO: Add your control notification handler code here
//	g_IonGauge.SetGaugeType(1); 2010.05.11 test
//	double x = g_IonGauge.TransStr2Pascal("870");
	g_APCGauge.SetGaugeType(0);
	double x = g_APCGauge.TransVal2Pascal(2000);
	g_APCGauge.SetGaugeType(1);
	x = g_APCGauge.TransVal2Pascal(2000);
	g_APCGauge.SetGaugeType(2);
	x = g_APCGauge.TransVal2Pascal(2000);
	g_APCGauge.SetGaugeType(3);
	x = g_APCGauge.TransVal2Pascal(2000);


	//DEBUGGING	
	//((CButton*)GetDlgItem(IDC_BUTTON_COM2_PROPERTY))->SetIcon( AfxGetApp()->LoadIcon(IDI_ICON_PROPERTY) ); 

	//::g_pVacProThread->PostThreadMessageW(WM_EXIT,0,0);
	//TerminateThread (g_pVacProThread->m_hThread,0);
//	g_pVacProThread->SuspendThread();
    HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));
	DWORD dw = WaitForSingleObject(g_hEvent_plc,1000);
	if( WAIT_TIMEOUT == dw || WAIT_FAILED == dw )
	{
		AfxMessageBox(TEXT("Error:Debug, WaitForSingleObject(g_hEvent_plc,1000);"));
		fn_AppendHistoryText(TEXT("Error Connect PLC: WaitForSingleObject(g_hEvent_plc,1000) ,time out;\r\n"));
	}
	else
	{
		ResetEvent(g_hEvent_plc); //���������̵߳���plc��غ���
		bool b = g_plc.OpenCom();
		SetEvent(g_hEvent_plc);   //���ͷ�plc
		if( b )
		{
			//��¼
			_stprintf(g_strBuffer,TEXT("%s  %s open,\r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.m_strCom);
			fn_AppendHistoryText(g_strBuffer);
			//�趨plc״̬


			EnterCriticalSection(&gCriticalSection_State);
			g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_TRY_COMM;
			LeaveCriticalSection(&gCriticalSection_State);


			PostMessage(WM_ITEM1_GET_NAMES,0,0);  //��һ�ε�����Ϣ������������ȴ�3s,ֱ�ӵ��á�֮����WM_TIMERÿ�θ��������
		}
		else
		{
			//��¼			
			_stprintf(g_strBuffer,TEXT("%s  Error: cannot open %s, dwLastError = %d \r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.m_strCom,g_plc.GetLastError());
			fn_AppendHistoryText(g_strBuffer);

			EnterCriticalSection(&gCriticalSection_State);
			g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_OPEN_COM_ERROR;
			LeaveCriticalSection(&gCriticalSection_State);
		}
		fn_DisplayPlcState();
		fn_ShowWindow();

#ifdef _ZHANGZIYE_DEBUG	
	//g_plc.SetSleepTime(0);	
	DWORD dwStart = GetTickCount();
	ResetEvent(g_hEvent_plc);//�����������̵߳�plc��ȡ����
	CString str = g_plc.GetDMstr(9000, 1);
	SetEvent(g_hEvent_plc);  //��ɺ���

	_stprintf(g_strBuffer,TEXT("g_plc.GetDMstr(9000, 1) %s: %d ms\r\n"),str,GetTickCount()-dwStart);	
	fn_AppendHistoryText(g_strBuffer);
#endif// _ZHANGZIYE_DEBUG

	}
	SetCursor(hcur);
}

void COASysInfoView::OnBnClickedButtonCloseConnection()
{
	// TODO: Add your control notification handler code here
	//debug, ����� ��� �Ƿ����� ����...

	bool bVacuumTest;
	EnterCriticalSection(&gCriSec_Vacuum);
			//memcpy(&sVacuumState,&gi_stVacuumTestState,sizeof(stVacuumTestState));
	bVacuumTest = gi_stVacuumTestState.bTest;
	LeaveCriticalSection(&gCriSec_Vacuum);

	if(bVacuumTest || g_bTestHearthRotation)
	{
		AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_ERROR_TEST],MB_OK|MB_ICONSTOP);
		return;
	}

	HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));
	DWORD dw = WaitForSingleObject(g_hEvent_plc,1000);
	if(WAIT_TIMEOUT==dw || WAIT_FAILED==dw)
	{
		AfxMessageBox(TEXT("Error:WaitForSingleObject(g_hEvent_plc,1000);"));
		fn_AppendHistoryText(TEXT("Error close connect PLC: WaitForSingleObject(g_hEvent_plc,1000) ,time out;\r\n"));
	}
	else
	{
		g_plc.CloseCom();
		SetEvent(g_hEvent_plc);//���ͷ�plc

		_stprintf(g_strBuffer,TEXT("%s  %s closed\r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.m_strCom);
		fn_AppendHistoryText(g_strBuffer);

		//�趨plc״̬
			EnterCriticalSection(&gCriticalSection_State);
			g_COACnfgDoc.m_iPlcState = _COM_PLC_STATE_NOT_CONNECT;
			LeaveCriticalSection(&gCriticalSection_State);

		fn_DisplayPlcState();
		fn_ShowWindow();
	}
	SetCursor(hcur);
}
BOOL COASysInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_CHAR:
		if( (GetFocus() == GetDlgItem(IDC_EDIT_HISTORY))  || (GetFocus() == GetDlgItem(IDC_BUTTON_CONNECT_PLC)))
		{
			//ͳһ��Сд�ַ�
			TCHAR pch;
			if(pMsg->wParam >= _T('A') && pMsg->wParam<= _T('Z'))
			{
				pch = pMsg->wParam + (_T('a') - _T('A'));
			}
			else
				pch = pMsg->wParam;


			//�����������״̬
			static int nCharCounter = 0;
			static bool bPass = 0; //2005.06.28
			static const TCHAR cPassword[] = _T("optorun"); //
			if(bPass)
			{

				bPass =  (cPassword[nCharCounter+1] == pch);
				if(bPass)
					nCharCounter++;
				else
					nCharCounter = 0;

				if(nCharCounter == sizeof(cPassword)/sizeof(TCHAR) -1-1)
				//if(nCharCounter == _tcslen(cPassword)-1)
				{
					CFormView::PreTranslateMessage(pMsg);
					fn_InDebugState();
					nCharCounter = 0;
					bPass = 0;
					return true;
				}
			}
			else
			{
				bPass = (pch == cPassword[0]);
			}

			static int nzCharCounter = 0;
			static bool bzPass = 0; //2005.06.28
			static const TCHAR czPassword[] = _T("zhangziye"); //
			if(bzPass)
			{
				bzPass =  (czPassword[nzCharCounter+1] == pch);
				if(bzPass)
					nzCharCounter++;
				else
					nzCharCounter = 0;

				if(nzCharCounter == sizeof(czPassword)/sizeof(TCHAR) -1-1)
				//if(nCharCounter == _tcslen(czPassword)-1)
				{
					CFormView::PreTranslateMessage(pMsg);
					fn_InDebugState();
					GetDlgItem(IDC_CHECK_RECORD_VACUUM)->ShowWindow(SW_SHOW);
					nzCharCounter = 0;
					bzPass = 0;
					return true;
				}
			}
			else
			{
				bzPass = (pch == czPassword[0]);
			}

		}
		break;
	}
	return CFormView::PreTranslateMessage(pMsg);
}

// ����Debug state,�ı�һЩ�������еĹؼ���������Щ����ֻ���������֮�ã�������ͨ�û�����
void COASysInfoView::fn_InDebugState(void)
{
	//�����֪�Ѿ�����DEBUG STATE
	_stprintf(g_strBuffer,TEXT("NOW WE ARE IN DEBUG STATE: %s\r\n"),g_COACnfgDoc.fn_GetLocalTimeStr(),g_plc.m_strCom);
	fn_AppendHistoryText(g_strBuffer);

	//
	gb_ProgrammerDebugNormal = true;

	m_editHistory.SetReadOnly(false);

	//��ʾ���д��ڲ��Խ���
	//ע	IDC_STATIC_SERIAL_PORT_TEST ~ IDC_STATIC_COM5_TEXT��������Դ��ID�������ڵġ������ӻ�ɾ�����������¿�֤��δ��룡
	CWnd* pwnd;
	for(int i = IDC_STATIC_SERIAL_PORT_TEST;i<=IDC_STATIC_COM5_TEXT;i++)
	{
		pwnd = GetDlgItem(i);
		if(pwnd)
		{
			pwnd->ShowWindow(SW_SHOW);
		}
	}
	fn_ShowCom();

}


//void COASysInfoView::OnBnClickedStaticSerialPortTest()
//{
//	// TODO: Add your control notification handler code here
//
//}
//void COASysInfoView::OnBnDoubleclickedStaticSerialPortTest()
//{
//	//�ر������Ѵ򿪴���, ������com1
//	//CloseAllOpenedCom();
//	//������дcomboBox�еĴ�����Ϣ
//	for(int i=IDC_COMBO_COM2;i<=IDC_COMBO_COM5;i++)
//	{
//		CComboBox* pCom = (CComboBox*)GetDlgItem(i);
//		FindCommPort(pCom);
//	}
//	//������ʾ������Ϣ
//	fn_ShowCom();
//}



void COASysInfoView::OnBnClickedButtonCom2Open()
{
	CString str;
	GetDlgItemText(IDC_COMBO_COM2,str);
	g_COM2_ODM.SetComPort(str);
	if(g_COM2_ODM.OpenCom())
	{
	}
	else
	{
		AfxMessageBox(_T("����û�д�"));
	}	
	fn_ShowCom();
}

void COASysInfoView::OnBnClickedButtonCom2Close()
{
	// TODO: Add your control notification handler code here
	g_COM2_ODM.CloseCom();
	fn_ShowCom();
}


#define _COM_ACK 0x06
#define _COM_NAK 0x15
void COASysInfoView::OnBnClickedButtonCom2Test()
{
	// TODO: Add your control notification handler code here
	if(g_COM2_ODM.IsOpen())
	{
		HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));
		//debugging
		char* pcTest;
		pcTest = "H\x06";
			
		g_COM2_ODM.SendCommand(pcTest,strlen(pcTest));
		DWORD dwRdBytes=0;
		g_COM2_ODM.ReceiveData(dwRdBytes,1024,g_chBuffer);
		if(dwRdBytes!=0)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString str;
			MultiByteToWideChar(CP_UTF8,0,g_chBuffer,strlen(g_chBuffer) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			str.Format(_T("%4d-%02d-%02d %02d:%02d:%02d\r\n[SEND]H\r\n[RECV]%s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,g_buffer);
			AfxMessageBox(str);
		}
		else
		{
			AfxMessageBox(_T("Error: Not receive any char.\r\n����û�н��յ��κ��ַ���\r\n\r\nû�������� XTC �� ODM "),MB_ICONSTOP);
			//AfxMessageBox(_T("���󣡴���û�н��յ��κ��ַ���\r\n\r\nû�������� XTC �� ODM "),MB_ICONSTOP);
		}

		SetCursor(hcur);
	}

}

void COASysInfoView::OnBnClickedButtonCom2Property()
{
	// TODO: Add your control notification handler code here
}

void COASysInfoView::OnBnClickedButtonCom3Open()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_COMBO_COM3,str);
	g_COM3_IBGL.SetComPort(str);
	if(g_COM3_IBGL.OpenCom())
	{
	}
	else
	{
		AfxMessageBox(_T("����û�д�"));
	}	
	fn_ShowCom();
}

void COASysInfoView::OnBnClickedButtonCom3Close()
{
	// TODO: Add your control notification handler code here
	g_COM3_IBGL.CloseCom();
	fn_ShowCom();
}

void COASysInfoView::OnBnClickedButtonCom3Test()
{
	// TODO: Add your control notification handler code here
	if(g_COM3_IBGL.IsOpen())
	{
		HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));
		//debugging
		char* pcTest;
		pcTest = pComTestStringForGL;//��ʼʱ "IBSQID\x0D"; //2010.04.14
			
		g_COM3_IBGL.SendCommand(pcTest,strlen(pcTest));
		DWORD dwRdBytes=0;
		g_COM3_IBGL.ReceiveData(dwRdBytes,1024,g_chBuffer);
		if(dwRdBytes!=0)
		{
			//2010.04.14
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString str;
			MultiByteToWideChar(CP_UTF8,0,g_chBuffer,strlen(g_chBuffer) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			MultiByteToWideChar(CP_UTF8,0,pcTest,strlen(pcTest) + 1,g_tchBuffer2,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			str.Format(_T("%4d-%02d-%02d %02d:%02d:%02d\r\n[SEND]%s\r\n[RECV]%s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,g_tchBuffer2,g_buffer);
			AfxMessageBox(str);

			/* //2010.04.14ɾ��
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString str;
			MultiByteToWideChar(CP_UTF8,0,g_chBuffer,strlen(g_chBuffer) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			str.Format(_T("%4d-%02d-%02d %02d:%02d:%02d\r\n[SEND]RC\r\n[RECV]%s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,g_buffer);
			AfxMessageBox(str);
			*/
		}
		else
		{
			AfxMessageBox(_T("Error: Not receive any char.\r\n���󣡴���û�н��յ��κ��ַ���\r\n\r\nû��������IB��GL"),MB_ICONSTOP);
		}

		SetCursor(hcur);
	}
}
#include "DlgSetSendChar.h"
//�趨IBS/GL �Ĵ������ַ���
void COASysInfoView::OnBnClickedButtonCom3Property()
{
	// TODO: Add your control notification handler code here
	CDlgSetSendChar dlg;
	dlg.m_strComment = _T("���趨 IBS/GL �Ĳ�������");
	int iLength = strlen(pComTestStringForGL);
	dlg.m_b0D0A = false;
	if( iLength>=2)
	{
		if( pComTestStringForGL[iLength-1] == 0x0D)
		{
			dlg.m_b0D0A = true;	
		}
	}

	if(dlg.m_b0D0A)
	{
		pComTestStringForGL[iLength-1] = 0;   //��β���ţ��൱��ȥ�� ��β�� 0D�����油��
		MultiByteToWideChar(CP_UTF8,0,pComTestStringForGL,strlen(pComTestStringForGL)  + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
		pComTestStringForGL[iLength-1] = 0x0D; //������� 0x0D
	}
	else
	{
		MultiByteToWideChar(CP_UTF8,0,pComTestStringForGL,strlen(pComTestStringForGL) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
	}
	
	dlg.m_strSend.Format(_T("%s"),g_buffer);
	
	if(IDOK == dlg.DoModal())
	{
		if( dlg.m_strSend.GetLength() < 1024 )
		{
			iLength = WideCharToMultiByte(CP_UTF8,0,dlg.m_strSend,dlg.m_strSend.GetLength()+1,pComTestStringForGL,1024,NULL,NULL);
			pComTestStringForGL[iLength] = '\0';
		}
	}
}

void COASysInfoView::OnBnClickedButtonCom4Open()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_COMBO_COM4,str);
	g_COM4_GasController.SetComPort(str);
	if(g_COM4_GasController.OpenCom())
	{
	}
	else
	{
		AfxMessageBox(_T("����û�д�"));
	}	
	fn_ShowCom();
}

void COASysInfoView::OnBnClickedButtonCom4Close()
{
	// TODO: Add your control notification handler code here
	g_COM4_GasController.CloseCom();
	fn_ShowCom();
}

void COASysInfoView::OnBnClickedButtonCom4Test()
{
	// TODO: Add your control notification handler code here
	if(g_COM4_GasController.IsOpen())
	{
		HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));
		//debugging
		char* pcTest;
		pcTest = pComTestStringForGasCont;//��ʼĬ��"RS232C\x0D\x0A";
			
		g_COM4_GasController.SendCommand(pcTest,strlen(pcTest));
		DWORD dwRdBytes=0;
		g_COM4_GasController.ReceiveData(dwRdBytes,1024,g_chBuffer);
		if(dwRdBytes!=0)
		{
			//2010.04.14
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString str;
			MultiByteToWideChar(CP_UTF8,0,g_chBuffer,strlen(g_chBuffer) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			MultiByteToWideChar(CP_UTF8,0,pcTest,strlen(pcTest) + 1,g_tchBuffer2,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			str.Format(_T("%4d-%02d-%02d %02d:%02d:%02d\r\n[SEND]%s\r\n[RECV]%s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,g_tchBuffer2,g_buffer);
			AfxMessageBox(str);
/*
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString str;
			MultiByteToWideChar(CP_UTF8,0,g_chBuffer,strlen(g_chBuffer) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			str.Format(_T("%4d-%02d-%02d %02d:%02d:%02d\r\n[SEND]RS232C\r\n[RECV]%s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,g_buffer);
			AfxMessageBox(str);

*/
		}
		else
		{
			AfxMessageBox(_T("Error: Not receive any char.\r\n���󣡴���û�н��յ��κ��ַ���\r\n\r\nû�������� GAS CONTROLLER "),MB_ICONSTOP);
		}

		SetCursor(hcur);
	}
}

//�趨 ���͸� Gas Controller���ַ���.(Ĭ����COM4)
void COASysInfoView::OnBnClickedButtonCom4Property()
{
	// TODO: Add your control notification handler code here
	CDlgSetSendChar dlg;
	dlg.m_strComment = _T("���趨 Gas Controller �Ĳ�������.\rPlease set the command characters for Gas Controller");
	int iLength = strlen(pComTestStringForGasCont);
	dlg.m_b0D0A = false;
	if( iLength>=2)
	{
		if( pComTestStringForGasCont[iLength-1] == 0x0D )
		{
			dlg.m_b0D0A = true;	
		}
	}

	if(dlg.m_b0D0A)
	{
		pComTestStringForGasCont[iLength-1] = 0;   //��β���ţ��൱��ȥ�� ��β�� 0D0A�����油��
		MultiByteToWideChar(CP_UTF8,0,pComTestStringForGasCont,strlen(pComTestStringForGasCont)  + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
		pComTestStringForGasCont[iLength-1] = 0x0D; //������� 0x0D
	}
	else
	{
		MultiByteToWideChar(CP_UTF8,0,pComTestStringForGasCont,strlen(pComTestStringForGasCont) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
	}
	
	dlg.m_strSend.Format(_T("%s"),g_buffer);
	
	if(IDOK == dlg.DoModal())
	{
		if( dlg.m_strSend.GetLength() < 1024 )
		{
			iLength = WideCharToMultiByte(CP_UTF8,0,dlg.m_strSend,dlg.m_strSend.GetLength(),pComTestStringForGasCont,1024,NULL,NULL);
			pComTestStringForGasCont[iLength] = '\0';
		}
	}
}

void COASysInfoView::OnBnClickedButtonCom5Open()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_COMBO_COM5,str);
	g_COM5_VGC500.SetComPort(str);
	if(g_COM5_VGC500.OpenCom())
	{
	}
	else
	{
		AfxMessageBox(_T("����û�д�"));
	}	
	fn_ShowCom();
}

void COASysInfoView::OnBnClickedButtonCom5Close()
{
	// TODO: Add your control notification handler code here
	g_COM5_VGC500.CloseCom();
	fn_ShowCom();
}



void COASysInfoView::OnBnClickedButtonCom5Test()
{
	// TODO: Add your control notification handler code here
	if(g_COM5_VGC500.IsOpen())
	{
		//pCom5TestString
		HCURSOR hcur = SetCursor(LoadCursor(NULL,IDC_WAIT));
		//debugging
		char* pcTest;
		pcTest = pCom5TestString;//"VER?\x0D\x0A";
			
		g_COM5_VGC500.SendCommand(pcTest,strlen(pcTest));
		DWORD dwRdBytes=0;
		g_COM5_VGC500.ReceiveData(dwRdBytes,1024,g_chBuffer);
		if(dwRdBytes!=0)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString str;
			MultiByteToWideChar(CP_UTF8,0,g_chBuffer,strlen(g_chBuffer) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			MultiByteToWideChar(CP_UTF8,0,pcTest,strlen(pcTest) + 1,g_tchBuffer2,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			str.Format(_T("%4d-%02d-%02d %02d:%02d:%02d\r\n[SEND]%s\r\n[RECV]%s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,g_tchBuffer2,g_buffer);
			AfxMessageBox(str);
		}
		else
		{
			AfxMessageBox(_T("Error: Not receive any data.\r\n���󣡴���û�н��յ��κ��ַ���\r\n\r\nû�������� VCC500 "),MB_ICONSTOP);
		}

		SetCursor(hcur);
	}
}

#include "Vcc500PropertyDlg.h"
void COASysInfoView::OnBnClickedButtonCom5Property()
{
	// TODO: Add your control notification handler code here
	Vcc500PropertyDlg dlg;
	int iLength = strlen(pCom5TestString);
	dlg.m_b0D0A = false;
	if( iLength>=2)
	{
		if( pCom5TestString[iLength-2] == 0x0D && pCom5TestString[iLength-1]==0x0A)
		{
			dlg.m_b0D0A = true;	
//			pCom5TestString[iLength-2] = 0; //��β���ţ��൱��ȥ�� ��β�� 0D0A�����油��
		}
	}

	if(dlg.m_b0D0A)
	{
		pCom5TestString[iLength-2] = 0;   //��β���ţ��൱��ȥ�� ��β�� 0D0A�����油��
		MultiByteToWideChar(CP_UTF8,0,pCom5TestString,strlen(pCom5TestString)  + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
		pCom5TestString[iLength-2] = 0x0D; //������� 0x0D
	}
	else
	{
		MultiByteToWideChar(CP_UTF8,0,pCom5TestString,strlen(pCom5TestString) + 1,g_buffer,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
	}
	
	dlg.m_strSend.Format(_T("%s"),g_buffer);
	
	if(IDOK == dlg.DoModal())
	{
		if( dlg.m_strSend.GetLength() < 1024 )
		{
			iLength = WideCharToMultiByte(CP_UTF8,0,dlg.m_strSend,dlg.m_strSend.GetLength(),pCom5TestString,1024,NULL,NULL);
			pCom5TestString[iLength] = '\0';
		}
	}
	
}

// ��ʾCOM2~COM5����ؽ��棨�����ڲ�������
int COASysInfoView::fn_ShowCom(void)
{
    if(g_COM2_ODM.IsOpen())
	{
		GetDlgItem(IDC_COMBO_COM2)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM2_OPEN)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM2_CLOSE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM2_TEST)->EnableWindow(true);
	//	GetDlgItem(IDC_BUTTON_COM2_PROPERTY)->EnableWindow(false);		
	}
	else
	{
		GetDlgItem(IDC_COMBO_COM2)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM2_OPEN)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM2_CLOSE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM2_TEST)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM2_PROPERTY)->EnableWindow(true);
	}

    if(g_COM3_IBGL.IsOpen())
	{
		GetDlgItem(IDC_COMBO_COM3)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM3_OPEN)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM3_CLOSE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM3_TEST)->EnableWindow(true);
	//	GetDlgItem(IDC_BUTTON_COM3_PROPERTY)->EnableWindow(false);		
	}
	else
	{
		GetDlgItem(IDC_COMBO_COM3)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM3_OPEN)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM3_CLOSE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM3_TEST)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM3_PROPERTY)->EnableWindow(true);
	}

	if(g_COM4_GasController.IsOpen())
	{
		GetDlgItem(IDC_COMBO_COM4)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM4_OPEN)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM4_CLOSE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM4_TEST)->EnableWindow(true);
	//	GetDlgItem(IDC_BUTTON_COM4_PROPERTY)->EnableWindow(false);		
	}
	else
	{
		GetDlgItem(IDC_COMBO_COM4)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM4_OPEN)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM4_CLOSE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM4_TEST)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM4_PROPERTY)->EnableWindow(true);
	}
		  
	if(g_COM5_VGC500.IsOpen())
	{
		GetDlgItem(IDC_COMBO_COM5)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM5_OPEN)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM5_CLOSE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM5_TEST)->EnableWindow(true);
	//	GetDlgItem(IDC_BUTTON_COM5_PROPERTY)->EnableWindow(false);		
	}
	else
	{
		GetDlgItem(IDC_COMBO_COM5)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM5_OPEN)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_COM5_CLOSE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM5_TEST)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_COM5_PROPERTY)->EnableWindow(true);
	}
	return 0;
}

void COASysInfoView::OnStnClickedStaticCom2Text()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("xxxx"));
}


void COASysInfoView::OnStnClickedStaticCom3Text()
{
	// TODO: Add your control notification handler code here
}

void COASysInfoView::OnStnClickedStaticCom4Text()
{
	// TODO: Add your control notification handler code here
}

void COASysInfoView::OnStnClickedStaticCom5Text()
{
	// TODO: Add your control notification handler code here
}

void COASysInfoView::OnBnClickedCheckRecordVacuum()
{
	// TODO: Add your control notification handler code here
	if( BST_CHECKED == ((CButton*)GetDlgItem(	IDC_CHECK_RECORD_VACUUM ))->GetCheck())
	{
		g_bOffLineDebug = true;
	}
	else
	{
		g_bOffLineDebug = false;
	}
}

//BOOL COASysInfoView::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: Add your message handler code here and/or call default
//	if( _en_Coater_UNKNOWN_TYPE == g_enOptorunCoaterType )
//	{
//		pDC->TextOutW(100,100,CString(TEXT("UnKnown Type")));
//	}
//
//	return CFormView::OnEraseBkgnd(pDC);
//}

void COASysInfoView::OnStnClickedStaticHelpTitle()
{
	// TODO: Add your control notification handler code here
}
