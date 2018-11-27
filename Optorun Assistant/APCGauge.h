/************************************************************************************
File name: APCGauge.h
comment:   Head file of class CAPCGauge.
Author:    zhang ziye
Last Edit: 2008.05.19
��ע��     �������ڶ�Ӧ��̨APC����չ�
************************************************************************************/
#pragma once
//���¶�������OPTORUN PLC I/Oʽ����	��һ�¶�����5����ռ��Լ���ʵ�ֺ����ж������ѹ��pascal�Ļ����ϵ��
#define _TYPE_APC_GAUGE_PSG_AND_PENNING 0	//��Ĭ����ռ����ͣ�
#define _TYPE_APC_GAUGE_HPG400	        1
//#define _TYPE_APC_GAUGE_UNKNOWN			2 
#define _TYPE_APC_GAUGE_PEG100			2	//2010.04.14 
#define _TYPE_APC_GAUGE_MPG400			3
#define _TYPE_APC_GAUGE_ULVAC_BMR2_SC1	4	//2010.09.15  ����Ϊ�¼ӵĵ�����ռ�
#define _TYPE_APC_GAUGE_MAX				5



class CAPCGauge
{
public:
	CAPCGauge(void);
public:
	~CAPCGauge(void);
public:
	int m_nGaugeType;
	int m_nOutputMode; //��ʱδ��
	
public:
	int GetGaugeType(void)
	{
		return m_nGaugeType;
	}
public:
	double TransVal2Pascal(WORD wVal);
public:
	bool SetGaugeType(int nType);
};
