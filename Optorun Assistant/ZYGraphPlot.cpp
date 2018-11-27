#include "StdAfx.h"
#include "ZYGraphPlot.h"
#include <math.h>

CZYGraphPlot::CZYGraphPlot(void)
: m_iYDrawType(_ZY_GRAPH_PLOT_TYPE_LOG)
{
	m_clrBk   = RGB(0,0,0);   //ͼ�α�����ɫ����ɫ
	m_clrGrid = RGB(0,128,64);//(51,128,128);//(51,153,51); //��������ɫ 51,128,128
	m_clrYScale = RGB(64,196,64);

	ixGridNum = 30;
	iyGridNum = 11;  // ��Ӧ1.0E+5 ~ 1.0E-6
	bxGridDisp = true; //��ʾ x ����(������)����ʼΪ��ʾ
	ixMinutesPerGrid = 1;  //x����ÿ�����ķ���������ʼΪ 1

	//��ʾ�����귶Χ
	yTop = 1.0E+5;
	yBot = 1.0E-6;
	xLeft = 0;
	xRight = (double) ixGridNum*60; //60��һ��

	//m_clrSet[_ZY_GRPAH_PLOT_MAX_CURVES];
	m_clrSet[0] = RGB(255,0,0); //rgb
	m_clrSet[1] = RGB(0,0,255); //RGB(255,192
	m_clrSet[2] = RGB(0,255,0); 
	m_clrSet[3] = RGB(0,255,255); 
	m_clrSet[4] = RGB(255,0,255);  //rgb
	m_clrSet[5] = RGB(255,255,0); 
	m_clrSet[6] = RGB(192,0,192); 
	m_clrSet[7] = RGB(0,128,128); 
	m_clrSet[8] = RGB(128,128,0); 
	m_clrSet[9] = RGB(128,0,128); 
	m_clrSet[10] = RGB(0,0,192); 
	m_clrSet[11] = RGB(0,192,192); 
	m_clrSet[12] = RGB(192,0,192); 
	m_clrSet[13] = RGB(0,192,128); 
	m_clrSet[14] = RGB(192,0,0); 
	m_clrSet[15] = RGB(0,192,0); 
	/*
	for(int i=16;i<_ZY_GRPAH_PLOT_COLOR_MAX;i++)
	{
		m_clrSet[i] = m_clrSet[i%16];
	}
	*/
	fn_InitOffset();
	m_alphaTitle = 0.6; //������ʾ��͸����
	m_crTitleBk = RGB(32,64,32); //����ͼ��ı���ɫ
	m_crTitle = RGB(128,255,128);   //����������ɫ

	this->m_alphaHint = 0.7;
	this->m_crHintBk = RGB(32,64,32);
	

}
void CZYGraphPlot::fn_InitOffset(void)
{
	/*����,25����. �����ĵ�Ϊԭ�� (0,0)��25���ڴ���������λ�á���ʾʱ��
					 +
					+++
				   +++++
				  +++0+++
				   +++++
					+++
					 +
	*/
	//for(int i=0;i<sizeof(m_pDiamondOffset)/sizeof(CPoint);
	//��1�� 1
	m_pDiamondOffset[0].x = 0;
	m_pDiamondOffset[0].y = -3;
	//��2��, 3 y=-2, x=[-1,1];
	m_pDiamondOffset[1].x = -1;
	m_pDiamondOffset[1].y = -2;
	m_pDiamondOffset[2].x = 0;
	m_pDiamondOffset[2].y = -2;
	m_pDiamondOffset[3].x = 1;
	m_pDiamondOffset[3].y = -2;
    //��3��, 5 y=-1, x=[-2,2];
	m_pDiamondOffset[4].x = -2;
	m_pDiamondOffset[4].y = -1;
	m_pDiamondOffset[5].x = -1;
	m_pDiamondOffset[5].y = -1;
	m_pDiamondOffset[6].x = 0;
	m_pDiamondOffset[6].y = -1;
	m_pDiamondOffset[7].x = 1;
	m_pDiamondOffset[7].y = -1;
	m_pDiamondOffset[8].x = 2;
	m_pDiamondOffset[8].y = -1;
    //��4��, 7 y=0,  x=[-3,3]
	m_pDiamondOffset[9].x = -3;
	m_pDiamondOffset[9].y = 0;
	m_pDiamondOffset[10].x = -2;
	m_pDiamondOffset[10].y = 0;
	m_pDiamondOffset[11].x = -1;
	m_pDiamondOffset[11].y = 0;
	m_pDiamondOffset[12].x = 0;
	m_pDiamondOffset[12].y = 0;
	m_pDiamondOffset[13].x = 1;
	m_pDiamondOffset[13].y = 0;
	m_pDiamondOffset[14].x = 2;
	m_pDiamondOffset[14].y = 0;
	m_pDiamondOffset[15].x = 3;
	m_pDiamondOffset[15].y = 0;
	//��5��, 5 y=1,  x=[-2,2]
	m_pDiamondOffset[16].x = -2;
	m_pDiamondOffset[16].y = 1;
	m_pDiamondOffset[17].x = -1;
	m_pDiamondOffset[17].y = 1;
	m_pDiamondOffset[18].x = 0;
	m_pDiamondOffset[18].y = 1;
	m_pDiamondOffset[19].x = 1;
	m_pDiamondOffset[19].y = 1;
	m_pDiamondOffset[20].x = 2;
	m_pDiamondOffset[20].y = 1;
	//��6��, 3 y=2, x=[-1,1]
	m_pDiamondOffset[21].x = -1;
	m_pDiamondOffset[21].y = 2;
	m_pDiamondOffset[22].x = 0;
	m_pDiamondOffset[22].y = 2;
	m_pDiamondOffset[23].x = 1;
	m_pDiamondOffset[23].y = 2;
	//��7��, 1 y=3, x=0
	m_pDiamondOffset[24].x = 0;
	m_pDiamondOffset[24].y = 3;

	//������, 1,2...7.��28����
	/*
	+
   ++
   +++
  ++0+
  +++++
 ++++++
 +++++++
	*/
	//��1��,y=-3
	m_pTriangleOffset[0].x =  0; 
	m_pTriangleOffset[0].y = -3;
	//��2��,y=-2
	m_pTriangleOffset[1].x = -1; 
	m_pTriangleOffset[1].y = -2;
	m_pTriangleOffset[2].x =  0;
	m_pTriangleOffset[2].y = -2;
	//��3��,y=-1
	m_pTriangleOffset[3].x = -1;
	m_pTriangleOffset[3].y = -1;
	m_pTriangleOffset[4].x =  0;
	m_pTriangleOffset[4].y = -1;
	m_pTriangleOffset[5].x =  1;
	m_pTriangleOffset[5].y = -1;
    //��4��,y=0
	m_pTriangleOffset[6].x = -2;
	m_pTriangleOffset[6].y =  0;
	m_pTriangleOffset[7].x = -1;
	m_pTriangleOffset[7].y =  0;
	m_pTriangleOffset[8].x =  0;
	m_pTriangleOffset[8].y =  0;
	m_pTriangleOffset[9].x =  1;
	m_pTriangleOffset[9].y =  0;
	//��5��,y=1
	m_pTriangleOffset[10].x =-2; 
	m_pTriangleOffset[10].y = 1;
	m_pTriangleOffset[11].x =-1; 
	m_pTriangleOffset[11].y = 1;
	m_pTriangleOffset[12].x = 0;
	m_pTriangleOffset[12].y = 1;
	m_pTriangleOffset[13].x = 1;
	m_pTriangleOffset[13].y = 1;
	m_pTriangleOffset[14].x = 2;
	m_pTriangleOffset[14].y = 1;
	//��6��,y=2. x=[-3,2]
	m_pTriangleOffset[15].x =-3;
	m_pTriangleOffset[15].y = 2;
	m_pTriangleOffset[16].x =-2; 
	m_pTriangleOffset[16].y = 2;
	m_pTriangleOffset[17].x =-1; 
	m_pTriangleOffset[17].y = 2;
	m_pTriangleOffset[18].x = 0;
	m_pTriangleOffset[18].y = 2;
	m_pTriangleOffset[19].x = 1;
	m_pTriangleOffset[19].y = 2;
	m_pTriangleOffset[20].x = 2;
	m_pTriangleOffset[20].y = 2;
	//��7��,y=3.x=[-3,3]
	m_pTriangleOffset[21].x =-3;
	m_pTriangleOffset[21].y = 3;
	m_pTriangleOffset[22].x =-2; 
	m_pTriangleOffset[22].y = 3;
	m_pTriangleOffset[23].x =-1; 
	m_pTriangleOffset[23].y = 3;
	m_pTriangleOffset[24].x = 0; 
	m_pTriangleOffset[24].y = 3;
	m_pTriangleOffset[25].x = 1;
	m_pTriangleOffset[25].y = 3;
	m_pTriangleOffset[26].x = 2;
	m_pTriangleOffset[26].y = 3;
	m_pTriangleOffset[27].x = 3;
	m_pTriangleOffset[27].y = 3;

//����
/*****
x [-2,3]; y[-3,2]
++++++ 
++++++
++++++
++0+++
++++++
++++++
******/
	int n=0;
	for(int i=-2;i<=3;i++)
	{
		for(int j=-3;j<=2;j++)
		{
			m_pRectOffset[n].x = i;
			m_pRectOffset[n++].y = j;
		}
	}

	// X ��״ 9��
/*******
+   +
 + +
  0
 + +
+   +
********/
	m_pXShapeOffset[0].x = -2;
	m_pXShapeOffset[0].y = -2;
	m_pXShapeOffset[1].x =  2;
	m_pXShapeOffset[1].y = -2;
	m_pXShapeOffset[2].x = -1;
	m_pXShapeOffset[2].y = -1;
	m_pXShapeOffset[3].x = 1;
	m_pXShapeOffset[3].y = -1;
	m_pXShapeOffset[4].x = 0;
	m_pXShapeOffset[4].y = 0;
	m_pXShapeOffset[5].x = -1;
	m_pXShapeOffset[5].y = 1;
	m_pXShapeOffset[6].x = 1;
	m_pXShapeOffset[6].y = 1;
	m_pXShapeOffset[7].x = -2;
	m_pXShapeOffset[7].y = 2;
	m_pXShapeOffset[8].x = 2;
	m_pXShapeOffset[8].y = 2;

	//ʮ����״ 9��
/***************
  +
  +
+++++
  +
  +
***************/
	m_pCrossOffset[0].x = 0;
	m_pCrossOffset[0].y = -2;
	m_pCrossOffset[1].x = 0;
	m_pCrossOffset[1].y = -1;
	m_pCrossOffset[2].x = -2;
	m_pCrossOffset[2].y = 0;
	m_pCrossOffset[3].x = -1;
	m_pCrossOffset[3].y = 0;
	m_pCrossOffset[4].x = 0;
	m_pCrossOffset[4].y = 0;
	m_pCrossOffset[5].x = 1;
	m_pCrossOffset[5].y = 0;
	m_pCrossOffset[6].x = 2;
	m_pCrossOffset[6].y = 0;
	m_pCrossOffset[7].x = 0;
	m_pCrossOffset[7].y = 1;
	m_pCrossOffset[8].x = 0;
	m_pCrossOffset[8].y = 2;

	//X�м��1����, 13��
/*****************
+ + +
 +++
  0
 +++
+ + +
*****************/
	//��1��
	m_pX1ShapeOffset[0].x = -2;
	m_pX1ShapeOffset[0].y = -2;
	m_pX1ShapeOffset[1].x =  0;
	m_pX1ShapeOffset[1].y = -2;
	m_pX1ShapeOffset[2].x =  2;
	m_pX1ShapeOffset[2].y = -2;
	//��2��
	m_pX1ShapeOffset[3].x = -1;
	m_pX1ShapeOffset[3].y = -1;
	m_pX1ShapeOffset[4].x =  0;
	m_pX1ShapeOffset[4].y = -1;
	m_pX1ShapeOffset[5].x =  1;
	m_pX1ShapeOffset[5].y = -1;
	//��3��	
	m_pX1ShapeOffset[6].x =  0;
	m_pX1ShapeOffset[6].y =  0;
	//��4��
	m_pX1ShapeOffset[7].x = -1;
	m_pX1ShapeOffset[7].y =  1;
	m_pX1ShapeOffset[8].x =  0;
	m_pX1ShapeOffset[8].y =  1;
	m_pX1ShapeOffset[9].x =  1;
	m_pX1ShapeOffset[9].y =  1;
	//��5��	
	m_pX1ShapeOffset[10].x =-2;
	m_pX1ShapeOffset[10].y = 2;
	m_pX1ShapeOffset[11].x = 0;
	m_pX1ShapeOffset[11].y = 2;
	m_pX1ShapeOffset[12].x = 2;
	m_pX1ShapeOffset[12].y = 2;
}

CZYGraphPlot::~CZYGraphPlot(void)
{
}


/*********************************************************************
�������ƣ�PlotLogGraph
���������CDC* pDestDC, Ŀ��DC
          CRect rtDest, Ŀ������
		  double yTop,  ͼ�ζ���ֵ
		  double yBot,  ͼ�ε׶�ֵ
		  int    iyGridNum, ������Grid ��
		  int    ixGridNum, ������Grid��(�ȷ���)
		  int    iDispWidth, ͼ������ʾ���
		  int    iNumber, ����ʾ��������
		  double** pData, ����ʾ����Դ��ָ�����飬ÿ��Ϊ double*
		  int*   ipDataDispLeft, ÿ��������ͼ���е������ʾλ��. �����ɸ���
		  int*   ipDataLeft,     ÿ����������������ڲ��Ĵ洢λ��.
		  int*   ipDataDispLen,  ÿ��������ʾ�����ݳ���.
		  LPCTSTR* lpDataName, ÿ����ʾ����Դ�����ƣ� 
ͼ�κ����겻�趨��λ���ŵ���ʡ��һ��Ĵ洢�ռ䣬���ϵ�һ���������ص�. 
          
���������
��������  ��ָ��DCָ����������ͼ��ͼ��������ΪLog��ʽ
          ��Ȼ���Ի��������ߣ����޶�����ͼ����ֻ��һ�������꣮	 ����
		  ͼ�ο��������ƶ�.
**********************************************************************/
bool CZYGraphPlot::PlotGraph(CDC* pDestDC, CRect rtDest)
{
	CDC memDC;
	CBitmap srcBitmap, *oldBitmap;

	CRect mrect = rtDest; //draw rect in memdc
	mrect.OffsetRect(-rtDest.left,-rtDest.top); // zero based in memdc

	//create memDC and inside bitmap
	memDC.CreateCompatibleDC(pDestDC);
	srcBitmap.CreateCompatibleBitmap(pDestDC,mrect.Width(),mrect.Height()); 
	oldBitmap = memDC.SelectObject(&srcBitmap);


	memDC.SetBkColor(m_clrBk);
	this->fn_prePlotGrid(memDC,mrect);
	
	//������
	double perX = (double) (mrect.Width()-4)/(xRight-xLeft);
	double perY;

	double yTempTop;
	if( m_iYDrawType == _ZY_GRAPH_PLOT_TYPE_LOG )
	{
		perY = (mrect.Height()-4)/(log10(yTop)-log10(yBot));
		yTempTop = log10(yTop);
		//k0 = ( double ) ( 2-(mrect.Height()-3)) / ( log10(yTop)-log10(yBot) );
		//k1 = (double) ( 2 - k0 * log10(yTop));
	}
	else
	{
		perY = (mrect.Height()-4)/( yTop - yBot );
		yTempTop = yTop;
		//k0 = (double) ( 2-(mrect.Height()-3)) / ( yTop-yBot );
		//k1 = (double) ( 2 - k0 * yTop);
	}
	
	CZYGraphData* p;
	CPen*   oldPen;
	POSITION pos = listData.GetHeadPosition();
	int i, j; //�����ñ���.  
	int x, y; //x,y����λ�ñ���
//	double dt; //��ʱdouble����
	int iXStart,iXEnd;//��ͼ��Ч��������ʼ����ֹλ��
	bool b;
	int iCurveCount = 0; //��¼ʵ����ͼ��������Ŀ
	int n;
	for (i=0;i < listData.GetCount();i++)
	{
		p = listData.GetNext(pos);

		if( 0 == p->iDataNumber || NULL == p->pyData )
		{ //���û�����ݣ�������
			continue;
		}
		iCurveCount++;
		
		//��ú��ʵ�����������յ� X λ�ã��ü��� ���� ��ʾ���� ������ݣ��Լ��ٺ�ʱ��
		b = fn_findPos(__max(xLeft-p->dxDispShift,p->pxData[0]),iXStart,p->pxData,p->iDataNumber,true);
		b &= fn_findPos(__min(xRight-p->dxDispShift,p->pxData[p->iDataNumber-1]),iXEnd,p->pxData,p->iDataNumber,false);
		if(!b)
		{//���û���ҵ����ʵ������յ㣬������
			continue;
		}

		if( _ZYGRAPH_STYLE_DISCRETE == p->iGraphStyle )
		{
			if(_ZYGRAPH_STYLE_DISCRETE_DOT == p->nDotStyle )
			{//��
				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop); 
					memDC.SetPixelV(x,y,p->crColor);
				}
			}
			else if(_ZYGRAPH_STYLE_DISCRETE_DIMOND == p->nDotStyle )
			{

				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					//����,ÿ��������25��С��,һ��һ��ɵ��
					for(n = 0;n<sizeof(m_pDiamondOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pDiamondOffset[n].x,y+m_pDiamondOffset[n].y,p->crColor);
					}					
				}
				//memDC.SelectObject(oldbr);
				//br.DeleteObject();
			}
			else if(_ZYGRAPH_STYLE_DISCRETE_RECT == p->nDotStyle )
			{//������

				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					//������,28��,һ��һ��ɵ��
					for(n = 0;n<sizeof(m_pRectOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pRectOffset[n].x,y+m_pRectOffset[n].y,p->crColor);
					}	
				}
			}
			else if(_ZYGRAPH_STYLE_DISCRETE_TRIANGLE == p->nDotStyle )
			{//������

				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					//������,28��,һ��һ��ɵ��
					for(n = 0;n<sizeof(m_pTriangleOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pTriangleOffset[n].x,y+m_pTriangleOffset[n].y,p->crColor);
					}	
				}
			}
			else if(_ZYGRAPH_STYLE_DISCRETE_X_SHAPE == p->nDotStyle )
			{

				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					//X,9��,һ��һ��ɵ��
					for(n = 0;n<sizeof(m_pXShapeOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pXShapeOffset[n].x,y+m_pXShapeOffset[n].y,p->crColor);
					}	
				}
			}
			else if(_ZYGRAPH_STYLE_DISCRETE_CROSS == p->nDotStyle )
			{

				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					//ʮ�֣�9��,һ��һ��ɵ��
					for(n = 0;n<sizeof(m_pCrossOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pCrossOffset[n].x,y+m_pCrossOffset[n].y,p->crColor);
					}	
				}
			}
			else if(_ZYGRAPH_STYLE_DISCRETE_X_1_SHAPE == p->nDotStyle)
			{

				for( j=iXStart;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					//X��1����, 13�㣬һ��һ��ɵ��
					for(n = 0;n<sizeof(m_pX1ShapeOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pX1ShapeOffset[n].x,y+m_pX1ShapeOffset[n].y,p->crColor);
					}	
				}
			}
		}
		else if(_ZYGRAPH_STYLE_LINE == p->iGraphStyle )
		{
			oldPen = memDC.SelectObject(&p->penLine);
			memDC.MoveTo( 2 + perX * (p->pxData[iXStart] + p->dxDispShift-xLeft), 2 - perY*(log10(p->pyData[iXStart])-yTempTop) );
			for( j=iXStart+1;j<=iXEnd;j++)
			{
				x = 2 + perX * (p->pxData[j] + p->dxDispShift - xLeft);
				y = 2 - perY*(log10(p->pyData[j])-yTempTop);
				memDC.LineTo(x,y);
			}

			if(j == iXEnd+1)
			{//��Ϊ LineTo ����������ֹ�㣬�˴����һ��.
				memDC.SetPixel(x,y,p->crColor);
			}
			memDC.SelectObject(oldPen);			
		}
		else if(_ZYGRAPH_STYLE_DOT_LINE == p->iGraphStyle )
		{//�㻭�ߣ���ÿ��֮����ͼ�Σ������֮��������
			oldPen = memDC.SelectObject(&p->penLine);
			x = 2 + perX * ( p->pxData[iXStart] + p->dxDispShift -xLeft);
			y = 2 - perY*(log10(p->pyData[iXStart])-yTempTop);
			memDC.MoveTo(x,y);
			switch(p->nDotStyle)
			{
			case _ZYGRAPH_STYLE_DISCRETE_DIMOND:
				for(n = 0;n<sizeof(m_pDiamondOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pDiamondOffset[n].x,y+m_pDiamondOffset[n].y,p->crColor);
				}
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					for(n = 0;n<sizeof(m_pDiamondOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pDiamondOffset[n].x,y+m_pDiamondOffset[n].y,p->crColor);
					}
					memDC.LineTo(x,y);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_RECT:
				for(n = 0;n<sizeof(m_pRectOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pRectOffset[n].x,y+m_pRectOffset[n].y,p->crColor);
				}
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					for(n = 0;n<sizeof(m_pRectOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pRectOffset[n].x,y+m_pRectOffset[n].y,p->crColor);
					}
					memDC.LineTo(x,y);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_TRIANGLE:
				for(n = 0;n<sizeof(m_pTriangleOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pTriangleOffset[n].x,y+m_pTriangleOffset[n].y,p->crColor);
				}
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					for(n = 0;n<sizeof(m_pTriangleOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pTriangleOffset[n].x,y+m_pTriangleOffset[n].y,p->crColor);
					}
					memDC.LineTo(x,y);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_X_SHAPE:
				for(n = 0;n<sizeof(m_pXShapeOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pXShapeOffset[n].x,y+m_pXShapeOffset[n].y,p->crColor);
				}
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					for(n = 0;n<sizeof(m_pXShapeOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pXShapeOffset[n].x,y+m_pXShapeOffset[n].y,p->crColor);
					}
					memDC.LineTo(x,y);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_CROSS:
				for(n = 0;n<sizeof(m_pCrossOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pCrossOffset[n].x,y+m_pCrossOffset[n].y,p->crColor);
				}
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					for(n = 0;n<sizeof(m_pCrossOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pCrossOffset[n].x,y+m_pCrossOffset[n].y,p->crColor);
					}
					memDC.LineTo(x,y);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_X_1_SHAPE:
				for(n = 0;n<sizeof(m_pX1ShapeOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pX1ShapeOffset[n].x,y+m_pX1ShapeOffset[n].y,p->crColor);
				}
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					for(n = 0;n<sizeof(m_pX1ShapeOffset)/sizeof(CPoint);n++)
					{
						memDC.SetPixelV(x+m_pX1ShapeOffset[n].x,y+m_pX1ShapeOffset[n].y,p->crColor);
					}
					memDC.LineTo(x,y);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_DOT:
			default:
				for( j=iXStart+1;j<=iXEnd;j++)
				{
					x =  2 + perX * ( p->pxData[j] + p->dxDispShift -xLeft) ; //��˴�2��ʼ
					y =  2 - perY*(log10(p->pyData[j])-yTempTop);
					memDC.LineTo(x,y);
				}
				//����ٴ�һ�㣬��Ϊ lineto�����������һ��
				memDC.SetPixelV(2 + perX * ( p->pxData[iXEnd] + p->dxDispShift -xLeft) ,2 - perY*(log10(p->pyData[iXEnd])-yTempTop),p->crColor);
				break;
			}
			memDC.SelectObject(oldPen);
		}
	}
	//�����
	fn_PlotFrame(memDC,mrect);

	//����������������ʾ begin
	CDC memDC2;
	CBitmap sBmp2,*oldBmp2;
	memDC2.CreateCompatibleDC(pDestDC);
	sBmp2.CreateCompatibleBitmap(pDestDC,mrect.Width(),mrect.Height()); 
	oldBmp2 = memDC2.SelectObject(&sBmp2);
	CRect rt;
	CSize sz = memDC2.GetTextExtent(m_strTitle);
	rt.left = 0;
	rt.top  = 0;
	rt.bottom = sz.cy + 4;
	rt.right  = sz.cx + 4;
	memDC2.SetBkMode(TRANSPARENT);
	memDC2.SetBkColor(m_crTitleBk);
	//memDC2.SetDCPenColor(m_crTitle);
	memDC2.SetTextColor(m_crTitle);
	memDC2.FillSolidRect(&rt,m_crTitleBk);
	memDC2.TextOut(2,2,m_strTitle);

	BLENDFUNCTION bm;
	bm.BlendOp=AC_SRC_OVER;
	bm.BlendFlags=0;
	bm.SourceConstantAlpha = m_alphaTitle*255;
	bm.AlphaFormat=0; 
	if(!m_strTitle.IsEmpty())
	{//������������ַ�	
		AlphaBlend(memDC.m_hDC,mrect.Width()/2-rt.Width()/2,3,rt.Width(),rt.Height(),memDC2.m_hDC,0,0,rt.Width(),rt.Height(),bm); 
	}
    //������ʾ����Ϊ�����������ұ� 1/5����.

	//int i, j; //�����ñ���.  
	//int x, y; //x,y����λ�ñ���
	rt.left = (mrect.Width() - 4 ) * 4 / 5; //�����
	rt.top  = 0;
	rt.right = mrect.Width() - 4 ; 
	rt.bottom = mrect.Height() - 4;

	memDC2.FillSolidRect(&mrect,m_crHintBk);
	sz = memDC2.GetTextExtent(CString(TEXT("TESTSTRING"))); //ȡ�õ�ǰ�ַ�����ĸ߶�
	y = 0; //��¼��ʼ���Ͻ�λ��
	int nInterval = 4; //����֮��ļ��	
	pos = listData.GetHeadPosition();
	for (i=0;i < listData.GetCount();i++)
	{
		p = listData.GetNext(pos);
		if( 0 == p->iDataNumber || NULL == p->pyData )
		{ //���û�����ݣ�������
			continue;
		}
		//������ʾ��
		oldPen = memDC2.SelectObject(&p->penLine);
        int y2 = y+sz.cy/2+nInterval/2;//�����µ�yλ��
		if( _ZYGRAPH_STYLE_LINE == p->iGraphStyle)
		{
			memDC2.MoveTo(3,y2);
			memDC2.LineTo(24,y2);
		}
		else
		{
			if( _ZYGRAPH_STYLE_DOT_LINE == p->iGraphStyle)
			{
				memDC2.MoveTo(3,y2);
				memDC2.LineTo(24,y2);
			}

			if( _ZYGRAPH_STYLE_DOT_LINE==p->iGraphStyle || _ZYGRAPH_STYLE_DISCRETE==p->iGraphStyle)
			{
				switch(p->nDotStyle)
				{
				case _ZYGRAPH_STYLE_DISCRETE_DOT:
					for(n = 3; n<24; n+=3)
					{
						//memDC2.SetPixelV(n,x,p->crColor);
						memDC2.SetPixelV(n,y2,p->crColor); //08.05.19
					}
					break;
				case _ZYGRAPH_STYLE_DISCRETE_DIMOND:
					for(n = 0;n<sizeof(m_pDiamondOffset)/sizeof(CPoint);n++)
					{
						memDC2.SetPixelV(14+m_pDiamondOffset[n].x,y2+m_pDiamondOffset[n].y,p->crColor);
					}
					break;
				case _ZYGRAPH_STYLE_DISCRETE_RECT:
					for(n = 0;n<sizeof(m_pRectOffset)/sizeof(CPoint);n++)
					{
						memDC2.SetPixelV(14+m_pRectOffset[n].x,y2+m_pRectOffset[n].y,p->crColor);
					}
					break;
				case _ZYGRAPH_STYLE_DISCRETE_TRIANGLE:
					for(n = 0;n<sizeof(m_pTriangleOffset)/sizeof(CPoint);n++)
					{
						memDC2.SetPixelV(14+m_pTriangleOffset[n].x,y2+m_pTriangleOffset[n].y,p->crColor);
					}
					break;
				case _ZYGRAPH_STYLE_DISCRETE_X_SHAPE:
					for(n = 0;n<sizeof(m_pXShapeOffset)/sizeof(CPoint);n++)
					{
						memDC2.SetPixelV(14+m_pXShapeOffset[n].x,y2+m_pXShapeOffset[n].y,p->crColor);
					}
					break;
				case _ZYGRAPH_STYLE_DISCRETE_CROSS:
					for(n = 0;n<sizeof(m_pCrossOffset)/sizeof(CPoint);n++)
					{
						memDC2.SetPixelV(14+m_pCrossOffset[n].x,y2+m_pCrossOffset[n].y,p->crColor);
					}
					break;
				case _ZYGRAPH_STYLE_DISCRETE_X_1_SHAPE:
					for(n = 0;n<sizeof(m_pX1ShapeOffset)/sizeof(CPoint);n++)
					{
						memDC2.SetPixelV(14+m_pX1ShapeOffset[n].x,y2+m_pX1ShapeOffset[n].y,p->crColor);
					}
					break;
				}
			}
		}

		memDC2.SelectObject(oldPen);
		//3��10��1��10��3	
		//д�ַ�
		static TCHAR pcT[256];
		
		swprintf(pcT,255,TEXT("% 4d %s"),(int)p->dxDispShift,p->strName);
		memDC2.SetTextColor(p->crColor);
		memDC2.TextOut(3+10+1+10+3,y+nInterval/2,pcT);//p->strName);
		y += sz.cy + nInterval;
	}
	if( y!=0)
	{
		bm.SourceConstantAlpha = m_alphaHint*255;
		if(y >= rt.Height())
		{
			AlphaBlend(memDC.m_hDC,rt.left,0,rt.Width(),rt.Height()-1,memDC2.m_hDC,0,0,rt.Width(),rt.Height()-1,bm); 
		}
		else
		{
			AlphaBlend(memDC.m_hDC,rt.left,rt.Height()/2-y/2,rt.Width(),y,memDC2.m_hDC,0,0,rt.Width(),y,bm); 
		}
	}

	memDC2.SelectObject(oldBmp2);
	sBmp2.DeleteObject();
	//��������������ͼʾ end	

	//Bitblt memory bitmap to view 
	pDestDC->BitBlt(rtDest.left,rtDest.top,mrect.Width(),mrect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(oldBitmap);
	srcBitmap.DeleteObject();
	return false;
}

//������������
void CZYGraphPlot::fn_prePlotGrid(CDC& dc, CRect& rect)
{
	//LineTo�������� LineTo(point)�е� point!!
	//(mrect.bottom , mrect.right)���������� mrect�У�
	
	//1, ����߿�. ռ�����¹���4����.
	//��ɫ
	dc.FillSolidRect(&rect,m_clrBk); 	
	dc.SetBkMode(TRANSPARENT ); //set transparent bkmode, so the backcolor will not appear in drawtext

	//2, ��������
	CPen penGrid(PS_SOLID,1,m_clrGrid);
	CPen* oldpen = dc.SelectObject(&penGrid);
	//���������
	dc.MoveTo(2,2);
	dc.LineTo(2,rect.bottom-3); //��Ϊmrect.bottom���������ڣ������Ѿ��õ��� 2 ��.
	dc.LineTo(rect.right-3,rect.bottom -3);
	dc.LineTo(rect.right-3,2);
	dc.LineTo(2,2);

	int i;
	double dSize = (double) (rect.Width()-4)/ixGridNum;

	if(bxGridDisp)
	{
		for( i=1; i<ixGridNum; i++ )
		{//�������ߣ����������ָ��
			dc.MoveTo(2+dSize*i, 2);
			dc.LineTo(2+dSize*i, rect.bottom-3);
		}
	}

	dSize = (double) (rect.Height()-4)/iyGridNum;
	for(i=1; i<iyGridNum; i++)
	//for(i=iyGridNum-1;i>=1;i--)
	{//�������ߣ����������ָ��
		dc.MoveTo(2, 2+dSize*i);
		dc.LineTo(rect.right -3, 2+dSize*i);
	}


	//дY�������ַ�
	dc.SetTextColor(m_clrYScale);
	CString str;
unsigned int old_exponent_format;
// Enable two-digit exponent format.2008.04.21
old_exponent_format = _set_output_format(_TWO_DIGIT_EXPONENT);
	str.Format(_T("(%.0f,%.1E)"),xRight,yTop);
	dc.TextOut(rect.Width()-2-dc.GetTextExtent(str).cx,4,str);
	str.Format(_T("(%.0f,%.1E)"),xLeft,yBot);
_set_output_format( old_exponent_format );
	//str.Format(_T("%.1E"),yBot);
	//dc.TextOut(rect.Width()-2-dc.GetTextExtent(str).cx,rect.Height()-dc.GetTextExtent(str).cy-2,str);
	dc.TextOut(4,rect.Height()-dc.GetTextExtent(str).cy-2,str);

	dc.SelectObject(oldpen);	
}

/******************************************************************************
// �ڸ���������pSrc�����в���ĳ����ֵ Ӧ�ö�Ӧ��λ��
���������double dData �����ҵ�ֵ
          double* pSrc ��������
		  int iSrcNum  �������е���������
		  bool bPrePos ���ҵ���Ӧλ�ã���ǰһ���Ǻ�һ. ��Ϊ��������̫������ȫ��ȡ�λ����ǰ1�����ƺ�1��
���������int& iPos    ���ҵ���λ��
����ֵ��  �Ƿ���ҵ���
******************************************************************************/
bool CZYGraphPlot::fn_findPos(double dData, int& iPos, double* pSrc, int iSrcNum, bool bPrePos)
{
	ASSERT(pSrc);
	if(iSrcNum<=1)
		return false;

	ASSERT(iSrcNum>1);


	iPos = -1;
	double dEpsilon = 1.0E-9;

	if( dData < pSrc[0] || dData > pSrc[iSrcNum-1] )
	{
		return false;
	}

	int iBegin = 0;
	int iEnd   = iSrcNum-1;
	int iMid;
	
	while( (iEnd-iBegin) > 1 )
	{
		iMid   = (iBegin+iEnd)/2;
		if( dData > pSrc[iMid] )
		{
			if( dData - pSrc[iMid] > dEpsilon )
			{
				iBegin = iMid;
			}
			else
			{
				//iPos = iMid;
				iBegin = iMid;
				iEnd = __min(iMid+1,iSrcNum-1);
				break;
			}
		}
		else
		{
			iEnd = iMid;
		}
	}

	if(bPrePos)
	{
		iPos = iBegin;
	}
	else
	{
		iPos = iEnd;
	}

	return true;
}

// �����
void CZYGraphPlot::fn_PlotFrame(CDC& dc , CRect& rect)
{
	COLORREF     gclr_PicLTOutside = RGB(172,168,153); //Left Top
	COLORREF     gclr_PicLTInside  = RGB(113,111,100);
	COLORREF     gclr_PicRBOutside = RGB(255,255,255); //Right bottom
	COLORREF     gclr_PicRBInside  = RGB(241,239,226);
	//LT Inside
	CPen penLTInside(PS_SOLID,1,gclr_PicLTInside);
	CPen* oldpen = dc.SelectObject(&penLTInside);
	dc.MoveTo(0,1);
	dc.LineTo(rect.right-1,1);
	dc.MoveTo(1,0);
	dc.LineTo(1,rect.bottom-1);
	//LT Outside;
	CPen penLTOutside(PS_SOLID,1,gclr_PicLTOutside);
	dc.SelectObject(&penLTOutside);

	bool b = penLTInside.DeleteObject();

	dc.MoveTo(0,0);
	dc.LineTo(rect.right,0);
	dc.MoveTo(0,0);
	dc.LineTo(0,rect.bottom);
	//RB Inside
	CPen penRBInside(PS_SOLID,1,gclr_PicRBInside);
	dc.SelectObject(&penRBInside);

	penLTOutside.DeleteObject();

	dc.MoveTo(1,rect.bottom-2);
	dc.LineTo(rect.right,rect.bottom-2);
	dc.MoveTo(rect.right-2,1);
	dc.LineTo(rect.right-2,rect.bottom);
	//RB Outside
	CPen penRBOutside(PS_SOLID,1,gclr_PicRBOutside);
	dc.SelectObject(&penRBOutside);

	penRBInside.DeleteObject();

	dc.MoveTo(0,rect.bottom-1);
	dc.LineTo(rect.right-1,rect.bottom-1);
	dc.MoveTo(rect.right-1,0);
	dc.LineTo(rect.right-1,rect.bottom);
	dc.SelectObject(oldpen);

	penRBOutside.DeleteObject();
}
/****************************************************************************
�������ƣ�SaveDCBitmap
����˵������DC��ָ������(rect)����bmp��ʽд���ļ�(filename)�У�
���������CDC* pDC, ԴDC
CRect rect, ָ������
CString filename, bmp�ļ�����
���������������ã��������ڡ�bmp�ļ�
2008.04.03���
****************************************************************************/
bool SaveDCBitmap(CDC* pDC,CRect rect,LPCTSTR lpszFileName)
{ 
	CDC* memDC=new CDC;
	memDC->CreateCompatibleDC(pDC);
	CBitmap* bmp=new CBitmap;
	bmp->CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());
	CBitmap* oldbitmap=memDC->SelectObject(bmp);
	//��ʱ��bmp���൱��һ����������memDC�л���etc���ǻ�������������
	if(!memDC->BitBlt(0,0,rect.Width(),rect.Height(),pDC,0,0,SRCCOPY))
	{
		AfxMessageBox(TEXT("BitBlt Error!"));
		return false;
	}
	//memDC->Ellipse(0,0,100,100);
	memDC->SelectObject(oldbitmap);

	BITMAPINFO bi;
	bi.bmiHeader.biSize=sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth=rect.Width();
	bi.bmiHeader.biHeight=rect.Height();
	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biBitCount=16;
	bi.bmiHeader.biCompression=BI_RGB;
	bi.bmiHeader.biSizeImage=0;
	bi.bmiHeader.biXPelsPerMeter=0;
	bi.bmiHeader.biYPelsPerMeter=0;
	bi.bmiHeader.biClrUsed=0;
	bi.bmiHeader.biClrImportant=0;

	int bitsize=rect.Width()*rect.Height()*2;

	BYTE* bits=new BYTE[bitsize];
	::GetDIBits(memDC->m_hDC,*bmp,0,rect.Height(),bits,&bi,DIB_RGB_COLORS); 

	BITMAPFILEHEADER bf;
	bf.bfType=(int)'M'*256+'B';
	bf.bfSize=bitsize;//sizeof(bf);
	bf.bfOffBits=sizeof(bi.bmiHeader)+sizeof(bf);
	bf.bfReserved1=0;
	bf.bfReserved2=0;

	CFile f(lpszFileName,CFile::modeCreate|CFile::modeWrite);
	f.Write(&bf,sizeof(bf));//ע������дbf,��дbi
	f.Write(&bi,sizeof(bi));
	f.Write(bits,bitsize);
	f.Close();

	delete[] bits;
	delete bmp;
	delete memDC;
	return true;
} 
// �õ�ǰ������������ʾ��. 
void CZYGraphPlot::fn_PlotIllustration(CDC* pDestDC, CRect& rect, COLORREF crClr, int nGraphStyle, int nDotStyle,int nPenStyle=PS_SOLID)
{
	CDC memDC;
	CBitmap srcBitmap, *oldBitmap;
	CRect rtDest = rect;

	CRect mrect = rtDest; //draw rect in memdc
	mrect.OffsetRect(-rtDest.left,-rtDest.top); // zero based in memdc

	//create memDC and inside bitmap
	memDC.CreateCompatibleDC(pDestDC);
	srcBitmap.CreateCompatibleBitmap(pDestDC,mrect.Width(),mrect.Height()); 
	oldBitmap = memDC.SelectObject(&srcBitmap);
	memDC.SetBkColor(m_clrBk);
	memDC.FillSolidRect(mrect,m_clrBk);

	CPen penGrid(PS_SOLID,1,m_clrGrid);
	CPen* oldpen = memDC.SelectObject(&penGrid);
	//���������
	memDC.MoveTo(2,2);
	memDC.LineTo(2,mrect.bottom-3); //��Ϊmrect.bottom���������ڣ������Ѿ��õ��� 2 ��.
	memDC.LineTo(mrect.right-3,mrect.bottom -3);
	memDC.LineTo(mrect.right-3,2);
	memDC.LineTo(2,2);

	CPen pen(nPenStyle,1,crClr);
	memDC.SelectObject(&pen);
	penGrid.DeleteObject();

	//����
	if( _ZYGRAPH_STYLE_DISCRETE == nGraphStyle || _ZYGRAPH_STYLE_DOT_LINE == nGraphStyle )
	{
		//��ȷ����Ҫ�����ٵ�
		int x,y,n;
		int nCount = ( mrect.Width() - 6 ) / ( 8 + 15) ;//
		double dPerDot =(double)  mrect.Width()/( nCount+1 );
		x = 0;
		y = mrect.Height()/2;
		for( int i=0; i< nCount; i++)
		{
			x =(i+1)*dPerDot;			
			switch(nDotStyle)
			{
			case _ZYGRAPH_STYLE_DISCRETE_DOT:
				memDC.SetPixelV(x,y,crClr);
				break;
			case _ZYGRAPH_STYLE_DISCRETE_DIMOND:
				for(n = 0;n<sizeof(m_pDiamondOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pDiamondOffset[n].x,y+m_pDiamondOffset[n].y,crClr);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_RECT:
				for(n = 0;n<sizeof(m_pRectOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pRectOffset[n].x,y+m_pRectOffset[n].y,crClr);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_TRIANGLE:
				for(n = 0;n<sizeof(m_pTriangleOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pTriangleOffset[n].x,y+m_pTriangleOffset[n].y,crClr);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_X_SHAPE:
				for(n = 0;n<sizeof(m_pXShapeOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pXShapeOffset[n].x,y+m_pXShapeOffset[n].y,crClr);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_CROSS:
				for(n = 0;n<sizeof(m_pCrossOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pCrossOffset[n].x,y+m_pCrossOffset[n].y,crClr);
				}
				break;
			case _ZYGRAPH_STYLE_DISCRETE_X_1_SHAPE:
				for(n = 0;n<sizeof(m_pX1ShapeOffset)/sizeof(CPoint);n++)
				{
					memDC.SetPixelV(x+m_pX1ShapeOffset[n].x,y+m_pX1ShapeOffset[n].y,crClr);
				}
				break;
			}
		}
	}

	//���߶�
	if(_ZYGRAPH_STYLE_LINE == nGraphStyle || _ZYGRAPH_STYLE_DOT_LINE == nGraphStyle)
	{
		memDC.MoveTo(6,mrect.Height()/2);
		memDC.LineTo(mrect.Width()-6,mrect.Height()/2);
	}

	pDestDC->BitBlt(rtDest.left,rtDest.top,mrect.Width(),mrect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(oldBitmap);
	srcBitmap.DeleteObject();
	memDC.SelectObject(oldpen);
	pen.DeleteObject();
}

// ���ڲ���1�����ݽ�����ʾ��ͼ
int CZYGraphPlot::fn_PlotIllustrUse1st(CDC* pDestDC, CRect& rect,COLORREF crBk, COLORREF crGrid)
{
	CZYGraphData* p;
	POSITION pos = listData.GetHeadPosition();
	if(pos)
	{
		p = listData.GetNext(pos);
		COLORREF crBk_b = m_clrBk;
		COLORREF crGrid_b = m_clrGrid;
		m_clrBk = crBk;
		m_clrGrid = crGrid;

		fn_PlotIllustration(pDestDC,rect,p->crColor,p->iGraphStyle,p->nDotStyle,p->nPenStyle);

		m_clrBk = crBk_b;
		m_clrGrid = crGrid_b;
	}
	return 0;
}
