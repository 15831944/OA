#pragma once
#include "afx.h"

#include "..\XMLMarkup\Markup.h"
#include "..\SyncCommSource\SysmacPLC.h"
#include "IonGauge.h"
#include "APCGauge.h"
#include "VacuumProcessThread.h"
#include "gascontroller.h"
#include "odmcom.h"
#include "VGC500.h"
#include "IBGLCom.h"
#include "ZYGraphPlot.h"
//������Ϊ����ʹ�ã���ʽrelease����Ӧȡ���ö���
#define _ZHANGZIYE_DEBUG
//#undef  _ZHANGZIYE_DEBUG

extern const double g_dProgramVersion;
extern const double g_dDataVersion;
extern bool g_bOffLineDebug;


enum   enumOptorunCoaterType{_en_Coater_UNKNOWN_TYPE,_en_Coater_Gener,_en_Coater_OTFC,_en_Coater_AR
};
extern enum enumOptorunCoaterType g_enOptorunCoaterType;

//�Ѿ�������MainFrm���ˣ�������ÿ�
//#define WM_INIT_OTHER_FORM  (WM_USER + 401)
//#define WM_MENU_SELECT_LNG_CMD    (WM_USER + 410)  //Ԥ��100���ռ䣬��һ�����붨��� 511

#define MAIN_FRM_START_WM	(WM_USER + 511) 
#define ITEM1_START_WM		(WM_USER + 600)
#define ITEM2_START_WM		(WM_USER + 800)
#define ITEM2_START_VACUUM_THREAD (WM_USER+850)
#define OA_APP_START_WM     (WM_USER + 900)
#define WM_ITEM3_ROTATOR_START  (WM_USER+920) 

		//   �趨����
		
#define _VACUUM_EXHAOSUT_PRESSURE_OK_TYPE 0  //0: ��նȴﵽ(1.0E-4), �ȶ�ʱ��  
#define _VACUUM_EXHAOSUT_END_TIME_OK_TYPE 1  //1: ֹͣʱ��ﵽ

#define _MACHINE_TYPE_GENER      0
#define _MACHINE_TYPE_SUPERAR    1
#define _MACHINE_TYPE_AR     2
#define _MACHINE_TYPE_1800   3
#define _MACHINE_TYPE_1550   4
#define _MACHINE_TYPE_1300   5
#define _MACHINE_TYPE_1100   6
#define _MACHINE_TYPE_900    7
extern int g_iMachineType;


#define  _VACUUM_PRESSURE_MAX (2*24*3600)
extern int    g_iVacuumPressurePos;     //��ǰ�����ڴ洢��նȵ�λ��.
extern double g_dVacuumVGCCh3[];   //�洢 VGC023 ch3 (penning)��նȣ��Ѿ�ת����Pascal��
extern double g_dVacuumIonGauge[]; //�洢 Ion Gauge (penning)��նȣ��Ѿ�ת����Pascal��
extern double g_dVacuumTime[];     //2008.04.08

extern int    g_iVacuumTestCnt;     //ÿ�β��Կ�ʼ�����ݵ����Ĵ���. //ͬ���� 
extern int    g_iVacuumStartPos;    //ÿ�β��Ե�����λ�� �洢�ռ����ʼλ��. 

#define OA_APP_DISPLAY_VAC_VALUE	(OA_APP_START_WM+0)
#define OA_APP_VACUUM_THREAD_ERR_OCCUR (OA_APP_START_WM+1)
#define WM_PLC_RD_WR_MSG    (OA_APP_START_WM+2)


//��item x ���͵���Ϣת����item y����mainframe��ת
//��Դ x �� HIWORD(WPARAM), y��LOWORD(WPARAM)��, LPARAMΪ��ת������Ϣ��ת������Ϣû�в���.
#define MAIN_FRM_ITEMx_2_ITEMy     (MAIN_FRM_START_WM+0)
#define MAIN_FRM_TEST	           (MAIN_FRM_START_WM+1)
#define MAIN_FRM_DISPLAY_VAC_VALUE (MAIN_FRM_START_WM+2)

//�� vacuum thread���������쳣������Ϣ
#define MAIN_FRM_VACUUM_THREAD_ERR_OCCUR     (MAIN_FRM_START_WM+3) 

#define WM_ITEM1_DISPLAY_PLC_STATE (ITEM1_START_WM + 0)
#define WM_ITEM1_ADD_RUN_HISTORY   (ITEM1_START_WM + 1)
#define WM_ITEM1_GET_NAMES         (ITEM1_START_WM + 2)

#define WM_ITEM2_DISPLAY_VAC_STATE (ITEM2_START_WM + 0)
#define WM_ITEM2_DISPLAY_VAC_VALUE (ITEM2_START_WM + 1)//��ʾ����������ն�ֵ

#define WM_ITEM2_VACUUM_THREAD ( ITEM2_START_WM + 2) //�� Vacuum Thread����������Ϣ
#define WM_ITEM2_TEST (ITEM2_START_WM+3)

#define WM_ITEM2_VACUUM_THREAD_QA (ITEM2_START_WM+4) //��Item2 ���͸�Vacuum Thread����Ϣ�����ڲ�ѯ��ʼ����

#define WM_VACUUM_THREAD_TO_ITEM2 (ITEM2_START_VACUUM_THREAD+0)
#define WM_VACUUM_THREAD_TIMER_EX (ITEM2_START_VACUUM_THREAD+1)
#define WM_VACUUM_THREAD_ITEM2_QA (ITEM2_START_VACUUM_THREAD+2) //��Vacuum Thread ���͸�Item2����Ϣ��������Ӧ��ʼ����


#define WM_ITEM3_DRAW_CRUCIBLE   (WM_ITEM3_ROTATOR_START+0)
#define WM_ITEM3_HEARTH_ROTATE_MONITOR   (WM_ITEM3_ROTATOR_START+1) //from view to thread
#define WM_ITEM3_HEARTH_ROTATE_DATA   (WM_ITEM3_ROTATOR_START+2) //from thread to view

#define WM_ITEM3_DOME_ROTATE_MONITOR  (WM_ITEM3_ROTATOR_START+3) //from view to thread
#define WM_ITEM3_DOME_ROTATE_DATA   (WM_ITEM3_ROTATOR_START+4) //from thread to view

//��ɫ����
extern const COLORREF gc_clrBlack;
extern const COLORREF gc_clrTrying;
extern const COLORREF gc_clrOK;
extern const COLORREF gc_clrAlert;
extern const COLORREF gc_clrWhite;
extern const COLORREF gc_clrRunning;

extern double   gc_dMaxPenningPa;
extern double   gc_dMinPenningPa;
extern double   gc_dMaxIonGaugePa;
extern double   gc_dMinIonGaugePa;

//DM��ַ����
extern const WORD   gc_DM_machinetype;
extern const WORD   gc_DM_sequencer;
extern const WORD   gc_DM_touchpanel;
extern const WORD   gc_DM_IonGaugeType;
extern const WORD   gc_DM_APCGaugeType;
extern const WORD   gc_DM_IonGaugeValue;
extern const WORD   gc_DM_PenningValue;

//�����ĸ���ַ�����������������г�ʼ��.
extern const WORD   gc_DM_AddressHeaterTempSVPV;
extern const WORD   gc_DM_AddressDome2TempSVPV; //����¶�����ֵ�뵱ǰֵ��2����ַ
#define _TEMP_MONITOR_HEATER_PV 0
#define _TEMP_DOME_HEATER_PV	1
#define _TEMP_MONITOR_HEATER_SV 2
#define _TEMP_DOME_HEATER_SV	3
#define _TEMP_DOME2_HEATER_PV   4
#define _TEMP_DOME2_HEATER_SV   5
#define _TEMP_HEATER_NUMBER     6
extern WORD gcp_DM_HeaterTemp[];

extern const int gc_plc_str_length;

//����ѡ���ַ�����
#define _OADOC_LNG_MENU_NAME	0
#define _OADOC_LNG_FILE_NAME	1

//���ϵͳ���ַ���
#define _OA_STR_SYSTEM_NUMBER				11 //����
#define _OA_STR_SYSTEM_ERROR_TEST			0 //��0��,���ڲ��ԣ�������
#define _OA_STR_SYSTEM_COMM_OBJECT_TIMEOUT  1
#define _OA_STR_SYSTEM_PLC_COMM_ERROR       2
#define _OA_STR_SYSTEM_PLC_COMM_API_ERROR   3
#define _OA_STR_SYSTEM_PLC_COMM_ENDCODE_ERR 4
#define _OA_STR_SYSTEM_CHANGE_LANGUAGE_NOTE 5
#define _OA_STR_SYSTEM_IS_TESTING_VAC_ERROR 6
#define _OA_STR_SYSTEM_VAC_DATA_NOT_SAVE_ERR 7
#define _OA_STR_SYSTEM_HEARTH_DATA_NOT_SAVE  8
#define _OA_STR_SYSTEM_LNG_FILE_NOT_EXIST    9
#define _OA_STR_SYSTEM_IS_TESTING_DOME_ERROR 10
//Item 
#define _ITEM1_NEED_TEXT_ID_CNT   11//��Ҫ��xml��ȡ�ַ�����ID�����˴���IDֻ��Ӧһ���ı�
#define _ITEM1_NEED_HELP_TEXT	  6 //��Ҫ��xml��ȡ�İ����ַ�������������ר�ų����ڰ�������

extern const int gc_item2_Text_ID_CNT;
//#define _ITEM2_NEED_TEXT_ID_CNT 10

//Formview��
#define _OA_FORM_VIEW_NUMBER	3
#define _OA_FORM_VIEW_SYSTEM	0
#define _OA_FORM_VIEW_VACUUM	1
#define _OA_FORM_VIEW_ROTATOR	2

//PLC connection state
#define _ITEM1_COM_PLC_STATE_CNT  6 //com��plc�˿�����״̬��������ʾ������״̬����
#define _COM_PLC_STATE_NOT_CONNECT 	0
#define _COM_PLC_STATE_TRY_COMM 	1
#define _COM_PLC_STATE_OPEN_COM_ERROR 	2
#define _COM_PLC_STATE_COMM_NORMAL 	3
#define _COM_PLC_STATE_COMM_ERROR 	4//CANCEL
#define _COM_PLC_STATE_COMM_NORESPOND 4
#define _COM_PLC_STATE_COMM_CONTENT_ERROR 5

//item2 VAC state
#define _ITEM2_VAC_STATE_CNT        3
#define _ITEM2_VAC_STATE_NOT_CONNECT 0
#define _ITEM2_VAC_STATE_PLC_OK      1
#define _ITEM2_VAC_STATE_TESTING     2

//item2 ������Ҫʹ�õ����ַ���
#define _ITEM2_OTHER_STR_NUMBER    18 //����
#define _ITEM2_USING_ION_GAUGE     0 //��0��
#define _ITEM2_CANCEL_WARNING_TEXT 1
#define _ITEM2_STR_LEAKRATE_HELP   2
#define _ITEM2_STR_RECORDER_HELP   3
#define _ITEM2_STR_EXHUAST_HELP   4
#define _ITEM2_STR_MANUAL_EXHUAST_MODE_ERR 5
#define _ITEM2_STR_MAIN_VALVE_CLOSE_ERR 6
#define _ITEM2_STR_START_PRESSURE_ERR   7
#define _ITEM2_STR_WAITTING_TIME_OVER 8
#define _ITEM2_STR_PLC_COMM_ERR       9
#define _ITEM2_STR_PLC_COMM_ERR_STOP 10
#define _ITEM2_STR_TESTTIME_INVALID_ERR  11
#define _ITEM2_STR_INTERVAL_TIME_ERR  12
#define _ITEM2_STR_START_PRESSURE_INVALID 13
#define _ITEM2_STR_WAITINT_TIME_MIN_ERR 14
#define _ITEM2_STR_PRESSURE_STABLE_TIME_MIN_ERR 15
#define _ITEM2_STR_NOT_DP_MACHINE_ERR   16
#define _ITEM2_STR_IONGAUGE_NOT_REMOTE_ERR 17

//item3 ������Ҫʹ�õ����ַ���
#define _ITEM3_OTHER_STR_NUMBER        9 //����
#define _ITEM3_STR_HEARTH_TEST_EXIT_Q  0 //������;�˳�ʱ����
#define _ITEM3_STR_HEARTH_TEST_FINISH  1 //������������ϲ
#define _ITEM3_STR_HAERTH_ROTAT_POS_ERR 2  //ת�ﲻ��λ������
#define _ITEM3_STR_HEARTH_ROTAT_OVER_TIME 3 //ת����ʱ
#define _ITEM3_STR_PLC_COMM_ERR  4
#define _ITEM3_STR_ANNULAR_HEARTH_NOT_ROTAT 5 //��������û�а�Ԥ�ڿ�ʼת��
#define _ITEM3_STR_ANNULAR_HEARTH_CONTROL_WARNING 6 //������������ģʽ����ROTATE,����.
#define _ITEM3_STR_ANNULAR_HEARTH_MONITOR_REMIND  7 //����, ��MONITOR TYPE=2(OMRON)���һ�������ʱ��xx.x ��ʾ, 30.0����300.
#define _ITEM3_STR_DOME_TEST_EXIT_Q	 8 //������;�û�����˳�ʱ����.

//�� .cpp�ļ��������Ķ��� extern���������ⲿ������.h������á�
//��������� �������� ����ǰ Ӧ�� extern ����const��������� ���ܽ��������Ӵ���
extern CString g_strSystem[];
extern const TCHAR* g_strProductVersion;
extern TCHAR g_strBuffer[];
extern CString g_item1_ID_Text[];
extern const int    g_item1_ID[];
extern CString g_item1_Help_Text[];
extern CString g_item1_plc_state[];
//extern const TCHAR* g_item1_ID_Name[];

extern CString g_item2_Vac_state[];
extern CString g_item2_String[];
extern CString g_item2_ID_Text[];
extern const int    g_item2_ID[];

extern const int gc_item3_Text_ID_CNT;
extern CString g_item3_ID_Text[];
extern const int    g_item3_ID[];
extern CString g_item3_String[];

extern int giHelpItems;
extern CODMCom g_COM2_ODM;
extern CGasController g_COM4_GasController;
extern CVGC500  g_COM5_VGC500;
extern CIBGLCom g_COM3_IBGL;
extern CSysmacPLC g_plc;
extern CIonGauge  g_IonGauge; 
extern CAPCGauge  g_APCGauge;
extern CVacuumProcessThread* g_pVacProThread;
extern CPlcRdWrThread*       g_pPlcRdWrThread;
extern HANDLE g_hEvent_plc;
extern CRITICAL_SECTION gCriticalSection_State;
extern CRITICAL_SECTION gCriSec_Vacuum;
extern CRITICAL_SECTION gCriSec_UI; 

#define _VACUUM_THREAD_STATE_ZERO				0
#define _VACUUM_THREAD_STATE_WAITING_REPLY		1
#define _VACUUM_THREAD_STATE_REPLIED_ABORT		2
#define _VACUUM_THREAD_STATE_REPLIED_RETRY 	    3
//#define _VACUUM_THREAD_STATE_REPLIED_CONTI 	    4

struct stVacuumTestState
{
	bool bTest;
	int  iThreadState;
		//#define _VACUUM_THREAD_STATE_ZERO				    0
		//#define _VACUUM_THREAD_STATE_WAITING_REPLY		1
		//#define _VACUUM_THREAD_STATE_REPLIED_ABORT		2
		//#define _VACUUM_THREAD_STATE_REPLIED_RETRY 	    3
	int  iMainRouting;
	int  iSubRouting;
	int  iTryTimes;
};

#define _VACUUM_TESTTYPE_EXHAUST  1
#define _VACUUM_TESTTYPE_LEAKRATE 2
#define _VACUUM_TESTTYPE_RECORDER 3

//������Щֻ�� ���̹߳�����Щ�� ���߳� �� ���̶߳��ж�д�ġ�
// ������д��bTesting

//ĳһ��ֻ�ܲ��� ���ٻ�©��
//���٣��Զ����Ƴ���״̬
//�ֶ�����
struct stVacuumTestEx{
	//ֻ�� bTesting �� iCurTestType ���߳�֮���л�ʹ�á���Ҫͬ����������������Ĳ�����ȫ�����߳��ڣ�����ͬ��������
	//���м�����
	bool bTesting;          //��ǰ�Ƿ��ڲ����С����Կ�ʼʱ����������true�������߳��жϡ�
	                        //����ʱ�����߳�
	// ��ǰ����״̬����. �趨����. ��ʼ���Ժ��ǲ���ı��, ���Բ���Ҫͬ������..
	// 1,���Գ���; 2,����©��; 3:��¼��
	int  iCurTestType;   //���߳�д�������̶߳����� bTesting == true������£������ܻ�ı䡣

	struct stExhaust
	{
		//   �趨����
		//0: ��նȴﵽ(1.0E-4), �ȶ�ʱ��  
		//1: ֹͣʱ��ﵽ
		int  iOKType; //�������Ҫ��ɵ����� //�û�����. �����̱߳��ܡ�

		//���ò���, �Զ�ֹͣ��¼����֮��նȴﵽ������һ��ʱ��
		double dOKPressure;      //ϣ���ﵽ��ֹͣ��ն�, Ĭ�� 1.0E-4
		DWORD  dwOKPressureTime; //�ﵽϣ����ֹͣ��նȺ��ȶ�ʱ��(�ȶ�)��

		//���ò���, �Զ�ֹͣ��¼����֮ʱ�䵽��
		//��¼12Сʱ ����ն�.(��Ӽ�¼12Сʱ�����ն�)
		bool   bLastRecordPressure; //�Ƿ���Ҫ���һ����¼ 
		DWORD  dwLastRecordTime;    //��Ҫ��¼��ʱ��. (ʱ�̵����,�����ʱ�ﵽ dOKPressure���¼ֹͣ������. ���û�дﵽ�����ֱ���ﵽ...)

		//����ʱ��¼����
		double dLastPressure;

		//�����¼����
		//int  iFinishType; //ʵ��ֹͣʱ��¼��������
			//���ٲ�����ɵ�����
			//a, �ﵽ�趨����նȣ� ���ȶ� ʱ��.  <= 1.0E-4  ( �����ȶ� 1 ���ӣ��ڲ����� ) �� ֹͣ��¼
			//b, ���ܣ�ֱ�Ӽ�¼��(���磬12��Сʱ)ʱ�䵽�����ֹͣ��
			//c, �û�ȡ����ֹͣ��
		    //d, ������ֹͣ

		//���� ���� ������أ��������û��ֹ�������

	//���м�����.
		int    iCurPos;        //��ǰ����ɵ�λ��.	
	}sExhaust;

	struct stLeakRate
	{		
		//���м�����
	    //int  iOKType;         //©�ʲ������˵�����  //0:ʱ�� �ﵽ 20���Ӻ�ֹͣ
		
		//���ò���,��ʼ����.
		//int  iWorkType;   //�������� 0:�����Զ����ƣ�ǰ�ᣬ�����ڳ���״̬�����봦�ڸ����(ch3�Ѿ��ܶ�ȡ)��
		                  //�����ڳ�����ʼ��,�Զ��ȴ�����.
		                  //1:�ֶ���ʼ���൱�ڼ�¼�� + ������

		//�����Զ������µ����ò���
		//�˴����Զ����ƣ����൱�� ���������洦��  �������� ҳ�棬 �ֶ����ٹر� MV.
		double dStartPressure;   //�����Զ�����ʱ����ʼ��նȡ�		                         
		int    iWaitTimeSeconds; //��ʼ��ȴ�ʱ��

		//int    iW;   // ��������ʱ, �Ƿ��������,��������. //��ʱΪ����.
		/*
		//��������ʱ������ PolyCold
		bool   bStopPFC;         //�Զ�����ʱ�Ƿ�ر� PolyCold.  true: �ر�PolyCold�� false:������ PolyCold
		bool   bPFCOnStartState; //��¼ PFC �� ��ʼʱ��״̬.
		bool   bPFCOnStopState;  //ֹͣ��¼ʱ  PFC ��״̬.
		*/

		//���ڼ���©�ʣ�����Ҫ���� �����߳��е�
		double dChamberVolume;           //����ҳߴ�. //���ò���, ���������й����м�¼�Ĳ���
		double dRecordedStartPressure;   //©�ʼ�⿪ʼʱ��¼����ն�
		double dRecordedEndPressure;     //ֹͣ���ʱ��¼����ն�
		double dRecordedStartIonGauge;   //©�ʼ�⿪ʼʱ��¼��Ion gauge��ն�( ���Ion Gauge �еĻ� )
		double dRecordedEndIonGauge;     //ֹͣ���ʱ��¼��Ion gauge��ն� ( ���Ion Gauge �еĻ� )
		int    iTimeElapseSeconds;       //�ӿ�ʼ��ֹͣʱʵ�ʾ�����ʱ�䡣 
		int    iPressureKeepTime;   //�ﵽ��նȺ��ȶ�һ��ʱ�䣮
		double fnLeakRate(void)
		{
			//m_dVacuumRateIonGauge = (m_dStartIonGauge-m_dIonGaugePascal)*m_dChamberVolume / (double)m_nMeasureTime ;
			return ( (dRecordedEndPressure -dRecordedStartPressure)*dChamberVolume /(double) iTimeElapseSeconds );
		};
		double fnIonGaugeLeakRate(void)
		{
			return ( (dRecordedEndIonGauge -dRecordedStartIonGauge)*dChamberVolume /(double) iTimeElapseSeconds );
		};		
	}sLeakRate;

	//���ò���
		int  iRecordTime;     //ϣ����¼ʱ�䳤��
		int  iRecordInterval; //��¼��ʱ����
};


//#define _VACUUM_CONDITION_RV 1
/********************************2018-8-29 Zhou Yi************************************************************/
//�� Item2 ��VacuumThread������Ϣʱ��LPARAM��ʹ�õĽṹ.
//struct stItem2VacuumThreadQA
//{
//	int iQAType; //ѯ����Ϣ������. 
//	             //0, ��ѯ���(��)
//	             //1, ִ�ж���(д), ���ȴ����
//
//	int   iCnt;         //��ǰ��ȡ/д��ĸ���
//	WORD  wAddress[20]; //��ȡ/д���ַ
//	WORD  wBit[20];     //д��ĵ�ַλ
//	bool  bDec[20];     //�Ƿ��á�10���ơ�����,��true:10���ƣ� false:16����
//    
//	WORD  wMask[20];    //��ȷ��mask
//
//};


/*
������¼����,δ�г���λ������. ��Ҫ����MASK�˳�,Ȼ�����������㼴��. 
������Դ��μ��������� < screen 0003 >
SRV-OP	3876	00=1	14=1	15=0
		3877	02=0	04=0	05=1	06=0	08=0	09=0

RV-OP	3876	00=1	14=1	15=0
		3877	02=0	04=1	05=1	06=0	08=0	09=0

MBP-ON	3876	00=1	14=1	15=1
		3877	02=0	04=1	05=1	06=0	08=0	09=0

SRV-CL	3876	00=1	14=1	15=1
		3877	02=0	04=0	05=0	06=0	08=0	09=0

MV-OP	3876	00=1	14=1	15=1
		3877	02=1	04=0	05=0	06=1	08=0	09=0
*/
//��Data�����ڳ��ٲ���ʱ�����ݼ�¼
struct stVacuumExhaustData
{
	static WORD wTP_1_Mask;    //TP1��Mask, ÿ�ε������� Mask "�� "�����ж�
	static WORD wTP_2_Mask;    //TP2��Mask, 

	double dPressure;
	bool   bPressureExist; //����ն�� ListCtrl �����


    //���� VGC023�� ch2ֻ�ܲ����գ� ch3ֻ�ܲ�����. �еĻ������������(GI-M2/M832��)��У����.
	//     �õĿ������������ ch2������
	//Ŀǰ VGC403�� ch2, ch3���ܲ��� ����յ������. ch2�ĸ�����൱��ԭ����ĵ����. plc����ͬһ��ַ
	//     �õĿ��ؾ�˵ ���趨�� ch2��ch3��նȾ�����ͨ�� ch3 ?
	//_enumPressureVGC: ����նȣ������ڲ�ֻ��¼ VGC ch3
	//_enumPressureBoth: ����ͬʱ��¼ Ion Gauge �� VGC ch3
	//_enumAction:   �ɶ����������ڲ���¼����ʱ��, ��ʾ����ն���Ԥ����д��.//
	                       //��¼��ʼ�б���ж���������ն�ʱ��������VGC403����������ʱ�̣��ڵ����������д��ʱ�̵�(ch2)��ն�.
	//_enumAction_CH2: ��¼�˶���ʱ�̣�������ʱ�̵�ch2��¼������ 2008.03.26���. //
	                       //��¼��ʼ�б�ֻ�ж�����û����ն�ʱ�� ������VGC403�����ڶ�������ʱ�̣�����ն��������( "1.0E+00(ch2) ")������ 
	//_enumNotRecord_CH2: ���򲻼�¼, ����VGC403���¼ ch2 ����ָ����նȵ�ʱ��. 2008.03.26��_enumNotRecord����
	                       //��¼��ʼ�б���ֻ����նȣ�ȴû�ж���״̬.�� ch2������(ֻ�ʺ�VGC403)
	//����������û�˫��Extra time �� ���ɼ�¼��ǰʱ��
	enum   {_enumPressureVGC,_enumPressureBoth,_enumAction,_enumAction_CH2,_enumNotRecord_CH2} enRecordType;
	//��¼�������������VGC403�����¼��ն�



	//�������пɺ��ԣ���Ϊʵ�ʲ����ô�
	DWORD  dwTimeVGC;      //�Զ���¼�� �� ch3 ��նȶ�Ӧʱ��
	DWORD  dwTimeIonGauge; //�Զ���¼�� �� IonGauge ��նȶ�Ӧʱ��
	DWORD  dwTimeAction;   //�û�˫�� Extra time��ʱ��¼��

	// _enumAction, ����Ҫ��¼��һ�γ��ֵ�״̬�������бȽϣ���ʼ����ɼ���.
	WORD   wTP1_OK_Value;  
	WORD   wTP2_OK_Value;

	bool   bRecord; // _enumAction �£�״̬�Ƿ��Ѿ���¼�� ÿ�ο�ʼ����ʱ����ɡ�false;
	                // _enumPressureXXX �� VGC��ch3 �Ƿ��¼

	bool   bIonGaugeRecord; //_enumPressureBoth��Ion Gauge���Ƿ��Ѿ���¼���
	
	CString sActionName;  //_enumAction������д�� Action name.
};
extern int g_iVacuumExhaustDataItems;
extern int g_iVacuumExhaustDataHighItems;
extern struct stVacuumExhaustData* g_pstVacuumExhaustData;
//�� OACnfgDoc.cpp�ж���
extern WORD   g_wVacuumRdAddress[];  //��Ҫ��ʼ��
extern bool   g_bVacuumRdDecimal[];
extern int    g_iVacuumRdDM_IO[];

extern struct stVacuumTestEx    g_stVacuumTestEx;
extern struct stVacuumTestState gi_stVacuumTestState;

//extern bool  volatile gb_UsingIonGauge;
extern bool  g_bRecordVacuum;  //�Ƿ��¼��ǰ����նȣ���ʼʱĬ�ϼ�¼��������plc���Զ�����
extern bool  gb_ProgrammerDebugNormal;

extern bool   g_bUseIongauge; //�Ƿ��ڳ��ٲ��Թ�����ʹ�� IonGauge.
extern CRITICAL_SECTION gCriticalSection_HearthState;
extern bool   g_bTestHearthDataSaved;
extern bool   g_bTestHearthRotation ; //�Ƿ����ڽ��в���������ת

extern CRITICAL_SECTION gCriticalSection_DomeState;
extern bool   g_bTestDomeRotation; //�Ƿ����ڽ���Dome��ת����
extern bool   g_bTestDomeStopByUser; //Domeֹͣ���Ե�ԭ���Ǳ��û���ֹ��

/*********************2018-8-29 Zhou Yi********************************/
//typedef struct OA_VACUUM_ACTION_STATE_CIO_BIT
//{
//	WORD wWRAddress; //cmd address
//	WORD wWRBit;
//
//	WORD wRDAddress; //state address
//	WORD wRDBit;
//}_OA_VACUUM_ACTION_STATE_CIO_BIT;

//��һ����ͼ�̣߳��������߳�����
struct _stDrawHearth
{
	CRect rect; //��ͼ����
	HWND  hwnd; //����ͼ����(���ڻ�ȡDC)
	int   iCrucibleMaxNumber;//�ж��١�Crucible
	int	  iCoderVal;//��ǰ���������� ( 0~359)

	//2007.12.21
	int   iHearth;
	union
	{
		int   iSpeed;
		int   iCurCurcible;
	};
};


typedef struct sVacuumPressure
{
	double dVacuumPenningPa;
	double dVacuumIonGaugePa;
}_sVacuumPressure;
extern _sVacuumPressure gs_VacuumPressure;


typedef struct sPlcCommError
{
	bool   bObjectErr;       //��δ�ȴ���  single object.�Ӷ�����ڲ�����ɽ���ͨ��
	DWORD  dwPlcLastError;   //plcͨ�ŵ�Error code
	DWORD  dwPlcLastErrorEx; //plcͨ��Error code�Ĳ���˵��.
	                         //��Ϊ API ��������ʱ��Ϊ m_dwLastSysApiError
	                         //��Ϊ ͨ�����ݳ������� EndCode Errorʱ��Ϊ m_iLastEndCode
}_sPlcCommError;
extern _sPlcCommError gs_plcCommError;

/************************************2018-8-29 Zhou Yi*************************************************/
//typedef struct sPlcRdWr
//{
//	DWORD idThread; // thread identifier
//	UINT Msg;       // message
//	WPARAM wParam;  // first message parameter
//	LPARAM lParam;   // second message parameter
//}_sPlcRdWr;

class COACnfgDoc : public CObject
{
public:
	COACnfgDoc(void);
public:
	~COACnfgDoc(void);
public:
	CMarkup     xml;

	// ini �ļ��������
	int m_iCurLng; //��ǰѡ����������
	int m_iLngCnt; //�ܿ�ѡ��������������

public:
	enum en_TestingVacuum
	{
		_en_NoTest = 0,_en_ExhaustRate,_en_LeakRate
	}m_enTestingVacuum;

public:
	CString     m_slngFile;		 //����ѡ���������ļ����ƣ�
	CStringList m_slistlngMenu;  //��ѡ���Menu��ʾ�ַ���List

public:
	//ҳ������
	CString		m_sTabLabel[_OA_FORM_VIEW_NUMBER];
	CString     m_sTabTooltip[_OA_FORM_VIEW_NUMBER];

public:
	//plc����״̬ 0: ���ڴ��ڹر�״̬. 1: ���ڴ򿪴���. 2:�����Ѿ��򿪣����ڽ������Ӳ���. 3:���ڴ򿪣�����plc����. 4:���ڴ򿪣�����plc������	
	int m_iPlcState; 

	//����
public:
	BOOL fn_IsLngFileExist(void);         //ȷ�� .lng�ļ��ɶ�ȡ�����в�ȷ������
	BOOL fn_ReleaseInnerLng(void); //��fn_IsLngFileExist����
public:
	BOOL fn_InitText(void);
	bool fn_InitLngMenuText(void);
public:
	bool fn_WriteCurLngInt(int iCurLng);
public:
	LPCTSTR fn_GetLocalTimeStr(void);
public:
	static double fn_TransPenningVoltPa(double dVolt);
public:
	void fn_CreateCommError(_sPlcCommError& sCommErr);
public:
	
public:
	static int fn_LinearRegression(double& a, double& b, double* x, double* y, int n);
};
