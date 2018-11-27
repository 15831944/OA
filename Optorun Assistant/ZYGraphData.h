#pragma once
/******************************************************************
������ʾ�������ࡣ�洢һ����ά�������ݼ�������ʾ������
*******************************************************************/
#define _ZYGRAPH_STYLE_DISCRETE  0 //����ɢ��
#define _ZYGRAPH_STYLE_LINE      1 //���ߣ�����ʱ���ʵķ���� nLinePenStyle���壬Ĭ��ΪPS_SOLID
#define _ZYGRAPH_STYLE_DOT_LINE  2 //�㻭��

#define _ZYGRAPH_STYLE_DISCRETE_DOT    0   //�����
#define _ZYGRAPH_STYLE_DISCRETE_DIMOND 1   //����
#define _ZYGRAPH_STYLE_DISCRETE_RECT   2   //������
#define _ZYGRAPH_STYLE_DISCRETE_TRIANGLE 3 //������
#define _ZYGRAPH_STYLE_DISCRETE_X_SHAPE 4 //X��״  
#define _ZYGRAPH_STYLE_DISCRETE_CROSS    5 //ʮ����
#define _ZYGRAPH_STYLE_DISCRETE_X_1_SHAPE 6 // X�м��1����

class CZYGraphData
{
public:
	CZYGraphData(void);
public:
	virtual ~CZYGraphData(void);

	double* pxData;      //x������, ��������
	double* pyData;      //   ��Ӧy����
	int     iDataNumber; //��������.

	//������ʾʱͼ�����������ƶ�.(x����)
	//��ν�����ƶ���ʵ���ǣ���ÿ�� x ��һ���Ĺ̶�ƫ����
	//ƫ����Ϊ��������ʾʱ�������������ƶ���ƫ����Ϊ�������������ƶ�
	double  dxDispShift; //��ʾ���ݵ� x ��ʼ����ֵ. ��ʼ��Ϊ0
	
	CString strName;     //��������

	int     iGraphStyle; //��������.Ĭ��Ϊ��
	int     nDotStyle;

	int     nPenStyle;
	CPen    penLine;    //������ʱ�Ļ���.

	COLORREF crColor;
	bool    bCanDelete; //��Ҫ���ڵ�ǰ���Խ�����ݲ���ɾ����
	                   //Ĭ�Ͽ�ɾ��(true)������ǰ����������Ҫ����д��false.
	
public:
	bool fn_ChangeColor(COLORREF crNewColor);
};
