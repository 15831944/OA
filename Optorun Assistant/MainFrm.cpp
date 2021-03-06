// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Optorun Assistant.h"

//#include "OASysInfoView.h"
//class COASysInfoView;

#include "MainFrm.h"
#include "OAVacuumView.h"
#include "OARotatorView.h"
#include "Optorun AssistantDoc.h"

#include "OACnfgDoc.h"

COACnfgDoc g_COACnfgDoc;
#define _LNG_MENU_IN_MAIN      1 //语言设置菜单在主菜单中的项
#define _LNG_MENU_SUB_POPMENU  2 //语言设置菜单在上述项中的子项


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_NOTIFY(CTCN_SELCHANGE, IDC_TABCTRL, OnTabSelChange)
	ON_MESSAGE(WM_INIT_OTHER_FORM,OnInitOtherForm)
	ON_MESSAGE(MAIN_FRM_ITEMx_2_ITEMy,OnItemXtoItemY)
	ON_MESSAGE(MAIN_FRM_TEST,OnMainFrmTest)
	ON_MESSAGE(MAIN_FRM_VACUUM_THREAD_ERR_OCCUR,OnMainFrmVacuumThreadErrOccur)
	ON_MESSAGE(MAIN_FRM_DISPLAY_VAC_VALUE,OnItem2DisplayVacValue)	
	ON_WM_INITMENUPOPUP()
	ON_COMMAND_RANGE(WM_MENU_SELECT_LNG_CMD, WM_MENU_SELECT_LNG_CMD+100, OnChangeLanguage)

	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction
bool CreateZyBincodeDwordCheckCode(BYTE* pS, int iLen, DWORD& dwCode);
bool VerifyZyBincodeDwordIntegrality(BYTE* pS,int iLen);
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	//m_menuLng.CreatePopupMenu();
	m_unOneSecondTimer = 1;

	//Test start . debug pass.
	const int iLen = 25;
	BYTE pS[iLen];
	for(int i=0;i<iLen;i++)
	{
		pS[i] = (BYTE)i;
	}
	DWORD dw=0;
	bool b = CreateZyBincodeDwordCheckCode(pS,iLen-sizeof(DWORD),dw);
	DWORD* pp = (DWORD*) (pS+iLen-sizeof(DWORD));
	*pp = dw;
	b &= VerifyZyBincodeDwordIntegrality(pS,iLen);




	//Test end
}

CMainFrame::~CMainFrame()
{
	if(m_unOneSecondTimer)
	{
		KillTimer(m_unOneSecondTimer);
	}
		
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	if(!g_COACnfgDoc.fn_InitText())
	{
		return -1;
	}
	 
	fn_UpdateMainMenu(g_COACnfgDoc.m_slistlngMenu ,WM_MENU_SELECT_LNG_CMD);

	/*
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	*/

	m_unOneSecondTimer = SetTimer(m_unOneSecondTimer,1000,NULL);

	//配置Tab高度
	CWinApp* pApp = AfxGetApp();
	m_nTabHeight = pApp->GetProfileInt(TEXT("MainFrame"),TEXT("TabHeight"),25);
	if( m_nTabHeight >= 100 || m_nTabHeight <= 5)
	{
		m_nTabHeight = 25;
	}

	//配置Tab上字体高度
	LOGFONT lf = m_wndTab.GetDefaultFont();
	m_nTabFontHeight = pApp->GetProfileInt(TEXT("MainFrame"),TEXT("TabFontHeight"),20);
	if( m_nTabFontHeight >= 100 || m_nTabFontHeight <= 5)
	{
		m_nTabFontHeight = 20;
	}
	lf.lfHeight = m_nTabFontHeight;
	
	//读取 IonGauge的可能模式
	g_IonGauge.m_nOutputMode = pApp->GetProfileInt(TEXT("DEBUG"),TEXT("IONGAUGE_RECORDER_MODE"),0);

	if (!m_wndTab.Create(WS_CHILD | WS_VISIBLE/*|CTCS_FOURBUTTONS|CTCS_DRAGMOVE|CTCS_CLOSEBUTTON*/ | CTCS_TOOLTIPS | CTCS_TOP, CRect( 0, 0, m_nTabHeight, m_nTabHeight), this, IDC_TABCTRL))
	{
		TRACE0("Failed to create tab control\n");
		return -1;
	}
	m_wndTab.SetControlFont(lf);
	PostMessage(WM_INIT_OTHER_FORM,0,0);		//ZHOU YI 自定义消息，初始化了其他的窗口

/*  //取消这个, 因为隐藏任务栏,界面并不友好
	//2007.12.24
   ::ShowWindow(::FindWindow(TEXT("Shell_TrayWnd"),NULL),SW_HIDE);//隐藏任务栏
	//先隐藏任务栏，再重设窗口风格, 取消 掉 MINIMIZE 和 MAXIMIZE BOX 是实现伪全屏的重要步骤
	LONG style = ::GetWindowLong(m_hWnd,GWL_STYLE); 
	style &=  ~(WS_MINIMIZEBOX);
	style &=  ~(WS_MAXIMIZEBOX);
	::SetWindowLong(m_hWnd,GWL_STYLE,style);

	int nFullWidth=GetSystemMetrics(SM_CXSCREEN);
	int nFullHeight=GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(0,0,nFullWidth,nFullHeight,1);
*/

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
/*	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
*/	
#ifdef _DEBUG
	if(::IsWindow( GetActiveView()->GetSafeHwnd()))
	{
		GetActiveView()->PostMessage(WM_ITEM1_ADD_RUN_HISTORY,0,(LPARAM)TEXT("\r\nMainFrm::From post message\r\n"));
	}
		
	DWORD pViewRemove=0;
	if(::IsWindow(m_wndTab.GetSafeHwnd()))
	{
		m_wndTab.GetItemData(0,pViewRemove);
		if(pViewRemove)
		{
			if(::IsWindow( ((CView*)pViewRemove)->GetSafeHwnd()))
			{
				((CView*)pViewRemove)->PostMessage(WM_ITEM1_ADD_RUN_HISTORY,0,(LPARAM)TEXT("\r\nMainFrm::From post message\r\n"));
			}
		}
	}
#endif

	//fn_FullScreen(); //可实现全屏显示
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION 
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
	//cs.style &= ~WS_MINIMIZEBOX; 2008.03.26


	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTabSDIFrameWnd::OnSelchangeTabctrl(pNMHDR,pResult);	
}


LRESULT CMainFrame::OnInitOtherForm(WPARAM wParam,  LPARAM lParam)
{
	//
	//int nFullWidth=GetSystemMetrics(SM_CXSCREEN);
	//int nFullHeight=GetSystemMetrics(SM_CYSCREEN);
	//MoveWindow(0,0,nFullWidth,nFullHeight,1);
	//::SetWindowPos(m_hWnd,HWND_NOTOPMOST,0,0,nFullWidth,nFullHeight,SWP_DRAWFRAME);


	//添加 真空度
	COAVacuumView* pView = new COAVacuumView;
	if(!pView)
		return 0;
	pView->Create(NULL, NULL, 0L,CFrameWnd::rectDefault, this, AFX_IDW_PANE_FIRST, NULL );
//	AddView(TEXT("真空度"),pView,TEXT("CFormView2 tooltip"));
	AddView(g_COACnfgDoc.m_sTabLabel[_OA_FORM_VIEW_VACUUM],pView,g_COACnfgDoc.m_sTabTooltip[_OA_FORM_VIEW_VACUUM]);	
	pView->OnInitialUpdate();



	//添加转动页面
	COARotatorView* pView3 = new COARotatorView;
	if(!pView3)
		return 0;
	pView3->Create(NULL, NULL, 0L,CFrameWnd::rectDefault, this, AFX_IDW_PANE_FIRST, NULL );
	AddView(g_COACnfgDoc.m_sTabLabel[_OA_FORM_VIEW_ROTATOR],pView3,g_COACnfgDoc.m_sTabTooltip[_OA_FORM_VIEW_ROTATOR]);	
	pView3->OnInitialUpdate();


	SetCurView(0);
	fn_InitLngPopMenu();


	ASSERT(GetSafeHwnd() ==AfxGetMainWnd()->GetSafeHwnd());

	return 0;
}

//从item x 发送的消息转发给item y，在mainframe中转
//从源 x 在 HIWORD(WPARAM), y在LOWORD(WPARAM)中, LPARAM为待转发的消息，转发的消息没有参数.
LRESULT CMainFrame::OnItemXtoItemY(WPARAM wParam,  LPARAM lParam)
{
	int item = LOWORD(wParam);
	if(m_wndTab.GetItemCount()<=item)
	{//如果总tab数小于等于item，参数出错。
		return 0;
	}

	DWORD dwData;
	m_wndTab.GetItemData(item,dwData);
	CView* pView = (CView*)dwData;

	ASSERT(NULL != pView->GetSafeHwnd());
	pView->PostMessage(UINT(lParam));
	return 0;
}

LRESULT CMainFrame::OnMainFrmVacuumThreadErrOccur(WPARAM wParam,  LPARAM lParam)
{
	CString str;
	//gs_plcCommError

	AfxMessageBox(_T("Erro Occur"));
	return 0;
}

LRESULT CMainFrame::OnItem2DisplayVacValue(WPARAM wParam, LPARAM lParam)
{
	DWORD dwData;
	m_wndTab.GetItemData(_OA_FORM_VIEW_VACUUM, dwData);
	CView* pView = (CView*)dwData;
	ASSERT(NULL != pView->GetSafeHwnd());
	pView->PostMessage(WM_ITEM2_DISPLAY_VAC_VALUE,wParam,1000);
	return 0;
}


LRESULT CMainFrame::OnMainFrmTest(WPARAM wParam,  LPARAM lParam)
{
	LPCTSTR psz = (LPCTSTR)lParam;
	AfxMessageBox(psz);
	return 0;
}
BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CTabSDIFrameWnd::OnNotify(wParam, lParam, pResult);
}

//弹出菜单前，对菜单状态进行修改
void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CTabSDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here
	if( _LNG_MENU_SUB_POPMENU == nIndex )
	{
		CMenu* submenu = GetMenu()->GetSubMenu(_LNG_MENU_IN_MAIN);
		if(!submenu)		{			return;		}
		submenu = submenu->GetSubMenu(_LNG_MENU_SUB_POPMENU);
		if(!submenu)		{			return;		}

		if(submenu->GetSafeHmenu() == pPopupMenu->GetSafeHmenu())
		{
			for(int i = 1;i<=g_COACnfgDoc.m_iLngCnt;i++)
			{
				submenu->CheckMenuItem(i, MF_BYPOSITION|( g_COACnfgDoc.m_iCurLng==i ? MF_CHECKED:MF_UNCHECKED ) );
			}
		}		
	}
}


//初始化菜单语言选择项内容
//显然需要等到 g_COACnfgDoc 初始化好后进行
BOOL CMainFrame::fn_InitLngPopMenu(void)
{
	m_menuLng.CreatePopupMenu();
	for(int i=0,nid = WM_MENU_SELECT_LNG_CMD; i<g_COACnfgDoc.m_slistlngMenu.GetCount();i++)
	{
		m_menuLng.InsertMenu(i,MF_BYPOSITION,nid++,g_COACnfgDoc.m_slistlngMenu.GetAt(g_COACnfgDoc.m_slistlngMenu.FindIndex(i)));	
	}
	return false;
}

//响应改变语言菜单项
void CMainFrame::OnChangeLanguage(UINT nID)
{
	int iCurLng = nID - WM_MENU_SELECT_LNG_CMD + 1;
	if(g_COACnfgDoc.m_iCurLng  != iCurLng)
	{
		g_COACnfgDoc.fn_WriteCurLngInt(iCurLng);
		AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_CHANGE_LANGUAGE_NOTE].IsEmpty()?
			TEXT("提示：下次进入本程序时将改动显示用语言。\r\nNote: The language will be updated next time."):g_strSystem[_OA_STR_SYSTEM_CHANGE_LANGUAGE_NOTE]);
		//AfxMessageBox(TEXT("提示：下次进入本程序时将改动显示用语言。\r\nNote: The language will be updated next time."));
	}
}

//更新菜单项的内容，实际是更新语言选择
BOOL CMainFrame::fn_UpdateMainMenu(CStringList &psl,int nID_From)
{
	//找View菜单
	CMenu* submenu = GetMenu()->GetSubMenu(_LNG_MENU_IN_MAIN);
	if(!submenu)
	{
		return false;
	}

	//找language子菜单
	submenu = submenu->GetSubMenu(_LNG_MENU_SUB_POPMENU);
	if(!submenu)
	{
		return false;
	}
	
	int j = 1;
	int nid = nID_From;

	for(;j<submenu->GetMenuItemCount();j++)
	{
		if( nid == submenu->GetMenuItemID(j))
		{
			submenu->RemoveMenu(nid,MF_BYCOMMAND);  
			nid++;
			j--;
		}
	}
	j = 3;
	int i, m = psl.GetCount();
	for(nid = nID_From,i=0; i<m ; i++,j++,nid++)
	{
		submenu->InsertMenu(j,MF_BYPOSITION,nid,psl.GetAt(psl.FindIndex(i)));		
	}
	return true;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
    if( m_unOneSecondTimer == nIDEvent )
	{//每秒1次，在标题栏显示当前时间		
		//CString str;
		SYSTEMTIME   st;   
		GetLocalTime(&st);   
		str.Format(TEXT("Optorun Assistant   %4d-%02d-%02d %02d:%02d:%02d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		SetWindowText(str);		
	}
	CTabSDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if(nID==SC_SCREENSAVE)
	{
		//无关紧要处可以不用 critical section 进行保护.
	//	if( g_stVacuumTestEx.bTesting || g_bTestHearthRotation)
		{//正在进行相关测试，则不允许屏幕保护
			//只要打开，则不进行屏保
			return;			
		}
	}
	CTabSDIFrameWnd::OnSysCommand(nID, lParam);
}

//BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	return CTabSDIFrameWnd::PreTranslateMessage(pMsg);
//}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	return CTabSDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	bool bTesting;
	EnterCriticalSection(&gCriSec_Vacuum);
		bTesting = g_stVacuumTestEx.bTesting ;
	LeaveCriticalSection(&gCriSec_Vacuum);

	//测试中不允许退出程序。
	if(bTesting)
	{
		AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_IS_TESTING_VAC_ERROR].IsEmpty()?
			TEXT("错误：当前不可退出，正在测试真空度相关内容。\r\nError: Cannot exit now!\r\nIt's testing vacuum."):g_strSystem[_OA_STR_SYSTEM_IS_TESTING_VAC_ERROR]);

		//AfxMessageBox(TEXT("错误：当前不可退出，正在测试真空度相关内容。\r\nError: Cannot exit now!\r\nIt's testing vacuum."),MB_OK | MB_ICONSTOP );
		return;
	}

	//g_bVacuumSaved = false;
	if(!g_bVacuumSaved)
	{
		if(IDYES != AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_VAC_DATA_NOT_SAVE_ERR].IsEmpty()?
			TEXT("有真空度数据尚未保存，确认退出吗？\r\nVacuum testing data not saved, Are you sure to exit now?"):g_strSystem[_OA_STR_SYSTEM_VAC_DATA_NOT_SAVE_ERR],MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
		//if(IDYES != AfxMessageBox(TEXT("有真空度数据尚未保存，确认退出吗？"),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
		{
			return;			
		}		
	}

	if(!g_bTestHearthDataSaved)
	{
		if(IDYES != AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_HEARTH_DATA_NOT_SAVE].IsEmpty()?
			TEXT("有坩埚转动数据尚未保存，确认退出吗？\r\nHearth testing data not saved, Are you sure to exit now?"):g_strSystem[_OA_STR_SYSTEM_HEARTH_DATA_NOT_SAVE],MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
		//if(IDYES!=AfxMessageBox(TEXT("有坩埚转动数据尚未保存，确认退出吗？"),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
		{
			return;			
		}
	}

	//g_bTestDomeRotation = false;
	EnterCriticalSection(&gCriticalSection_DomeState);
		bTesting = g_bTestDomeRotation ;
	LeaveCriticalSection(&gCriticalSection_DomeState);
	if(bTesting)
	{
		AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_IS_TESTING_DOME_ERROR].IsEmpty()?
			TEXT("错误：当前不可退出，正在测试Dome转动。请先取消转动测试再尝试退出本程序。\r\nError: Cannot exit now! It's testing Dome rotation."):g_strSystem[_OA_STR_SYSTEM_IS_TESTING_DOME_ERROR]);		
		return;			
		
	}

	
	//::ShowWindow(::FindWindow(TEXT("Shell_TrayWnd"),NULL),SW_SHOW);//显示任务栏
	
	CTabSDIFrameWnd::OnClose();
}

void CMainFrame::fn_FullScreen(void)
{
	 static bool isfull=true;
     static CRect m_OldWndRect;
     static CMenu m_OrgMenu;
 
	 /*
	      LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
          style&=~WS_CAPTION;
		  ::SetWindowLong(m_hWnd,GWL_STYLE,style);
          ::SetWindowPos(m_hWnd,HWND_NOTOPMOST,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE);
             //全屏
          ShowWindow(SW_SHOWMAXIMIZED);
          RecalcLayout();//重绘框架
		  return;
	  */
	 if(isfull==true)
     {
          GetWindowRect(&m_OldWndRect);//得到原窗口位置
          //隐藏工具栏和状态栏
		  /*
          if(m_wndToolBar.IsWindowVisible())
          {
               m_wndToolBar.ShowWindow(SW_HIDE);
          }
		  

          if(m_wndStatusBar.IsWindowVisible())
          {
               m_wndStatusBar.ShowWindow(SW_HIDE);
          }
		  */
          //隐藏菜单栏
          CMenu* pOldMenu=GetMenu();
          m_OrgMenu.Attach(pOldMenu->Detach());
          SetMenu((CMenu*)NULL);
          //隐藏标题栏
          LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
          style&=~WS_CAPTION;
          ::SetWindowLong(m_hWnd,GWL_STYLE,style);
          //窗体最前
          ::SetWindowPos(m_hWnd,HWND_NOTOPMOST,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE);
             //全屏
          ShowWindow(SW_SHOWMAXIMIZED);
          RecalcLayout();//重绘框架
          isfull=false;
     }
     else
     {
          //显示工具栏和状态栏
//          m_wndToolBar.ShowWindow(SW_SHOW);
         // m_wndStatusBar.ShowWindow(SW_SHOW);
          //显示菜单栏
          SetMenu(&m_OrgMenu);
          m_OrgMenu.Detach();
          //显示标题栏
          LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
          style|=WS_CAPTION;
          ::SetWindowLong(m_hWnd,GWL_STYLE,style);
          //取消窗体最前
          //::SetWindowPos(AfxGetMainWnd(),m_hWnd,HWND_NOTOPMOST,m_OldWndRect.left,m_OldWndRect.top,
           //m_OldWndRect.right-m_OldWndRect.left,m_OldWndRect.bottom-m_OldWndRect.top,SWP_SHOWWINDOW);
		  ::SetWindowPos(m_hWnd,HWND_NOTOPMOST,m_OldWndRect.left,m_OldWndRect.top,
           m_OldWndRect.right-m_OldWndRect.left,m_OldWndRect.bottom-m_OldWndRect.top,SWP_SHOWWINDOW);
		  //恢复
          ShowWindow(SW_SHOWNOACTIVATE);
          MoveWindow(m_OldWndRect);
          RecalcLayout();//重绘框架
          isfull=true;
         }
         return;
}

/*********************************************************************
函数名称：VerifyZyBincodeDwordIntegrality
输入参数：BYTE* pS, 源二进制字节串，字节
          int iLen, 二进制字节串的总长度
输出参数：校验结果是否正确
函数说明：校验二进制字节串是否符合zy自定义的规则，通常用于二进制文件的读取时，校验完整性。
自定义规则：总字节串的末sizeof(DWORD)字节内存放的是之前的 所有DWORD字节值的连续
            异或。如果源字节串扣除末尾DWORD值后，不是整数个DWORD，则丢弃
			末尾DWORD之前的字节。
Last Edit: 2008.04.22, By zhangzy
**********************************************************************/
bool VerifyZyBincodeDwordIntegrality(BYTE* pS,int iLen)
{
	//DWORD 32bit unsigned integer
	if(iLen<sizeof(DWORD))
		return false;

	int i; //临时变量

	DWORD dwDest = 0;// = (DWORD)(pS+iLen-sizeof(DWORD));//直接这样写恐怕会出现字节对齐问题
	for(i=0;i<sizeof(DWORD);i++) 
	{
//		dwDest |= (*(pS+iLen-i))<<(i*8); //违反了低前高后的原则，改为下行
		dwDest |= (*(pS+iLen+i-sizeof(DWORD)))<<(i*8);
	}
	
	DWORD dwSource = 0;
	DWORD* pdw = (DWORD*)pS;
	for(i=0;i< (iLen-sizeof(DWORD))/sizeof(DWORD);i++)
	{
		dwSource ^= *pdw;
		pdw += 1;
	}
	return (dwDest == dwSource);
}

/*********************************************************************
函数名称：CreateZyBincodeDwordCheckCode
输入参数：BYTE* pS, 源二进制字节串，字节
          int iLen, 二进制字节串的总长度
		  DWORD& dwCode, 生成的校验码.  也可看成是输出.
输出参数：生成校验码过程是否正确
函数说明：根据二进制字符串，生成二进制校验码，通常用于二进制文件存储时添加完整性校验码。
自定义规则：参见函数 VerifyZyBincodeDwordIntegrality
备注：    此处的 pS 与 VerifyZyBincodeDwordIntegrality中的pS不同。
          此处的 pS 不包含校验码，而VerifyZyBincodeDwordIntegrality中已在末尾包含
Last Edit: 2008.04.22, By zhangzy
**********************************************************************/
bool CreateZyBincodeDwordCheckCode(BYTE* pS, int iLen, DWORD& dwCode)
{
	if(iLen<sizeof(DWORD))
		return false;

	int i,j; //临时变量
	dwCode = 0;
	BYTE* p[sizeof(DWORD)];
	for( i = 0; i< sizeof(DWORD); i++)
	{
		p[i] = (BYTE*)&dwCode + i;
	}

	BYTE* pTemp = pS;
	int   iTemp = iLen/sizeof(DWORD);
	for(i=0; i< iTemp;i++)
	{
		for(j=0; j<sizeof(DWORD); j++)
		{
			*p[j] ^= *(pTemp + j);
		}
		pTemp += sizeof(DWORD);
	}

	return true;
}