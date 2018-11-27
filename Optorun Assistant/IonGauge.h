/************************************************************************************
File name: IonGauge.h
comment:   Head file of class CIonGauge.
Author:    zhang ziye
Last Edit: 2007.08.22
��ע��������ṩ�� ���еļ��� ������չ� ����ʶ��� ��ѹ--> Pa ��նȼ���
************************************************************************************/
#pragma once

//���¶�������OPTORUN PLC I/Oʽ����	�����¶�����5��������ռƵĵ�ѹpascal���㷽����
#define _TYPE_ION_GAUGE_NONE   0	//����������ռƣ�
#define _TYPE_ION_GAUGE_GIM2   1
#define _TYPE_ION_GAUGE_M832   2
#define _TYPE_ION_GAUGE_BPG400 3
#define _TYPE_ION_GAUGE_GID7   4
#define _TYPE_ION_GAUGE_BPG402 5 //2008.02.14���. VGC403��
#define _TYPE_ION_GAUGE_MAX    5

//������ģ�����ʽ��
#define _ION_GAUGE_MODE_LINEAR 0 //Ĭ��ֵ
/****************�������ֵ���������ʽ��û��ʹ��********************************/
#define _ION_GAUGE_MODE_LOG    1
#define _ION_GAUGE_REC_HOLD    2 //GI-M2 �д��趨

class CIonGauge
{
public:
	CIonGauge(void);
public:
	~CIonGauge(void);
public:
	// Ion Gauge Type
	int m_nGaugeType;
	int m_nOutputMode;
	bool m_bIonGaugeExist; //��SetGaugeType����ʱ���á�
public:
	// Set Gauge Type from PLC DM RE
	bool SetGaugeType(LPCSTR strGaugeType);
public:
	// Get Gauge Type Name
	void GetGaugeTypeName(CString& strTypeName);
	CString GetGaugeTypeName(void);
public:

	int GetGaugeType(void)
	{
		return m_nGaugeType;
	}
public:
	// Translate vacuum pressure from volt to pascal
	double TransStr2Pascal(LPCSTR strVolt);

	//Translate plc DM buffer value TO pascal
	double TransVal2Pascal(WORD wVal);
public:
	bool SetGaugeType(int nType);
public:

	bool HasIonGauge(void)
	{
		return m_bIonGaugeExist;
		//return ( m_nGaugeType != _TYPE_ION_GAUGE_NONE );
	}
public:
	int SetOutputMode(int nNewOutputMode);
public:
	bool SetGaugeTypeEx(int nType, bool bExsit);
};
