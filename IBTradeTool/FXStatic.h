#pragma once


// CFXStatic
#include "EWrapper.h"

class CFXStatic : public CStatic
{
	DECLARE_DYNAMIC(CFXStatic)
public:
	CFont m_font;
	COLORREF m_colorbk,m_colornow,m_colorup,m_colordown,m_colornormal;
	CString m_strcurrency;
	CString m_strpriceformat;
	CString m_strprice,m_strdisplayprice;

	void SetDirection(int direction);//direction =1 up, direction =2 down, direction=0 normal
	void SetPriceAndDirection(double bid, double ask,int direction);


	int m_index;
	bool m_bselected;
	CFXStatic *m_fxstatics;
	

public:
	CFXStatic();
	virtual ~CFXStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


