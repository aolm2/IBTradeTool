#pragma once


// CEditEx

class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)

public:
	CEditEx();
	virtual ~CEditEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


