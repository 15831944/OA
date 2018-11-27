// DrawCrucibleThread.cpp : implementation file
//

#include "stdafx.h"
#include "Optorun Assistant.h"
#include "DrawCrucibleThread.h"
#include "OACnfgDoc.h"

#include <math.h>
#define pi 3.1415926535897932
// CDrawCrucibleThread

#define ZZY_LED_NUMBER_WIDTH  15  //��ֵӦ�����������ͼƬ�ߴ����.
#define ZZY_LED_NUMBER_HEIGHT 20


COLORREF g_clrHearthExteriorCircle = RGB(0,128,0); 
COLORREF g_clrHearthCrucibleCircle = RGB(0,128,0); //pen color
COLORREF g_clrHearthCrucibleInside = GetSysColor(COLOR_3DFACE); //brush color
COLORREF g_clrHearthCrucibleText   = RGB(0,0,128); //font
COLORREF g_clrHearthAngle		   = RGB(0,0,0);
COLORREF g_clrHearthOriginLine     = RGB(200,0,0); //ԭ����


IMPLEMENT_DYNCREATE(CDrawCrucibleThread, CWinThread)

CDrawCrucibleThread::CDrawCrucibleThread()
{
}

CDrawCrucibleThread::~CDrawCrucibleThread()
{
}

BOOL CDrawCrucibleThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	m_LEDNumber.LoadBitmap(IDB_LED_NUMBER);
	return TRUE;
}

int CDrawCrucibleThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDrawCrucibleThread, CWinThread)
	ON_THREAD_MESSAGE(WM_ITEM3_DRAW_CRUCIBLE,  OnDrawCrucible)
END_MESSAGE_MAP()

/*
���� lParam �����ݽ���Crucible��ͼ�������ٷ�����
*/
void CDrawCrucibleThread::OnDrawCrucible(WPARAM wParam, LPARAM lParam)
{
	struct _stDrawHearth* pstDrawData = (struct _stDrawHearth*)lParam;

	if( ! IsWindow(pstDrawData->hwnd))
	{
		return;
	}

//	if(pstDrawData->iCrucibleMaxNumber == 0)
//		return; //������������ʱ����.

	HDC hDC = GetDC(pstDrawData->hwnd);
	CDC* pDC = CDC::FromHandle(hDC);

	CDC memDC;
	CBitmap srcBitmap, *oldBitmap;

	//create memDC and inside bitmap
	memDC.CreateCompatibleDC(pDC);
	srcBitmap.CreateCompatibleBitmap(pDC,pstDrawData->rect.Width(),pstDrawData->rect.Height()); 
	oldBitmap = memDC.SelectObject(&srcBitmap);

	memDC.SetBkColor(GetSysColor(COLOR_3DFACE)); //TEST


	int iSquareWidth = __min(pstDrawData->rect.Width(),pstDrawData->rect.Height());
	/************************************************************************************
	�������α߳�Ϊ W, С����ֱ��Ϊ d ,������������Բֱ�� W-d.
	��ͼ��Ϊ�����ڡ���С��������һ�𣬡�ÿ������Ӧռ�ݡ�����Բ�߳���Լ��1.2*d.
	�ɵ÷��̡�1.2*d*n = pi * (W-d)�������С�n Ϊ���������� pi
	d = pi * W /(1.2*n + pi)
	*************************************************************************************/
	double d = pi * (double)iSquareWidth / (1.4 * pstDrawData->iCrucibleMaxNumber + pi);

	//֮���Խ�ֱ��ת��������������Ϊ��Ļ���ԡ��㡡�����㵥λ�ģ������� ��Բ����Ҫռ�� 1 ��.
	//�����ֱ���Ͱ뾶�����Ե�����λ�ģ����뾶 = (ֱ��-1)/2.
	//С����ֱ��
	int iCrucibleDiameter = (int) d;
	//ת�� ����
	if( iCrucibleDiameter % 2 == 0)
		iCrucibleDiameter++;

	//����Բֱ��,��֤���� .
	int iExteriorDiameter = iSquareWidth  -  iCrucibleDiameter;
	if( iExteriorDiameter % 2 == 0)
		iExteriorDiameter--;

	iSquareWidth = iCrucibleDiameter + iExteriorDiameter - 1; //�Ѱ������±�

	//�ܻ�ͼ�ߴ� (��������)
	CRect rect;
	rect.left = 0;
	rect.top  = 0;
	rect.right  = iSquareWidth;
	rect.bottom = iSquareWidth;

	//LineTo�������� LineTo(point)�е� point!!

	//1,����ɫ
	memDC.FillSolidRect(0,0,pstDrawData->rect.Width(),pstDrawData->rect.Height(),GetSysColor(COLOR_3DFACE)); 	
	//memDC.FillSolidRect(0,0,pstDrawData->rect.Width(),pstDrawData->rect.Height(),RGB(0,0,0)); 	

	memDC.SetBkMode(TRANSPARENT ); //set transparent bkmode, so the backcolor will not appear in drawtext

	CPen penExteriorCircle(PS_SOLID,1,g_clrHearthExteriorCircle);
	CPen* oldpen = memDC.SelectObject(&penExteriorCircle);

	//�����Ͻ�д��Hearth��
	memDC.TextOut(0,0,pstDrawData->iHearth==0?TEXT("1"):TEXT("2") ,1);

	CBrush* oldbrush = NULL;
	CFont*  oldfont = NULL;

	int cx,cy;
	if(pstDrawData->iCrucibleMaxNumber == 0)
	{//����������
		CRect rtOutside = rect;
		//��Բ
		rtOutside.DeflateRect(2,2);
		memDC.Arc(&rtOutside,CPoint(0,0),CPoint(0,0)); //����Բ
		//��Բ
		rtOutside.DeflateRect(rtOutside.Width()/8,rtOutside.Width()/8);
		memDC.Arc(&rtOutside,CPoint(0,0),CPoint(0,0)); //����Բ

		//������000λ��
		cx = rtOutside.CenterPoint().x;
		cy = rtOutside.CenterPoint().y;
		
		double dAngle = 0; //
		double dStartAngle = pstDrawData->iCoderVal * pi /180; 

		//����СԲ�����ڡ���ͼ�ϵ�����.  //��ͼ�����ꡡ����Ϊ��. ��СԲ������·�����
		int x = cx - (double)( rtOutside.Width()/2 * sin(dStartAngle-dAngle) - 0.5 ); //�Ƕ������ԭ�㡡��˵���Ǹ�ֵ��������Ҫ��ȥ
		int y = cy + (double)( rtOutside.Width()/2 * cos(dStartAngle-dAngle) + 0.5 );
		memDC.MoveTo(cx,cy);
		memDC.LineTo(x,y);

		//��ǰλ����
		CPen penHearthOrigin(PS_SOLID,1,g_clrHearthOriginLine);
		memDC.SelectObject(&penHearthOrigin);
		memDC.MoveTo(cx,cy);
		int iHalfWidth = rtOutside.Width() /2;
		memDC.LineTo(cx,cy + iHalfWidth );
		//С��ͷ
		memDC.MoveTo(cx,cy + iHalfWidth );
		memDC.LineTo(cx-4,cy + iHalfWidth-8 );
		memDC.MoveTo(cx,cy + iHalfWidth  );
		memDC.LineTo(cx+4,cy + iHalfWidth -8 );

		//������λ�������������ֵ �Լ���ǰ�ٶ�
		int iNumberX = cx - ZZY_LED_NUMBER_WIDTH * 1.5;
		int iNumberY = cy;//- ZZY_LED_NUMBER_HEIGHT/2;
		if( pstDrawData->iCoderVal>1000 ||  pstDrawData->iCoderVal< 0 )
		{
			pstDrawData->iCoderVal = abs(pstDrawData->iCoderVal)%1000;
		}
		CDC   csDC;
		csDC.CreateCompatibleDC(&memDC);
		CBitmap* oldNumberBitmap = csDC.SelectObject(&m_LEDNumber);
		
		int iNumber;
		for( int i=0;i<3;i++)
		{
			iNumber = 0==i ? pstDrawData->iCoderVal/100:(1==i?(pstDrawData->iCoderVal/10)%10:pstDrawData->iCoderVal%10) ;
			memDC.BitBlt(iNumberX+i*ZZY_LED_NUMBER_WIDTH,iNumberY, ZZY_LED_NUMBER_WIDTH,ZZY_LED_NUMBER_HEIGHT,&csDC, ZZY_LED_NUMBER_WIDTH*iNumber,0,SRCCOPY);

			//
			iNumber = 0==i ? pstDrawData->iSpeed/100:(1==i?(pstDrawData->iSpeed/10)%10:pstDrawData->iSpeed%10);
			memDC.BitBlt(iNumberX+i*ZZY_LED_NUMBER_WIDTH,iNumberY-ZZY_LED_NUMBER_HEIGHT-1, ZZY_LED_NUMBER_WIDTH,ZZY_LED_NUMBER_HEIGHT,&csDC, ZZY_LED_NUMBER_WIDTH*iNumber,0,SRCCOPY);
		}

		csDC.SelectObject(oldNumberBitmap);
	}
	else
	{//��������

		//2, ������Բ
		//��ȡ����Բ�����. Arc������msdn��û�п��������������±ߵ�˵������ Ellipse����������
		CRect rtExteriorRect;
		rtExteriorRect.left   = iCrucibleDiameter/2 ; 
		rtExteriorRect.top    = iCrucibleDiameter/2 ;
		rtExteriorRect.right  = rtExteriorRect.left + iExteriorDiameter;
		rtExteriorRect.bottom = rtExteriorRect.top  + iExteriorDiameter;//�۵�����
		//	memDC.Arc(&rtExteriorRect,CPoint(0,0),CPoint(0,0)); //����Բ

		//3,����������СԲ��
		CPen penCrucibleCircle(PS_SOLID,1,g_clrHearthCrucibleCircle);
		memDC.SelectObject(&penCrucibleCircle);

		CBrush brCrucibleInside(g_clrHearthCrucibleInside);
		oldbrush = memDC.SelectObject(&brCrucibleInside);

		CFont ftCrucibleTextFont;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
		lf.lfHeight = iCrucibleDiameter - 4 ;                      // request a 12-pixel-height font
		_tcscpy (lf.lfFaceName, TEXT("Arial"));        // request a face name "Arial"
		VERIFY(ftCrucibleTextFont.CreateFontIndirect(&lf));  // create the font
		oldfont = memDC.SelectObject(&ftCrucibleTextFont);

		memDC.SetTextColor(g_clrHearthCrucibleText);
		//�������ľ��ȷֲ��ڡ�����Բ�ϣ�����ʼ�Ƕ��ڡ��·�����.
		//������ʱ��ֲ�
		//��ʼ�Ƕ� ���� ����
		int x,y; //СԲ����
		int r = iCrucibleDiameter / 2 ;//СԲ�뾶

		//����Բ��Բ������
		cx = rtExteriorRect.CenterPoint().x ;
		cy = rtExteriorRect.CenterPoint().y ;
		//����Բ���뾶
		int cr = rtExteriorRect.Width() / 2;

		CString str;
		CRect rtCrucible;
		double dIncAngle = 2 * pi / pstDrawData->iCrucibleMaxNumber; //�Ƕȵ���
		double dStartAngle = pstDrawData->iCoderVal * pi /180;
		//double dStartAngle = 20 * pi /180;
		for(int i=0;i<pstDrawData->iCrucibleMaxNumber;i++)
		{
			//����СԲ�����ڡ���ͼ�ϵ�����.  //��ͼ�����ꡡ����Ϊ��. ��СԲ������·�����
			x = cx - (double)( cr*sin(dStartAngle-i*dIncAngle) - 0.5 ); //�Ƕ������ԭ�㡡��˵���Ǹ�ֵ��������Ҫ��ȥ
			y = cy + (double)( cr*cos(dStartAngle-i*dIncAngle) + 0.5 );

			if(i==0)
			{
				memDC.MoveTo(cx,cy);
				memDC.LineTo(x,y);
			}

			//�ó�СԲ����Ӿ���
			rtCrucible.left = x - r;
			rtCrucible.top  = y - r;
			rtCrucible.right = x + r + 1; //Ellipse���������������±��أ��ʡ�+1
			rtCrucible.bottom = y + r + 1;

			memDC.Ellipse(&rtCrucible);
			str.Format( TEXT("%d"), i+1 );
			memDC.DrawText(str,&rtCrucible,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		//��ǰλ����
		CPen penHearthOrigin(PS_SOLID,1,g_clrHearthOriginLine);
		memDC.SelectObject(&penHearthOrigin);
		memDC.MoveTo(cx,cy);
		memDC.LineTo(cx,cy + iSquareWidth/2 - iCrucibleDiameter );
		//С��ͷ
		memDC.MoveTo(cx,cy + iSquareWidth/2 - iCrucibleDiameter );
		memDC.LineTo(cx-4,cy + iSquareWidth/2 - iCrucibleDiameter-8 );
		memDC.MoveTo(cx,cy + iSquareWidth/2 - iCrucibleDiameter );
		memDC.LineTo(cx+4,cy + iSquareWidth/2 - iCrucibleDiameter-8 );


		//������λ�������������ֵ �Լ�������
		int iNumberX = cx - ZZY_LED_NUMBER_WIDTH * 1.5;
		int iNumberY = cy;//- ZZY_LED_NUMBER_HEIGHT/2;
		if( pstDrawData->iCoderVal>1000 ||  pstDrawData->iCoderVal< 0 )
		{
			pstDrawData->iCoderVal = abs(pstDrawData->iCoderVal)%1000;
		}
		CDC   csDC;
		csDC.CreateCompatibleDC(&memDC);
		CBitmap* oldNumberBitmap = csDC.SelectObject(&m_LEDNumber);
		
		int iNumber;
		for( int i=0;i<3;i++)
		{
			iNumber = 0==i ? pstDrawData->iCoderVal/100:(1==i?(pstDrawData->iCoderVal/10)%10:pstDrawData->iCoderVal%10) ;
			memDC.BitBlt(iNumberX+i*ZZY_LED_NUMBER_WIDTH,iNumberY, ZZY_LED_NUMBER_WIDTH,ZZY_LED_NUMBER_HEIGHT,&csDC, ZZY_LED_NUMBER_WIDTH*iNumber,0,SRCCOPY);
		}

		iNumber = pstDrawData->iCurCurcible/10;
		memDC.BitBlt(iNumberX+ZZY_LED_NUMBER_WIDTH,iNumberY-ZZY_LED_NUMBER_HEIGHT-1,ZZY_LED_NUMBER_WIDTH,ZZY_LED_NUMBER_HEIGHT,&csDC, ZZY_LED_NUMBER_WIDTH*iNumber,0,SRCCOPY);
		iNumber = pstDrawData->iCurCurcible%10;
		memDC.BitBlt(iNumberX+2*ZZY_LED_NUMBER_WIDTH,iNumberY-ZZY_LED_NUMBER_HEIGHT-1,ZZY_LED_NUMBER_WIDTH,ZZY_LED_NUMBER_HEIGHT,&csDC, ZZY_LED_NUMBER_WIDTH*iNumber,0,SRCCOPY);

		csDC.SelectObject(oldNumberBitmap);


	} //������������


	//Bitblt memory bitmap to view 
	pDC->BitBlt(pstDrawData->rect.left,pstDrawData->rect.top,pstDrawData->rect.Width(),pstDrawData->rect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(oldBitmap);
	srcBitmap.DeleteObject();
	memDC.SelectObject(oldpen);
	if(oldbrush)
		memDC.SelectObject(oldbrush);
	if(oldfont)
		memDC.SelectObject(oldfont);

	ReleaseDC(pstDrawData->hwnd,hDC);
}

// CDrawCrucibleThread message handlers
