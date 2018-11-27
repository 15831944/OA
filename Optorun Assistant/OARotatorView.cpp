// OARotatorView.cpp : implementation file
//2008.07.12 ��������Ĭ��Hearth1��ʱ�룬Hearth2˳ʱ��.���ѡ��. S589�Ǳ���������ĵ�һ̨˫��������
//Hearth2Ϊ����������ʼ���Ի���ִ���"�ڹ涨ʱ����δ�ܽ����ֶ�����ģʽ."
// CW, ������ֵ���٣��������Ͽ�Ϊ��ʱ�룡 CW������Ͽ����Ǵ������Ͽ�������Ͽ��Ƿ��ģ�
// ��ӻ����������ٳ����������ӣ����٣����Ȳ��� 2008.04.02
// �������� FIX ģʽ�£�CW/CCW����ת����process��PITCH������

/*
Domeת�����ԣ�tΪ��Ȧת��ʱ��(seconds)�� nΪÿ����ת��Ȧ����
n = 60 / t;
Ҫ�� n = 50ʱ���������� +/-0.1. 

���� dn/n = -dt/t  . ��ͬת����,�Ե�Ȧʱ�侫��Ҫ��ͬ. 
�� abs(dt/t) = abs(dn/n) < 0.1 / 50 = 0.002.
��Ȧ����dt = t * dn/n = (60/n) * 0 = 

��ƫ�� dt = 1 seconds, ������ʱ�� t > 1/0.002 = 500 seconds
n=50, Ȧ�� = 500/(60/50) = 417 Ȧ.
n=40, Ȧ�� = 500/(60/40) = 334 
n=30, Ȧ�� = 500/(60/30) = 250
n=20, Ȧ�� = 500/(60/20) = 167
n=10, Ȧ�� = 500/(60/10) = 84
*/

/*
PLC DA->Dome������->���
�����㷨��������:
1, �۳�ǰ30Ȧ����
2, �ټ�¼��speed*10��Ȧ������: speed = 50�����¼��500Ȧ
3, ʵ���ܼ�¼Ȧ����=  30 + speed*10.
4, ���ݷ���:
   a, ����Ԥ��ʱ�� t0 = 60/speed.
   b, �Լ�¼Ȧ����ʱ���������, �ҳ����� t0 ,2*t0, 3*t0, 4*t0һ����Χ�ڵ����ݸ���(n1,n2,n3,n4).(��ʱδ���� 4��t0,Ҳ�Ͳ����� 5��t0. ) 
   c, ƽ��ÿȦʱ�� = ��ʱ�� / ( n1 + 2*n2 + 3*n3 + 4*n4 )
   d, ���10Ȧʱ�� = xx.xx��, RPM
*/
// Edit by: ����ҵ
#include "stdafx.h"
#include "Optorun Assistant.h"
#include "OARotatorView.h"
#include "OACnfgDoc.h"
#include "DrawCrucibleThread.h"
#include "RotatorThread.h"
#include "DomeRotateThread.h"
#include <math.h>
#include <stdlib.h>


#define _OA_HEARTH_TYPE_NONE    0 //û������
#define _OA_HEARTH_TYPE_ANNULAR    1 //��������
#define _OA_HEARTH_TYPE_CRUCIBLE   2 //������

#define _HEARTH_MONITOR_TYPE_OMRON 2

#define _ROTATOR_CRUCIBLE_LIST_COL_NO     0
#define _ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE 1
//#define _ROTATOR_CRUCIBLE_LIST_COL_RESUTL 2
#define _ROTATOR_CRUCIBLE_LIST_COL_TARGET   1
#define _ROTATOR_CRUCIBLE_LIST_COL_START 2
#define _ROTATOR_CRUCIBLE_LIST_COL_END   3
#define _ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME 4
#define _ROTATOR_CRUCIBLE_LIST_COL_RESULT 5

#define _ROTATOR_DOME_LIST_COL_SPEED		0
#define _ROTATOR_DOME_LIST_COL_START_CNT	1
#define _ROTATOR_DOME_LIST_COL_END_CNT		2
#define _ROTATOR_DOME_LIST_COL_WAITING_TIME	3
#define _ROTATOR_DOME_LIST_COL_TEST_TIME	4
#define _ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION	5
#define _ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE	6


#define _ROTATE_CRUCIBLE_STOP_JUDGE_TIME  5000 //���жϵ�������ȷ����ֹͣ״̬����Ҫ�����ʱ�䡣�ж������Ǳ�����ֵ����


#define _ROTATE_DOME_NOT_COUNT	30//������Dome�ܼ����ǰnȦ

struct _stReadDomeThreadData
{
	WORD	wDomeRotationCount;
	DWORD	dwTickTime;
};
#include <vector>
std::vector<_stReadDomeThreadData> g_stReadDomeThreadData; //��¼Domeת������
//vector �ռ䲻��ʱ���Զ����������������� �¿ռ��С = ԭ�ռ��С*1.5

// COARotatorView
//�������������� ת�� 10���ʼ���  180����ת�Ƕ�.
struct _stHearthRotationState
{
	int iCurListItem;
	
	int iTargetCoder;  //��������������֮ǰȷ��
	                   //�������������������ҽ��յ�����֮��ʼ��ʱ�� �ȴ�10���Ӻ��û��ɸ߼����ƣ����� ��һ��
	           
	union{
	int iTargetSpeed;
	int iTargetPos;    //�Ի�����������Ŀ��ת���ٶȡ����Ե���������Ŀ��������
	};

	union{
	int iStartPos;     //����ʱ��λ��.���Ե�������Ч
	int iStartCoder;   //��������ʱ��ʵ�ʼ�����ʼ������ֵ
	};

	int iCurHearth;    //��ǰ������
	int iCurCoder;
	int iCurCrucible;  //
	
	DWORD dwStartTime; //����ʱ�� tickCount;
    int iRotationTime; //��ǰ�Ѿ�ת����ʱ��, ms��λ. �����(����)ʱ�� 0.
	
	int iLastCoder;    //�ϴ�ת���� ������ֵ. ����ʱ���һ��. 
	int iLastCrucible; //�ϴ�ת��� ������.   ����ʱ���һ��. //08.01.03
	
	bool bStop;        //����ʱΪ false.
	DWORD dwStopTime;  //ms, ��ǰû���жϵ��˶���ʱ��. ����ʱ�� 0. 
	                   //ֹͣת�������� 1: Q10.11, Q10.12��Ϊ�㣬 ͬʱ iCurCoder == iLastCoder
	                   //�жϵ�������תʱ bstop = true, ��¼��ʱ�� tickCount. �Ժ��ʱ������ת�� �������ʼ��־��
	
	bool bReversal;    //�Ƿ���ת��  ����������ʱ�ж�. 


	//��������
	bool bAnnularIntervalWaiting;  //ת�����ֹͣ�󣬵���һ��ת����ʼǰ(ֹͣ״̬)
	bool bAnnularBeginingWaiting;  //�տ�ʼת���ȴ������߲�����ͬʱΪ��(ת��״̬)
	DWORD dwStartingWaitingTick;   //


	//���������� �������������û����ã����ʼ��
	int  iAnnularStartWaitTime;    //������ʼ��ѯ�ĵȴ�ʱ��
	int  iAnnularIntervalWaitTime; //�����м�ȴ���ʱ��
	bool bAnnularPositive;         //����������ת�ķ���, true��Ӧ˳ʱ��CW
	int  iAnnularTestAngle;        //���ԽǶ� ( 30 ~ 360 ), ȡ��������������.

	//08.01.03
	int  iCoderChangeCounts;       //��¼�������仯������ ����� +1����С�� -1�������򱣳�.
	                               //������������Ծݴ�֪����ʵ�� �������仯����(������������)
	                               //ÿ�γ�ʼ��ʱ��0�� 08.01.03��ӣ���Ӧ�еĻ�̨����� ����ת
	
	int  iCoderNotChangeTime;      //��ʼ��û�м�鵽 Coder �仯��ʱ�䳤��. ���Coder�����仯����������0.��ʼʱ��0.
	DWORD dwCoderNotChangeStartTickTime; //û�м�鵽 Coder �仯����ʼʱ��. //��ʼʱ��¼��ǰ��TciTime.ÿ��Coder�����仯�����
	int  iCrucibleChangeCounts;    //ת���м�¼���������仯��Ŀ. //��Ϊ���ܴ��ڷ�ת�����Խ��������ʵ��ת����������ͬ����������1.
	                               //��ʼ����0.
	enum {_enHearthReversalUnknown,_enHearthReversalTrue,_enHearthReversalFalse}enHaveReversalFunction;     //���������󣬳�ʼ��Ϊ false. ת������У�����ж�.

}g_stHearthRotationState; //���ڱ������߳���ʹ�ã����ж� ת�� ����������ʹ��.

#define _ROTATE_DOME_ABNOMAL_TIMES 4 //����쳣�������. �����ô��Ȧ�ż�⵽һ��ת��
struct _stDomeRotationState
{
	int iCurListItem;  //��ǰ����Item
	int iTargetSpeed;  //Ŀ���ٶ�
	DWORD dwStartTime; //����ʱ�� tickCount;
    int iRotationTime; //��ǰ�Ѿ�ת����ʱ��, ms��λ. �����(����)ʱ�� 0.
	bool bStop;        //����ʱΪ false.

	
	DWORD dwTotalCountTime;
	DWORD dwLimit[_ROTATE_DOME_ABNOMAL_TIMES][2]; // [0]����ֵ,[1]������.��ÿ�β�����ʼ�׶γ�ʼ��һ��
	int	  n[_ROTATE_DOME_ABNOMAL_TIMES];	//����ʱ���Ӧ����Ч����.[0]��Ӧ1��.��ÿ�β�����ʼʱ����һ��.��Ч������;�����ۻ�
	DWORD dwStandardValue;
	DWORD dwEpsilon;// = 500; //ms, ƫ��ֵ, ÿ��ת�������Ӧ�ֲ��� ��׼ʱ���n����+/- ƫ��ֵ��Χ֮��. ����ֵ
	int   nAbnomalCount;      //���ڷ�Χ�ڵ����ݼ���

	//��������ʱ�乩��������������ʹ�á�
	double dAveSecPR;	//ƽ��ÿȦת������Ҫ��ʱ��.��λΪ <Second>��
	double dFiguredSecPR;//���������ÿȦת������Ҫ��ʱ��.��λΪ <Second>��

}g_stDomeRotationState;

bool gb_AnnularCW[2];     //2008.07.14������������ת����Ĭ��ΪHearth1 ��ʱ��ת(CW������)��Hearth2��ת(CCW)��
                          //true:CW

//ֻ�ڡ�����cpp���õ��ı���.��Ϊ��ϣ����ͷ�ļ������á�"OACnfgDoc.h"������û�ж�������Ա����.�� InitDialog�г�ʼ��
#define _OA_ROTATOR_DRAW_HEARTH_BUFFER_SIZE 20
struct _stDrawHearth g_stDrawCrucibleBuffer[_OA_ROTATOR_DRAW_HEARTH_BUFFER_SIZE][_OPTORUN_HEARTH_MAX]; //_OPTORUN_CRUCIBLE_MAX = 2
_stDrawHearth* GetOneDrawCrucibleBuffer(int iHearth)
{
	static int iBufPos[_OPTORUN_HEARTH_MAX] = { 0, 0 };

	ASSERT( (UINT)iHearth < _OPTORUN_HEARTH_MAX );

	iBufPos[iHearth]++;
	if(iBufPos[iHearth] >= _OA_ROTATOR_DRAW_HEARTH_BUFFER_SIZE)
	{
		iBufPos[iHearth] = 0;
	}

	return &g_stDrawCrucibleBuffer[iBufPos[iHearth]][iHearth];
}

CDrawCrucibleThread* g_pDrawCrucibleThread = NULL;
CRotatorThread* g_pRotateThread = NULL;
CDomeRotateThread* g_pDomeRotateThread = NULL;

	TCHAR gszRotatorBuf[512];


IMPLEMENT_DYNCREATE(COARotatorView, CFormView)

COARotatorView::COARotatorView()
	: CFormView(COARotatorView::IDD)
{
	m_bInit = false;
	m_OAHearth[0].iHearthType = _OA_HEARTH_TYPE_NONE;
	m_OAHearth[1].iHearthType = _OA_HEARTH_TYPE_NONE;

	g_stHearthRotationState.bAnnularPositive = true;
	g_stHearthRotationState.iAnnularStartWaitTime = 5000; //ms
	g_stHearthRotationState.iAnnularIntervalWaitTime = 5000;
	g_stHearthRotationState.iAnnularTestAngle = 180; //���� 180�� (��Ȧ)

	g_stHearthRotationState.enHaveReversalFunction = _stHearthRotationState::_enHearthReversalUnknown;//û�з�ת����.

	gb_AnnularCW[0] = true;     //true:CW,2008.07.14������������ת����Ĭ��ΪHearth1 CW(���Ͽ�Ϊ��ʱ�룡)��Hearth2 CCW��
	gb_AnnularCW[1] = false;    //false:CCW

	g_stReadDomeThreadData.reserve(1000); //
	/*
	_stReadDomeThreadData ss;
	ss.dwTickTime = DWORD(0xFFFFFFFF);
	ss.wDomeRotationCount = 1;
	g_stReadDomeThreadData.push_back(ss);

	ss.dwTickTime = DWORD(1234);
	ss.wDomeRotationCount = 15;
g_stReadDomeThreadData.push_back(ss);


	double dAveSecPR = (double) ((DWORD)(g_stReadDomeThreadData.back().dwTickTime-g_stReadDomeThreadData.front().dwTickTime))/((WORD)(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData.front().wDomeRotationCount));

	int x = g_stReadDomeThreadData.size();
	//x = g_stReadDomeThreadData.capacity();
	

	ss.dwTickTime = GetTickCount();
	ss.wDomeRotationCount = 1234;
	g_stReadDomeThreadData.push_back(ss);

	for(int i=0;i<100;i++)
	{
		ss.dwTickTime = GetTickCount();
		ss.wDomeRotationCount = i;
		g_stReadDomeThreadData.push_back(ss);
		x = g_stReadDomeThreadData.size();
		//TRACE2("size = %d		capacity = %d\r\n",x,g_stReadDomeThreadData.capacity());
	}
	
	g_stReadDomeThreadData.clear();
	TRACE2("After clear size = %d		capacity = %d\r\n",g_stReadDomeThreadData.size(),g_stReadDomeThreadData.capacity());

//	TRACE2("After clear at(0)dwTickTime = %d		at(0).wDomeRotationCount = %d\r\n",g_stReadDomeThreadData.at(0).dwTickTime,g_stReadDomeThreadData.at(0).wDomeRotationCount);
	*/
}

COARotatorView::~COARotatorView()
{

}

void COARotatorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ROTATOR_CRUCIBLE, m_ListctrlCrucible);
	DDX_Control(pDX, IDC_STATIC_TEST_TIME, m_staticTestHearthTime);
	DDX_Control(pDX, IDC_LIST_ROTATOR_DOME, m_ListctrlDome);
	DDX_Control(pDX, IDC_STATIC_DOME_THREAD_STATE, m_staticexDomeThreadState);
}

BEGIN_MESSAGE_MAP(COARotatorView, CFormView)
//	ON_BN_CLICKED(IDC_BTN_INIT_CRUCIBLE, &COARotatorView::OnBnClickedBtnInitCrucible)
ON_BN_CLICKED(IDC_BTN_INIT_HEARTH, &COARotatorView::OnBnClickedBtnInitHearth)
ON_BN_CLICKED(IDC_BUTTON_TEST, &COARotatorView::OnBnClickedButtonTest)
ON_WM_TIMER()
ON_WM_ERASEBKGND()
ON_BN_CLICKED(IDC_BTN_INSERT_HEARTH1, &COARotatorView::OnBnClickedBtnInsertHearth1)
ON_BN_CLICKED(IDC_BTN_INSERT_HEARTH2, &COARotatorView::OnBnClickedBtnInsertHearth2)
ON_BN_CLICKED(IDC_BTN_DELETE_HEARTH_ROW, &COARotatorView::OnBnClickedBtnDeleteHearthRow)
ON_BN_CLICKED(IDC_BTN_SAVE_HEARTH_LIST, &COARotatorView::OnBnClickedBtnSaveHearthList)
ON_BN_CLICKED(IDC_BTN_START_HEARTH_TEST, &COARotatorView::OnBnClickedBtnStartHearthTest)
ON_BN_CLICKED(IDC_BTN_CANCEL_HEARTH_TEST, &COARotatorView::OnBnClickedBtnCancelHearthTest)
ON_BN_CLICKED(IDC_BTN_CREATE_HEARTH1, &COARotatorView::OnBnClickedBtnCreateHearth1)
ON_BN_CLICKED(IDC_BTN_CREATE_HEARTH2, &COARotatorView::OnBnClickedBtnCreateHearth2)
ON_BN_CLICKED(IDC_BTN_STATISTICS_EVALUATION, &COARotatorView::OnBnClickedBtnStatisticsEvaluation)
ON_BN_CLICKED(IDC_BTN_AUTOCREATE_DOME_SPEED, &COARotatorView::OnBnClickedBtnAutocreateDomeSpeed)
ON_BN_CLICKED(IDC_BTN_INSERT_DOME_SPEED, &COARotatorView::OnBnClickedBtnInsertDomeSpeed)
ON_BN_CLICKED(IDC_BTN_DELETE_DOME_ROW, &COARotatorView::OnBnClickedBtnDeleteDomeRow)
ON_BN_CLICKED(IDC_BTN_SAVE_DOME_LIST, &COARotatorView::OnBnClickedBtnSaveDomeList)
ON_BN_CLICKED(IDC_BTN_START_DOME_TEST, &COARotatorView::OnBnClickedBtnStartDomeTest)
ON_BN_CLICKED(IDC_BTN_CANCEL_DOME_TEST, &COARotatorView::OnBnClickedBtnCancelDomeTest)
END_MESSAGE_MAP()

// COARotatorView diagnostics
#ifdef _DEBUG
void COARotatorView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void COARotatorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// COARotatorView message handlers
BOOL COARotatorView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void COARotatorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	fn_InitView();

	fn_ShowDomeWindow();
	fn_ShowWindow();
}

// ����OnInitialDialog�е���һ�Σ���ʼ�� RotatorView����
void COARotatorView::fn_InitView(void)
{
	fn_SetIDsText();


	COLORREF g_HearthTextColor = RGB(0,128,255);//GetSysColor(COLOR_WINDOWTEXT);
	//m_staticHearth1Coder.textColor(g_HearthTextColor);
	//m_staticHearth1CruciblePos.textColor(g_HearthTextColor);
	//m_staticHearth2CruciblePos.textColor(g_HearthTextColor);

	//#define _OA_ROTATOR_DRAW_CRUCIBLE_BUFFER_SIZE 20
	//��ȡ��ͼ��
	CRect prect[_OPTORUN_HEARTH_MAX];
	GetDlgItem(IDC_STATIC_HEARTH1_PIC)->GetWindowRect(&prect[0]);
	GetDlgItem(IDC_STATIC_HEARTH2_PIC)->GetWindowRect(&prect[1]);
	ScreenToClient(&prect[0]); //ת���ɱ�ҳ������
	ScreenToClient(&prect[1]);

	HWND hWnd = GetSafeHwnd();
	ASSERT(IsWindow(hWnd));
	int i,j;
	for(i=0;i<_OPTORUN_HEARTH_MAX;i++)
	{
		for(j=0;j<_OA_ROTATOR_DRAW_HEARTH_BUFFER_SIZE;j++)
		{
			g_stDrawCrucibleBuffer[j][i].hwnd = hWnd;
			g_stDrawCrucibleBuffer[j][i].rect = prect[i];
			g_stDrawCrucibleBuffer[j][i].iHearth = i;
		}
	}

	g_pDrawCrucibleThread = (CDrawCrucibleThread*)AfxBeginThread(RUNTIME_CLASS(CDrawCrucibleThread), THREAD_PRIORITY_NORMAL,0,0);
	ASSERT(g_pDrawCrucibleThread);

	g_pRotateThread = (CRotatorThread*)AfxBeginThread(RUNTIME_CLASS(CRotatorThread), THREAD_PRIORITY_NORMAL,0,0);
	ASSERT(g_pRotateThread);
	g_pRotateThread->m_hWnd = GetSafeHwnd();

	g_pDomeRotateThread =(CDomeRotateThread*)AfxBeginThread(RUNTIME_CLASS(CDomeRotateThread), THREAD_PRIORITY_NORMAL,0,0);
	ASSERT(g_pDomeRotateThread);
	g_pDomeRotateThread->m_hWnd = GetSafeHwnd();

	CRect rt1,rt2,rt3;
	CDC* pdc = m_ListctrlCrucible.GetDC();
	CSize sizet = pdc->GetTextExtent(CString(TEXT("Hearth ")));
	CSize sizec = pdc->GetTextExtent(CString(TEXT(" 00:00:00 ")));
	m_ListctrlCrucible.ReleaseDC(pdc);
	m_ListctrlCrucible.SetExtendedStyle(m_ListctrlCrucible.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
//	CRect rect;
//	m_ListctrlCrucible.GetClientRect(rect);	
	m_ListctrlCrucible.InsertColumn(_ROTATOR_CRUCIBLE_LIST_COL_NO, TEXT("Hearth"),LVCFMT_LEFT,sizet.cx);
	m_ListctrlCrucible.InsertColumn(_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE, TEXT("Target"),LVCFMT_CENTER,sizet.cx);
	m_ListctrlCrucible.InsertColumn(_ROTATOR_CRUCIBLE_LIST_COL_START,TEXT("Start"),LVCFMT_CENTER,sizet.cx);
	m_ListctrlCrucible.InsertColumn(_ROTATOR_CRUCIBLE_LIST_COL_END,TEXT("End"),LVCFMT_CENTER,sizet.cx);
	m_ListctrlCrucible.InsertColumn(_ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME,TEXT("Test Time"),LVCFMT_CENTER,sizec.cx);
	m_ListctrlCrucible.GetClientRect(&rt3);
	m_ListctrlCrucible.InsertColumn(_ROTATOR_CRUCIBLE_LIST_COL_RESULT, TEXT("Result"),LVCFMT_LEFT, rt3.Width()-sizet.cx*4-sizec.cx - GetSystemMetrics(SM_CXVSCROLL) );


	//��֤ʵ�ʵ� column ��
	CHeaderCtrl* pmyHeaderCtrl = m_ListctrlCrucible.GetHeaderCtrl();
	int iTest = pmyHeaderCtrl->GetItemCount();

	//�ƶ� m_ListctrlCrucible
	
	GetWindowRect(&rt1); //ҳ�洰��
	m_ListctrlCrucible.GetWindowRect(&rt2); //�б��λ��
	rt3.bottom = rt1.bottom - 3;
	rt3.right  = rt2.right;
	rt3.left   = rt2.left;
	rt3.top    = rt2.top;
	ScreenToClient(&rt3);
	m_ListctrlCrucible.MoveWindow(&rt3);
	
	ClientToScreen(&rt3);//��ԭ�����������ʹ��

	m_ListctrlDome.GetWindowRect(&rt2);//
	//rt3.bottom//rt3.top ���ֲ���
	rt3.right = rt1.right-3;
	rt3.left  = rt2.left;
	ScreenToClient(&rt3);
	m_ListctrlDome.MoveWindow(&rt3);

	//���ҷָ��߶�λ
	GetDlgItem(IDC_STATIC_DOME_HEARTH_SPACE)->GetWindowRect(&rt2);
	rt2.top = rt1.top + 3;
	rt2.bottom = rt1.bottom - 3;
	ScreenToClient(&rt2);
	GetDlgItem(IDC_STATIC_DOME_HEARTH_SPACE)->MoveWindow(&rt2);



	m_ListctrlDome.SetExtendedStyle(m_ListctrlCrucible.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_SPEED, TEXT("Speed"),LVCFMT_LEFT,sizet.cx-1);
	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_START_CNT, TEXT("Start"),LVCFMT_CENTER,sizet.cx);
	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_END_CNT,TEXT("End"),LVCFMT_CENTER,sizet.cx);
	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_WAITING_TIME,TEXT("Wait"),LVCFMT_CENTER,sizet.cx);
	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_TEST_TIME,TEXT("Test Time"),LVCFMT_CENTER,sizec.cx);
	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION,TEXT("SecPR"),LVCFMT_CENTER,sizec.cx);
	m_ListctrlDome.GetClientRect(&rt3);
	m_ListctrlDome.InsertColumn(_ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE, TEXT("RPM"),LVCFMT_LEFT, rt3.Width()-sizet.cx*4-sizec.cx*2 - GetSystemMetrics(SM_CXVSCROLL) );




	CString str;
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DOME_SPEED);
	for(int i=10;i<=50;i+=5)
	{//���������£���дת���ٶ�. 
		str.Format(_T("%d"),i);
		pComboBox->AddString(str);
	}
	pComboBox->SetCurSel(0);

	//08.01.04
	m_staticTestHearthTime.GetWindowRect(&rt3);
	m_staticTestHearthTime.setFont(rt3.Height()-4);

	m_staticexDomeThreadState.GetWindowRect(&rt3);
	m_staticexDomeThreadState.setFont(rt3.Height()-4);

}

void COARotatorView::fn_ShowWindow(void)
{
	if(m_bInit)
	{//��ʼ���ɹ�
		bool bTest;
		EnterCriticalSection(&gCriticalSection_HearthState);
			bTest = g_bTestHearthRotation;
		LeaveCriticalSection(&gCriticalSection_HearthState);
		CString str;
		if(m_OAHearth[0].iHearthType != _OA_HEARTH_TYPE_NONE)
		{
			if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[0].iHearthType)
			{//������
				GetDlgItem(IDC_COMBO_HEARTH_12)->EnableWindow(!bTest);			    
			}
			else
			{//��������
				GetDlgItem(IDC_COMBO_HEARTH_1)->EnableWindow(!bTest);
			}
			GetDlgItem(IDC_BTN_CREATE_HEARTH1)->EnableWindow(!bTest);	
			GetDlgItem(IDC_BTN_INSERT_HEARTH1)->EnableWindow(!bTest);
		}

		if(m_OAHearth[1].iHearthType != _OA_HEARTH_TYPE_NONE)
		{
			if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[1].iHearthType)
			{
				GetDlgItem(IDC_COMBO_HEARTH_22)->EnableWindow(!bTest);
			}
			else
			{
				GetDlgItem(IDC_COMBO_HEARTH_2)->EnableWindow(!bTest);
			}

			GetDlgItem(IDC_BTN_CREATE_HEARTH2)->EnableWindow(!bTest);			//20160329jiang
			GetDlgItem(IDC_BTN_INSERT_HEARTH2)->EnableWindow(!bTest);
			// 2007.12.18����ʱ��ֹ�� Hearth2 ���η�ʽ
			if(m_OAHearth[1].iHearthType == _OA_HEARTH_TYPE_ANNULAR)
			{
				GetDlgItem(IDC_BTN_INSERT_HEARTH2)->EnableWindow(true); //2008.07.14
				GetDlgItem(IDC_BTN_CREATE_HEARTH2)->EnableWindow(true);
				GetDlgItem(IDC_CHECK_HEARTH1_DIRECTION)->EnableWindow(!bTest);//����ʱ��������ѡ��
				GetDlgItem(IDC_CHECK_HEARTH2_DIRECTION)->EnableWindow(!bTest);
			}
			else
			{				//08.01.04 Debugging
				GetDlgItem(IDC_BTN_CREATE_HEARTH2)->EnableWindow(!bTest);
				//GetDlgItem(IDC_COMBO_HEARTH_2)->EnableWindow(!bTest);
				GetDlgItem(IDC_BTN_INSERT_HEARTH2)->EnableWindow(!bTest);
			}
			
		}
		
		GetDlgItem(IDC_BTN_START_HEARTH_TEST)->EnableWindow(!bTest);
		GetDlgItem(IDC_BTN_CANCEL_HEARTH_TEST)->EnableWindow(bTest);
		GetDlgItem(IDC_BTN_DELETE_HEARTH_ROW)->EnableWindow(!bTest);
		//GetDlgItem(IDC_BTN_INSERT_HEARTH2)->EnableWindow(!bTest);
		if(bTest)
		{
			m_staticTestHearthTime.textColor(RGB(255,0,0));
		}
		else
		{
			//m_staticTestHearthTime.textColor(RGB(0,0,0));
			m_staticTestHearthTime.textColor(GetSysColor(COLOR_3DFACE));
		}
	}
	else
	{//��δ��ʼ��
		GetDlgItem(IDC_BTN_INIT_HEARTH)->EnableWindow(true);
		GetDlgItem(IDC_BTN_DELETE_HEARTH_ROW)->EnableWindow(false);
		//GetDlgItem(IDC_BTN_SAVE_HEARTH_LIST)->EnableWindow(false);
		GetDlgItem(IDC_BTN_START_HEARTH_TEST)->EnableWindow(false);
		GetDlgItem(IDC_BTN_CANCEL_HEARTH_TEST)->EnableWindow(false);
	}

	//��ʱ��ֹHearth1,��Ϊ������� Hearth1��ת����ʽ debugging 2007.12.18
//	GetDlgItem(IDC_BTN_INSERT_HEARTH1)->EnableWindow(false);
//	GetDlgItem(IDC_BTN_CREATE_HEARTH1)->EnableWindow(false);
}

/*
��Ҫ����
	������ȡ�������䵱ǰ״̬���ݴ˳�ʼ������
	��������/6248 ����0000�������plc�ڲ�״̬
	��
�������������߳�ִ�У�ִ��һ�κ��ֹ�ڶ��Σ�
*/

//int _stVacuumThreadData::iTest = 30; 
void COARotatorView::OnBnClickedBtnInitHearth()
{
	//TEST
	// WORD wTest = ( (1<<1) | (1<<(1+4)) );
	//int iSpeed = 160;
  //  WORD wValue = ((iSpeed/100)<<8) + (((iSpeed/10)%10)<<4) +iSpeed%10;
	WORD wTest = 1<<2;
	WORD wValue = 1<<2 | 1<<3;
	WORD wX = wTest & wValue;
/*
	DWORD dwEndTime = 0x1234;
	DWORD dwStartTime = 0x012;
	int iX = 3;
	int iTemp = 0.5 + (double)(dwEndTime-dwStartTime)/iX/1000 ;
		//(g_stHearthRotationState.dwCoderNotChangeStartTickTime - g_stHearthRotationState.dwStartTime)/iTurnBit/1000;
*/
	//��ʽ��ʼ
	bool btrue = true;
	bool bExist = false;
	for(int i =0;i<_OPTORUN_HEARTH_MAX;i++)
	{
		btrue &= g_pRotateThread->fn_GetHearthInfo(i,bExist,m_OAHearth[i].iCrucibleMaxNumber);
		if(btrue)
		{
			if(bExist)
			{
				g_pRotateThread->fn_GetHearthMonitorType(i,m_OAHearth[i].iHearthMonitorType);//2008.07.15

				if(0==m_OAHearth[i].iCrucibleMaxNumber)
				{
					m_OAHearth[i].iHearthType = _OA_HEARTH_TYPE_ANNULAR;
					g_pRotateThread->fn_GetHearthSpeed(i,m_OAHearth[i].iSpeed);
				}
				else
				{
					m_OAHearth[i].iHearthType = _OA_HEARTH_TYPE_CRUCIBLE;
					g_pRotateThread->fn_GetHearthCrucibleNumber(i,m_OAHearth[i].iCurCrucible );	
				}

				g_pRotateThread->fn_GetHearthCoderValue(i,m_OAHearth[i].iCoderVal);
						
				//����һ�θ���
				//if( m_OAHearth[i].iHearthType != _OA_HEARTH_TYPE_ANNULAR )
				{
					_stDrawHearth* pDrawCrucible = ::GetOneDrawCrucibleBuffer(i);
					pDrawCrucible->iCrucibleMaxNumber = m_OAHearth[i].iCrucibleMaxNumber ;
					pDrawCrucible->iCoderVal = m_OAHearth[i].iCoderVal;
					g_pDrawCrucibleThread->PostThreadMessage(WM_ITEM3_DRAW_CRUCIBLE,0,LPARAM(pDrawCrucible));
				}
			}
			else
			{
				m_OAHearth[i].iHearthType = _OA_HEARTH_TYPE_NONE;
				m_OAHearth[i].iCrucibleMaxNumber = 0;
				g_pRotateThread->fn_GetHearthSpeed(i,m_OAHearth[i].iSpeed);
			}
		}
		else
		{
			AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR].IsEmpty()?
				TEXT("����plcͨ�Ų�������\r\n plc communication failed."):g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);
			break;
		}
		
		//��ʼ��һ��
		//g_pRotateThread->fn_InitHearthToPT(i); //2007.12.29, ��������������.
	}

	m_bInit = btrue;

	//ASSERT(m_bInit  &&  _OA_HEARTH_TYPE_NONE != m_OAHearth[0].iHearthType);
	//TESTing debugging 2007.12.20
	/*
	m_OAHearth[0].iHearthType = _OA_HEARTH_TYPE_ANNULAR;
	m_OAHearth[0].iCrucibleMaxNumber = 0;
	m_OAHearth[0].iCoderVal = 180;
	m_OAHearth[0].iSpeed = 123;
	m_OAHearth[1].iHearthType = _OA_HEARTH_TYPE_CRUCIBLE;//_OA_HEARTH_TYPE_ANNULAR;
	m_OAHearth[1].iCrucibleMaxNumber = 12;
	m_OAHearth[1].iCoderVal = 270;
	m_OAHearth[1].iCurCrucible = 10;
	m_bInit = true;
	SetDlgItemText(IDC_CHECK_START_FROM_SELECTION,TEXT("������԰棬����ʹ�ã�"));
	//g_bTestHearthRotation = true;
	*/
	//Test End
	
	
	if(m_bInit)
	{
		fn_AfterInitHearth();
	
	//not-debug 2007.12.18
	//	AfxMessageBox(TEXT("Debug: fn_AfterInitHearth\r\nInitiate Success!"));
	}
	else
	{
		//AfxMessageBox(TEXT("Debug: Initiate Error! m_bInit==false!"),MB_OK|MB_ICONSTOP);
	}

	fn_ShowWindow();
}

void COARotatorView::OnDraw(CDC* pDC)
{
	//pDC->TextOut(100,100,CString(TEXT("xxx")));
	if(m_bInit)
	{	
		for(int i=0;i<_OPTORUN_HEARTH_MAX;i++)
		{
			if( m_OAHearth[i].iHearthType != _OA_HEARTH_TYPE_NONE )
			{
				_stDrawHearth* pDrawCrucible = ::GetOneDrawCrucibleBuffer(i);
				pDrawCrucible->iCrucibleMaxNumber = m_OAHearth[i].iCrucibleMaxNumber ;
				pDrawCrucible->iCoderVal = m_OAHearth[i].iCoderVal;
				pDrawCrucible->iCurCurcible = m_OAHearth[i].iCurCrucible;
				g_pDrawCrucibleThread->PostThreadMessage(WM_ITEM3_DRAW_CRUCIBLE,0,LPARAM(pDrawCrucible));
			}
		}	
	}
}

//������ת
void COARotatorView::OnBnClickedButtonTest()
{
	// TODO: Add your control notification handler code here
	static bool timerBegin = false;
	if(!timerBegin)
	{
		SetTimer(1,500,NULL);
		timerBegin = true;
	}
//	OnBnClickedBtnInitHearth();
}

void COARotatorView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/*
	static int iCoderVal = 0;
	if(1==nIDEvent)
	{
		iCoderVal +=1;
		iCoderVal = iCoderVal%360;
		for(int i=0;i<_OPTORUN_HEARTH_MAX;i++)
		{			
				_stDrawHearth* pDrawCrucible = ::GetOneDrawCrucibleBuffer(i);
				pDrawCrucible->iCrucibleMaxNumber =12 ;
				pDrawCrucible->iCoderVal = iCoderVal;
				g_pDrawCrucibleThread->PostThreadMessage(WM_ITEM3_DRAW_CRUCIBLE,0,LPARAM(pDrawCrucible));			
		}		
	}
	*/

	//����һ�£��Ժ����.
	// OnBnClickedBtnInitHearth();

	CFormView::OnTimer(nIDEvent);
}

BOOL COARotatorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return CFormView::OnEraseBkgnd(pDC);
}



/*
#define _OA_HEARTH_TYPE_NONE    0 //û������
#define _OA_HEARTH_TYPE_ANNULAR    1 //��������
#define _OA_HEARTH_TYPE_CRUCIBLE   2 //������
IDC_STATIC_CRUCIBLE1_EXIST_TEXT
IDC_STATIC_CODER1_TEXT
IDC_STATIC_CORDER1_VALUE
IDC_STATIC_CRUCIBLE1_POS_TEXT
IDC_STATIC_CRUCIBLE1_POS_NUMBER
*/
// ���ڳ�ʼ��Hearth�ɹ��󣬵���һ�Ρ�������ʾHearth�ĳ�ʼ״̬�����磬�Ƿ���ʾ���Լ�ʲô����.
void COARotatorView::fn_AfterInitHearth(void)
{

	ASSERT(m_bInit);
	GetDlgItem(IDC_BTN_INIT_HEARTH)->ShowWindow(SW_HIDE); //���س�ʼ����ť
	GetDlgItem(IDC_STATIC_TEST_TIME)->ShowWindow(SW_SHOW);
	//Hearth 1
	//IDC_COMBO_HEARTH_1  ANNULAR
	//IDC_COMBO_HEARTH_12 ������
	if( _OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[0].iHearthType )
	{
		CString str;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HEARTH_12);
		pComboBox->ShowWindow(SW_SHOW);
		for(int i=0;i<m_OAHearth[0].iCrucibleMaxNumber;i++)
		{
			str.Format(_T("%d"),i+1);
			pComboBox->AddString(str);
		}
		pComboBox->SetCurSel(0);

		GetDlgItem(IDC_BTN_INSERT_HEARTH1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_CREATE_HEARTH1)->ShowWindow(SW_SHOW);
	}
	else if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[0].iHearthType)
	{
		CString str;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HEARTH_1);
		pComboBox->ShowWindow(SW_SHOW);
		for(int i=900;i>=50;i-=50)
		{//���������£���дת���ٶ�. 
			str.Format(_T("%d"),i);
			pComboBox->AddString(str);
		}
		pComboBox->SetCurSel(0);
		GetDlgItem(IDC_BTN_INSERT_HEARTH1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_CREATE_HEARTH1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_HEARTH1_DIRECTION)->ShowWindow(SW_SHOW);
		CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_HEARTH1_DIRECTION);
		btn->SetCheck(gb_AnnularCW[0]==true?BST_CHECKED:BST_UNCHECKED);

		GetDlgItem(IDC_STATIC_MONITOR_ALERT)->ShowWindow(m_OAHearth[0].iHearthMonitorType==_HEARTH_MONITOR_TYPE_OMRON ?SW_SHOW:SW_HIDE );
	}
	else
	{ //no hearth
		//GetDlgItem(IDC_STATIC_HEARTH1_TEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_HEARTH_1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_INSERT_HEARTH1)->ShowWindow(SW_HIDE);
	}

	//Hearth2
	//IDC_COMBO_HEARTH_2  ANNULAR
	//IDC_COMBO_HEARTH_22 ������
	if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[1].iHearthType)
	{
		CString str;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HEARTH_22);
		pComboBox->ShowWindow(SW_SHOW);
		for(int i=0;i<m_OAHearth[1].iCrucibleMaxNumber;i++)
		{
			str.Format(_T("%d"),i+1);
			pComboBox->AddString(str);
		}	
		pComboBox->SetCurSel(0);
		GetDlgItem(IDC_BTN_INSERT_HEARTH2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_CREATE_HEARTH2)->ShowWindow(SW_SHOW);
	}
	else if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[1].iHearthType)
	{
		CString str;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HEARTH_2);
		pComboBox->ShowWindow(SW_SHOW);
		for(int i=900;i>=50;i-=50)
		{//���������£���дת���ٶ�. 
			str.Format(_T("%d"),i);
			pComboBox->AddString(str);
		}
		pComboBox->SetCurSel(0);
		GetDlgItem(IDC_BTN_INSERT_HEARTH2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_CREATE_HEARTH2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_HEARTH2_DIRECTION)->ShowWindow(SW_SHOW);
		CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_HEARTH2_DIRECTION);
		btn->SetCheck(gb_AnnularCW[1]==true?BST_CHECKED:BST_UNCHECKED);
		GetDlgItem(IDC_STATIC_MONITOR_ALERT)->ShowWindow(m_OAHearth[1].iHearthMonitorType==_HEARTH_MONITOR_TYPE_OMRON ?SW_SHOW:SW_HIDE );
	}
	else
	{ //no hearth
		//GetDlgItem(IDC_STATIC_HEARTH2_TEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_HEARTH_2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_INSERT_HEARTH2)->ShowWindow(SW_HIDE);
	}
}


void COARotatorView::OnBnClickedBtnInsertHearth1()
{
	// TODO: Add your control notification handler code here
	ASSERT(m_bInit  &&  _OA_HEARTH_TYPE_NONE != m_OAHearth[0].iHearthType);
	CString str;
	
	bool bPassCheck = false;
	if( _OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[0].iHearthType)
	{//������ ָ��Ŀ������λ��		
		GetDlgItemText(IDC_COMBO_HEARTH_12,str);
		int iCrucible = _ttoi(str);
		if( iCrucible>0 && iCrucible<= m_OAHearth[0].iCrucibleMaxNumber)
		{ // [1,iCrucibleMaxNumber]
			bPassCheck = true;
		}
	}
	else if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[0].iHearthType)
	{//��������ָ�� ת���ٶ� (0~999)
		GetDlgItemText(IDC_COMBO_HEARTH_1,str);
		int iCoderVal = _ttoi(str);
		if( iCoderVal>0 && iCoderVal<= 999)
		{ //
			bPassCheck = true;
		}
	}

	if(bPassCheck)
	{
		int iItems;
		POSITION pos = m_ListctrlCrucible.GetFirstSelectedItemPosition();
		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			iItems = m_ListctrlCrucible.GetNextSelectedItem(pos);
		}
		else
		{//û��ѡ�������뵽���һ��
			iItems = m_ListctrlCrucible.GetItemCount();
		}
		m_ListctrlCrucible.InsertItem(iItems,TEXT("1"));
		m_ListctrlCrucible.SetItemText(iItems,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,str);
	}
}

void COARotatorView::OnBnClickedBtnInsertHearth2()
{
	// TODO: Add your control notification handler code here
	ASSERT(m_bInit  &&  _OA_HEARTH_TYPE_NONE != m_OAHearth[1].iHearthType);
	CString str;
	
	bool bPassCheck = false;
	if( _OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[1].iHearthType)
	{//������ ָ��Ŀ������λ��		
		GetDlgItemText(IDC_COMBO_HEARTH_22,str);
		int iCrucible = _ttoi(str);
		if( iCrucible>0 && iCrucible<= m_OAHearth[1].iCrucibleMaxNumber)
		{ // [1,iCrucibleMaxNumber]
			bPassCheck = true;
		}
	}
	else if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[1].iHearthType)
	{//��������ָ�� ת���ٶ�
		GetDlgItemText(IDC_COMBO_HEARTH_2,str);
		int iCoderVal = _ttoi(str);
		if( iCoderVal>=0 && iCoderVal< 1000)
		{ //[0,360)
			bPassCheck = true;
		}
	}

	if(bPassCheck)
	{
		int iItems;
		POSITION pos = m_ListctrlCrucible.GetFirstSelectedItemPosition();
		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			iItems = m_ListctrlCrucible.GetNextSelectedItem(pos);
		}
		else
		{//û��ѡ�������뵽���һ��
			iItems = m_ListctrlCrucible.GetItemCount();
		}
		m_ListctrlCrucible.InsertItem(iItems,TEXT("2"));
		m_ListctrlCrucible.SetItemText(iItems,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,str);
	}
}

//ɾ����ǰѡ��ĵ�һ��
void COARotatorView::OnBnClickedBtnDeleteHearthRow()
{
		POSITION pos = m_ListctrlCrucible.GetFirstSelectedItemPosition();
		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			int iItems = m_ListctrlCrucible.GetNextSelectedItem(pos);
			m_ListctrlCrucible.DeleteItem(iItems);

			if( iItems < m_ListctrlCrucible.GetItemCount())
			{
				m_ListctrlCrucible.SetItemState(iItems,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
				m_ListctrlCrucible.EnsureVisible(iItems,false);
			}
		}
}

void COARotatorView::OnBnClickedBtnSaveHearthList()
{
	//TEST Start
/*
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = TEXT("print");
	ShExecInfo.lpFile = TEXT("c:\\ipwuxian_1.pdf");		
	ShExecInfo.lpParameters = TEXT("");	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
*/
//	fn_AutoSaveHearthList();
	//TEST End

	// TODO: Add your control notification handler code here
//	int iLength;	
	TCHAR  lptchTmp[512];  //��������
	TCHAR cbuf[512];
	DWORD bufsize = 512;
	
	int iCnt = m_ListctrlCrucible.GetItemCount();
	if(iCnt==0)
	{	
		SYSTEMTIME   st;   
		CString str;
		GetLocalTime(&st);   
		str.Format(TEXT("Error: no data! \r\n%4d-%02d-%02d %02d:%02d:%02d\r\n"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		AfxMessageBox(str);
		return ; 
	}

	::GetModuleFileName(NULL,lptchTmp,512);
	CString str(lptchTmp);
	int ibackslash = str.ReverseFind(TEXT('\\'));
	lptchTmp[ibackslash + 1]= TEXT('\0');

	bufsize = 512-2;
	::GetComputerName(cbuf,&bufsize);

	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	str.Format(TEXT("-OA-HEARTH-%4d%02d%02d %02d%02d%02d.log"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	_tcscat(cbuf,str);
	_tcscat(lptchTmp,cbuf);
		CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrTitle = _T("Save Hearth Rotation Data into File");
	dlg.m_ofn.lpstrFilter = _T("Optorun Assitant log(*.log)\0*.log\0");
	dlg.m_ofn.lpstrInitialDir = lptchTmp; 
	dlg.m_ofn.lpstrFile = lptchTmp;

	if(dlg.DoModal()==IDOK)
	{
		str = dlg.GetPathName();
		if(-1==str.Find(_T(".log")))
		{str +=_T(".log");}

		fn_SaveHearthList(str);
/*		
			CFile fFile;
			if(!fFile.Open(str,CFile::modeCreate|CFile::modeWrite))
			{
				return ;
			}

			CString str;

			fFile.SeekToBegin();
			//Save unicode file head
#ifdef UNICODE
			lptchTmp[0] = 0xFEFF; //FF�ڵ� FE�ڸ�
			fFile.Write(lptchTmp,1*sizeof(TCHAR)); 
#endif

			TCHAR* p = TEXT("OPTORUN ASSITANT TEST FILE\x0d\x0a");
			fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

			p = TEXT("FILE TYPE: HEARTH ROTATION\x0d\x0a");
			fFile.Write(p,_tcslen(p)*sizeof(TCHAR));


			int iLength;
			iLength = _stprintf(lptchTmp,TEXT("PROGRAM VERSION: %.3f\x0d\x0a"),g_dProgramVersion);
			if(iLength)
				fFile.Write( lptchTmp,iLength * sizeof(TCHAR) );

			MultiByteToWideChar(CP_UTF8,0,__DATE__,strlen(__DATE__) + 1,cbuf,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
			iLength = _stprintf(lptchTmp,TEXT("PROGRAM DATE: %s\x0d\x0a"),cbuf);
			if(iLength)
				fFile.Write( lptchTmp,iLength * sizeof(TCHAR) );

			iLength = _stprintf(lptchTmp,TEXT("PROGRAM DATA VERSION: %.3f\x0d\x0a"),g_dDataVersion);
			if(iLength)
				fFile.Write(lptchTmp, iLength * sizeof(TCHAR));

			//�ļ�����ʱ��
			DWORD bufsize = 256;
			::GetComputerName(cbuf,&bufsize);
			iLength = wsprintf(lptchTmp,TEXT("%s%s\x0d\x0a"),TEXT("Computer Name: "),cbuf);
			fFile.Write(lptchTmp,iLength*sizeof(TCHAR));

			SYSTEMTIME   st;   
			GetLocalTime(&st); 
			str.Format(TEXT("Save Time: %4d-%02d-%02d %02d:%02d:%02d\x0d\x0a"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			fFile.Write(str,sizeof(TCHAR) * str.GetLength()); 

			//��������
			iLength = _stprintf(lptchTmp,TEXT("Hearth 1 TYPE: %s\x0d\x0a"),_OA_HEARTH_TYPE_NONE == m_OAHearth[0].iHearthType ? TEXT("No Hearth") :(_OA_HEARTH_TYPE_ANNULAR==m_OAHearth[0].iHearthType?TEXT("Annular"):TEXT("Cup")) );
			if(iLength)
				fFile.Write(lptchTmp, iLength * sizeof(TCHAR));

			iLength = _stprintf(lptchTmp,TEXT("Hearth 2 TYPE: %s\x0d\x0a"),_OA_HEARTH_TYPE_NONE == m_OAHearth[1].iHearthType ? TEXT("No Hearth") :(_OA_HEARTH_TYPE_ANNULAR==m_OAHearth[1].iHearthType?TEXT("Annular"):TEXT("Cup")) );
			if(iLength)
				fFile.Write(lptchTmp, iLength * sizeof(TCHAR));

			//������ʼ��־
			p=TEXT("<DATA>\x0d\x0a");
			fFile.Write(p,_tcslen(p)*sizeof(TCHAR));


				HDITEM hdi;
				CHeaderCtrl* pmyHeaderCtrl = m_ListctrlCrucible.GetHeaderCtrl();

				//�б�ͷ
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

				//�б�����
				for(int i=0;i<iCnt;i++)
				{
					iLength = _stprintf(lptchTmp,TEXT("%s\t%s\t%s\t%s\t%s\t%s\r\n"),m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_NO),
						m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE),
						m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_START),
						m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_END),
						m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME),
						m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_RESULT));
					fFile.Write(lptchTmp,iLength*sizeof(TCHAR));
				}

			p=TEXT("</DATA>\x0d\x0a");
			fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
			*/
	}
}

void COARotatorView::OnBnClickedBtnStartHearthTest()
{
	// >> ��ȡ��ǰת���б���ʼ��. �Լ���ת��λ��
	int iItem = 0;
	if( BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_START_FROM_SELECTION ))->GetCheck())
	{
		POSITION pos = m_ListctrlCrucible.GetFirstSelectedItemPosition();
		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			iItem = m_ListctrlCrucible.GetNextSelectedItem(pos);
		}
	}

	//ȷ�ϵ�ǰ��С��������
	if(iItem >= m_ListctrlCrucible.GetItemCount() )
	{//
		return;
	}

	//��ǰ�Ƿ��л�������,�л��������Ļ�,Control ģʽ�Ƿ�Ϊ Rotate.2008.07.15
	if(m_OAHearth[0].iHearthType==_OA_HEARTH_TYPE_ANNULAR || m_OAHearth[1].iHearthType==_OA_HEARTH_TYPE_ANNULAR)
	{
		bool bHearth1 = false;
		bool bHearth2 = false;
		CString str;
		for(int i=iItem;i<m_ListctrlCrucible.GetItemCount();i++)
		{
			str = m_ListctrlCrucible.GetItemText(i,0);
			if(str == TEXT("1"))
				bHearth1 = true;
			else if(str==TEXT("2"))
				bHearth2 = true;
		}

		int iCtrlState=0;
		bool bAlreadyWarning = false;//ȷ������������
		if( (bHearth1&&m_OAHearth[0].iHearthType==_OA_HEARTH_TYPE_ANNULAR) )
		{
			if( !g_pRotateThread->fn_GetAnnularHearthCntrl(0,iCtrlState))
			{//û��plcͨ�ţ�//����������
				//AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);
				AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR].IsEmpty()?TEXT("���󣺣��������ƴ��� PLC û��������Ӧ.����ֹͣ��\r\nError: plc communication failed."):g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);
				return;
			}
			if(iCtrlState==0) // Fixģʽ���򾯸棬���û�ѡ���Ƿ������Ĭ��Ϊ��
			{
				//����---��ʾ������������Controlģʽ����Rotate�������򽫲���ת��������ȷ��������
				if(IDNO==AfxMessageBox(g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_CONTROL_WARNING].IsEmpty()?TEXT("Hearth Control����Rotateģʽ��������ת����\r\nTOGGLE SWITCH->HEARTH CONTROL\r\n������"):g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_CONTROL_WARNING]
				,MB_ICONQUESTION | MB_DEFBUTTON2 | MB_YESNO))
				//if(IDNO==AfxMessageBox(g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_CONTROL_WARNING] ,MB_ICONQUESTION | MB_DEFBUTTON2 | MB_YESNO))
				{
					return;
				}
				bAlreadyWarning = true;
			}
		}
		if( bHearth2&&m_OAHearth[1].iHearthType==_OA_HEARTH_TYPE_ANNULAR &&!bAlreadyWarning)
		{
			if( !g_pRotateThread->fn_GetAnnularHearthCntrl(1,iCtrlState))
			{//û��plcͨ�ţ�//����������
				//AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);
				AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR].IsEmpty()?TEXT("���󣺣��������ƴ��� PLC û��������Ӧ.����ֹͣ��\r\nError: plc communication failed."):g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);
				return;
			}
			if(iCtrlState==0) // Fixģʽ���򾯸棬���û�ѡ���Ƿ������Ĭ��Ϊ��
			{
				if(IDNO==AfxMessageBox(g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_CONTROL_WARNING].IsEmpty()?TEXT("Hearth Control����Rotateģʽ��������ת����\r\nTOGGLE SWITCH->HEARTH CONTROL\r\n������"):g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_CONTROL_WARNING]
				,MB_ICONQUESTION | MB_DEFBUTTON2 | MB_YESNO))
				{
					return;
				}
			}
		}
	}


	CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_HEARTH1_DIRECTION);
	gb_AnnularCW[0] = (btn->GetCheck()==BST_CHECKED)?true:false;
	btn = (CButton*)GetDlgItem(IDC_CHECK_HEARTH2_DIRECTION);
	gb_AnnularCW[1] = (btn->GetCheck()==BST_CHECKED)?true:false;

	fn_StartHearthTest(iItem);
}

void COARotatorView::OnBnClickedBtnCancelHearthTest()
{
	// TODO: Add your control notification handler code here
	bool bTest;
	EnterCriticalSection(&gCriticalSection_HearthState);
	bTest = g_bTestHearthRotation;
	LeaveCriticalSection(&gCriticalSection_HearthState);

	if(bTest)
	{
		//if(IDYES == AfxMessageBox(TEXT("���ڲ�������ת����,�����Ҫ�˳�?"),MB_YESNO | MB_ICONQUESTION|MB_DEFBUTTON2))
		if(IDYES == AfxMessageBox(g_item3_String[_ITEM3_STR_HEARTH_TEST_EXIT_Q].IsEmpty()?TEXT("���ڲ�������ת����,�����Ҫȡ������?\r\nIt's testing Hearth rotation, Are you sure to cancel?"):g_item3_String[_ITEM3_STR_HEARTH_TEST_EXIT_Q]
			,MB_YESNO | MB_ICONQUESTION|MB_DEFBUTTON2))
		{
			EnterCriticalSection(&gCriticalSection_HearthState);
			g_bTestHearthRotation = false;
			LeaveCriticalSection(&gCriticalSection_HearthState);

			if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[g_stHearthRotationState.iCurHearth].iHearthType)
			{//��������,ֹͣת��. ģ�ⰴһ��
				//gb_AnnularCW
				//g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.bAnnularPositive);
				g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,gb_AnnularCW[g_stHearthRotationState.iCurHearth]);
			}
			fn_ShowWindow();
		}
	}
}


LRESULT COARotatorView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_ITEM3_HEARTH_ROTATE_DATA:
		fn_OnHearthMonitorThread(wParam,lParam);
		break;
	case WM_ITEM3_DOME_ROTATE_DATA:
		fn_OnDomeMonitorThread(wParam,lParam);
		break;
	}
	return CFormView::WindowProc(message, wParam, lParam);
}

void COARotatorView::fn_OnDomeMonitorThread(WPARAM wParam, LPARAM lParam)
{
	_stDomeThreadData* pst = (_stDomeThreadData*)lParam;
	bool bStopByUser = false;

	//enPlcNoAnswer,enAckRight,enErrNotRotation,enErrSpeed,enErrHighState,enErrStop,enErrDoorSwitchOff,enErrSwitchNotManual}enStatus;
	static int iPlcErrCounts = 0;
	iPlcErrCounts = (iPlcErrCounts++)%4;
	static _stReadDomeThreadData staticDomeThreadData;

	int iFigureResult = 0;

	static bool st_bWaitingForStop = false; //���ڵȴ� ����̷߳����� STOP ��Ϣ,��������һ�Ρ�
	//��ɵ������ʱ��st_bWaitingForStop = true; �ȴ�����߳� STOP��Ϣ
	//enErrStart: st_bWaitingForStop = false;
	//enErrStop:  ������һ������ʱ�����˳���

#define _XYZ_DOME_MAX_ROTATION_TIME		20000 //��λms, 20s. û��Ȧ���仯����ȴ��������ʱ�䡣
	switch(pst->enStatus)
	{
	case _stDomeThreadData::enAckRight:
		if( st_bWaitingForStop )
			break;
		//�����쳣�������
		//1,��ʱ�� ת�� ���仯
		//2,ת��֮������1������û�и���plc��¼���ٶȡ���
		//3,ת���仯���ʱ�� ����Ԥ��ֵ����plc���ܷ���©�ǣ�
//		m_staticexDomeThreadState.textColor(GetSysColor(COLOR_3DFACE));//����״̬
#ifdef _DEBUG
//		TRACE2("dwTickTime = %d, wRdData = %d\r\n",pst->dwTickTime,pst->wRdData);
		if(g_stReadDomeThreadData.size()>0 && g_stReadDomeThreadData.back().wDomeRotationCount != pst->wRdData)
		{
			TRACE2("dwTimeSpace = %.02f, RotationCnt = %d\r\n",(double)(pst->dwTickTime-g_stReadDomeThreadData.back().dwTickTime)/1000,
				pst->wRdData - g_stReadDomeThreadData.back().wDomeRotationCount);	
		}
#endif

		/*
PLC DA->Dome������->���
�����㷨��������:
1, �۳�ǰ30Ȧ����
2, �ټ�¼��speed*10��Ȧ������: speed = 50�����¼��500Ȧ
3, ʵ���ܼ�¼Ȧ����=  30 + speed*10.
4, ���ݷ���:
   a, ����Ԥ��ʱ�� t0 = 60/speed.
   b, �Լ�¼Ȧ����ʱ���������, �ҳ����� t0 ,2*t0, 3*t0, 4*t0һ����Χ�ڵ����ݸ���(n1,n2,n3,n4).(��ʱδ���� 4��t0,Ҳ�Ͳ����� 5��t0. ) 
   c, ƽ��ÿȦʱ�� = ��ʱ�� / ( n1 + 2*n2 + 3*n3 + 4*n4 )
   d, ���10Ȧʱ�� = xx.xx��, RPM
*/

		//��¼���ݿ�ʼ
		if( g_stReadDomeThreadData.size() == 0 )
		{//��һ��
			staticDomeThreadData.dwTickTime = pst->dwTickTime;
			staticDomeThreadData.wDomeRotationCount = pst->wRdData;
			g_stReadDomeThreadData.push_back(staticDomeThreadData);//���������ĩβ.

			_stprintf(gszRotatorBuf,TEXT("%d"),g_stReadDomeThreadData.back().wDomeRotationCount);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_START_CNT,gszRotatorBuf);
			break;
		}
		else
		{
			//��¼һ��ʱ��ֵ
			_stprintf(gszRotatorBuf,TEXT("%d"),(pst->dwTickTime - g_stReadDomeThreadData.front().dwTickTime)/1000);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_WAITING_TIME,gszRotatorBuf);
			
			if(g_stReadDomeThreadData.back().wDomeRotationCount == pst->wRdData)
			{//Ȧ��û�з����仯
				//�����涨ʱ�䣬û�з���Ȧ���仯��������
				if((DWORD)( pst->dwTickTime-g_stReadDomeThreadData.back().dwTickTime) > _XYZ_DOME_MAX_ROTATION_TIME )
				{
					m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
					m_staticexDomeThreadState.SetWindowText(TEXT("Error: cannot detect dome rotation!\r\n���󣺳����ⲻ��Ȧ���仯����ȷ������������PLC����."));
				}
				break;
			}
			else
			{
				//��¼
				staticDomeThreadData.dwTickTime = pst->dwTickTime;
				staticDomeThreadData.wDomeRotationCount = pst->wRdData;
				g_stReadDomeThreadData.push_back(staticDomeThreadData);//���������ĩβ.
			}
		}
		//��¼���ݽ���-----------------------------


		//��ʾ���ж�����-----------------------------------
		if( g_stReadDomeThreadData.size() > _ROTATE_DOME_NOT_COUNT )
		{
			DWORD dwTickInterval;
			int iCounted = 0;
			if( 1 == (g_stReadDomeThreadData.back().wDomeRotationCount - g_stReadDomeThreadData[g_stReadDomeThreadData.size()-2].wDomeRotationCount) )
			{
				dwTickInterval = g_stReadDomeThreadData.back().dwTickTime - g_stReadDomeThreadData[g_stReadDomeThreadData.size()-2].dwTickTime;
				for(int i=0 ; i<_ROTATE_DOME_ABNOMAL_TIMES ; i++)
				{
					if( dwTickInterval >= g_stDomeRotationState.dwLimit[i][0] && dwTickInterval <= g_stDomeRotationState.dwLimit[i][1])
					{
						g_stDomeRotationState.n[i]++;
						g_stDomeRotationState.dwTotalCountTime += dwTickInterval;	
						iCounted = 1;
						break;
					}
				}
			}
			//�쳣��ֵ
			if( 0 == iCounted )
			{
				g_stDomeRotationState.nAbnomalCount++;
			}
#ifdef _DEBUG
			_stprintf(gszRotatorBuf,TEXT("1:%d, 2:%d, 3:%d, 4:%d, Abn:%d\r\n"),g_stDomeRotationState.n[0],
				g_stDomeRotationState.n[1],g_stDomeRotationState.n[2],g_stDomeRotationState.n[3],
				g_stDomeRotationState.nAbnomalCount
				);
			TRACE(gszRotatorBuf);
#endif //_DEBUG

			int n=0;
			for(int i=0;i<_ROTATE_DOME_ABNOMAL_TIMES;i++)
			{
				n += (i+1)*g_stDomeRotationState.n[i];
			}

			if( n!=0 )
			{
				double dT0 =(double) g_stDomeRotationState.dwTotalCountTime/n; //����ֵ
				//dT0 /= 1000.0;

				//����ʾ��ǰ������
				if(g_stDomeRotationState.nAbnomalCount < 5)
				{
				_stprintf(gszRotatorBuf,TEXT("%d: AveSecPR= %.3f, EstimatedSecPR= %.3f"),g_stReadDomeThreadData.size(), 
						((double)((g_stReadDomeThreadData.back().dwTickTime - g_stReadDomeThreadData[2].dwTickTime)/(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData[2].wDomeRotationCount)) )/1000.0,
						dT0/1000.0
						);	
				m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
				m_staticexDomeThreadState.textColor( RGB(0,192,0));
				}
				else
				{//�쳣�෢������������.
					_stprintf(gszRotatorBuf,TEXT("%d: AveSecPR= %.3f, EstimatedSecPR= %.3f,  Abnormal:%d"),g_stReadDomeThreadData.size(), 
						((double)((g_stReadDomeThreadData.back().dwTickTime - g_stReadDomeThreadData[2].dwTickTime)/(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData[2].wDomeRotationCount)) )/1000.0,
						dT0/1000.0,g_stDomeRotationState.nAbnomalCount
						);	
					m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
					m_staticexDomeThreadState.textColor( RGB(0,192,0));
					m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
				}

				int iRealCount = 0;
				for(int i=0 ; i<_ROTATE_DOME_ABNOMAL_TIMES ; i++)
				{//���쳣��ʱ���ۼƽ���
					iRealCount += (i+1)*g_stDomeRotationState.n[i];
				}

				//�Ѵﵽ��Ч����ֵ.�����˳��ˣ�g_stReadDomeThreadData.size()-->iRealCount
				if( ( iRealCount - g_stDomeRotationState.nAbnomalCount) > (_ROTATE_DOME_NOT_COUNT + g_stDomeRotationState.iTargetSpeed*10 ) )
				{
					//i,����ͣ
					EnterCriticalSection(&gCriticalSection_DomeState);
					g_bTestDomeRotation = false; ////�ȴ�����̷߳��� stop��Ϣ.
					LeaveCriticalSection(&gCriticalSection_DomeState);
					st_bWaitingForStop  = true;

					//ii,��������������䵱ǰ��
					_stprintf(gszRotatorBuf,TEXT("%d"),g_stReadDomeThreadData.back().wDomeRotationCount);
					m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_END_CNT,gszRotatorBuf);
					_stprintf(gszRotatorBuf,TEXT("%d"),(GetTickCount() - g_stDomeRotationState.dwStartTime)/1000);
					m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_TEST_TIME,gszRotatorBuf);
					_stprintf(gszRotatorBuf,TEXT("%.3f"),dT0/1000);
					m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION,gszRotatorBuf);

					_stprintf(gszRotatorBuf,TEXT("%.1f"),60.0 / (dT0/1000) );
					m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE,gszRotatorBuf);

					m_staticexDomeThreadState.bkColor(RGB(0,64,0)); //���ʱ��д������ɫ
				}
				/*
				//��Ч����û�ﵽ����Ҳ����100��. ���������10�ε�ƽ��ֵ���Ƿ��ȶ��� 0.0005֮��
				else if((g_stReadDomeThreadData.size() - g_stDomeRotationState.nAbnomalCount)>(_ROTATE_DOME_NOT_COUNT+100))
				{

				}
				*/
				break;
			}

		} //if( g_stReadDomeThreadData.size() > _ROTATE_DOME_NOT_COUNT )
		
		else if(g_stReadDomeThreadData.size() > 3 )
		{
			_stprintf(gszRotatorBuf,TEXT("%d: AveSecPR= %.3f"),g_stReadDomeThreadData.size(), 
				((double)((g_stReadDomeThreadData.back().dwTickTime - g_stReadDomeThreadData[2].dwTickTime)/(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData[2].wDomeRotationCount)) )/1000.0);	
			m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
			m_staticexDomeThreadState.textColor( RGB(0,128,0));
		}
		
		else
		{
			_stprintf(gszRotatorBuf,TEXT("%d: Waiting..."),g_stReadDomeThreadData.size());
			m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
			m_staticexDomeThreadState.textColor( RGB(0,128,0));
			m_staticexDomeThreadState.bkColor(GetSysColor(COLOR_3DFACE));//�ָ�����������ɫ
		}
		//��ʾ���ж����ݽ���-----------------------------------/

		/*

		if( g_stReadDomeThreadData.size() > (_ROTATE_DOME_NOT_COUNT + g_stDomeRotationState.iTargetSpeed * 10) )
		{//����Ԥ����������ֹͣת������������
			//i,����ͣ
			EnterCriticalSection(&gCriticalSection_DomeState);
				g_bTestDomeRotation = false; ////�ȴ�����̷߳��� stop��Ϣ.
			LeaveCriticalSection(&gCriticalSection_DomeState);
			st_bWaitingForStop  = true;
			
			
			double dT0 = 0.001; ////ƽ��ÿȦ��ʱ�䣬��λ s
			int n1,n2,n3,n4; //1��,2��,3��,4����׼ʱ��ĸ���.
			fn_CalcDomeRotationData(dT0,n1,n2,n3,n4,(double)(60/g_stDomeRotationState.iTargetSpeed));

			//ii,��������������䵱ǰ��
			_stprintf(gszRotatorBuf,TEXT("%d"),g_stReadDomeThreadData.back().wDomeRotationCount);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_END_CNT,gszRotatorBuf);
			_stprintf(gszRotatorBuf,TEXT("%d"),(GetTickCount() - g_stDomeRotationState.dwStartTime)/1000);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_TEST_TIME,gszRotatorBuf);
			_stprintf(gszRotatorBuf,TEXT("%.3f"),dT0/1000);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION,gszRotatorBuf);

			_stprintf(gszRotatorBuf,TEXT("%.1f"),60.0 / (dT0/1000) );
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE,gszRotatorBuf);
		}
		*/

		/*

		//ÿ�����·���
		//�����ǰ����, 2 �����ϼ�¼���ݴ������. ����������ֹͣ
		iFigureResult = fn_FigureDomeThreadData(  g_stDomeRotationState.dAveSecPR, g_stDomeRotationState.dFiguredSecPR) ;

		if(	100 ==  iFigureResult || 50==iFigureResult)
		{
			//i,����ͣ			
			EnterCriticalSection(&gCriticalSection_DomeState);
				g_bTestDomeRotation = false; ////�ȴ�����̷߳��� stop��Ϣ.
			LeaveCriticalSection(&gCriticalSection_DomeState);

			//ii,��������������䵱ǰ��
			_stprintf(gszRotatorBuf,TEXT("%d"),g_stReadDomeThreadData.back().wDomeRotationCount);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_END_CNT,gszRotatorBuf);
			_stprintf(gszRotatorBuf,TEXT("%d"),(GetTickCount() - g_stDomeRotationState.dwStartTime)/1000);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_TEST_TIME,gszRotatorBuf);
			_stprintf(gszRotatorBuf,TEXT("%.02f"),g_stDomeRotationState.dFiguredSecPR);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION,gszRotatorBuf);

			_stprintf(gszRotatorBuf,TEXT("%.01f"),60.0 / g_stDomeRotationState.dFiguredSecPR );
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE,gszRotatorBuf);
			//˳���������β���.

			st_bWaitingForStop  = true;
		}
		else
		{
			//ֻ��һ�����ݣ���д��ʼ������ֵ
			if(g_stReadDomeThreadData.size()==1)
			{
				_stprintf(gszRotatorBuf,TEXT("%d"),g_stReadDomeThreadData.front().wDomeRotationCount);
			}

			_stprintf(gszRotatorBuf,TEXT("%d"),(g_stReadDomeThreadData.back().dwTickTime - g_stReadDomeThreadData.front().dwTickTime)/1000);
			m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_WAITING_TIME,gszRotatorBuf);			
			
			//0:û�п�ʼԤ��
			//1:��ƽ��ֵ������
			//2:ƽ��ֵ��Ԥ��ֵ�������ã�����ʱ�в�����ֹͣ����
			
			if( 2 == iFigureResult)
			{
				_stprintf(gszRotatorBuf,TEXT("%03d: Average SecPR = %.3f , estimated SecPR = %.3f"),g_stReadDomeThreadData.size(), g_stDomeRotationState.dAveSecPR,g_stDomeRotationState.dFiguredSecPR);
				m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
				m_staticexDomeThreadState.textColor( RGB(0,128,0));	
				m_staticexDomeThreadState.bkColor(GetSysColor(COLOR_3DFACE));
			}
			else if( 1 == iFigureResult)
			{
				_stprintf(gszRotatorBuf,TEXT("%03d: Average SecPR = %.3f "),g_stReadDomeThreadData.size(), g_stDomeRotationState.dAveSecPR);
				m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
				m_staticexDomeThreadState.textColor( RGB(0,128,0));	
				m_staticexDomeThreadState.bkColor(GetSysColor(COLOR_3DFACE));
			}
			else //0==iFigureResult
			{
				_stprintf(gszRotatorBuf,TEXT("Waiting: %03d"),g_stReadDomeThreadData.size());
				m_staticexDomeThreadState.SetWindowText(gszRotatorBuf);
				m_staticexDomeThreadState.textColor( RGB(0,128,0));	
				m_staticexDomeThreadState.bkColor(GetSysColor(COLOR_3DFACE));
			}

		}
		*/
		break;
	case	_stDomeThreadData::enErrStart:
		TRACE1("START:Target Speed = %d\r\n",g_stDomeRotationState.iTargetSpeed);
		st_bWaitingForStop = false;
		g_stReadDomeThreadData.clear();//���

		//��ʼ��һ��g_stDomeRotationState����
		g_stDomeRotationState.dwEpsilon = 490; //490ms
		g_stDomeRotationState.dwTotalCountTime = 0;
		g_stDomeRotationState.nAbnomalCount = 0;
		g_stDomeRotationState.dwStandardValue =  60000/g_stDomeRotationState.iTargetSpeed;
		for(int i=0;i<_ROTATE_DOME_ABNOMAL_TIMES;i++)
		{
			g_stDomeRotationState.dwLimit[i][0] = (i+1)*g_stDomeRotationState.dwStandardValue - g_stDomeRotationState.dwEpsilon;
			g_stDomeRotationState.dwLimit[i][1] = (i+1)*g_stDomeRotationState.dwStandardValue + g_stDomeRotationState.dwEpsilon;
			g_stDomeRotationState.n[i] = 0;
		}

		m_staticexDomeThreadState.SetWindowText(TEXT("Starting..."));
		m_staticexDomeThreadState.bkColor(GetSysColor(COLOR_3DFACE));//�ָ�����������ɫ

		break;
	case _stDomeThreadData::enErrStop:
		g_stReadDomeThreadData.clear();//���
		EnterCriticalSection(&gCriticalSection_DomeState);
		bStopByUser  = g_bTestDomeStopByUser;
		LeaveCriticalSection(&gCriticalSection_DomeState);
		if(!bStopByUser)
		{
			//�����Ƿ���δ����У��еĻ���ʼ����
			if( g_stDomeRotationState.iCurListItem + 1 < m_ListctrlDome.GetItemCount() )
			{
				fn_StartDomeTest(g_stDomeRotationState.iCurListItem+1);
			}
			else
			{//ȫ����ɣ��Զ�����һ�ν��.
				fn_AutoSaveDomeList();
				fn_SaveRestoreDomeConfigure(false);//�ָ�Dome״̬.
			}
		}
		fn_ShowDomeWindow();
		break;
	case _stDomeThreadData::enPlcNoAnswer:
		g_stReadDomeThreadData.clear();//���
		m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
		//m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? GetSysColor(COLOR_3DHIGHLIGHT) :GetSysColor(COLOR_3DFACE));
		m_staticexDomeThreadState.SetWindowText(TEXT("Error: PLC Read Error!"));
		
		_stprintf(gszRotatorBuf,TEXT("%d"),(GetTickCount() - g_stDomeRotationState.dwStartTime)/1000);
		m_ListctrlDome.SetItemText(g_stDomeRotationState.iCurListItem,_ROTATOR_DOME_LIST_COL_WAITING_TIME,gszRotatorBuf);
		
		//m_staticexDomeThreadState.textColor(GetSysColor(COLOR_3DFACE));
		break;
	case _stDomeThreadData::enErrNotRotation:
		g_stReadDomeThreadData.clear();//���
		m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
		m_staticexDomeThreadState.SetWindowText(TEXT("Error: Dome is not Rotating!"));
		break;
	case _stDomeThreadData::enErrSpeed:
		g_stReadDomeThreadData.clear();//���
		m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
		m_staticexDomeThreadState.SetWindowText(TEXT("Error: Dome speed is outof range!"));
		break;
	case _stDomeThreadData::enErrHighState:
		g_stReadDomeThreadData.clear();//���
		m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
		m_staticexDomeThreadState.SetWindowText(TEXT("Error: Dome state must be <High>!"));
		break;
	case _stDomeThreadData::enErrDoorSwitchOff: //not rotation
		g_stReadDomeThreadData.clear();//���.
		m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
		m_staticexDomeThreadState.SetWindowText(TEXT("Error: Dome door switch is off!"));
		break;
	case _stDomeThreadData::enErrSwitchNotManual: //not rotation
		g_stReadDomeThreadData.clear();//���
		m_staticexDomeThreadState.bkColor( iPlcErrCounts==3 ? RGB(255,255,0):RGB(255,0,0));
		m_staticexDomeThreadState.SetWindowText(TEXT("Error: Dome Switch is not <MANUAL>!"));
		break;
	}
}

//�Ӽ���̷߳��������ݰ���lParam�������Ե�������
void COARotatorView::fn_OnHearthMonitorThread(WPARAM wParam, LPARAM lParam)
{
	_stHearthThreadData* pst = (_stHearthThreadData*)lParam;
	bool bTest;
	EnterCriticalSection(&gCriticalSection_HearthState);
	bTest = g_bTestHearthRotation;
	LeaveCriticalSection(&gCriticalSection_HearthState);

	int iTurnBit;//,iReversalBit;
	bool bReallyFinish;       //�����ɣ� //�������ǲ��Ե� ͣת������Ԥ�ڵı�������(��������). ����������������ת���ѵ���Ԥ�ڵı�������
	bool bRotateStopError;  //����ת��״̬����
	bool bRotatePosError; //ת������λ����

	static int iRotateErrorTimes = 0;
	_stDrawHearth* pDrawCrucible;
	CString str;

	switch(pst->enStatus)
	{
	case _stHearthThreadData::enAckRight:
		g_stHearthRotationState.iCurCoder = pst->wRdData[g_stHearthRotationState.iCurHearth==0?0:2]; //0:2��μ� plcrdwrthread.cpp���������
		g_stHearthRotationState.iCurCrucible = pst->wRdData[g_stHearthRotationState.iCurHearth==0?1:3];

		//������״̬, 2008.01.03
		if(g_stHearthRotationState.iLastCoder != g_stHearthRotationState.iCurCoder)
		{
			g_stHearthRotationState.iCoderNotChangeTime = 0; //���Բ���Ҫ��ֱ�Ӽ�¼��ǰʱ�̼���

			g_stHearthRotationState.dwCoderNotChangeStartTickTime = pst->dwTickTime; 
			
			if(g_stHearthRotationState.iCurCoder>g_stHearthRotationState.iLastCoder)
			{
				g_stHearthRotationState.iCoderChangeCounts++;
			}
			else
			{
				g_stHearthRotationState.iCoderChangeCounts--;
			}
			g_stHearthRotationState.bStop = false;
		}
		else
		{
			//g_stHearthRotationState.iCoderChangeCounts = 0;	
		}

		//������
		if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[g_stHearthRotationState.iCurHearth].iHearthType)
		{
			bReallyFinish = false;
			bRotateStopError = false;
			bRotatePosError = false;

			if(g_stHearthRotationState.iTargetPos == g_stHearthRotationState.iStartPos)
				bReallyFinish = true;

			//���������ű仯����Ϊ�����м��������ܻ����������ŵģ��������ս����ת������������2.(����ת����)
			if(g_stHearthRotationState.iLastCrucible != g_stHearthRotationState.iCurCrucible)
			{
				g_stHearthRotationState.iCrucibleChangeCounts++;
			}
			
			//��������ͣʱ�� (_ROTATE_CRUCIBLE_STOP_JUDGE_TIMEʱ���ڱ�����ֵû�б仯). 2008.01.03
			if(GetTickCount() - g_stHearthRotationState.dwCoderNotChangeStartTickTime >= _ROTATE_CRUCIBLE_STOP_JUDGE_TIME)
			{
				if( abs(g_stHearthRotationState.iTargetCoder - g_stHearthRotationState.iCurCoder) < 3  && g_stHearthRotationState.iTargetPos==g_stHearthRotationState.iCurCrucible)
				{//������ֵ����� 3�� ֮�ڣ�����Ŀ����������ָ����������ͬ��������ͣ�ˣ�
					bReallyFinish = true;
					g_stHearthRotationState.bStop = true;
				}
				else
				{//����ת������ �������������������ʱȡ����
					if(GetTickCount() - g_stHearthRotationState.dwCoderNotChangeStartTickTime >= _ROTATE_CRUCIBLE_STOP_JUDGE_TIME*4)
					{//�ӳ�ת������ʱ��,��Ϊ �е�20��������������.�����������!
						bRotateStopError = true;
					}
				}
			}

			/* 2008.01.03
			//�ж��Ƿ�ֹͣת��
			//Hearth1 ��ת��Q10.14 = 1 ,��ת: Q10.15=1;
			//Hearth2 ��ת��Q10.11 = 1 ,��ת: Q10.12=1; //��������Ч���������������,  ���ҿ��ܶԲ�ͬ���͵�����������ȷ//�ſ���� 
			iTurnBit = (g_stHearthRotationState.iCurHearth == 0 ? 14:11);
			iReversalBit = (g_stHearthRotationState.iCurHearth == 0 ? 15:12);
			if(  ((WORD) 1<<iTurnBit | (WORD)1<<iReversalBit ) & pst->wRdData[4] )
			{//ת���У������ж�
				g_stHearthRotationState.bStop = false;
				if( g_stHearthRotationState.iLastCoder == g_stHearthRotationState.iCurCoder)
				{//��ȣ� 
					iRotateErrorTimes++;
					if( iRotateErrorTimes > 100 )
					{
						bRotateStopError = true;   //ת���������ˣ� ��ʱ��û�� ������ֵ�仯 ��plcָʾ����ת����
						iRotateErrorTimes = 0; //������һ��0
					}
				}
				else
				{
					iRotateErrorTimes = 0;
				}
			}
			else
			{//û���жϵ�ת��
				iRotateErrorTimes = 0;
				if(g_stHearthRotationState.bStop)
				{
					//��������������ת״̬�����ұ��������С�� 3�ȣ� ������ͣ��
					//S561 һֱû����ͣ,�ܸ�
					if( abs(g_stHearthRotationState.iTargetCoder - g_stHearthRotationState.iCurCoder) < 3  && g_stHearthRotationState.iTargetPos==g_stHearthRotationState.iCurHearth )
					{
						bReallyFinish = true;
					}

					//ֹͣ״̬���� 4 ���ӣ�������ת�������ˣ�   ת������λ!
					if( pst->dwTickTime - g_stHearthRotationState.dwStopTime > 4000)
					{
						//������ΪDebugging,  ��ʱȡ�� ���� 2007.12.29
						//S561,  ���´����������,����ԭ�� ������ ������ȷ̽�⵽ �Ƿ�����ת״̬

					
						//ֹͣ״̬����4���ӣ����Ҿ��� Ŀ�� ������ֵ ���� 2 �ȣ����жϳ���
						//if( abs(g_stHearthRotationState.iTargetCoder - g_stHearthRotationState.iCurCoder) > 2 )
						{
						//	bRotatePosError = true;
						}
						

					}
				}
				else
				{//��һ�ν���˴�
					g_stHearthRotationState.dwStopTime = pst->dwTickTime;
					//��Ӧ�ڴ˴������Ƿ� ��ת ��ȷ�� �Ѿ�ֹͣ�� �� �Ƿ�ת�Ѿ��漰�� plc�ڲ��� ���ܲ���ͳһ�����Բ���ͳһ�жϡ�
				}
				g_stHearthRotationState.bStop = true;
			}
			*/

			if(bTest)
			{//����״̬
				if(bReallyFinish)
				{//�Ѿ�ֹͣ�����¼��ǰ���ķ�ʱ�䣬 ���ƶ�����һ�У� ���û����ȫ��ֹͣ.
					
					//��д��ʼ����λ��
					str.Format(TEXT("%2d"),g_stHearthRotationState.iStartPos );
					m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_START,str);

					//��д��ֹ����λ��
					str.Format(TEXT("%2d"),g_stHearthRotationState.iTargetPos );
					m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_END,str);

					//��д���վ���ʱ��
					//g_stHearthRotationState.dwCoderNotChangeStartTickTime
					//����iTurnBit
					iTurnBit = 0.5+ (double)(g_stHearthRotationState.dwCoderNotChangeStartTickTime - g_stHearthRotationState.dwStartTime)/1000;
					//str.Format(TEXT("%02d:%02d"),iTurnBit/60,iTurnBit%60);
					str.Format(TEXT("%3d s"),iTurnBit);
					m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME,str);

					/*
			if( g_stHearthRotationState.iTargetPos > g_stHearthRotationState.iStartPos)
			{
				if(g_stHearthRotationState.iTargetPos-g_stHearthRotationState.iStartPos
					>= m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
				{
					g_stHearthRotationState.bReversal = true;
				}
				else
				{
					g_stHearthRotationState.bReversal = false;
				}
			}
			else
			{
				if(g_stHearthRotationState.iTargetPos+m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber - g_stHearthRotationState.iStartPos
					> m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
				{
					g_stHearthRotationState.bReversal = true;
				}
				else
				{
					g_stHearthRotationState.bReversal = false;
				}
			}*/
					//g_stHearthRotationState.iCoderChangeCounts, + ��coder���, - ��coder��С.
					//g_stHearthRotationState.iCrucibleChangeCounts, Crucible�仯����.
					//���ɣ���ΪOTFCֻ����ת��û�з�ת����.�ڲ���¼��


					if(g_stHearthRotationState.iCoderChangeCounts > 0)
					{
						iTurnBit = (g_stHearthRotationState.iTargetPos + m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber - g_stHearthRotationState.iStartPos)%m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber;
					}
					else if(g_stHearthRotationState.iCrucibleChangeCounts==0)
					{
						iTurnBit = 0;
					}
					else
					{
						iTurnBit = -(g_stHearthRotationState.iCrucibleChangeCounts-2);
						//abs(g_stHearthRotationState.iTargetPos - g_stHearthRotationState.iStartPos);
						// 1-->17 ���Ա��� -16,hehe
					}

					/*
					//��дƽ��ת��1������λ��ʱ��.
					//����ʵ��ת������˼���Hearth, ����iTurnBit��
					//ѭ����С���
					iTurnBit = abs(g_stHearthRotationState.iTargetPos - g_stHearthRotationState.iStartPos);
					if(iTurnBit > m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
					{
						iTurnBit = m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber - iTurnBit;
					}			

					if( g_stHearthRotationState.iCrucibleChangeCounts - iTurnBit == 2) 
					{//��¼���������ű仯��Ŀ�� ѭ����С��� ��2 �����з�ת
						//���� 1�� bug�� ����û�з�ת���ܵģ� 20������ ��¼�� 11�Σ�����С�����9.���� bug����
						if(g_stHearthRotationState.iCoderChangeCounts < 0)
						{
							g_stHearthRotationState.enHaveReversalFunction = _stHearthRotationState::_enHearthReversalTrue;//�з�ת����.
							iTurnBit = -iTurnBit;
						}
						else
						{
							iTurnBit = abs(g_stHearthRotationState.iTargetPos - g_stHearthRotationState.iStartPos);
						}
					}
					else
					{//����û�з�ת��ת����ʵ�ʼ���� ��鵽��ת���仯��Ŀ.
						iTurnBit = g_stHearthRotationState.iCrucibleChangeCounts;
						if(g_stHearthRotationState.iCrucibleChangeCounts > m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber >>1)
						{//��¼����ת����Ŀ������������1�룬���ұ���û�з�ת
							g_stHearthRotationState.enHaveReversalFunction = _stHearthRotationState::_enHearthReversalFalse;//û�з�ת����.
						}
					}
					*/

					double dTemp;
					if(iTurnBit==0)
					{
						dTemp = 0;
					}
					else
					{
						dTemp = (double) (g_stHearthRotationState.dwCoderNotChangeStartTickTime - g_stHearthRotationState.dwStartTime)/abs(iTurnBit)/1000;
					}
					//str.Format(TEXT("%d, Ave= %02d:%02d"),iTurnBit,iTemp/60,iTemp%60);
					str.Format(TEXT("%d, Ave= %.1f s"),iTurnBit,(double)dTemp);
					m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_RESULT,str);

					/*
					iTurnBit = g_stHearthRotationState.iTargetPos - g_stHearthRotationState.iStartPos;
					if( g_stHearthRotationState.iTargetPos > g_stHearthRotationState.iStartPos)
					{
						if(g_stHearthRotationState.iTargetPos-g_stHearthRotationState.iStartPos
							>= m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
						{
							iTurnBit =  m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber-(g_stHearthRotationState.iTargetPos-g_stHearthRotationState.iStartPos);
						}
						else
						{
							iTurnBit = g_stHearthRotationState.iTargetPos-g_stHearthRotationState.iStartPos;
						}
					}
					else
					{ 

						if(g_stHearthRotationState.iStartPos-g_stHearthRotationState.iTargetPos
							>= m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
						{
							iTurnBit =  m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber-(g_stHearthRotationState.iStartPos-g_stHearthRotationState.iTargetPos);
						}
						else
						{
							iTurnBit = g_stHearthRotationState.iStartPos-g_stHearthRotationState.iTargetPos;
						}
					}					
					//debugging.
					str.Format(TEXT("%02d:%02d"),((g_stHearthRotationState.dwCoderNotChangeStartTickTime - g_stHearthRotationState.dwStartTime)/iTurnBit/1000)/60,
						((g_stHearthRotationState.dwCoderNotChangeStartTickTime - g_stHearthRotationState.dwStartTime)/iTurnBit/1000)%60);
					m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_RESULT,str);
					*/


					if( m_ListctrlCrucible.GetItemCount() > g_stHearthRotationState.iCurListItem + 1)
					{//������һ��
						g_stHearthRotationState.iCurListItem++;
						fn_StartHearthTest(g_stHearthRotationState.iCurListItem);
						

						//Debugging Test 2008.01.03
						//str.Format(TEXT("Start: List item %d"),g_stHearthRotationState.iCurListItem+1);
						//SetDlgItemText( IDC_STATIC_DEBUG_TEXT,str);
						//���
					}
					else
					{//�Ѿ������һ�У��򱨸���ɣ����˳�!
						EnterCriticalSection(&gCriticalSection_HearthState);
						g_bTestHearthRotation = false;
						LeaveCriticalSection(&gCriticalSection_HearthState);

						fn_AutoSaveHearthList();//Add, 2008.01.08
						//AfxMessageBox(TEXT("��ϲ������ת��������ɣ�"));					
						AfxMessageBox(g_item3_String[_ITEM3_STR_HEARTH_TEST_FINISH].IsEmpty()?TEXT("��ϲ������ת��������ɣ�\r\nCongradulation: Hearth rotation test finished! "):g_item3_String[_ITEM3_STR_HEARTH_TEST_FINISH]);
					}
				}
				else
				{
					if( bRotatePosError || bRotateStopError)
					{//ֹͣ����״̬. �����淢��ת������
						EnterCriticalSection(&gCriticalSection_HearthState);
						g_bTestHearthRotation = false;
						LeaveCriticalSection(&gCriticalSection_HearthState);

						if(bRotatePosError)
						{
							//AfxMessageBox(TEXT("���� ����ת������λ"));
							AfxMessageBox(g_item3_String[_ITEM3_STR_HAERTH_ROTAT_POS_ERR].IsEmpty()?TEXT("���� ����ת������λ��\r\nError: Hearth rotation error."):g_item3_String[_ITEM3_STR_HAERTH_ROTAT_POS_ERR]);
						}
						else
						{
							//AfxMessageBox(TEXT("���� ת����ʱ��"));
							AfxMessageBox(g_item3_String[_ITEM3_STR_HEARTH_ROTAT_OVER_TIME].IsEmpty()?TEXT("���� ����ת����ʱ����\r\nError: Hearth rotation overtime."):g_item3_String[_ITEM3_STR_HEARTH_ROTAT_OVER_TIME]);
						}
					}
					else
					{//������;��IDC_STATIC_TEST_TIME
						//����iTurnBit;						
						iTurnBit = 0.5+ (double)(pst->dwTickTime - g_stHearthRotationState.dwStartTime)/1000;
						if( pst->dwTickTime < g_stHearthRotationState.dwStartTime)
							iTurnBit = 0;
						str.Format(TEXT("%02d:%02d"),iTurnBit/60, iTurnBit%60);
						SetDlgItemText(IDC_STATIC_TEST_TIME,str);
					}
				}
			}
		}//������
		else
		{//��������
			//ASSERT(0==g_stHearthRotationState.iCurHearth);  //�ݲ�֧�� Hearth2�ǻ��������� 2007.12.27//ȡ��,2008.07.15
			//�˴��� cancel ����� �ᷢ�� bug����,������

			if(bTest)
			{ //����������������
				bReallyFinish = false;
				bRotateStopError = false;
				bRotatePosError = false;
				
				//3884.08 Hearth1 ,//Hearth2,2008.07.15
				if( ((WORD) 1<<8)  & (g_stHearthRotationState.iCurHearth==0?pst->wRdData[_HEARTH_READ_PLC_ADD_ROTATE_1_ANNULAR]:pst->wRdData[_HEARTH_READ_PLC_ADD_ROTATE_2_ANNULAR] ))
				{//ת����
					g_stHearthRotationState.bStop = false;
					if( g_stHearthRotationState.iLastCoder == g_stHearthRotationState.iCurCoder)
					{//��ȣ� 
						iRotateErrorTimes++;
						if( iRotateErrorTimes > 1000 ) //2009.12.14 100->1000 S702\S703�������٣�����ת���ر�����Լ����10��.
						{
							bRotateStopError  = true;   //ת���������ˣ� ��ʱ��û�� ������ֵ�仯 ��plcָʾ����ת����
							iRotateErrorTimes = 0;      //������һ��0
						}
					}
					else
					{//�����
						iRotateErrorTimes = 0;
					}
				}
				else
				{
					//�������������Թ��̱��봦��ת��״̬. ���򱨴�.  //�˴������� �ȴ�״̬�ͷǵȴ�״̬ͬ���ڲ���״̬.
					//������ڵȴ���һ�ο�ʼ������!
					iRotateErrorTimes = 0;
					if(g_stHearthRotationState.bStop)
					{//���� debugging . 2007.12.28
						//ֹͣ״̬���� 100 ���ӣ�������ת�������ˣ� ����.���� ʱ���ж�, �ܿ��ȴ�״̬������
						//2009.12.14�� 1000����
						if( pst->dwTickTime - g_stHearthRotationState.dwStopTime > 1000000)
						{
							bRotateStopError = true;
						}
					}
					else
					{//��һ�ν���˴�
						g_stHearthRotationState.dwStopTime = pst->dwTickTime;
					}
					g_stHearthRotationState.bStop = true;
				}

				static bool bOnceDifferent  = false;
				if( g_stHearthRotationState.bAnnularBeginingWaiting )
				{
					if( GetTickCount()-g_stHearthRotationState.dwStartingWaitingTick > g_stHearthRotationState.iAnnularStartWaitTime)
					{
						if( 0 == iRotateErrorTimes && false == g_stHearthRotationState.bStop)
						{
							g_stHearthRotationState.bAnnularBeginingWaiting = false;
							g_stHearthRotationState.iStartCoder = g_stHearthRotationState.iCurCoder;
							
							//2008.01.03, S561��תʱ��ʵ�ʱ�������С�ķ����ߣ����� ȡһ�η�
							//�Բ���180�Ƕ���˵�����һ��. ��ʱ�����趨 ���ԽǶ�.
							if(!g_stHearthRotationState.bAnnularPositive)
							{//��ת
								g_stHearthRotationState.iTargetCoder = (g_stHearthRotationState.iCurCoder + g_stHearthRotationState.iAnnularTestAngle)%360;
							}
							else
							{//��ת
								g_stHearthRotationState.iTargetCoder = ( 360 + g_stHearthRotationState.iCurCoder - g_stHearthRotationState.iAnnularTestAngle)%360;
							}
							//ÿ�ο�ʼ����ǰ, �� һ����ͬΪ false
							bOnceDifferent = false;

							
							//��¼��ʼ������ֵ
							str.Format(TEXT("% 3d"),g_stHearthRotationState.iStartCoder);
							m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem, _ROTATOR_CRUCIBLE_LIST_COL_START,str);

							g_stHearthRotationState.dwStartTime = pst->dwTickTime ; //���¼�¼��ʼʱ��

						}
					}
				}
				else if( g_stHearthRotationState.bAnnularIntervalWaiting )
				{
					if( GetTickCount() - g_stHearthRotationState.dwStartingWaitingTick > g_stHearthRotationState.iAnnularIntervalWaitTime )
					{
						g_stHearthRotationState.bAnnularIntervalWaiting = false;
						fn_StartHearthTest(g_stHearthRotationState.iCurListItem);
					}
				}
				else
				{//���� ���Թ���
					//360�Ȳ���,�Ƚ��鷳,��Ϊ�п��ܴ��ڶ�� iCurCoder ��  TargetCoder,���Ա����и�������¼ �� ���ٳ��ֹ�һ�β�ͬ����ܼ��������ж�.
					if(bOnceDifferent)
					{//���ٴ��� 1 �α�����ֵ��ͬ,�ſ��Լ�� �Ƿ� ���
						//g_stHearthRotationState.iCoderChangeCounts
						//if( g_stHearthRotationState.bAnnularPositive )
						if( g_stHearthRotationState.iCoderChangeCounts > 0) //
						{//������ֵ���������, ��ǰ������ֵ ���� Ŀ�������ֵ 3������, ����ֹͣ.
							if( (g_stHearthRotationState.iCurCoder + 360 - g_stHearthRotationState.iTargetCoder) % 360 < 4 )
							{
								bReallyFinish = true;
							}

							//2008.04.02�����.����Ӧ�ٶȵ�ʱ������ʱ�����û�б�Ҫ.
							if(  GetTickCount()-g_stHearthRotationState.dwStartTime > 10*60*1000 )
							{//������������
								if( (g_stHearthRotationState.iCurCoder != g_stHearthRotationState.iLastCoder) && (g_stHearthRotationState.iCurCoder + 360 - g_stHearthRotationState.iStartCoder) % 360 >= 20 )
								{   ////��ǰ����������ֵ���ϴαȷ����仯. ���ұ�����ֵ����ʼ�仯����20��
									bReallyFinish = true;
								}
							}
						}
						else
						{// ������ֵ�ݼ������
							if( (g_stHearthRotationState.iTargetCoder + 360 - g_stHearthRotationState.iCurCoder) %360 < 4)
							{
								bReallyFinish = true;
							}

							//2008.04.02�����.����Ӧ�ٶȵ�ʱ������ʱ�����û�б�Ҫ.
							if(  GetTickCount()-g_stHearthRotationState.dwStartTime > 10*60*1000 )
							{//������������
								if( (g_stHearthRotationState.iCurCoder != g_stHearthRotationState.iLastCoder) && (g_stHearthRotationState.iStartCoder + 360 - g_stHearthRotationState.iCurCoder) %360 >= 20 )
								{   //������ֵ�仯����20��
									bReallyFinish = true;
								}
							}

						}
					}
					else
					{
						if(g_stHearthRotationState.iCurCoder != g_stHearthRotationState.iLastCoder)
						{
							bOnceDifferent = true;
						}
					}

					if(bReallyFinish)
					{
						//���1�������ֹͣ��ת!
						//2007.12.29
						//g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.bAnnularPositive);
						g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,gb_AnnularCW[g_stHearthRotationState.iCurHearth]);
						//����ֹͣλ��
						str.Format(TEXT("% 3d"),g_stHearthRotationState.iCurCoder);
						m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem, _ROTATOR_CRUCIBLE_LIST_COL_END,str);

						//���澭��ʱ��
						int iTemp = 0.5+(double)( pst->dwTickTime - g_stHearthRotationState.dwStartTime )/1000;
						str.Format(TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60 ,iTemp%60);
						m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME,str);

						//g_stHearthRotationState.iCoderChangeCounts
						// ��Ȧ���� ʱ��/1000 * 360 
						////��¼ʵ��ת���ĽǶ���
						if( g_stHearthRotationState.iCoderChangeCounts > 0)
						{//����������
							iTemp = (360 + g_stHearthRotationState.iCurCoder - g_stHearthRotationState.iStartCoder)%360;
						}
						else
						{//�������ݼ�
							iTemp = (g_stHearthRotationState.iStartCoder + 360 -g_stHearthRotationState.iCurCoder)%360;
						}
						iTemp = 0.5 + (double)(pst->dwTickTime - g_stHearthRotationState.dwStartTime ) * 0.36  / iTemp; //ת���� 1Ȧ 360�� ��ʱ�� *360 /1000

						str.Format(TEXT("%02d:%02d:%02d"),iTemp/3600,(iTemp/60)%60, iTemp%60);
						m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_RESULT,str);


						if( m_ListctrlCrucible.GetItemCount() > g_stHearthRotationState.iCurListItem + 1)
						{//������һ��
							g_stHearthRotationState.iCurListItem++;
							//  ����
							//�����м��Ъ��
							g_stHearthRotationState.bAnnularIntervalWaiting = true;
							g_stHearthRotationState.dwStartingWaitingTick = GetTickCount();
						}
						else
						{//�Ѿ������һ�У��򱨸���ɣ����˳�!
							EnterCriticalSection(&gCriticalSection_HearthState);
							g_bTestHearthRotation = false;
							LeaveCriticalSection(&gCriticalSection_HearthState);
							fn_AutoSaveHearthList();//Add, 2008.01.08
							//AfxMessageBox(TEXT("��ϲ������ת��������ɣ�"));	
							AfxMessageBox(g_item3_String[_ITEM3_STR_HEARTH_TEST_FINISH].IsEmpty()?TEXT("��ϲ������ת��������ɣ�\r\nCongradulation: Hearth rotation test finished! "):g_item3_String[_ITEM3_STR_HEARTH_TEST_FINISH]);
						}
					}
					else
					{
						if( bRotatePosError || bRotateStopError)
						{//ֹͣ����״̬. �����淢��ת������
							EnterCriticalSection(&gCriticalSection_HearthState);
							g_bTestHearthRotation = false;
							LeaveCriticalSection(&gCriticalSection_HearthState);

							if(bRotatePosError)
							{
								//AfxMessageBox(TEXT("���� ����ת������λ"));
								AfxMessageBox(g_item3_String[_ITEM3_STR_HAERTH_ROTAT_POS_ERR].IsEmpty()?TEXT("���� ����ת������λ��\r\nError: Hearth rotation error."):g_item3_String[_ITEM3_STR_HAERTH_ROTAT_POS_ERR]);
							}
							else
							{
								//AfxMessageBox(TEXT("���� ת����ʱ��"));
								AfxMessageBox(g_item3_String[_ITEM3_STR_HEARTH_ROTAT_OVER_TIME].IsEmpty()?TEXT("���� ����ת����ʱ����\r\nError: Hearth rotation overtime."):g_item3_String[_ITEM3_STR_HEARTH_ROTAT_OVER_TIME]);
							}

						}
						else
						{//������;��IDC_STATIC_TEST_TIME
							iTurnBit = 0.5+ (double)(pst->dwTickTime - g_stHearthRotationState.dwStartTime)/1000;
							if( pst->dwTickTime < g_stHearthRotationState.dwStartTime)
								iTurnBit = 0;
							str.Format(TEXT("%02d:%02d"),iTurnBit/60, iTurnBit%60);
							//str.Format(TEXT("%02d:%02d"),((pst->dwTickTime - g_stHearthRotationState.dwStartTime)/1000)/60, ((pst->dwTickTime - g_stHearthRotationState.dwStartTime)/1000)%60);
							SetDlgItemText(IDC_STATIC_TEST_TIME,str);
						}
					}
				}//�������Թ���
			}//��������������
		}//��������

		//���ͻ�ͼָ� ��������������֮�⣬�����û�������ʾ֮��.
		if( m_OAHearth[0].iHearthType != _OA_HEARTH_TYPE_NONE )
		{
			pDrawCrucible = ::GetOneDrawCrucibleBuffer(0);
			m_OAHearth[0].iCoderVal = pst->wRdData[0];

			//pDrawCrucible->iCurCurcible = m_OAHearth[i].iCurCrucible;
			pDrawCrucible->iCrucibleMaxNumber = m_OAHearth[0].iCrucibleMaxNumber ;
			pDrawCrucible->iCoderVal = m_OAHearth[0].iCoderVal;


			if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[0].iHearthType)
			{
				pDrawCrucible->iSpeed = m_OAHearth[0].iSpeed;
			}
			else
			{
				m_OAHearth[0].iCurCrucible = pst->wRdData[1];
				pDrawCrucible->iCurCurcible = m_OAHearth[0].iCurCrucible;
			}
			g_pDrawCrucibleThread->PostThreadMessage(WM_ITEM3_DRAW_CRUCIBLE,0,LPARAM(pDrawCrucible));
		}

		if( m_OAHearth[1].iHearthType != _OA_HEARTH_TYPE_NONE )
		{
			pDrawCrucible = ::GetOneDrawCrucibleBuffer(1);
			m_OAHearth[1].iCoderVal = pst->wRdData[2];

			pDrawCrucible->iCrucibleMaxNumber = m_OAHearth[1].iCrucibleMaxNumber ;
			pDrawCrucible->iCoderVal = m_OAHearth[1].iCoderVal;

			if(_OA_HEARTH_TYPE_ANNULAR == m_OAHearth[1].iHearthType)
			{
				pDrawCrucible->iSpeed = m_OAHearth[1].iSpeed;
			}
			else
			{
				m_OAHearth[1].iCurCrucible = pst->wRdData[3];
				pDrawCrucible->iCurCurcible = m_OAHearth[1].iCurCrucible;
			}
			g_pDrawCrucibleThread->PostThreadMessage(WM_ITEM3_DRAW_CRUCIBLE,0,LPARAM(pDrawCrucible));
		}

		g_stHearthRotationState.iLastCoder = g_stHearthRotationState.iCurCoder;
		g_stHearthRotationState.iLastCrucible = g_stHearthRotationState.iCurCrucible;

		fn_ShowWindow();
		break;
	case _stHearthThreadData::enPlcNoAnswer:
		EnterCriticalSection(&gCriticalSection_HearthState);
		g_bTestHearthRotation = false;
		LeaveCriticalSection(&gCriticalSection_HearthState);
		fn_ShowWindow();
		//AfxMessageBox(_T("���� PLC û��������Ӧ.����ֹͣ\r\ncase _stHearthThreadData::enPlcNoAnswer:"),MB_OK|MB_ICONSTOP);
		AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR].IsEmpty()?TEXT("���󣺣�����״̬δ��Ӧ�� PLC û��������Ӧ.����ֹͣ��\r\nError: plc communication failed."):g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);
		break;
	}
}

//iItem����ʼ ���Ե��к�
bool COARotatorView::fn_StartHearthTest(int iItem)
{
	if( iItem >= m_ListctrlCrucible.GetItemCount())
	{//��ֹ�������� ɾ��! 2007.12.28
		EnterCriticalSection(&gCriticalSection_HearthState);
			g_bTestHearthRotation = false;
		LeaveCriticalSection(&gCriticalSection_HearthState);
		fn_ShowWindow();
		return false;
	}

	g_stHearthRotationState.iCurListItem = iItem;
	//�����ǰ���������� //�Է��ǵڶ��β���.
	m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_START,TEXT(""));
	m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_END,TEXT(""));
	m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME,TEXT(""));
	m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_RESULT,TEXT(""));	

	//debugging 2007.12.18 LVIS_SELECTED
	//m_ListctrlCrucible.SetItem(iItem,0,LVIF_STATE,0,0,LVIS_SELECTED,LVIS_SELECTED,0);
	m_ListctrlCrucible.SetItemState(iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_ListctrlCrucible.EnsureVisible(iItem,false);

	CString str = m_ListctrlCrucible.GetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_NO);
	
	g_stHearthRotationState.iCurHearth = _ttoi(str) - 1;  //��1,2ת�� 0,1
	ASSERT(g_stHearthRotationState.iCurHearth< 2 );
	ASSERT(m_OAHearth[g_stHearthRotationState.iCurHearth].iHearthType != _OA_HEARTH_TYPE_NONE);
	
	//Ŀ��ֵ
	str = m_ListctrlCrucible.GetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE);
	g_stHearthRotationState.iTargetPos = _ttoi(str);

	g_stHearthRotationState.iRotationTime = 0;
	g_stHearthRotationState.bStop = false;

	if( m_OAHearth[g_stHearthRotationState.iCurHearth].iHearthType ==_OA_HEARTH_TYPE_ANNULAR)
	{//�������� iTargetSpeed
		g_stHearthRotationState.iTargetSpeed = g_stHearthRotationState.iTargetPos;
	}
	else
	{   //��������Ŀ��������Ӧ�� ����������
		double d = 360/m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber;
		g_stHearthRotationState.iTargetCoder  = (int)( d*(g_stHearthRotationState.iTargetPos-1) + 0.5);
	}
	
	// >> ��ȡ��ǰHearth������������λ��
	bool bPlcOK = g_pRotateThread->fn_GetHearthCoderValue(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.iLastCoder);
	g_stHearthRotationState.iCurCoder = g_stHearthRotationState.iLastCoder;
	
	if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[g_stHearthRotationState.iCurHearth].iHearthType)	
	{//������
		bPlcOK &= g_pRotateThread->fn_GetHearthCrucibleNumber(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.iCurCrucible);
		//���������жϣ��Ƿ���ת.  ��Ϊ��ת���� ����ͣ��.  ˳ת�򲻴���ͣ��.
		if(bPlcOK)
		{
			g_stHearthRotationState.iStartPos             = g_stHearthRotationState.iCurCrucible;
			g_stHearthRotationState.iLastCrucible         = g_stHearthRotationState.iCurCrucible;
			g_stHearthRotationState.iCrucibleChangeCounts = 0;
			//G1268, 12������
			// 1-->7 ��ת
			// 7-->1 ��ת
			if( g_stHearthRotationState.iTargetPos > g_stHearthRotationState.iStartPos)
			{
				if(g_stHearthRotationState.iTargetPos-g_stHearthRotationState.iStartPos
					>= m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
				{
					g_stHearthRotationState.bReversal = true;
				}
				else
				{
					g_stHearthRotationState.bReversal = false;
				}
			}
			else
			{
				if(g_stHearthRotationState.iTargetPos+m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber - g_stHearthRotationState.iStartPos
					> m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber>>1)
				{
					g_stHearthRotationState.bReversal = true;
				}
				else
				{
					g_stHearthRotationState.bReversal = false;
				}
			}
		}
		else
		{
			//AfxMessageBox(TEXT("Error: plc����������"));
			AfxMessageBox(g_item3_String[_ITEM3_STR_PLC_COMM_ERR].IsEmpty()?
				TEXT("����plcͨ�Ų�������\r\n plc communication failed."):g_item3_String[_ITEM3_STR_PLC_COMM_ERR]);

			return false;
		}		
		// >>����ת��Ŀ�꣬������ģ�ⴥ������ת������.
		//debugging,����� ������.
		ASSERT(g_stHearthRotationState.iTargetPos<=m_OAHearth[g_stHearthRotationState.iCurHearth].iCrucibleMaxNumber);
		g_pRotateThread->fn_SetCruciblePosPT(g_stHearthRotationState.iCurHearth, g_stHearthRotationState.iTargetPos);
		Sleep(200);
		//if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[g_stHearthRotationState.iCurHearth].iHearthType)
		g_pRotateThread->fn_RunCruciblePT(g_stHearthRotationState.iCurHearth);

		//��д��ʼ����λ��.08.01.05
		str.Format(TEXT("%2d"),g_stHearthRotationState.iStartPos );
		m_ListctrlCrucible.SetItemText(g_stHearthRotationState.iCurListItem , _ROTATOR_CRUCIBLE_LIST_COL_START,str);

	}
	else
	{//��������,2007.12.24
		//����Ŀ����ת�ٶ�
		g_pRotateThread->fn_SetHearthSpeed(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.iTargetSpeed);

		//���Ŀ���ٶ�, 2007.12.29
		m_OAHearth[g_stHearthRotationState.iCurHearth].iSpeed = g_stHearthRotationState.iTargetSpeed;

		//��ʼ��ת. g_stHearthRotationState.bAnnularReversal �ṩ�߼����湩�û�����
		//g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.bAnnularPositive);
		g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,gb_AnnularCW[g_stHearthRotationState.iCurHearth]);
		Sleep(400);
		//ȷ���Ѵ�����ת״̬
		bool bRunning = false;
		g_pRotateThread->fn_GetHearthRunState(g_stHearthRotationState.iCurHearth,bRunning,true);
		if(!bRunning)
		{//���û����ת�����ٷ���һ�ο�ʼ��ת����
			//g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,g_stHearthRotationState.bAnnularPositive);
			g_pRotateThread->fn_RunAnnularHearthPT(g_stHearthRotationState.iCurHearth,gb_AnnularCW[g_stHearthRotationState.iCurHearth]);
			Sleep(400);
			g_pRotateThread->fn_GetHearthRunState(g_stHearthRotationState.iCurHearth,bRunning,true);
			if(!bRunning)
			{//��Ȼ����ֹͣ״̬���򱨴��˳���
				//AfxMessageBox(TEXT("Error: ��������û�а�Ԥ�ڿ�ʼ��ת��"));
				AfxMessageBox(g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_NOT_ROTAT].IsEmpty()?
				TEXT("����: ��������û�а�Ԥ�ڿ�ʼ��ת��\r\n Annualr hearth start rotation failed."):g_item3_String[_ITEM3_STR_ANNULAR_HEARTH_NOT_ROTAT]);
				return false;
			}
		}

		g_stHearthRotationState.bAnnularBeginingWaiting  = true; //�����ʼ�ȴ�״̬
		g_stHearthRotationState.bAnnularIntervalWaiting  = false;
		g_stHearthRotationState.dwStartingWaitingTick = GetTickCount();
	}
	//��ʼ��ʱ
	g_stHearthRotationState.dwStartTime = GetTickCount();

	//������״̬, 2008.01,03
	g_stHearthRotationState.iCoderChangeCounts = 0;	
	g_stHearthRotationState.iCoderNotChangeTime = 0;
	g_stHearthRotationState.dwCoderNotChangeStartTickTime = g_stHearthRotationState.dwStartTime; 
	g_stHearthRotationState.iCrucibleChangeCounts = 0;

	// >> ������Ϣ���̣߳��������ѭ����
		// TODO: Add your control notification handler code here
	EnterCriticalSection(&gCriticalSection_HearthState);
		g_bTestHearthRotation = true;
	LeaveCriticalSection(&gCriticalSection_HearthState);
	g_pRotateThread->PostThreadMessage(WM_ITEM3_HEARTH_ROTATE_MONITOR,0,g_stHearthRotationState.iCurHearth);
	fn_ShowWindow();
	return true;
}

void COARotatorView::OnBnClickedBtnCreateHearth1()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(_OA_HEARTH_TYPE_CRUCIBLE==m_OAHearth[0].iHearthType? IDC_COMBO_HEARTH_12:IDC_COMBO_HEARTH_1,str);
	//GetDlgItemText(IDC_COMBO_HEARTH_1,str);
	int iStartPos = abs(_ttoi(str));
	fn_CreateHearthTestOrder(0,iStartPos);
}

void COARotatorView::OnBnClickedBtnCreateHearth2()
{
	CString str;
	GetDlgItemText(_OA_HEARTH_TYPE_CRUCIBLE==m_OAHearth[1].iHearthType? IDC_COMBO_HEARTH_22:IDC_COMBO_HEARTH_2,str);
	int iStartPos = abs(_ttoi(str));
//	ASSERT(iStartPos>0 && iStartPos<100);
	fn_CreateHearthTestOrder(1,iStartPos);
}

void COARotatorView::fn_CreateHearthTestOrder(int iHearth, int iStartPos)
{
	CString str;
	ASSERT(m_bInit);
	ASSERT( _OA_HEARTH_TYPE_NONE != m_OAHearth[iHearth].iHearthType);
	int iItem = m_ListctrlCrucible.GetItemCount();
	if( _OA_HEARTH_TYPE_ANNULAR == m_OAHearth[iHearth].iHearthType )
	{
		if(_HEARTH_MONITOR_TYPE_OMRON==m_OAHearth[iHearth].iHearthMonitorType)
		{//2008.07.15
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("300"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("250"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("200"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("150"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("100"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("50"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("20"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("10"));
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("5"));
		}
		else
		{
			//900~100, 50. ���������� �ٶ� (0~999)
			for( int i = 900;i>0;i-=100)
			{
				str.Format(TEXT("%d"),i);
				m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
				m_ListctrlCrucible.SetItemText(iItem++,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,str);
			}
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,TEXT("50"));
		}
	}
	else if( _OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[iHearth].iHearthType )
	{//��������Ŀ��������

		//08.01.03
		int iCurPos = iStartPos;
		for(int i=0;i<m_OAHearth[iHearth].iCrucibleMaxNumber;i++)
		{
			iCurPos += i;
			iCurPos = iCurPos%m_OAHearth[iHearth].iCrucibleMaxNumber;
			str.Format(TEXT("%2d"),iCurPos!=0?iCurPos:m_OAHearth[iHearth].iCrucibleMaxNumber);			
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,str);
			iItem++;
		}
		/*
		int i;
		int iHalfNumber = m_OAHearth[iHearth].iCrucibleMaxNumber >> 1;  // /2
		
		for( i = 0; i<= iHalfNumber; i++)
		{
			iCurPos += i;
			iCurPos = iCurPos%m_OAHearth[iHearth].iCrucibleMaxNumber;
			str.Format(TEXT("%2d"),iCurPos!=0?iCurPos:m_OAHearth[iHearth].iCrucibleMaxNumber);			
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,str);
			iItem++;
		}

		for( i = iHalfNumber;i>0;i--)
		{
			iCurPos += m_OAHearth[iHearth].iCrucibleMaxNumber;
			iCurPos -= i;
			iCurPos = iCurPos%m_OAHearth[iHearth].iCrucibleMaxNumber;
			str.Format(TEXT("%2d"),iCurPos!=0?iCurPos:m_OAHearth[iHearth].iCrucibleMaxNumber);
			m_ListctrlCrucible.InsertItem(iItem,0==iHearth?TEXT("1"):TEXT("2"));
			m_ListctrlCrucible.SetItemText(iItem,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE,str);
			iItem++;
		}
		*/
	}
}

BOOL COARotatorView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_DELETE)
		{
			if(GetFocus()->GetSafeHwnd() == m_ListctrlCrucible.GetSafeHwnd())
			{
				if(!g_bTestHearthRotation)//���Թ����в�����ɾ��.2008.01.05
					OnBnClickedBtnDeleteHearthRow();
				return false;
			}
		}
	}
	return CFormView::PreTranslateMessage(pMsg);
}



void COARotatorView::fn_SetIDsText(void)
{
	for(int i=0;i<gc_item3_Text_ID_CNT;i++)
	{
		if(!g_item3_ID_Text[i].IsEmpty())
			SetDlgItemText(g_item3_ID[i],g_item3_ID_Text[i]);
	}
}

int COARotatorView::fn_SaveHearthList(LPCTSTR pszFileName)
{
	TCHAR  lptchTmp[512];  //��������
	TCHAR cbuf[512];

	CFile fFile;
	if(!fFile.Open(pszFileName,CFile::modeCreate|CFile::modeWrite))
	{
		return 0;
	}

	CString str;

	fFile.SeekToBegin();
	//Save unicode file head
#ifdef UNICODE
	cbuf[0] = 0xFEFF; //FF�ڵ� FE�ڸ�
	fFile.Write(cbuf,1*sizeof(TCHAR)); 
#endif

	TCHAR* p = TEXT("OPTORUN ASSITANT TEST FILE\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	p = TEXT("FILE TYPE: HEARTH ROTATION\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));


	int iLength;
	iLength = _stprintf(lptchTmp,TEXT("PROGRAM VERSION: %.3f\x0d\x0a"),g_dDataVersion);
	if(iLength)
		fFile.Write( lptchTmp,iLength * sizeof(TCHAR) );

	MultiByteToWideChar(CP_UTF8,0,__DATE__,strlen(__DATE__) + 1,cbuf,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
	iLength = _stprintf(lptchTmp,TEXT("PROGRAM DATE: %s\x0d\x0a"),cbuf);
	if(iLength)
		fFile.Write( lptchTmp,iLength * sizeof(TCHAR) );

	iLength = _stprintf(lptchTmp,TEXT("PROGRAM DATA VERSION: %.2f\x0d\x0a"),g_dProgramVersion);
	if(iLength)
		fFile.Write(lptchTmp, iLength * sizeof(TCHAR));

	//�ļ�����ʱ��
	DWORD bufsize = 256;
	::GetComputerName(cbuf,&bufsize);
	iLength = wsprintf(lptchTmp,TEXT("%s%s\x0d\x0a"),TEXT("Computer Name: "),cbuf);
	fFile.Write(lptchTmp,iLength*sizeof(TCHAR));

	SYSTEMTIME   st;   
	GetLocalTime(&st); 
	str.Format(TEXT("Save Time: %4d-%02d-%02d %02d:%02d:%02d\x0d\x0a"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	fFile.Write(str,sizeof(TCHAR) * str.GetLength()); 

	//��������
	iLength = _stprintf(lptchTmp,TEXT("Hearth 1 TYPE: %s\x0d\x0a"),_OA_HEARTH_TYPE_NONE == m_OAHearth[0].iHearthType ? TEXT("No Hearth") :(_OA_HEARTH_TYPE_ANNULAR==m_OAHearth[0].iHearthType?TEXT("Annular"):TEXT("Cup")) );
	if(iLength)
		fFile.Write(lptchTmp, iLength * sizeof(TCHAR));
	//������������ת����,2008.07.14
	if(_OA_HEARTH_TYPE_ANNULAR==m_OAHearth[0].iHearthType)
	{
		iLength = _stprintf(lptchTmp,TEXT("Hearth 1 Rotate Direction: %s\x0d\x0a"),gb_AnnularCW[0]==true?TEXT("CW"):TEXT("CCW") );
		if(iLength)
			fFile.Write(lptchTmp, iLength * sizeof(TCHAR));
	}

	iLength = _stprintf(lptchTmp,TEXT("Hearth 2 TYPE: %s\x0d\x0a"),_OA_HEARTH_TYPE_NONE == m_OAHearth[1].iHearthType ? TEXT("No Hearth") :(_OA_HEARTH_TYPE_ANNULAR==m_OAHearth[1].iHearthType?TEXT("Annular"):TEXT("Cup")) );
	if(iLength)
		fFile.Write(lptchTmp, iLength * sizeof(TCHAR));
	//������������ת����,2008.07.14
	if(_OA_HEARTH_TYPE_ANNULAR==m_OAHearth[1].iHearthType)
	{
		iLength = _stprintf(lptchTmp,TEXT("Hearth 2 Rotate Direction: %s\x0d\x0a"),gb_AnnularCW[1]==true?TEXT("CW"):TEXT("CCW") );
		if(iLength)
			fFile.Write(lptchTmp, iLength * sizeof(TCHAR));
	}

	//������ʼ��־
	p=TEXT("<DATA>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));


	HDITEM hdi;
	CHeaderCtrl* pmyHeaderCtrl = m_ListctrlCrucible.GetHeaderCtrl();

	//�б�ͷ
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

	//�б�����
	int iCnt = m_ListctrlCrucible.GetItemCount();
	for(int i=0;i<iCnt;i++)
	{
		iLength = _stprintf(lptchTmp,TEXT("%s\t%s\t%s\t%s\t%s\t%s\r\n"),m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_NO),
			m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_CRUCIBLE),
			m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_START),
			m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_END),
			m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME),
			m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_RESULT));
		fFile.Write(lptchTmp,iLength*sizeof(TCHAR));
	}

	p=TEXT("</DATA>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
	return 1;
}

int COARotatorView::fn_SaveDomeList(LPCTSTR pszFileName)
{
	TCHAR  lptchTmp[512];  //��������
	TCHAR cbuf[512];

	CFile fFile;
	if(!fFile.Open(pszFileName,CFile::modeCreate|CFile::modeWrite))
	{
		return 0;
	}

	CString str;

	fFile.SeekToBegin();
	//Save unicode file head
#ifdef UNICODE
	cbuf[0] = 0xFEFF; //FF�ڵ� FE�ڸ�
	fFile.Write(cbuf,1*sizeof(TCHAR)); 
#endif

	TCHAR* p = TEXT("OPTORUN ASSITANT TEST FILE\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	p = TEXT("FILE TYPE: DOME ROTATION\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	int iLength;
	iLength = _stprintf(lptchTmp,TEXT("PROGRAM VERSION: %.3f\x0d\x0a"),g_dDataVersion);
	if(iLength)
		fFile.Write( lptchTmp,iLength * sizeof(TCHAR) );

	MultiByteToWideChar(CP_UTF8,0,__DATE__,strlen(__DATE__) + 1,cbuf,1024); // strlen(..) + 1����֤���ַ����Ľ�������һ��ת�� 
	iLength = _stprintf(lptchTmp,TEXT("PROGRAM DATE: %s\x0d\x0a"),cbuf);
	if(iLength)
		fFile.Write( lptchTmp,iLength * sizeof(TCHAR) );

	iLength = _stprintf(lptchTmp,TEXT("PROGRAM DATA VERSION: %.3f\x0d\x0a"),g_dProgramVersion);
	if(iLength)
		fFile.Write(lptchTmp, iLength * sizeof(TCHAR));

	//�ļ�����ʱ��
	DWORD bufsize = 256;
	::GetComputerName(cbuf,&bufsize);
	iLength = wsprintf(lptchTmp,TEXT("%s%s\x0d\x0a"),TEXT("Computer Name: "),cbuf);
	fFile.Write(lptchTmp,iLength*sizeof(TCHAR));

	SYSTEMTIME   st;   
	GetLocalTime(&st); 
	str.Format(TEXT("Save Time: %4d-%02d-%02d %02d:%02d:%02d\x0d\x0a"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	fFile.Write(str,sizeof(TCHAR) * str.GetLength()); 

	//������ʼ��־
	p=TEXT("<DATA>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));

	HDITEM hdi;
	CHeaderCtrl* pmyHeaderCtrl = m_ListctrlDome.GetHeaderCtrl();

	//�б�ͷ
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

	//�б�����
	int iCnt = m_ListctrlDome.GetItemCount();
	for(int i=0;i<iCnt;i++)
	{
		iLength = _stprintf(lptchTmp,TEXT("%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n"),m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_SPEED),
			m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_START_CNT),
			m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_END_CNT),
			m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_WAITING_TIME),
			m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_TEST_TIME),
			m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION),
			m_ListctrlDome.GetItemText(i,_ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE));
		fFile.Write(lptchTmp,iLength*sizeof(TCHAR));
	}

	p=TEXT("</DATA>\x0d\x0a");
	fFile.Write(p,_tcslen(p)*sizeof(TCHAR));
	return 1;
}

// �Զ������ļ����������浱ǰHearth�б����ݣ�������ֹͣʱ�Զ�����
int COARotatorView::fn_AutoSaveHearthList(void)
{
	TCHAR  lptchTmp[512];  //��������
	TCHAR cbuf[512];
	DWORD bufsize = 512;
	
	//Path
	::GetModuleFileName(NULL,lptchTmp,512);
	CString str(lptchTmp);
	int ibackslash = str.ReverseFind(TEXT('\\'));
	lptchTmp[ibackslash + 1]= TEXT('\0');

	//fileName
	::GetComputerName(cbuf,&bufsize);
	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	str.Format(TEXT("-OA-HEARTH-%4d%02d%02d %02d%02d%02d.log"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	_tcscat(cbuf,str); //computer name + time = filename

	_tcscat(lptchTmp,cbuf); // path + filename.

	return fn_SaveHearthList(lptchTmp);
}
int COARotatorView::fn_AutoSaveDomeList(void)
{
	TCHAR  lptchTmp[512];  //��������
	TCHAR cbuf[512];
	DWORD bufsize = 512;
	
	//Path
	::GetModuleFileName(NULL,lptchTmp,512);
	CString str(lptchTmp);
	int ibackslash = str.ReverseFind(TEXT('\\'));
	lptchTmp[ibackslash + 1]= TEXT('\0');

	//fileName
	::GetComputerName(cbuf,&bufsize);
	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	str.Format(TEXT("-OA-Dome-%4d%02d%02d %02d%02d%02d.log"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	_tcscat(cbuf,str); //computer name + time = filename

	_tcscat(lptchTmp,cbuf); // path + filename.

	return fn_SaveDomeList(lptchTmp);
}

// �����Իع鷽�̣�Y = a + bx
// data[rows*2]���飺X, Y��rows������������a, b�����ػع�ϵ��. ����x,y��ʽ�洢�Ķ�ά����
// SquarePoor[4]�����ط������ָ��: �ع�ƽ���ͣ�ʣ��ƽ���ͣ��ع�ƽ���ʣ��ƽ����
// ����ֵ��0���ɹ���-1����
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor=NULL)
{
    int m;
    double *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
    if (data == 0 || a == 0 || b == 0 || rows < 1)
        return -1;
    for (p = data, m = 0; m < rows; m ++)
    {
        xa += *p ++;
        ya += *p ++;
    }
    xa /= rows;                                     // Xƽ��ֵ
    ya /= rows;                                     // Yƽ��ֵ
    for (p = data, m = 0; m < rows; m ++, p += 2)
    {
        Lxx += ((*p - xa) * (*p - xa));             // Lxx = Sum((X - Xa)ƽ��)
        Lxy += ((*p - xa) * (*(p + 1) - ya));       // Lxy = Sum((X - Xa)(Y - Ya))
    }
    *b = Lxy / Lxx;                                 // b = Lxy / Lxx
    *a = ya - *b * xa;                              // a = Ya - b*Xa
    if (SquarePoor == 0)
        return 0;
    // �������
    SquarePoor[0] = SquarePoor[1] = 0.0;
    for (p = data, m = 0; m < rows; m ++, p ++)
    {
        Lxy = *a + *b * *p ++;
        SquarePoor[0] += ((Lxy - ya) * (Lxy - ya)); // U(�ع�ƽ����)
        SquarePoor[1] += ((*p - Lxy) * (*p - Lxy)); // Q(ʣ��ƽ����)
    }
    SquarePoor[2] = SquarePoor[0];                  // �ع鷽��
    SquarePoor[3] = SquarePoor[1] / (rows - 2);     // ʣ�෽��
    return 0;
}

/*

*/
void COARotatorView::OnBnClickedBtnStatisticsEvaluation()
{
	// TODO: Add your control notification handler code here
	int nCount = m_ListctrlCrucible.GetItemCount();
	if( !m_bInit || nCount==0 )
		return;

	//ֻ�е��������ݲſ��Խ������λع�ͳ��
	if(_OA_HEARTH_TYPE_CRUCIBLE != m_OAHearth[0].iHearthType && _OA_HEARTH_TYPE_CRUCIBLE != m_OAHearth[1].iHearthType)
		return;

	int i;
	int iTemp;
	double dTemp;
	CString str,strTemp;
	int iHearth1PositiveDataNumber = 0; //��ת
	int iHearth1NegativeDataNumber = 0; //��ת
	int iHearth2PositiveDataNumber = 0; //��ת
	int iHearth2NegativeDataNumber = 0; //��ת
	
	double *pH1PositiveData = NULL;
	double *pH1NegativeData = NULL;

	double *pH2PositiveData = NULL;
	double *pH2NegativeData = NULL;

	//������ռ�
	if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[0].iHearthType)
	{
		pH1PositiveData = new double[nCount<<1];
		pH1NegativeData = new double[nCount<<1];

		ASSERT(pH1PositiveData && pH1NegativeData);
		if(!pH1PositiveData || !pH1NegativeData)
			goto label_Hearth_Statistics_Return;
	}

	if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[1].iHearthType)
	{
		pH2PositiveData = new double[nCount<<1];
		pH2NegativeData = new double[nCount<<1];

		ASSERT(pH2PositiveData && pH2NegativeData);
		if(!pH2PositiveData || !pH2NegativeData)
			goto label_Hearth_Statistics_Return;
	}

	//�ϼ����ݱ���п��ܵ�����.
	//CString str;
	for( i=0; i<nCount; i++ )
	{
		iTemp = _ttoi(m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_NO));
		if( iTemp == 1 )
		{//Hearth1
			if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[0].iHearthType)
			{
				dTemp = _tstof(m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME));
				if(dTemp>0.1) 
				{//�в���ʱ��(����ʱ�� ͨ���� 1������)��˵��������
					iTemp = _ttoi(m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_RESULT));
					if(iTemp>0)
					{
						//��ת������������
						pH1PositiveData[ (iHearth1PositiveDataNumber<<1) ] = iTemp;   //X ������
						pH1PositiveData[ (iHearth1PositiveDataNumber<<1) +1 ] = dTemp; //Y ʱ��
						iHearth1PositiveDataNumber++;
					}
					else if(iTemp<0)
					{   //��ת����������������Ϊ����д���ݵ�ʱ�򣬷�ת���������� ���ű�ʾ
						pH1NegativeData[ (iHearth1NegativeDataNumber<<1) ] = -iTemp;   //X ������
						pH1NegativeData[ (iHearth1NegativeDataNumber<<1) +1 ] = dTemp; //Y ʱ��
						iHearth1NegativeDataNumber++;
					}
				}			
			}
		}
		else if( iTemp == 2 )
		{//Hearth2
			if(_OA_HEARTH_TYPE_CRUCIBLE == m_OAHearth[1].iHearthType)
			{
				dTemp = _tstof(m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_TEST_TIME));
				if(dTemp>0.1) 
				{//�в���ʱ��(����ʱ�� ͨ���� 1������)��˵��������
					iTemp = _ttoi(m_ListctrlCrucible.GetItemText(i,_ROTATOR_CRUCIBLE_LIST_COL_RESULT));
					if(iTemp>0)
					{
						//��ת������������
						pH2PositiveData[ (iHearth2PositiveDataNumber<<1) ] = iTemp;   //X ������
						pH2PositiveData[ (iHearth2PositiveDataNumber<<1) +1 ] = dTemp; //Y ʱ��
						iHearth2PositiveDataNumber++;
					}
					else if(iTemp<0)
					{   //��ת����������������Ϊ����д���ݵ�ʱ�򣬷�ת���������� ���ű�ʾ
						pH2NegativeData[ (iHearth2NegativeDataNumber<<1) ] = -iTemp;   //X ������
						pH2NegativeData[ (iHearth2NegativeDataNumber<<1) +1 ] = dTemp; //Y ʱ��
						iHearth2NegativeDataNumber++;
					}
				}			
			}
		}
	}

	double a[4];  //a ��Ӧ 1+ 1- 2+ 2-
	double b[4];  //b ��Ӧ 1+ 1- 2+ 2-
	int    c[4];  //��Ӧ�Ļع麯������ֵ
	memset( a, 0, sizeof(double) * 4 );
	memset( b, 0, sizeof(double) * 4 );
	memset( c, 0, sizeof(int) * 4 );

	if( iHearth1PositiveDataNumber > 2)
	{ //2�����ϲŽ���ͳ�Ʒ���
		c[0] = LinearRegression(pH1PositiveData,iHearth1PositiveDataNumber,&a[0],&b[0]);
	}

	if( iHearth1NegativeDataNumber > 2)
	{ //2�����ϲŽ���ͳ�Ʒ���
		c[1] = LinearRegression(pH1NegativeData,iHearth1NegativeDataNumber,&a[1],&b[1]);
	}

	if( iHearth2PositiveDataNumber > 2)
	{ //2�����ϲŽ���ͳ�Ʒ���
		c[2] = LinearRegression(pH2PositiveData,iHearth2PositiveDataNumber,&a[2],&b[2]);
	}

	if( iHearth2NegativeDataNumber > 2)
	{ //2�����ϲŽ���ͳ�Ʒ���
		c[3] = LinearRegression(pH2NegativeData,iHearth2NegativeDataNumber,&a[3],&b[3]);
	}

	//���� Hearth1 �������
	if(iHearth1PositiveDataNumber>2 && 0==c[0])
	{//Hearth1 ��ת ���ݷ�����ȷ
		strTemp.Format(TEXT("Hearth1 clockwise rotation\r\n\t<beginning and ending time>: %.1f seconds.\r\n\t<each cup>: %.1f seconds.\r\n"),a[0],b[0]);
		str += strTemp;
	}

	if(iHearth1NegativeDataNumber>2 && 0==c[1])
	{//Hearth1 ��ת ���ݷ�����ȷ
		if( iHearth1PositiveDataNumber>2 && 0==c[0] )
		{ //Hearth1 ��ת ��������
			strTemp.Format(TEXT("Hearth1 counter clockwise rotation\r\n\t<beginning and ending time>: %.1f seconds.\r\n\t\tgreater than clockwise: %.1f.\r\n\t<each cup>: %.1f seconds.\r\n"),a[1],a[1]-a[0],b[1]);
		}
		else
		{//û�з�ת����
			strTemp.Format(TEXT("Hearth1 counter clockwise rotation\r\n\t<beginning and ending time>: %.1f seconds.\r\n\t<each cup>: %.1f seconds.\r\n"),a[1],b[1]);
		}
		str += strTemp;
	}


	//���� Hearth2 �������
	if(iHearth2PositiveDataNumber>2 && 0==c[2])
	{//Hearth2 ��ת ���ݷ�����ȷ
		strTemp.Format(TEXT("Hearth2 clockwise rotation\r\n\t<beginning and ending time>: %.1f seconds.\r\n\t<each cup>: %.1f seconds.\r\n"),a[2],b[2]);
		str += strTemp;
	}
	if(iHearth2NegativeDataNumber>2 && 0==c[3])
	{//Hearth2 ��ת ���ݷ�����ȷ
		if( iHearth2PositiveDataNumber>2 && 0==c[2] )
		{ //Hearth1 ��ת ��������
			strTemp.Format(TEXT("Hearth2 counter clockwise rotation\r\n\t<beginning and ending time>: %.1f seconds.\r\n\t\tgreater than clockwise: %.1f.\r\n\t<each cup>: %.1f seconds."),a[3],a[3]-a[2],b[3]);
		}
		else
		{//û�з�ת����
			strTemp.Format(TEXT("Hearth2 counter clockwise rotation\r\n\t<beginning and ending time>: %.1f seconds.\r\n\t<each cup>: %.1f seconds."),a[3],b[3]);
		}
		str += strTemp;
	}


	//���� ���� ���
	if(!str.IsEmpty())
		AfxMessageBox(str);

label_Hearth_Statistics_Return:
	//�ͷſռ�
	if(pH1PositiveData)
		delete [] pH1PositiveData;
	if(pH1NegativeData)
		delete [] pH1NegativeData;
	if(pH2PositiveData)
		delete [] pH1PositiveData;
	if(pH2NegativeData)
		delete [] pH1NegativeData;
}

void COARotatorView::OnBnClickedBtnAutocreateDomeSpeed()
{
	int iItem = m_ListctrlDome.GetItemCount();
	m_ListctrlDome.InsertItem(iItem++,TEXT("10"));
	m_ListctrlDome.InsertItem(iItem++,TEXT("15"));
	m_ListctrlDome.InsertItem(iItem++,TEXT("20"));
	m_ListctrlDome.InsertItem(iItem++,TEXT("25"));
	m_ListctrlDome.InsertItem(iItem++,TEXT("30"));
}

void COARotatorView::OnBnClickedBtnInsertDomeSpeed()
{
	CString str;	
	GetDlgItemText(IDC_COMBO_DOME_SPEED,str);
	int iSpeed = _ttoi(str);
	str.Format(TEXT("%d"),iSpeed);

	if( iSpeed>30 && iSpeed<=60 )
	{
		static bool bSure=false; //ȷ��һ�Σ��ڶ����Ժ�Ϳ�ʼĬ�Ͽ���.
		if(IDYES != AfxMessageBox(TEXT("Speed %s is greater than 30, Are you sure?"),MB_YESNO | MB_ICONQUESTION|(bSure?MB_DEFBUTTON1:MB_DEFBUTTON2)))
		{return;}
		bSure = true;
	}

	if( iSpeed >= 10 && iSpeed <= 60)
	{
		int iItems;
		POSITION pos = m_ListctrlDome.GetFirstSelectedItemPosition();

		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			iItems = m_ListctrlDome.GetNextSelectedItem(pos);

			bool bTest = false;
			EnterCriticalSection(&gCriticalSection_DomeState);
				bTest = g_bTestDomeRotation;
			LeaveCriticalSection(&gCriticalSection_DomeState);
			if(bTest)
			{//���������У���������ǰ����������Ŀ.ֻ��������.
				if(iItems<=g_stDomeRotationState.iCurListItem)
					return;
			}

		}
		else
		{//û��ѡ�������뵽���һ��
			iItems = m_ListctrlDome.GetItemCount();
		}
		m_ListctrlDome.InsertItem(iItems,str);
	}
	else if( iSpeed>60)
	{
		AfxMessageBox(TEXT("Speed should less than 60."));
	}
}

//ɾ����ǰѡ��ĵ�һ��
void COARotatorView::OnBnClickedBtnDeleteDomeRow()
{
		POSITION pos = m_ListctrlDome.GetFirstSelectedItemPosition();
		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			int iItems = m_ListctrlDome.GetNextSelectedItem(pos);

			bool bTest = false;
			EnterCriticalSection(&gCriticalSection_DomeState);
			bTest = g_bTestDomeRotation;
			LeaveCriticalSection(&gCriticalSection_DomeState);
			if(bTest)
			{//���������У�������ɾ����ǰ��ǰ�� item��
				if(iItems<=g_stDomeRotationState.iCurListItem)
					return;
			}

			m_ListctrlDome.DeleteItem(iItems);
			if( iItems < m_ListctrlDome.GetItemCount())
			{
				m_ListctrlDome.SetItemState(iItems,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
				m_ListctrlDome.EnsureVisible(iItems,false);
			}
		}
}

void COARotatorView::OnBnClickedBtnSaveDomeList()
{
	TCHAR  lptchTmp[512];  //��������
	TCHAR cbuf[512];
	DWORD bufsize = 512;
	
	int iCnt = m_ListctrlDome.GetItemCount();
	if(iCnt==0)
	{	
		SYSTEMTIME   st;   
		CString str;
		GetLocalTime(&st);   
		str.Format(TEXT("Error: no data! \r\n%4d-%02d-%02d %02d:%02d:%02d\r\n"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		AfxMessageBox(str);
		return ; 
	}

	::GetModuleFileName(NULL,lptchTmp,512);
	CString str(lptchTmp);
	int ibackslash = str.ReverseFind(TEXT('\\'));
	lptchTmp[ibackslash + 1]= TEXT('\0');

	bufsize = 512-2;
	::GetComputerName(cbuf,&bufsize);

	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	str.Format(TEXT("-OA-Dome-%4d%02d%02d %02d%02d%02d.log"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	_tcscat(cbuf,str);
	_tcscat(lptchTmp,cbuf);
		CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrTitle = _T("Save Dome Rotation Data into File");
	dlg.m_ofn.lpstrFilter = _T("Optorun Assitant log(*.log)\0*.log\0");
	dlg.m_ofn.lpstrInitialDir = lptchTmp; 
	dlg.m_ofn.lpstrFile = lptchTmp;

	if(dlg.DoModal()==IDOK)
	{
		str = dlg.GetPathName();
		if(-1==str.Find(_T(".log")))
		{str +=_T(".log");}

		fn_SaveDomeList(str);
	}
}

void COARotatorView::OnBnClickedBtnStartDomeTest()
{
	// >> ��ȡ��ǰת���б���ʼ��. �Լ���ת��λ��
	int iItem = 0;
	if( BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_START_FROM_SELECTION_DOME ))->GetCheck())
	{
		POSITION pos = m_ListctrlDome.GetFirstSelectedItemPosition();
		if(pos)
		{//�ڵ�ǰѡ����֮ǰ����
			iItem = m_ListctrlDome.GetNextSelectedItem(pos);
		}
	}

	//ȷ�ϵ�ǰ��С��������
	if(iItem >= m_ListctrlDome.GetItemCount() )
	{//
		return;
	}

	fn_SaveRestoreDomeConfigure(true);	//�洢��ǰ�Ĳ���.

	fn_StartDomeTest(iItem);
}

void COARotatorView::OnBnClickedBtnCancelDomeTest()
{
	bool bTest;
	EnterCriticalSection(&gCriticalSection_DomeState);
		bTest = g_bTestDomeRotation;
	LeaveCriticalSection(&gCriticalSection_DomeState);

	if(bTest)
	{
		if(IDYES == AfxMessageBox(g_item3_String[_ITEM3_STR_DOME_TEST_EXIT_Q].IsEmpty()?TEXT("���ڲ���Domeת��,�����Ҫֹͣ����?\r\nIt's testing Dome rotation, Are you sure to cancel?"):g_item3_String[_ITEM3_STR_DOME_TEST_EXIT_Q]
			,MB_YESNO | MB_ICONQUESTION|MB_DEFBUTTON2))
		{
			////��Ҫ�ܷ�ӳ����ȷ���û�ֹͣ
			EnterCriticalSection(&gCriticalSection_DomeState);
			g_bTestDomeRotation = false; 
			g_bTestDomeStopByUser = true;
			LeaveCriticalSection(&gCriticalSection_DomeState);
			
			fn_SaveRestoreDomeConfigure(false);//�ָ�Dome״̬.
			fn_ShowDomeWindow();
			return;

		}
	}
}

/****************************************************************
�������ƣ�fn_SaveRestoreDomeConfigure
����˵������¼���ָ�����Domeǰ��״̬����Ϊ�ڲ����У���������Щ״̬��
���������bSave true: ����ǰDome״̬������ڴ���
				false:�ָ��������Dome״̬
#define _ZZY_DOME_ROTATE_LOW	1
#define _ZZY_DOME_ROTATE_HIGH	0
����ֵ���ݲ���.
�����и����˸���״̬�µ�Speed.
*****************************************************************/
bool COARotatorView::fn_SaveRestoreDomeConfigure(bool bSave)
{
	static bool bParaSaved = false;
	static int  iSpeed = 10;
	static int  iLow   = _ZZY_DOME_ROTATE_LOW;
	static int  iAUTO_OFF_MANUAL = _ZZY_DOME_SWITCH_OFF;
	static int  iHighSpeed = 30;//���ڻָ� HighState ���ٶ�ֵ

	if(bSave)
	{
		g_pDomeRotateThread->fn_GetDomeHighSpeed(iHighSpeed);
		g_pDomeRotateThread->fn_GetDomeRotationState(iSpeed, iLow);
		g_pDomeRotateThread->fn_GetDomeRotationControl(iAUTO_OFF_MANUAL);
	}
	else
	{
		g_pDomeRotateThread->fn_SetDomeHighSpeed(iHighSpeed);
		g_pDomeRotateThread->fn_SetDomeRotationState(iSpeed, iLow);
		g_pDomeRotateThread->fn_ToggleDomeSwitch(iAUTO_OFF_MANUAL);
	}
	
	return true;
}


/*****************************************************************************
�������ƣ�fn_StartDomeTest
˵������ĳ���кſ�ʼ����Dome���
���������int iItem����ʼ ���Ե��к�
******************************************************************************/
bool COARotatorView::fn_StartDomeTest(int iItem)
{
	
	if( iItem >= m_ListctrlDome.GetItemCount())
	{//��ֹ�������� ɾ��!
		EnterCriticalSection(&gCriticalSection_DomeState);
			g_bTestDomeRotation = false;
		LeaveCriticalSection(&gCriticalSection_DomeState);
		fn_ShowDomeWindow();
		return false;
	}

	g_stDomeRotationState.iCurListItem = iItem;
	//�����ǰ���������� //�Է��ǵڶ��β���.
	m_ListctrlDome.SetItemText(iItem,_ROTATOR_DOME_LIST_COL_START_CNT,TEXT(""));
	m_ListctrlDome.SetItemText(iItem,_ROTATOR_DOME_LIST_COL_END_CNT,TEXT(""));
	m_ListctrlDome.SetItemText(iItem,_ROTATOR_DOME_LIST_COL_WAITING_TIME,TEXT(""));
	m_ListctrlDome.SetItemText(iItem,_ROTATOR_DOME_LIST_COL_TEST_TIME,TEXT(""));	
	m_ListctrlDome.SetItemText(iItem,_ROTATOR_DOME_LIST_COL_SEC_PER_REVOLUTION,TEXT(""));
	m_ListctrlDome.SetItemText(iItem,_ROTATOR_DOME_LIST_COL_REVOLUTION_PER_MINUTE,TEXT(""));

	m_ListctrlDome.SetItemState(iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_ListctrlDome.EnsureVisible(iItem,false);

	//��ȡ��ǰ����Ŀ���ٶ�
	CString str = m_ListctrlDome.GetItemText(iItem,_ROTATOR_DOME_LIST_COL_SPEED);
	g_stDomeRotationState.iTargetSpeed = _ttoi(str);
	ASSERT(g_stDomeRotationState.iTargetSpeed< 51);

	g_stDomeRotationState.iRotationTime = 0;
	g_stDomeRotationState.bStop = false;

	//��ʼ��ʱ
	g_stDomeRotationState.dwStartTime = GetTickCount();

	// >> ������Ϣ���̣߳��������ѭ����
	EnterCriticalSection(&gCriticalSection_DomeState);
		g_bTestDomeStopByUser = false;
		g_bTestDomeRotation = true;
	LeaveCriticalSection(&gCriticalSection_DomeState);
	g_pDomeRotateThread->PostThreadMessage(WM_ITEM3_DOME_ROTATE_MONITOR,0,(LPARAM)g_stDomeRotationState.iTargetSpeed);
	fn_ShowDomeWindow();
	return true;
}
void COARotatorView::fn_ShowDomeWindow(void)
{
	bool bTest = false;
	EnterCriticalSection(&gCriticalSection_DomeState);
	bTest = g_bTestDomeRotation;
	LeaveCriticalSection(&gCriticalSection_DomeState);

	GetDlgItem(IDC_BTN_AUTOCREATE_DOME_SPEED)->EnableWindow(!bTest);
	//GetDlgItem(IDC_COMBO_DOME_SPEED)->EnableWindow(bTest);	
	//GetDlgItem(IDC_BTN_INSERT_DOME_SPEED)->EnableWindow(bTest);
	//GetDlgItem(IDC_BTN_DELETE_DOME_ROW)->EnableWindow(!bTest);
	//GetDlgItem(IDC_BTN_SAVE_DOME_LIST)->EnableWindow(!bTest);
	GetDlgItem(IDC_BTN_START_DOME_TEST)->EnableWindow(!bTest);
	GetDlgItem(IDC_BTN_CANCEL_DOME_TEST)->EnableWindow(bTest);

}

/********************************************************************************
�������ƣ�fn_FigureDomeThreadData
����˵�������� ����g_stReadDomeThreadDataƽ�����ʱ�估������� ʱ��
���������double& dAveSecPR��ƽ�����ʱ��
		  double& dFiguredSecPR,������ļ��ʱ��
����ֵ��  int����
			0:û�п�ʼԤ��
			1:��ƽ��ֵ������
			2:ƽ��ֵ��Ԥ��ֵ�������ã�����ʱ�в�����ֹͣ����
			50:�Ѿ��������ת��������ǿ��ֹͣ.
			100:Ԥ��ȡ�óɹ����Ѿ������� Ԥ������������ֹͣ�ˡ�
********************************************************************************/
#define _XYZ_DOME_FIGURE_MIN_ROTATIONS	10   //��ʼ��������СȦ��
#define _XYZ_DOME_FIGURE_MAX_ROTATIONS	1000 //�������ô��Ȧ����ʹ�Բ��ܳɹ������������Ҳǿ�� ���� true;
#define _XYZ_DOME_ROTATION_SecPR_RESOLUTION 0.015
int COARotatorView::fn_FigureDomeThreadData( double& dAveSecPR, double& dFiguredSecPR)
{
	static double static_pdSecPR[_XYZ_DOME_FIGURE_MAX_ROTATIONS]; //��Ȧ���ʱ�䡣//��¼ÿ�μ��ʱ����Ȧ���ı�ֵ, ��������һ��. 
	static double static_pdSecPR_LineRegression[_XYZ_DOME_FIGURE_MAX_ROTATIONS];  //���μ�¼����Ԥ��ֵ
	
	//����Ԥ�⣬ʡ�� ǰ5�����ݣ��ӵ�8�����ݿ�ʼ����(����3)��
	static double static_pd_b[_XYZ_DOME_FIGURE_MAX_ROTATIONS]; //���ڴ������Ԥ���� б�� y = a + bx�е�b.
	static double x_constant1[_XYZ_DOME_FIGURE_MAX_ROTATIONS]; //ȫ1����������Ԥ��
	static bool   x_b_constant1_Initialed = false;//�Ƿ��ʼ����
	if(!x_b_constant1_Initialed)
	{//ֻ��ʼ��һ��
		for(int i=0;i<_XYZ_DOME_FIGURE_MAX_ROTATIONS;i++)
		{
			x_constant1[i] = i;
		}
		x_b_constant1_Initialed = true;
	}

	dAveSecPR = 0;
	dFiguredSecPR = 0;
	int iDomeDataSize = g_stReadDomeThreadData.size();
	if( iDomeDataSize <= 1 )
	{
		return 0;
	}
	else
	{
		const int ci_leaveoutcnt = 5;  //�۳�ǰ x �����ݲ�������㡣//5

		//���ݱ���ʱ���Ѿ�����ÿ����¼֮��� ת����ͬ.
		if( iDomeDataSize == 2)
		{
			dAveSecPR = (double) ((DWORD)(g_stReadDomeThreadData.back().dwTickTime-g_stReadDomeThreadData.front().dwTickTime))/((WORD)(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData.front().wDomeRotationCount));
			dAveSecPR *= 0.001; //ms->s
		}
		else if( iDomeDataSize < 10) 
		{////��һ�����ݼ�����ܲ��ã���Ϊ��ʼλ�ò��ܱ�֤
			dAveSecPR = (double) ((DWORD)(g_stReadDomeThreadData.back().dwTickTime-g_stReadDomeThreadData[1].dwTickTime))/((WORD)(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData[1].wDomeRotationCount));
			dAveSecPR *= 0.001; //ms->s
		}
		else //>10���ӵ�5Ȧ֮��ʼ
		{
			dAveSecPR = (double) ((DWORD)(g_stReadDomeThreadData.back().dwTickTime-g_stReadDomeThreadData[ci_leaveoutcnt].dwTickTime))/((WORD)(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData[ci_leaveoutcnt].wDomeRotationCount));
			dAveSecPR *= 0.001; //ms->s
		}		

		//ÿ��ֻ���¼һ����ֵ.
		static_pdSecPR[ iDomeDataSize - 1] =
			(double) ((DWORD)(g_stReadDomeThreadData.back().dwTickTime-g_stReadDomeThreadData[iDomeDataSize-2].dwTickTime))/((WORD)(g_stReadDomeThreadData.back().wDomeRotationCount-g_stReadDomeThreadData[iDomeDataSize-2].wDomeRotationCount));
		static_pdSecPR[ iDomeDataSize - 1] *= 0.001;//ms->s
		
		if(	iDomeDataSize > ci_leaveoutcnt+3)
		{
			COACnfgDoc::fn_LinearRegression(x_constant1[0],static_pd_b[iDomeDataSize-1],&x_constant1[ci_leaveoutcnt],&static_pdSecPR[ci_leaveoutcnt],iDomeDataSize-1-ci_leaveoutcnt);
			static_pdSecPR_LineRegression[iDomeDataSize-1] = x_constant1[0] + static_pd_b[iDomeDataSize-1];
			dFiguredSecPR = static_pdSecPR_LineRegression[ iDomeDataSize - 1];

			//�����涨Ȧ�����ٿ�ʼ����Ԥ��.
			if( iDomeDataSize>_XYZ_DOME_FIGURE_MIN_ROTATIONS && ((WORD)(g_stReadDomeThreadData.back().wDomeRotationCount - g_stReadDomeThreadData.front().wDomeRotationCount) > _XYZ_DOME_FIGURE_MIN_ROTATIONS) )
			{
				// �������
				// ����iEstimateDataNumber������Ԥ��֮��������dEstimatedErr�룬���� |б��| < dEstimatedSlope ;
				const double dEstimatedErr = 0.005;
				const double dEstimatedSlope = 0.005;//ֱ��б������ʲ�
				int iEstimateDataNumber = (iDomeDataSize > 40) ? iDomeDataSize/10 : 4; //�Զ��� 1/10�����ݽ��й���.
				if(iEstimateDataNumber > 6)
					iEstimateDataNumber = 6;
				if( fn_Is_x_near( &static_pdSecPR_LineRegression[iDomeDataSize-iEstimateDataNumber-1] ,iEstimateDataNumber, dEstimatedErr) 
					&& fn_Is_x_near( &static_pd_b[iDomeDataSize-iEstimateDataNumber-1] ,iEstimateDataNumber, dEstimatedSlope) )
				{
					return 100;
				}
				else
				{
					//TRACE1("%3f\r\n",static_pd_b[iDomeDataSize-1]);
					return 2;
				}
			}

			if( iDomeDataSize > _XYZ_DOME_FIGURE_MAX_ROTATIONS)
			{
				dFiguredSecPR = dAveSecPR;
				return 50;
			}
			else 
				return 1;
		}
		else
		{
			return 1;
		}

	}
	return 0;
}
//�ж����� n ��xֵ�Ƿ����ֵ����� d_epsilon֮��
bool COARotatorView::fn_Is_x_near( double* x,int n,double d_epsilon)
{
	if(n<=2)
		return false;
	double dmax = x[0];
	double dmin = x[0];
	for(int i=1;i<n;i++)
	{
		if( x[i] > dmax)
		{
			dmax = x[i];
		}
		if( x[i] < dmin)
		{
			dmin = x[i];
		}
	}
	return ( ( dmax - dmin ) < d_epsilon ) ;
}

//�����,ȡ�м�100���ƽ��ֵ.
//����ֵ,ƫ�� 

		
//			
//double dT0 = 0.001; ////ƽ��ÿȦ��ʱ�䣬��λ s
//			int n1,n2,n3,n4; //1��,2��,3��,4����׼ʱ��ĸ���.
/********************************************************************************************
�������ƣ�fn_CalcDomeRotationData
����˵��������Domeת��ֵ
����˵����double& dT0 , [output] Domeת����Ȧƽ��ʱ�䣬��λΪ s
		  int& n1,      [output] ת��ʱ����ƽ��ʱ�丽���ļ�¼����
		  int& n2,		[output] ת��ʱ����2��ƽ��ʱ�丽���ļ�¼����
		  int& n3,		[output] ת��ʱ����3��ƽ��ʱ�丽���ļ�¼����
		  double dStandValue,    ƽ��ʱ��ı�׼ʱ�䣮������ƽ��ʱ�䡡~ ��׼ʱ�䣩
�ٶ�1:	��ƽ��ʱ�䡡Լ���ڡ������ı�׼ʱ�䣬���߾���ֵӦ��� < 0.1. ������Ŀ���ǵõ���ƽ��ʱ���ʵ��ֵ.
�ٶ�2:    ÿ��ת�����ʱ�䡡Ӧ�ֲ��� ��׼ʱ���n����+/- ƫ��ֵ��Χ֮�ڣ����ƫ��ֵͬ���Ǹ���ʵ�ʵó��Ĺ���ֵ.
˵��:	  �۳�ǰ��_ROTATE_DOME_NOT_COUNT�������ݲ����м���.
		  �����ڡ�g_stReadDomeThreadData��֮��			
********************************************************************************************/
int	COARotatorView::fn_CalcDomeRotationData(double& dT0, int& n1, int& n2,int& n3,int& n4, double dStandValue)
{
	const DWORD dwEpsilon = 500; //ms, ƫ��ֵ, ÿ��ת�������Ӧ�ֲ��� ��׼ʱ���n����+/- ƫ��ֵ��Χ֮��. ����ֵ
	n1=n2=n3=n4=0;
	int iDataSize = g_stReadDomeThreadData.size();
	DWORD dwStandValue = dStandValue * 1000; //ת���� DWORDֵ�������.
	DWORD  dwTotalTime = g_stReadDomeThreadData.back().dwTickTime - g_stReadDomeThreadData[_ROTATE_DOME_NOT_COUNT].dwTickTime;
	DWORD  dwTickInterval; //��ʱ����
	//�޶���Χ
	DWORD  dw1Down = dwStandValue - dwEpsilon;
	DWORD  dw1Up   = dwStandValue + dwEpsilon;
	DWORD  dw2Down = 2*dwStandValue - dwEpsilon;
	DWORD  dw2Up   = 2*dwStandValue + dwEpsilon;
	DWORD  dw3Down = 3*dwStandValue - dwEpsilon;
	DWORD  dw3Up   = 3*dwStandValue + dwEpsilon;
	DWORD  dw4Down = 4*dwStandValue - dwEpsilon;
	DWORD  dw4Up   = 4*dwStandValue + dwEpsilon;

	for(int i=_ROTATE_DOME_NOT_COUNT; i<iDataSize; i++)
	{
		if( 1 == (g_stReadDomeThreadData[i].wDomeRotationCount - g_stReadDomeThreadData[i-1].wDomeRotationCount) )
		{
			dwTickInterval = g_stReadDomeThreadData[i].dwTickTime - g_stReadDomeThreadData[i-1].dwTickTime;
			if( dwTickInterval >= dw1Down && dwTickInterval <= dw1Up )
			{
				n1++;
			}
			else if( dwTickInterval >= dw2Down && dwTickInterval <= dw2Up )
			{
				n2++;
			}
			else if( dwTickInterval >= dw3Down && dwTickInterval <= dw3Up )
			{
				n3++;
			}
			else if( dwTickInterval >= dw4Down && dwTickInterval <= dw4Up )
			{
				n4++;
			}
			else
			{
				//�۳�ʱ�����������޶���Χ֮�ڵ�ʱ��
				dwTotalTime -= g_stReadDomeThreadData[i].dwTickTime - g_stReadDomeThreadData[i-1].dwTickTime;
			}

		}
		else
		{//�۳�Ȧ�����Ǽ��1��ʱ��
			dwTotalTime -= g_stReadDomeThreadData[i].dwTickTime - g_stReadDomeThreadData[i-1].dwTickTime;
		}
	}

	dT0 =(double) dwTotalTime /(n1+2*n2+3*n3+4*n4);

	return 0;
}