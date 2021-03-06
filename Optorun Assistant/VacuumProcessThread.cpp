// VacuumProcessThread.cpp : implementation file
//
#include "stdafx.h"
#include "Optorun Assistant.h"
#include "VacuumProcessThread.h"
#include "OACnfgDoc.h"

//Zhou Yi 2018 - 8 - 27 次外部变量没有使用，因此将其注释
//extern COACnfgDoc g_COACnfgDoc; 

UINT_PTR g_VacuumTimer = NULL;

char  vt_pszSendBuf[512]; //vacuum thread buffer
char  vt_pszRecvBuf[512];

//仅在Timer中使用，供流程判断
bool  bPenningRd  = false;
bool  bIonGaugeRd = false;
int   nPlcRdTryTimes = 0;

bool  bVacuumTest = false; //临时变量

stVacuumTestState sVacuumState;//临时变量


#define _VACUUM_THREAD_DATA_BUF_COUNT 20
//用于线程函数回传消息使用
struct _stVacuumThreadData l_stVacuumThreadData[_VACUUM_THREAD_DATA_BUF_COUNT+2]; //用于回传消息缓冲，本线程适用的变量
int    l_iThreadMonitorData = 0;              //当前使用到的MonitorData号, 本线程内部使用 l_ 前缀(Local Variable)

/*
程序启动后进入 状态 2,bTesting=false， 
用户点击开始后 bTesting=true. 线程中判断 从 2->1，则发送开始记录信息，并转入1
               1状态下，如果没有响应，则临时转到 3，在线程响应函数末尾处 转入 2.
所以实际上线程响应函数 开始时的真正状态只有 1和2，而3则是临时状态，可以用其他变量进行判别。
*/
#define _VACUUM_THREAD_TIMER_WORKING      1 //正常读取状态
#define _VACUUM_THREAD_TIMER_WAITING      2 //没有开始记录时的等待状态
int     giVacuumThreadState = _VACUUM_THREAD_TIMER_WAITING;//初始等待状态  //只在本线程内部使用，故不用多线程同步保护。


VOID CALLBACK gfn_VacuumTimerProc(
	HWND hwnd,     // handle of window for timer messages
	UINT uMsg,     // WM_TIMER message
	UINT idEvent,  // timer identifier
	DWORD dwTime   // current system time
)
{
	if(g_VacuumTimer == idEvent)
	{
		//debugging 2007.11.21
	
		PostMessage(NULL,WM_VACUUM_THREAD_TIMER_EX,0,0);
		return;//以下取消
	}
	return;
}
// CVacuumProcessThread

IMPLEMENT_DYNCREATE(CVacuumProcessThread, CPlcRdWrThread)

CVacuumProcessThread::CVacuumProcessThread()
{
	m_dwLastError = 0;	
	m_hWnd = NULL;
}

CVacuumProcessThread::~CVacuumProcessThread()
{
}

BOOL CVacuumProcessThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	ASSERT(m_hWnd!=NULL);
	g_VacuumTimer = SetTimer(NULL,1,1000,gfn_VacuumTimerProc);
	
	//debugging
	PostMessage(m_hWnd,WM_ITEM2_TEST,0,0);

	return TRUE;
}

int CVacuumProcessThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}
BEGIN_MESSAGE_MAP(CVacuumProcessThread, CWinThread)
	ON_THREAD_MESSAGE(WM_VACUUM_THREAD_TIMER_EX,  OnVacuumThreadTimerEx)
	ON_THREAD_MESSAGE(WM_ITEM2_VACUUM_THREAD_QA,  OnItem2VacuumThreadQA)
END_MESSAGE_MAP()


//从 Item2 发送来的消息响应函数
void CVacuumProcessThread::OnItem2VacuumThreadQA(WPARAM wParam, LPARAM lParam)
{
	return;
}

//响应处理线程定时器消息
void CVacuumProcessThread::OnVacuumThreadTimerEx(WPARAM wParam, LPARAM lParam)  //次函数在启动软件后，在定时器的作用下一直处于重复运行状态，每隔一秒调用一次
{
	ASSERT(NULL != m_hWnd);
	//static DWORD dwRdbytes = 0;
	static bool bTesting;
	EnterCriticalSection(&gCriSec_Vacuum);
		bTesting = g_stVacuumTestEx.bTesting ;
	LeaveCriticalSection(&gCriSec_Vacuum);

	static DWORD dwStart = GetTickCount();  
	
	if(bTesting)  //检测到面板上的Start被按下之后，每秒钟都会执行和判断
	{
		if( _VACUUM_THREAD_TIMER_WAITING == giVacuumThreadState )
		{   //如果之前的状态是等待，则应转到正常工作状态，通知主线程本线程启动操作
			//通知主线程,本线程开始工作!
			l_iThreadMonitorData++;
			if(l_iThreadMonitorData >= _VACUUM_THREAD_DATA_BUF_COUNT)				//debug:是否少了两组数据，因为定义时候的COUNT为20+2！
			{
				l_iThreadMonitorData = 0;
			}

			GetLocalTime(&l_stVacuumThreadData[l_iThreadMonitorData].st);
			l_stVacuumThreadData[l_iThreadMonitorData].iStatus = _VACUUM_THREAD_START;

			//l_stVacuumThreadData[l_iThreadMonitorData].iBufNumber = gi_MonitorData;
			l_stVacuumThreadData[l_iThreadMonitorData].dwTickTime = GetTickCount();    //ZHOU YI：开始计时，计时初始化
			
			PostMessage(m_hWnd,WM_VACUUM_THREAD_TO_ITEM2,0,(LPARAM)&l_stVacuumThreadData[l_iThreadMonitorData]);
			giVacuumThreadState = _VACUUM_THREAD_TIMER_WORKING; //进入正常工作状态（采集数据）
		}

		//进行测量，第二组数据开始
		l_iThreadMonitorData++;
		if(l_iThreadMonitorData >= _VACUUM_THREAD_DATA_BUF_COUNT)
		{
			l_iThreadMonitorData = 0;
		}
		l_stVacuumThreadData[l_iThreadMonitorData].dwTickTime = GetTickCount(); 
		GetLocalTime(&l_stVacuumThreadData[l_iThreadMonitorData].st);

		bool bOK = true;
		for(int i=0;i<_VACUUM_READ_PLC_ADDRESS_CNT;i++)
		{
			if(g_iVacuumRdDM_IO[i]==0)
			{// DM
				if( ReadPLCDMEvent(g_wVacuumRdAddress[i],l_stVacuumThreadData[l_iThreadMonitorData].wRdData[i],g_bVacuumRdDecimal[i]) )
				{
					continue;
				}
				else
				{
					//重试一次
					if( ReadPLCDMEvent(g_wVacuumRdAddress[i],l_stVacuumThreadData[l_iThreadMonitorData].wRdData[i],g_bVacuumRdDecimal[i]) )
					{
						continue;
					}
					else			//20160608
					{
						EnterCriticalSection(&gCriSec_Vacuum);
						g_stVacuumTestEx.bTesting = false;
						LeaveCriticalSection(&gCriSec_Vacuum);
						bOK = false;
						break;
					}
				}
			}
			else
			{//CIO
				ReadPLCCIOEvent(g_wVacuumRdAddress[i],l_stVacuumThreadData[l_iThreadMonitorData].wRdData[i]);   //读取PLC数据
			}
		}
		
		if(bOK) 
		{
			l_stVacuumThreadData[l_iThreadMonitorData].iStatus = _VACUUM_ACK_RIGHT; //读取有数据
		}
		else
		{
			l_stVacuumThreadData[l_iThreadMonitorData].iStatus = _VACUUM_NO_ANSWER;	//读取无反应
		}

		PostMessage(m_hWnd,WM_VACUUM_THREAD_TO_ITEM2,0,(LPARAM)&l_stVacuumThreadData[l_iThreadMonitorData]);  //Zhou Yi 数据采集成功之后，发送一个消息并且附加一个数据信息传出去。
	}
	else
	{//不在测试过程
		//当前的工作状态如果是正常工作状态，则停止，并转入等待状态
		//此时是用户按了停止按钮或者 停止状态到达。
		if( _VACUUM_THREAD_TIMER_WORKING == giVacuumThreadState )
		{
			l_iThreadMonitorData++;
			if(l_iThreadMonitorData >= _VACUUM_THREAD_DATA_BUF_COUNT)
			{
				l_iThreadMonitorData = 0;
			}

			GetLocalTime(&l_stVacuumThreadData[l_iThreadMonitorData].st);
			l_stVacuumThreadData[l_iThreadMonitorData].iStatus = _VACUUM_THREAD_END;

			PostMessage(m_hWnd,WM_VACUUM_THREAD_TO_ITEM2,0,(LPARAM)&l_stVacuumThreadData[l_iThreadMonitorData]);
			
			giVacuumThreadState = _VACUUM_THREAD_TIMER_WAITING; //进入等待工作状态
		}
	}	
}
// 取得 APC的真空度
bool CVacuumProcessThread::fn_GetVGCch3Pressure(double& dPressure)
{
	WORD wPenningValue;
	if( ReadPLCDMEvent(gc_DM_PenningValue,wPenningValue,true) )
	{
		//dPressure = g_COACnfgDoc.fn_TransPenningVoltPa(double(wPenningValue)); //2008.05.19
		dPressure = g_APCGauge.TransVal2Pascal(wPenningValue);
		return true;
	}
	else
	{
		return false;
	}
}

// 检查主阀是否开启
// Gener:   3877.06
// OTFC MV2:3877.07
bool CVacuumProcessThread::fn_CheckMainValveStatus(bool& bOpen)
{
	WORD wMainValve;
	if( ReadPLCCIOEvent(3877,wMainValve) )
	{
		if( wMainValve & (1<<6))
		{
			bOpen = true;
		}
		else
		{
			bOpen = false;
		}
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


//Gener SERIALA:03805.13
//OTFC     MV2 :03805.14
bool CVacuumProcessThread::fn_SwitchMainValve(void)
{
	if( !WritePLCCIOEvent(3805,1<<13))
	{
		return false;
	}

	Sleep(200);

	if( !WritePLCCIOEvent(3805,1<<14))
	{
		return false;
	}
	return true;

	/*
	//2007.12.20. S561并不能关闭 MV2
	if( WritePLCCIOEvent(3805,0))
	{
		return true;
	}
	Sleep(200);

	if( WritePLCCIOEvent(3805,1<<14))
	{
		return true;
	}
	*/
}


//模拟触摸屏动作， 所以首先调用方要确认处于　<手动排气>　模式．
// 关闭主阀，由调用方确认主阀处于开启状态.
// 调用结束后需要进行状态确认（１０秒内）.
//3876.10<手动排气>　3876.09<自动排气>
//因为二者是互斥的, 只要查询一个即可
bool CVacuumProcessThread::fn_CheckExhaustMode(bool& bExhaustModeManual)
{
	WORD wWord;
	if( ReadPLCCIOEvent(3876,wWord) )
	{
		if( wWord & (1<<10))
		{
			bExhaustModeManual = true;
		}
		else
		{
			bExhaustModeManual = true;
		}
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


//切换后，调用方需要确认．
//<手动> SERIALA:03805.03
//<自动> SERIALA:03805.02
bool CVacuumProcessThread::fn_SetExhaustMode(bool bManual)
{
	if( WritePLCCIOEvent(3805, bManual ? 1<<03 : 1<<02 ))
	{
		return true;
	}
	return false;
}

// 查询是扩散泵
//拡散ﾎﾟﾝﾌﾟ 9053.00  0=無 1=有
bool CVacuumProcessThread::fn_IsDiffusionPump(bool& bDiffusionPump)
{

	WORD wWord;
	if(ReadPLCDMEvent(9053,wWord))
	{
		bDiffusionPump = (((wWord & (1<<00))!=0) || (wWord & (2<<00))!=0);  //20160329jiang
		return true;
	}
	return false;
}

bool CVacuumProcessThread::fn_IsIonGaugeRemoteControl(bool& bRemoteControl)
{
	WORD wWord;
	if( ReadPLCCIOEvent(3871,wWord) )
	{
		if( wWord & (1<<8))
		{
			bRemoteControl = true;
		}
		else
		{
			bRemoteControl = true;
		}
		return true;
	}
	else
	{
		return false;
	}
	return false;
}
