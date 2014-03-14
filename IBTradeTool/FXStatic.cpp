// FXStatic.cpp : implementation file
//

#include "stdafx.h"
#include "FXStatic.h"


// CFXStatic

IMPLEMENT_DYNAMIC(CFXStatic, CStatic)

CFXStatic::CFXStatic()
{
	m_font.CreateFontA(18,0,0,0,400,FALSE,FALSE,0,
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,"Tahoma");

	m_colorbk = RGB(0,0,0);
	m_colornow = RGB(255,255,255);
	m_colordown = RGB(255,130,80);
	m_colorup = RGB(0,250,0);
	m_colornormal = RGB(255,255,255);

	m_strcurrency = "???.???";
	m_strpriceformat = "%7.5f   %7.5f ";
	m_strprice = m_strdisplayprice = "no data";

	m_bselected = false;

}

CFXStatic::~CFXStatic()
{
}


BEGIN_MESSAGE_MAP(CFXStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CFXStatic message handlers




BOOL CFXStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
	//return CStatic::OnEraseBkgnd(pDC);
}


void CFXStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting


	CRect rect;
	GetClientRect(rect); 

	CDC dcMemory;
	dcMemory.CreateCompatibleDC(&dc);	// Select the bitmap into the in-memory DC
	dcMemory.SetBkMode(TRANSPARENT);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	CBitmap* pOldBitmap = dcMemory.SelectObject(CBitmap::FromHandle(bmp));

	if (m_bselected)
		dcMemory.FillSolidRect(rect,RGB(120,120,120));
	else
		dcMemory.FillSolidRect(rect,m_colorbk);

	dcMemory.SelectObject(&m_font);

	dcMemory.SetTextColor(RGB(255,255,255));
	dcMemory.TextOutA(5,0,m_strcurrency);

	dcMemory.SetTextColor(m_colornow);
	dcMemory.TextOutA(75,0,m_strdisplayprice);

	dc.BitBlt(0,0,rect.Width(),rect.Height(),&dcMemory,0,0,SRCCOPY);
	dcMemory.SelectObject(pOldBitmap);
	dcMemory.DeleteDC();
	DeleteObject(bmp);
}


void CFXStatic::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/*
	KillTimer(1);
	SetDirection(0);
	*/
	
	CStatic::OnTimer(nIDEvent);
}


void CFXStatic::SetDirection( int direction )
{
	if (direction == 0)
	{
		m_strdisplayprice = m_strprice;
		m_colornow = m_colornormal;
	}
	else if (direction == 1)
	{
		m_strdisplayprice = m_strprice + "¡ü";
		m_colornow = m_colorup;
	}
	else if (direction == 2)
	{
		m_strdisplayprice = m_strprice + "¡ý";
		m_colornow = m_colordown;
	}

	RedrawWindow(NULL, NULL,  RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);

}

void CFXStatic::SetPriceAndDirection( double bid, double ask,int direction )
{
	m_strprice.Format(m_strpriceformat,bid,ask);
	SetDirection(direction);
	//SetTimer(1,1000,NULL);

}


void CFXStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//m_bselected = true;
	//RedrawWindow(NULL, NULL,  RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
	CStatic::OnLButtonDblClk(nFlags, point);
}


void CFXStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int i;
	for (i=0;i<5;i++)
	{
		m_fxstatics[i].m_bselected = false;
		m_fxstatics[i].RedrawWindow(NULL, NULL,  RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
	}

	m_bselected = true;
	RedrawWindow(NULL, NULL,  RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
	CStatic::OnLButtonDown(nFlags, point);
}
