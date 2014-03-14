// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "EditEx.h"


// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{

}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CEditEx message handlers



void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if   ((nChar >= '0' && nChar <= '9')||	nChar == '.'||
		nChar == VK_BACK ||	nChar == VK_DELETE)         //msdnµÄvirtual   key 
	{ 
		CString   str; 
		GetWindowText(str);
		if (str.GetLength() == 0)
		{
			if(nChar == '.')
				return;
		}
		else if (str.Find(_T("."),0) > -1)
		{
			if (nChar == '.')
				return;
		}
		else if (str.GetAt(0) == '0')
		{
			if (nChar == '.')
			{
				CEdit::OnChar(nChar,nRepCnt,nFlags);
				return;
			}
			else if (nChar == '0')
			{
				return;
			}
		}				
		CEdit::OnChar(nChar,nRepCnt,nFlags); 
	} 
}
