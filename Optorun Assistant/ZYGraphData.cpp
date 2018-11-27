#include "StdAfx.h"
#include "ZYGraphData.h"

CZYGraphData::CZYGraphData(void)
{
	bCanDelete = true;  //Ĭ�Ͽ�ɾ��
	pxData = NULL;      //x������, ������
	pyData = NULL;      //y����
	iDataNumber = 0;    //��������.

	//������ʾʱͼ�����������ƶ�.(x����)
	//��ν�����ƶ���ʵ���ǣ���ÿ�� x ��һ���Ĺ̶�ƫ����
	//ƫ����Ϊ��������ʾʱ�������������ƶ���ƫ����Ϊ�������������ƶ�
	dxDispShift = 0; //��ʾ���ݵ� x ��ʼ����ֵ. ��ʼ��Ϊ0
	
	iGraphStyle = _ZYGRAPH_STYLE_LINE; //��������.Ĭ��Ϊ����
	
	nPenStyle = PS_SOLID;
	nDotStyle = _ZYGRAPH_STYLE_DISCRETE_DOT;

	crColor   = RGB(255,0,0);

	penLine.CreatePen(nPenStyle,1,crColor);
}

CZYGraphData::~CZYGraphData(void)
{
	if(pxData)
	{
		delete [] pxData;
		pxData = NULL;
	}
	if(pyData)
	{
		delete [] pyData;
		pyData = NULL;
	}
	iDataNumber = 0;

	penLine.DeleteObject();
}

bool CZYGraphData::fn_ChangeColor(COLORREF crNewColor)
{
	if( crNewColor != crColor)
	{
		crColor = crNewColor;
		penLine.DeleteObject();
		penLine.CreatePen(nPenStyle,1,crColor);
	}
	return false;
}
