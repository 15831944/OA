// "ZYGraphPlot.h"
#pragma once
#include "atltypes.h"

#include "ZYGraphData.h"//CZYGraphData��ͷ�ļ�

#define _ZY_GRAPH_PLOT_TYPE_LIN 0  //���Ի�ͼ
#define _ZY_GRAPH_PLOT_TYPE_LOG 1  //������

#define _ZY_GRPAH_PLOT_COLOR_MAX 16 //����Ϊ�����ݷ�����ɫ

bool SaveDCBitmap(CDC* pDC,CRect rect,LPCTSTR lpszFileName);

class CZYGraphPlot
{
public:
	CZYGraphPlot(void);
public:
	~CZYGraphPlot(void);

	COLORREF m_clrSet[_ZY_GRPAH_PLOT_COLOR_MAX];  //����Ϊ�����ݵ���ʱ������ɫ.
	//��������ɫ = m_clrSet[i%_ZY_GRPAH_PLOT_COLOR_MAX];

	//ͼ�α�����ɫ��������ɫ. �ڳ�ʼ��ʱ���м�����������.
	COLORREF m_clrBk;     //������ɫ
	COLORREF m_clrGrid;   //��������ɫ
	COLORREF m_clrYScale; //y���������ɫ

	//
	CString m_strTitle;   //���⣬��ͼ���м��ϲ�
	double  m_alphaTitle; //������ʾ��͸����
	COLORREF m_crTitleBk; //����ͼ��ı���ɫ
	COLORREF m_crTitle;   //����������ɫ
	
	double  m_alphaHint;  //ͼ����ʾ��͸����
	COLORREF m_crHintBk;  //ͼ����ʾ�ı���ɫ.

	//x,y�Ữ�ֵ�������. �ڳ�ʼ�����м�����������.
	int      ixGridNum;
	int      iyGridNum;

	bool     bxGridDisp; //��ʾ x ����(������)����ʼΪ��ʾ
	int      ixMinutesPerGrid;  //x����ÿ�����ķ���������ʼΪ 1

	//��ʾ�����귶Χ
	double   yTop;
	double   yBot;
	double   xLeft;
	double   xRight;

	//��ͼ����, y���ʾ����
	int m_iYDrawType; //Ĭ��Ϊ����

	CPoint m_pDiamondOffset[25]; //���Σ���25��С��.
	CPoint m_pRectOffset[36];    //���Σ�36��С��.
	CPoint m_pTriangleOffset[28];//�����Σ�28��С��.
	CPoint m_pXShapeOffset[9];   //X��״, 9����
	CPoint m_pCrossOffset[9];    //ʮ�ּ�,9����
	CPoint m_pX1ShapeOffset[13]; //X�м��1���ߣ�13����

	CList <CZYGraphData*, CZYGraphData* &> listData;

public:
	bool PlotGraph(CDC* pDestDC,CRect rtDest);
private:
	void fn_InitOffset(void);//��ʼ������ʱ��ƫ����
	void fn_prePlotGrid(CDC& dc, CRect& rect);
	// �����
	void fn_PlotFrame(CDC& dc , CRect& rect);
public:
	// �ڸ���������pSrc�����в���ĳ����ֵ Ӧ�ö�Ӧ��λ��
	bool fn_findPos(double dData, int& iPos, double* pSrc, int iSrcNum,bool bPrePos);

public:
	// �õ�ǰ������������ʾ��. 
	void fn_PlotIllustration(CDC* pDestDC, CRect& rect, COLORREF crClr, int nGraphStyle, int nDotStyle,int nPenStyle);
public:
	// ���ڲ���1�����ݽ�����ʾ��ͼ
	int fn_PlotIllustrUse1st(CDC* pDestDC, CRect& rect,COLORREF crBk, COLORREF crGrid);
};
