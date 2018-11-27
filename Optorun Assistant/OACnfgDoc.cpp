#include "StdAfx.h"
#include "OACnfgDoc.h"
#include "resource.h"       // main symbols
#include <math.h>

//����汾����ʾ�ڻ��������
const TCHAR* g_strProductVersion = TEXT("1.05.2010.09.15");//TEXT("1.04.2010.06.01");//TEXT("2008.07.15");//TEXT("2008.04.02");//TEXT("2008.01.02");
//version : 1.04.2010.06.01", ���� PEG100��Ӧ. 
//version : 1.05.2010.09.15, ���� Ulvac ������ռ� BMR2\SC1 (APC��)��Ӧ.
const double g_dProgramVersion = 1.05; //1.04->PEG100//1.02->2009.12.14 ��ӦS702��S703����������ת��
//const TCHAR*  g_strLastProgramDate = TEXT("PROGRAM DATA: 2007.12.10"); //
const double g_dDataVersion = 1.01;

//����Ӳ�����е���ʹ�á� �ڽ�������������ʾ��
//�����ͬ����Ҫ�ѻ�����ʱ��ÿ�θ��ĸ����á�
bool  g_bOffLineDebug = false;

enum enumOptorunCoaterType g_enOptorunCoaterType = _en_Coater_UNKNOWN_TYPE;

//#define _OA_STR_SYSTEM_NUMBER
CString g_strSystem[_OA_STR_SYSTEM_NUMBER];//�˳�ʱ����������xml����

CODMCom g_COM2_ODM;
CGasController g_COM4_GasController;
CVGC500  g_COM5_VGC500;
CIBGLCom g_COM3_IBGL;
CSysmacPLC g_plc;
CVacuumProcessThread* g_pVacProThread = NULL ;
CPlcRdWrThread*       g_pPlcRdWrThread = NULL;
CIonGauge  g_IonGauge; 
CAPCGauge  g_APCGauge;
HANDLE g_hEvent_plc; //�¼���ͨ�����ڷ���plc������ȡ�ÿ���Ȩ

CRITICAL_SECTION gCriticalSection_State; //�ٽ��������ڳ���״̬���ƣ��ڹ��캯���г�ʼ����
CRITICAL_SECTION gCriSec_Vacuum; //�ٽ���������������״̬���ƣ��ڹ��캯���г�ʼ���� ���߳����̶߳���Ҫ����

CRITICAL_SECTION gCriSec_UI;     //�ٽ���������(�û��ӿ�)�������ñ���ʱ�Ŀ��ƣ��ڹ��캯���г�ʼ����

int    g_iVacuumExhaustDataCurrent = 0;
int    g_iVacuumExhaustDataItems = 16;    //���ٲ��Ե������������		20160704����OTFC2350 MTFC900
int    g_iVacuumExhaustDataHighItems = 7; //���ٲ��������У������ʱ������������ʱ�������ܲ�������նȣ����û�����д������ʶ����״̬.
struct stVacuumExhaustData* g_pstVacuumExhaustData = NULL;

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
#define _3876_00_CHAMBER_DOOR 0
#define _3876_11_EXHAUST 11 //2007.12.20,δ��
#define _3876_14_RP_ON	 14
#define _3876_15_MBP_ON  15


#define _3877_02_FV1     2
#define _3877_04_RV      4
#define _3877_05_SRV     5
#define _3877_06_MV      6
#define _3877_08_LV      8
#define _3877_09_SLV     9
WORD stVacuumExhaustData::wTP_1_Mask = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON)|(1<<_3876_15_MBP_ON);
WORD stVacuumExhaustData::wTP_2_Mask = (1<<_3877_02_FV1)|(1<<_3877_04_RV)|(1<<_3877_05_SRV)|(1<<_3877_06_MV)|(1<<_3877_08_LV)|(1<<_3877_09_SLV);


struct stVacuumTestEx    g_stVacuumTestEx;
//0:û�еȴ���ϵ
//1:�����߳����ڵȴ����߳���Ӧ
//2:���߳��Ѿ���Ӧ

int g_iMachineType = _MACHINE_TYPE_GENER; //��Ĥ�����ͣ�Ĭ��ΪGener. �ڲ��һ�������ʱ���г�ʼ����Ŀǰ��������ʼ����Ĥ�����ѡ��ʱ���ݻ���

struct stVacuumTestState gi_stVacuumTestState; //�� COACnfgDoc �г�ʼ��

//CRITICAL_SECTION gCriticalSection_Hearth; 
//bool  volatile gb_UsingIonGauge = true; //���Թ����Ƿ�ʹ��IonGauge��Ĭ��ʹ�á��Ƿ���IonGauge����� g_IonGauge type.
bool  gb_VacuumPercent  = false; //�Ƿ��ǰٷֱ���ʽ�洢��ն�����

const TCHAR* g_strLngPath = TEXT("\\language");
TCHAR g_strBuffer[1024]; //ֻ�������̵߳���������
TCHAR g_strTimeBuffer[256];//ר������ʱ��
const TCHAR* g_strFileData = _T("OA_FILE_DATA");
const TCHAR* g_strEnglishName  = _T("OA_English.lng");
const TCHAR* g_strChineseName  = _T("OA_Chinese.lng");
const TCHAR* g_strLngNameTplt = TEXT("LNG_%d_NAME"); //name template
const TCHAR* g_strxmlRoot = TEXT("OPTORUN");

#define PLCDM_ADDRESS
//��Щ��ֱֵַ��д�ں�����.
PLCDM_ADDRESS const WORD   gc_DM_machinetype   = 9000;
PLCDM_ADDRESS const WORD   gc_DM_sequencer     = 9010;
PLCDM_ADDRESS const WORD   gc_DM_touchpanel    = 9020;

PLCDM_ADDRESS const WORD   gc_DM_VacuumType    = 9066; // 0001 �� %, 0002 �� Pa. Ӧ�����ѹֵû��ϵ.�������λΪPa
PLCDM_ADDRESS const WORD   gc_DM_IonGaugeType  = 9085; //������ռ����, 0=û��,1=GI-M2,2=IOZ_M832,3=IOZ_BPG400,4=GI-D7, 5=IOZ_BPG402+VGC403?
													   //��Ӧ����ĵ�����ռƣ���5����ʱռ��ԭch2
PLCDM_ADDRESS const WORD   gc_DM_APCGaugeType  = 9088; //APC����ռ����2008.05.12���. ��Ӧ G1275 ʹ�� VGC403����ch2��ʹ�� Ƥ�����PSG500(���ܲ��������).
                                                       //��Ӧch3
PLCDM_ADDRESS const WORD   gc_DM_IonGaugeValue = 6587;
PLCDM_ADDRESS const WORD   gc_DM_PenningValue  = 3400; //ch3, (3400) = (3421)*4


//��9069��ʼ�����ĸ���ַ���ֱ�Ϊ monitor Heater�¶�����ֵ��ַ��DomeHeater����ֵ��ַ
//                               monitor HeaterĿ��ֵ��DomeHeaterĿ��ֵ
PLCDM_ADDRESS const WORD   gc_DM_AddressHeaterTempSVPV = 9069;
PLCDM_ADDRESS const WORD   gc_DM_AddressDome2TempSVPV = 9079;  //���
//�����ĸ���ַ��������������г�ʼ��.
PLCDM_ADDRESS WORD gcp_DM_HeaterTemp[_TEMP_HEATER_NUMBER]={4210,4128,4215,4132,4180,4182};
//�����2���ǿ��ܴ��ڵĲ�ڵ�ַ.

PLCDM_ADDRESS WORD gc_DM_VacPump = 9053; //�����pump���� (.00==1: ��ɢ��) (.01=1:dry pump).�����ֻʶ�� ��ɢ��?
PLCDM_ADDRESS WORD gc_DM_HAS_DP2 = 9097; //����DP2 ? 0=�ޣ�1=��. 


const int gc_plc_str_length = 10;
const COLORREF gc_clrBlack  = RGB(0,0,0);
const COLORREF gc_clrTrying = RGB(128,128,0);
const COLORREF gc_clrOK     = RGB(0,128,0);
const COLORREF gc_clrAlert  = RGB(255,0,0);
const COLORREF gc_clrWhite  = RGB(255,255,255);
const COLORREF gc_clrRunning = RGB(255,0,0);

//4199 and 176�ο��� plc,  �ڲ���¼����ʾʱ����׼��������Χ���� ********Pa��ʾ
double   gc_dMaxPenningPa = 1.0E+6;// = COACnfgDoc::fn_TransPenningVoltPa(4199-1);
double   gc_dMinPenningPa = 1.0E-9;//COACnfgDoc::fn_TransPenningVoltPa(176+1);
//
//Ion gauge �������� CIonGauge�����������ɸı䣬�ⲿ���ɶԴ˽��иı�
//extern const double gc_dMaxIonGaugePa;
//extern const double gc_dMinIonGaugePa;


//�Զ����Թ����У�����ͻȻplc��ϵʧ�ܣ���ֹͣ����֮ǰ���Զ�������Դ���
const int      gc_iMaxPLCtryTimes = 3; 
int            g_icurPLCtryTimes = 0; //��ǰ���Դ���

//item1(����COASysInfoView)��Ҫ�� xml �ļ��ж�ȡ�ı��� ����ID ����//������������ʱ�ֹ����
//#define      _ITEM1_NEED_TEXT_ID_CNT 2 //������"OACnfgDoc.h"�У���ø� FormViewͬʱ����
//����1�̶���Ҫĳ���ı��Ĵ���
const int    g_item1_ID[_ITEM1_NEED_TEXT_ID_CNT] 
				= { IDC_BUTTON_CONNECT_PLC,IDC_BUTTON_CLOSE_CONNECTION,IDC_STATIC_CUR_STATE_TITLE,IDC_ITEM1_SYS_GROUP,IDC_STATIC_PRODUCT_VERSION,IDC_STATIC_PC_NAME,
					IDC_STATIC_MACHINE_TYPE,IDC_STATIC_SEQUENCER,IDC_STATIC_TOUCHPANEL_VER,IDC_STATIC_HELP_TITLE,IDC_STATIC_HISTORY}; //ID������ //��Ҫ�� view�� extern
//�� xml �ļ��е���������(�������� item1 element ֮��). ����һ��
const TCHAR* g_item1_ID_Name[_ITEM1_NEED_TEXT_ID_CNT] = {TEXT("connectPLC"),TEXT("disconnectPLC"),TEXT("curPLCstate"),TEXT("sysGroup"),TEXT("productVer"),TEXT("pcName"),TEXT("machineType"),TEXT("sequencerVer"),TEXT("touchpanelVer"),TEXT("helpTitle"),TEXT("runHistory")}; //
CString g_item1_ID_Text[_ITEM1_NEED_TEXT_ID_CNT]; //��Ҫ�� view�� extern
//item1 ����2ĳ��������Ҫ����ı��л�

//item1 ����3
int giHelpItems = _ITEM1_NEED_HELP_TEXT;
CString g_item1_Help_Text[_ITEM1_NEED_HELP_TEXT];
CString g_item1_plc_state[_ITEM1_COM_PLC_STATE_CNT];


//item2 g_item2_ID��g_item2_ID_Name �������ݱ��뱣��ͬ���ߴ�,ͬ������. //�� doc���캯���������ASSERTȷ��.IDC_STATIC_EXHAUST_NEED_TIME_TEXT
const int    g_item2_ID[]
				= {IDC_STATIC_PENNING_VALUE_NAME,IDC_BTN_START_VACUUM_TEST,IDC_BTN_CANCEL_VACUUM_TEST,
					IDC_STATIC_COMMENT_TEXT,IDC_RADIO_LEAK_RATE,IDC_RADIO_EXHAUST,IDC_RADIO_RECORDER,
					IDC_STATIC_COATER_TEXT,IDC_STATIC_INTERVAL_TEXT,IDC_STATIC_RECORD_TIME_TEXT,IDC_STATIC_ELAPSED_TIME_TEXT,
					IDC_STATIC_LEAKRATE_START_PRESSURE_TEXT,IDC_STATIC_LEAKRATE_WAIT_TIME_TEXT,IDC_STATIC_LEAKRATE_KEEP_TIME_TEXT,IDC_EDIT_VACUUM_HELP,
				    IDC_BTN_ADD_VACUUM_FILE,IDC_BTN_CURVE_PROPERTY,IDC_BTN_DELETE_CURVE	};
//�� xml �ļ��е���������(�������� item1 element ֮��). ����һ��
const TCHAR* g_item2_ID_Name[]
				= {TEXT("penningName"),TEXT("startTest"),TEXT("cancelTest"),TEXT("Comment"),TEXT("radioLeak"),TEXT("radioExhaust"),
					TEXT("radioRecorder"),TEXT("coaterVolume"),TEXT("recordInterval"),TEXT("recordTime"),
					TEXT("elapsedTime"),TEXT("expectPressure"),TEXT("maxWaitTime"),TEXT("pressurekeepTime"),TEXT("helpInittext"),
				    TEXT("addFiles"),TEXT("curvePro"),TEXT("delCurve")}; //
const int  gc_item2_Text_ID_CNT = sizeof(g_item2_ID)/sizeof(int);//ITEM2 ��Ҫ�� language�ж�ȡ��id������_ITEM2_NEED_TEXT_ID_CNT;

CString g_item2_ID_Text[gc_item2_Text_ID_CNT]; //��Ҫ�� view�� extern
CString g_item2_Vac_state[_ITEM2_VAC_STATE_CNT];
CString g_item2_String[_ITEM2_OTHER_STR_NUMBER];//item2����Ҫʹ�õ��Ķ����ַ���

//CString g_dlgVacuumThreadError_Str[_DLGVACTHRERROR_STR_NUMBER];

bool  g_bRecordVacuum = true; //�Ƿ��¼��ǰ����նȣ���ʼʱĬ�ϼ�¼��������plc���Զ���������ն�ҳ������ check button����

//item3
const int g_item3_ID[] = {IDC_BTN_INIT_HEARTH,IDC_CHECK_START_FROM_SELECTION,IDC_BTN_CREATE_HEARTH1,IDC_BTN_INSERT_HEARTH1,
			IDC_BTN_CREATE_HEARTH2,IDC_BTN_INSERT_HEARTH2,IDC_BTN_DELETE_HEARTH_ROW,IDC_BTN_SAVE_HEARTH_LIST,IDC_BTN_START_HEARTH_TEST,
			IDC_BTN_CANCEL_HEARTH_TEST,IDC_CHECK_HEARTH1_DIRECTION,IDC_CHECK_HEARTH2_DIRECTION,IDC_STATIC_MONITOR_ALERT,
			IDC_BTN_INSERT_DOME_SPEED,IDC_BTN_AUTOCREATE_DOME_SPEED,IDC_CHECK_START_FROM_SELECTION_DOME,IDC_BTN_DELETE_DOME_ROW,IDC_BTN_SAVE_DOME_LIST,
			IDC_BTN_START_DOME_TEST,IDC_BTN_CANCEL_DOME_TEST};
const TCHAR* g_item3_ID_Name[] = {TEXT("initHearth"),TEXT("startFromSelRow"),TEXT("autoCreateHearth1"),TEXT("insertHearth1"),
			TEXT("autoCreateHearth2"),TEXT("insertHearth2"),TEXT("delListRow"),TEXT("saveList"),TEXT("startHearthTest"),
			TEXT("cancelHearthTest"),TEXT("h1DrtCheck"),TEXT("h2DrtCheck"),TEXT("monitorAlert"),
			TEXT("insertDomeSpeed"),TEXT("autoCreateDomeSpeed"),TEXT("startDomeFromSelRow"),TEXT("delDomeTestRow"),TEXT("saveDomeList"),
			TEXT("startDomeTest"),TEXT("cancelDomeTest")};
const int gc_item3_Text_ID_CNT = sizeof(g_item3_ID)/sizeof(int);
CString g_item3_ID_Text[gc_item3_Text_ID_CNT]; //��Ҫ�� view�� extern		
CString g_item3_String[_ITEM3_OTHER_STR_NUMBER];//item2����Ҫʹ�õ��Ķ����ַ���

#ifdef _ZHANGZIYE_DEBUG
//debug
CArray <int,int&> g_item1_ID_Text2;
#endif

_sVacuumPressure gs_VacuumPressure;
_sPlcCommError gs_plcCommError;

//��նȿռ��С
//#define  _VACUUM_PRESSURE_MAX (2*24*3600)
int    g_iVacuumTestCnt     = 0;     //ÿ�β��Կ�ʼ�����ݵ����Ĵ���. //ͬ���� 
int    g_iVacuumStartPos    = 0;     //ÿ�β��Ե�����λ�� �洢�ռ����ʼλ��. 

int    g_iVacuumPressurePos = 0;     //��ǰ�����ڴ洢��նȵ�λ��.
double g_dVacuumVGCCh3[_VACUUM_PRESSURE_MAX];   //�洢 VGC023 ch3 (penning)��նȣ��Ѿ�ת����Pascal��
double g_dVacuumIonGauge[_VACUUM_PRESSURE_MAX]; //�洢 Ion Gauge (penning)��նȣ��Ѿ�ת����Pascal��
                                                //��VGC403֮��g_dVacuumIonGauge����˴洢 VGC403��ch2.
double g_dVacuumTime[_VACUUM_PRESSURE_MAX];     //�洢��¼ʱ�̣�������ʾ���˴���ָ������ʼλ�õ�ʱ����λΪ�롣


WORD   g_wVacuumRdAddress[_VACUUM_READ_PLC_ADDRESS_CNT];
bool   g_bVacuumRdDecimal[_VACUUM_READ_PLC_ADDRESS_CNT]; //�Ƿ��ȡ 10 ���ƶ�ȡ
int    g_iVacuumRdDM_IO[_VACUUM_READ_PLC_ADDRESS_CNT];   //0���� DM������Ϊ��I/O

bool   g_bUseIongauge = true; //�����IonGauge,�Ƿ��ڲ��Գ��ٵĹ�����ʹ�á�Iongauge��Ĭ��ʹ��.
CZYGraphPlot g_CZYGraphPlot; //2008.04.08��ӣ���ն�������ʾ��

//���� ��ʼ��ֹͣ
//��������Ϣ��Ӧ�￪ʼ��ֹͣ
//����������  ѭ���ڼ��ñ�������Ϊ falseʱ �˳���
CRITICAL_SECTION gCriticalSection_HearthState;
bool   g_bTestHearthRotation = false; //�Ƿ����ڽ��в���������ת
bool   g_bTestHearthDataSaved = true;

CRITICAL_SECTION gCriticalSection_DomeState;
bool   g_bTestDomeRotation = false; //�Ƿ����ڽ���Dome��ת����
bool   g_bTestDomeStopByUser = false;//Domeֹͣ���Ե�ԭ�����û�ֹͣ.


//������Ա debug һЩͨ���Ĳ��������ͨ������Ҫ��ʹ����Ա������Щ���ܡ�
//������Ա����ͨ��һ���ķ�ʽʹ�� gb_xxx��Ϊ true.
bool gb_ProgrammerDebugNormal = true; //����������б�Ҫ�����ڱ���ʱ���趨��Щ��Ϊtrue.

COACnfgDoc::COACnfgDoc(void):m_iPlcState(_COM_PLC_STATE_NOT_CONNECT)
{
	//ȷ��g_item2_ID��g_item2_ID_Name �������ݱ���ͬ���ߴ�
	ASSERT( gc_item2_Text_ID_CNT == sizeof(g_item2_ID_Name)/sizeof(TCHAR*) );
	ASSERT( gc_item3_Text_ID_CNT == sizeof(g_item3_ID_Name)/sizeof(TCHAR*) );
	
	gs_VacuumPressure.dVacuumIonGaugePa = 1.0E+5;
	gs_VacuumPressure.dVacuumPenningPa  = 1.0E+5;
	m_enTestingVacuum = _en_NoTest;


	//����plc��ȡ�߳�
//	g_pPlcRdWrThread = (CPlcRdWrThread*)AfxBeginThread(RUNTIME_CLASS(CPlcRdWrThread), THREAD_PRIORITY_NORMAL,0,0);


	//debugging���ŵ� Vacuum View������
	//������ն���ؼ���߳�
	//g_pVacProThread = (CVacuumProcessThread*)AfxBeginThread(RUNTIME_CLASS(CVacuumProcessThread), THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	//g_pVacProThread  = (CVacuumProcessThread*)AfxBeginThread(RUNTIME_CLASS(CVacuumProcessThread), THREAD_PRIORITY_NORMAL,0,0);


	//���� plc�����¼������ڿ���һ��ʱ����ֻ��һ���̲߳��� ����
	g_hEvent_plc = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetEvent(g_hEvent_plc);

	//��ʼ�� ״̬������ �ٽ���������Ҫ���� ״̬���� bVacuumTest�ȣ�������ȡ���ٽ�������.
	/*
	EnterCriticalSection(&gCriticalSection);
	//do something
	LeaveCriticalSection(&gCriticalSection);
	*/
	InitializeCriticalSection(&gCriticalSection_State);
	InitializeCriticalSection(&gCriSec_Vacuum);
	InitializeCriticalSection(&gCriSec_UI);
	InitializeCriticalSection(&gCriticalSection_HearthState);
	InitializeCriticalSection(&gCriticalSection_DomeState);
	

	//��ʼ��gi_stVacuumTestState
	gi_stVacuumTestState.bTest = false;
	gi_stVacuumTestState.iMainRouting = 0;
	gi_stVacuumTestState.iSubRouting = 0;
	gi_stVacuumTestState.iThreadState = 0;
	gi_stVacuumTestState.iTryTimes = 0;

	m_iCurLng = -1;
	m_iLngCnt = -1;
	//m_iPlcState = _COM_PLC_STATE_NOT_CONNECT;
		
#ifdef _ZHANGZIYE_DEBUG
	try
	{
		int x  = g_item1_ID_Text2.GetSize();
	}
	catch(...)
	{
		int x = 3;
	}
#endif

	g_stVacuumTestEx.bTesting = false;
	g_stVacuumTestEx.iCurTestType = _VACUUM_TESTTYPE_RECORDER;     //Ĭ����նȲ�������Ϊ��¼��
	g_stVacuumTestEx.iRecordInterval = 60; //60 ����
	g_stVacuumTestEx.iRecordTime = 3600;   //60 ����
	//g_stVacuumTestEx.sExhaust.

	//��Vacuum�����߳�ʹ��.
	g_wVacuumRdAddress[_VACUUM_READ_PLC_ADDRESS_SN_VGC] = gc_DM_PenningValue; //3400
	g_wVacuumRdAddress[_VACUUM_READ_PLC_ADDRESS_SN_IONGAUGE] = gc_DM_IonGaugeValue;//6587;
	g_wVacuumRdAddress[_VACUUM_READ_PLC_ADDRESS_SN_TP_1] = 3876;
	g_wVacuumRdAddress[_VACUUM_READ_PLC_ADDRESS_SN_TP_2] = 3877;

	g_bVacuumRdDecimal[_VACUUM_READ_PLC_ADDRESS_SN_VGC] = true;  //10����
	g_bVacuumRdDecimal[_VACUUM_READ_PLC_ADDRESS_SN_IONGAUGE] = true;
	g_bVacuumRdDecimal[_VACUUM_READ_PLC_ADDRESS_SN_TP_1] = false; //16����
	g_bVacuumRdDecimal[_VACUUM_READ_PLC_ADDRESS_SN_TP_2] = false;

	g_iVacuumRdDM_IO[_VACUUM_READ_PLC_ADDRESS_SN_VGC] = 0; //DM
	g_iVacuumRdDM_IO[_VACUUM_READ_PLC_ADDRESS_SN_IONGAUGE] = 0; //DM
	g_iVacuumRdDM_IO[_VACUUM_READ_PLC_ADDRESS_SN_TP_1] = 1; // I/O
	g_iVacuumRdDM_IO[_VACUUM_READ_PLC_ADDRESS_SN_TP_2] = 1; // I/O


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
	g_pstVacuumExhaustData = new stVacuumExhaustData[g_iVacuumExhaustDataItems];
	g_pstVacuumExhaustData[0].bPressureExist = true;
	g_pstVacuumExhaustData[0].dPressure = 1.0E+05;
	g_pstVacuumExhaustData[0].enRecordType = stVacuumExhaustData::_enumAction;
	g_pstVacuumExhaustData[0].sActionName = TEXT("SRV OP");
	g_pstVacuumExhaustData[0].wTP1_OK_Value = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON); // ��¼����λ
	g_pstVacuumExhaustData[0].wTP2_OK_Value = (1<<_3877_05_SRV);

	g_pstVacuumExhaustData[1].bPressureExist = false;
	g_pstVacuumExhaustData[1].enRecordType = stVacuumExhaustData::_enumAction_CH2;
	g_pstVacuumExhaustData[1].sActionName = TEXT("RV OP");
	g_pstVacuumExhaustData[1].wTP1_OK_Value = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON); // ��¼����λ
	g_pstVacuumExhaustData[1].wTP2_OK_Value = (1<<_3877_04_RV) | (1<<_3877_05_SRV) ;

	g_pstVacuumExhaustData[2].bPressureExist = true;
	g_pstVacuumExhaustData[2].dPressure = 1.0E+04;
	g_pstVacuumExhaustData[2].enRecordType = stVacuumExhaustData::_enumAction;																//2018-9-14 Zhouyi
	g_pstVacuumExhaustData[2].sActionName = TEXT("MBP ON");																					//2018-9-14 Zhouyi
	g_pstVacuumExhaustData[2].wTP1_OK_Value = (1 << _3876_00_CHAMBER_DOOR) | (1 << _3876_14_RP_ON) | (1 << _3876_15_MBP_ON); // ��¼����λ	//2018-9-14 Zhouyi
	g_pstVacuumExhaustData[2].wTP2_OK_Value = (1<<_3877_04_RV) | (1<<_3877_05_SRV) ;														//2018-9-14 Zhouyi

	//g_pstVacuumExhaustData[2].enRecordType = stVacuumExhaustData::_enumNotRecord_CH2;													//2018-9-14 Zhouyi
	//2007.12.07, MBP�б�Ƶ��������¡��� 1.3E+3��ʼ����������� 1.0E+4����																//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[2].sActionName = TEXT("MBP ON");																			//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[2].wTP1_OK_Value = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON) | (1<<_3876_15_MBP_ON); // ��¼����λ	//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[2].wTP2_OK_Value = (1<<_3877_04_RV) | (1<<_3877_05_SRV) ;													//2018-9-14 Zhouyi

	g_pstVacuumExhaustData[3].bPressureExist = true;
	g_pstVacuumExhaustData[3].dPressure = 1.3E+03;
	g_pstVacuumExhaustData[3].enRecordType = stVacuumExhaustData::_enumNotRecord_CH2;													//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[3].enRecordType = stVacuumExhaustData::_enumAction;														//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[3].sActionName = TEXT("MBP ON");																			//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[3].wTP1_OK_Value = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON) | (1<<_3876_15_MBP_ON); // ��¼����λ	//2018-9-14 Zhouyi
	//g_pstVacuumExhaustData[3].wTP2_OK_Value = (1<<_3877_04_RV) | (1<<_3877_05_SRV);													//2018-9-14 Zhouyi


	g_pstVacuumExhaustData[4].bPressureExist = true;
	g_pstVacuumExhaustData[4].dPressure = 1.0E+01;
	g_pstVacuumExhaustData[4].enRecordType = stVacuumExhaustData::_enumNotRecord_CH2;

	g_pstVacuumExhaustData[5].bPressureExist = true;
	g_pstVacuumExhaustData[5].dPressure = 6.6E+0;
	g_pstVacuumExhaustData[5].enRecordType = stVacuumExhaustData::_enumAction;
	g_pstVacuumExhaustData[5].sActionName = TEXT("SRV RV CL");
	g_pstVacuumExhaustData[5].wTP1_OK_Value = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON) | (1<<_3876_15_MBP_ON); // ��¼����λ
	g_pstVacuumExhaustData[5].wTP2_OK_Value = 0 ;

	g_pstVacuumExhaustData[6].bPressureExist = false;
	g_pstVacuumExhaustData[6].enRecordType = stVacuumExhaustData::_enumAction_CH2;
	g_pstVacuumExhaustData[6].sActionName = TEXT("MV OP");
	g_pstVacuumExhaustData[6].wTP1_OK_Value = (1<<_3876_00_CHAMBER_DOOR)|(1<<_3876_14_RP_ON) | (1<<_3876_15_MBP_ON); // ��¼����λ
	g_pstVacuumExhaustData[6].wTP2_OK_Value = (1<<_3877_02_FV1) | (1<< _3877_06_MV) ;

	g_pstVacuumExhaustData[7].bPressureExist = true;
	g_pstVacuumExhaustData[7].dPressure = 3.0E-3;
	g_pstVacuumExhaustData[7].enRecordType = stVacuumExhaustData::_enumPressureVGC;

	g_pstVacuumExhaustData[8].bPressureExist = true;
	g_pstVacuumExhaustData[8].dPressure = 2.0E-3;
	g_pstVacuumExhaustData[8].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[9].bPressureExist = true;
	g_pstVacuumExhaustData[9].dPressure = 1.3E-3;
	g_pstVacuumExhaustData[9].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[10].bPressureExist = true;
	g_pstVacuumExhaustData[10].dPressure = 1.0E-3;
	g_pstVacuumExhaustData[10].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[11].bPressureExist = true;
	g_pstVacuumExhaustData[11].dPressure = 5.0E-4;
	g_pstVacuumExhaustData[11].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[12].bPressureExist = true;
	g_pstVacuumExhaustData[12].dPressure = 1.5E-4;
	g_pstVacuumExhaustData[12].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[13].bPressureExist = true;
	g_pstVacuumExhaustData[13].dPressure = 1.0E-4;
	g_pstVacuumExhaustData[13].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[14].bPressureExist = true;
	g_pstVacuumExhaustData[14].dPressure = 1.0E-4;
	g_pstVacuumExhaustData[14].enRecordType = stVacuumExhaustData::_enumPressureBoth;

	g_pstVacuumExhaustData[15].bPressureExist = true;
	g_pstVacuumExhaustData[15].dPressure = 5.0E-5;
	g_pstVacuumExhaustData[15].enRecordType = stVacuumExhaustData::_enumPressureBoth;
//stVacuumExhaustData* g_pstVacuumExhaustData
}

COACnfgDoc::~COACnfgDoc(void)
{
	DeleteCriticalSection(&gCriticalSection_State);
	CloseHandle(g_hEvent_plc);

	delete [] g_pstVacuumExhaustData;
}


BOOL COACnfgDoc::fn_IsLngFileExist(void)
{
	//fn_ReleaseInnerLng(); //debug
	CWinApp* pApp = AfxGetApp();
	if(NULL==pApp)
	{
		return false;
	}

	m_iCurLng = pApp->GetProfileInt(TEXT("LANGUAGE"),TEXT("LNG_CURRENT"),1);
	m_iLngCnt = pApp->GetProfileInt(TEXT("LANGUAGE"),TEXT("LNG_COUNT"),0);
	if( m_iLngCnt< 0 )
	{
		return fn_ReleaseInnerLng();
	}

	if(m_iCurLng > m_iLngCnt)
	{
		m_iCurLng = 1;
	}

	//ȷ�������ļ�����
	for(int i = m_iCurLng; i <= m_iLngCnt; i++)
	{
		_stprintf(g_strBuffer,TEXT("LNG_%d_FILE"),i);
		CString strFile = pApp->GetProfileString(TEXT("LANGUAGE"),g_strBuffer,NULL);
		if(strFile.GetLength() != 0)
		{
			CString str = pApp->m_pszProfileName;
			int nPos = str.ReverseFind(TEXT('\\'));
			str = str.Left(nPos);
			m_slngFile = str.Left(nPos) + TEXT("\\language\\") + strFile;

			HANDLE hFile = CreateFile(m_slngFile,0,FILE_SHARE_READ,0,OPEN_EXISTING,
				FILE_ATTRIBUTE_READONLY,NULL);
			if(GetLastError()==0)
			{//������ļ��ɹ�
				if( i != m_iCurLng )
				{//�����ǰ�򿪵��ļ������ ���ò����ϣ�������д ini�ļ�
					m_iCurLng = i;
					pApp->WriteProfileInt(TEXT("LANGUAGE"),TEXT("LNG_CURRENT"),m_iCurLng);
					AfxMessageBox(g_strSystem[_OA_STR_SYSTEM_LNG_FILE_NOT_EXIST].IsEmpty()?
						TEXT("ѡ���������ļ�û���ҵ������������ļ�(OptorunAssistant.ini).\r\nlanguage file not exist. Please check \"OptorunAssistant.ini\" file"):g_strSystem[_OA_STR_SYSTEM_LNG_FILE_NOT_EXIST]);
					//AfxMessageBox(TEXT("Preselected language file not exist. Please check \"OptorunAssistant.ini\" file"));
				}

				CloseHandle(hFile);
				return true;
			}
		}
	}
	//����ѭ����û���ҵ�������ļ����ͷ���Դ
	return fn_ReleaseInnerLng();
}

BOOL COACnfgDoc::fn_ReleaseInnerLng(void)
{
		CWinApp* pApp = AfxGetApp();
	
		//1, �ж�·���Ƿ����
		CString strPath = pApp->m_pszProfileName;
		int nPos = strPath.ReverseFind(TEXT('\\'));
		strPath  = strPath.Left(nPos);
		strPath += TEXT("\\language\\");

		BOOL b = CreateDirectory(strPath,NULL);		
		if( !b )
		{
			DWORD dw = GetLastError();
			if( dw != 183 )
			{//123, Ŀ¼������ȷ����183��Ŀ¼�Ѿ����ڣ������ٴ�����
				return false;
			}
		}

		//2, �ͷš�OA_English.lng
		HRSRC hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_OA_ENGLISH_LNG),g_strFileData);
		if(hSrc == NULL)	
			return false;

		HGLOBAL hGlobal = LoadResource(NULL,hSrc);
		if(hGlobal == NULL)	
			return false;

		LPVOID lp = LockResource(hGlobal);
		DWORD dwSize = SizeofResource(NULL,hSrc);

		CString strFile;
		strFile = strPath + g_strEnglishName;	
	
		CFile file;
		if( file.Open(strFile, CFile::modeCreate|CFile::modeWrite))
		{
			file.Write(lp,dwSize);
			file.Close();
		}		
		FreeResource(hGlobal);
		//m_iLgCnt = 2;
		m_iCurLng = 1;
		m_slngFile = strFile;//english

		//3, �ͷš�OA_Chinese.lng
		hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_OA_CHINESE_LNG),g_strFileData);
		if(hSrc == NULL)	
			return false;

		hGlobal = LoadResource(NULL,hSrc);
		if(hGlobal == NULL)	
			return false;

		lp = LockResource(hGlobal);
		dwSize = SizeofResource(NULL,hSrc);
		strFile = strPath + g_strChineseName;
	
		if( file.Open(strFile, CFile::modeCreate|CFile::modeWrite))
		{
			file.Write(lp,dwSize);
			file.Close();
		}		
		FreeResource(hGlobal);


		//4, ��ʼ��m_lngFilename������д ini�ļ�
		m_iLngCnt  = 2;		
		m_iCurLng  = 2;
		m_slngFile = strFile;//


		pApp->WriteProfileInt(TEXT("LANGUAGE"),TEXT("LNG_CURRENT"),m_iLngCnt);
		pApp->WriteProfileInt(TEXT("LANGUAGE"),TEXT("LNG_COUNT"),2);

		_stprintf(g_strBuffer,g_strLngNameTplt,1);
		pApp->WriteProfileString(TEXT("LANGUAGE"),g_strBuffer,TEXT("English"));
		_stprintf(g_strBuffer,TEXT("LNG_%d_FILE"),1);
		pApp->WriteProfileString(TEXT("LANGUAGE"),g_strBuffer,g_strEnglishName);

		_stprintf(g_strBuffer,g_strLngNameTplt,2);
		pApp->WriteProfileString(TEXT("LANGUAGE"),g_strBuffer,TEXT("��������"));
		_stprintf(g_strBuffer,TEXT("LNG_%d_FILE"),2);
		pApp->WriteProfileString(TEXT("LANGUAGE"),g_strBuffer,g_strChineseName);

		return true;
}

bool COACnfgDoc::fn_InitLngMenuText(void)
{
	CWinApp* pApp = AfxGetApp();
	if(NULL==pApp)
		return false;

	if( 0 != m_slistlngMenu.GetCount())
	{
		m_slistlngMenu.RemoveAll();
	}

	CString str;
	for(int i = 1; i <= m_iLngCnt; i++)
	{
		_stprintf(g_strBuffer,g_strLngNameTplt,i);
		str = pApp->GetProfileString(TEXT("LANGUAGE"),g_strBuffer,NULL);
		m_slistlngMenu.AddTail(str);
	}
		
	return true;
}

//��ʼ��
BOOL COACnfgDoc::fn_InitText(void)
{
	if(!fn_IsLngFileExist())
	{
		return false;
	}
	fn_InitLngMenuText(); //OK

	bool b = xml.Load(m_slngFile);
	if(	!b )
	{
		//debug, ֱ�ӡ�false �ɵ��¡���������
		_stprintf(g_strBuffer,TEXT("ERROR: Languange file <%s> cannot be load correctly!\r\n\r\nCAUSE: %s"),m_slngFile,xml.GetError());
		AfxMessageBox(g_strBuffer);
		//AfxMessageBox(xml.GetError());
		return true;//false;
	}

	if(xml.FindChildElem(TEXT("sysString")))
	{
		xml.IntoElem();
		int iSysStr = __min(_OA_STR_SYSTEM_NUMBER,_ttoi(xml.GetAttrib(TEXT("count"))));
		for(int i = 0;i<iSysStr;i++)
		{
			_stprintf(g_strBuffer,TEXT("str%d"),i+1);
			if(xml.FindChildElem(g_strBuffer))
			{		
				g_strSystem[i] = xml.GetChildData();								
			}			
		}
		xml.OutOfElem();
	}

	for(int i=0;i< _OA_FORM_VIEW_NUMBER;i++)
	{
		_stprintf(g_strBuffer,TEXT("item%d"),i+1);
		if ( xml.FindChildElem(g_strBuffer) )
		{
			xml.IntoElem();

			m_sTabLabel[i]   = xml.GetAttrib(TEXT("tabname"));
			m_sTabTooltip[i] = xml.GetAttrib(TEXT("tooltip"));
			switch(i)
			{
			case _OA_FORM_VIEW_SYSTEM: //0				
				if(xml.FindChildElem(TEXT("PlcState")))
				{
					xml.IntoElem();
					for(int j=0;j<_ITEM1_COM_PLC_STATE_CNT;j++)
					{//�������β��Һ���д
						_stprintf(g_strBuffer,TEXT("state%d"),j+1);
						g_item1_plc_state[j] = xml.GetAttrib(g_strBuffer);										
					}
					xml.OutOfElem();
				}

				if(xml.FindChildElem(TEXT("constID")))
				{
					xml.IntoElem();
					for(int j=0;j<_ITEM1_NEED_TEXT_ID_CNT;j++)
					{//�������β��Һ���д
						if(xml.FindChildElem(g_item1_ID_Name[j]))
						{		
							g_item1_ID_Text[j] = xml.GetChildData();										
						}
					}
					xml.OutOfElem();
				}

				if(xml.FindChildElem(TEXT("helpStr")))
				{
					xml.IntoElem();
					giHelpItems = __min(_ITEM1_NEED_HELP_TEXT,_ttoi(xml.GetAttrib(TEXT("count"))));
					_stprintf(g_strBuffer,TEXT("help"));
					for(int j=0;j<giHelpItems;j++)
					{//�������β��Һ���д
						//_stprintf(g_strBuffer,TEXT("help%d"),j+1);
						if(xml.FindChildElem(TEXT("help")))
						{		
							g_item1_Help_Text[j] = xml.GetChildData();										
						}
					}
					xml.OutOfElem();
				}				
				break;

			case _OA_FORM_VIEW_VACUUM: //1
				if(xml.FindChildElem(TEXT("VacState")))
				{
					xml.IntoElem();
					for(int j=0;j<_ITEM2_VAC_STATE_CNT;j++)
					{//�������β��Һ���д
						_stprintf(g_strBuffer,TEXT("state%d"),j+1);
						g_item2_Vac_state[j] = xml.GetAttrib(g_strBuffer);										
					}
					xml.OutOfElem();
				}
				
				if(xml.FindChildElem(TEXT("constID")))
				{
					xml.IntoElem();
					for(int j=0;j<gc_item2_Text_ID_CNT;j++)
					{//�������β��Һ���д
						if(xml.FindChildElem(g_item2_ID_Name[j]))
						{		
							g_item2_ID_Text[j] = xml.GetChildData();										
						}
					}
					xml.OutOfElem();
				}

				if(xml.FindChildElem(TEXT("otherString")))
				{
					xml.IntoElem();
					int iStrCnt = __min(_ITEM2_OTHER_STR_NUMBER,_ttoi(xml.GetAttrib(TEXT("count"))));
					for(int i = 0;i<iStrCnt;i++)
					{
						_stprintf(g_strBuffer,TEXT("str%d"),i+1);
						if(xml.FindChildElem(g_strBuffer))
						{		
							g_item2_String[i] = xml.GetChildData();								
						}			
					}
					xml.OutOfElem();
				}
				break;
			case _OA_FORM_VIEW_ROTATOR:
				if(xml.FindChildElem(TEXT("constID")))
				{
					xml.IntoElem();
					for(int j=0;j<gc_item3_Text_ID_CNT;j++)
					{//�������β��Һ���д
						if(xml.FindChildElem(g_item3_ID_Name[j]))
						{		
							g_item3_ID_Text[j] = xml.GetChildData();										
						}
					}
					xml.OutOfElem();
				}
				if(xml.FindChildElem(TEXT("otherString")))
				{
					xml.IntoElem();
					int iStrCnt = __min(_ITEM2_OTHER_STR_NUMBER,_ttoi(xml.GetAttrib(TEXT("count"))));
					for(int i = 0;i<iStrCnt;i++)
					{
						_stprintf(g_strBuffer,TEXT("str%d"),i+1);
						if(xml.FindChildElem(g_strBuffer))
						{		
							g_item3_String[i] = xml.GetChildData();								
						}			
					}
					xml.OutOfElem();
				}
				break;
			}
			xml.OutOfElem();
		}
	}	

	return true;
}

bool COACnfgDoc::fn_WriteCurLngInt(int iCurLng)
{
	if(iCurLng>m_iLngCnt)
		return false;
	CWinApp* pApp = AfxGetApp();
	if(NULL==pApp)
		return false;

	m_iCurLng = iCurLng;
	pApp->WriteProfileInt(TEXT("LANGUAGE"),TEXT("LNG_CURRENT"),iCurLng);

	return false;
}

LPCTSTR COACnfgDoc::fn_GetLocalTimeStr(void)
{
	SYSTEMTIME   st;   
	GetLocalTime(&st);   
	_stprintf(g_strTimeBuffer,TEXT("%4d-%02d-%02d %02d:%02d:%02d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	return (LPCTSTR)g_strTimeBuffer;
}

//ch3 penning
double COACnfgDoc::fn_TransPenningVoltPa(double dVolt)
{
	//3400��ʹ�� /4000��������ԭ���� /10000.0��20071009
	//return pow(10.0, dVolt * 7.0 / 4000.0 -7.0 ); //2008.02.18
	if(g_IonGauge.GetGaugeType() == 5 || g_IonGauge.GetGaugeType()==3) //VGC403 && MPG400  //2008.02.18
	{ //MPG400 . ==3 ,G1270����ռ�������S565/S566һ������ S565 DM9085=5, G1270 DM9085=3 .08.03.03		
		return pow( 10, dVolt / 400 * 11 / 10 - 6); // -8 mbar , 1mbar = 100 Pa,����ֱ�� -6
	}
	else
	{
		return pow( 10.0, dVolt * 7.0 / 4000.0 - 7.0 );
	}

	//p = 10^( (U - 7.75) / 0.75 + c ) ;// c = 2 , pa
}

void COACnfgDoc::fn_CreateCommError(_sPlcCommError& sCommErr)
{
	sCommErr.bObjectErr = ((g_pVacProThread->m_dwLastError == _VACUUM_PROCESS_LAST_ERR_WAIT_OBJECT)?true:false);
	if(!sCommErr.bObjectErr)
	{
		sCommErr.dwPlcLastError = g_plc.m_dwLastError;
		if(sCommErr.dwPlcLastError == _SYNCCOM_ERROR_SYS_API_)
		{
			sCommErr.dwPlcLastErrorEx = g_plc.m_dwLastSysApiError;
		}
		else if(sCommErr.dwPlcLastError == _SYSMAC_PLC_ERROR_END_CODE_)
		{
			sCommErr.dwPlcLastErrorEx = (DWORD)g_plc.m_iLastErrEndCode;
		}
	}
}

/***************************************************************************************************
�������ƣ�fn_LinearRegression
������
      [out] double& a;  ���Իع�õ��� б��
	  [out] double& b;  ���Իع�õ��� �ؾ�
	  [in]  double* x;  ������� x ����
	  [in]  double* y;  ������� y ����
	  [in]  int n;      ������� x,y ����ĳߴ�
˵���������Իع鷽�̣�Y = a + bx
*****************************************************************************************************/
int COACnfgDoc::fn_LinearRegression(double& a, double& b, double* x, double* y, int n=0)
{
    int m;
    double Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
    if (x == 0 || y == 0 || n < 1)
        return -1;
    for (m = 0; m < n; m ++)
    {
        xa += x[m];
        ya += y[m];
    }
    xa /= n;                                     // Xƽ��ֵ
    ya /= n;                                     // Yƽ��ֵ
    for (m = 0; m < n; m ++)
    {
        Lxx += ((x[m] - xa) * (x[m] - xa));             // Lxx = Sum((X - Xa)ƽ��)
        Lxy += ((x[m] - xa) * (y[m] - ya));       // Lxy = Sum((X - Xa)(Y - Ya))
    }
	
    b = Lxy / Lxx;                                 // b = Lxy / Lxx
    a = ya - b * xa;                              // a = Ya - b*Xa
	return 1;
}
/*
// �����Իع鷽�̣�Y = a + bx
// dada[rows*2]���飺X, Y��rows������������a, b�����ػع�ϵ��
// SquarePoor[4]�����ط������ָ��: �ع�ƽ���ͣ�ʣ��ƽ���ͣ��ع�ƽ���ʣ��ƽ����
// ����ֵ��0���ɹ���-1����
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor)
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
*/