#pragma once


// COrderDlg dialog
#include "EditEx.h"
struct ForexPair;

#define MINTARGETDOT 8
#define MINSTOPDOT 8

class COrderDlg : public CDialog
{
	DECLARE_DYNAMIC(COrderDlg)

public:
	CEditEx m_editconditionprice,m_edittargetprice,m_editstopprice;

	int m_forexindex;
	int m_orderaction;
	int m_ordertype;
	double m_conditionprice,m_targetprice,m_stopprice;
	bool m_balert;
	int m_quantity;
	ForexPair *m_forextpair;

	bool IsPriceValid(CString strprice,int numberofdot);
	


public:
	COrderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COrderDlg();

// Dialog Data
	enum { IDD = IDD_DIALOGCREATEORDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_combocurrency;
	CComboBox m_comboquantity;
	CButton m_checkAlert;
	CEdit m_editquantity;
//	CButton m_raidobuylmt;
	CButton m_radiobuystp;
	CButton m_radioselllmt;
	CButton m_radiosellstp;
	afx_msg void OnBnClickedRadiobuylmt();
	afx_msg void OnBnClickedRadiobuystp();
	afx_msg void OnBnClickedRadioselllmt();
	afx_msg void OnBnClickedRadiosellstp();
	afx_msg void OnCbnSelchangeComboquantity();
	CButton m_radiobuylmt;
	afx_msg void OnCbnSelchangeCombocurrency();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckalert();
	CButton m_radiobuytocover;
	CButton m_radioselltocover;
	afx_msg void OnBnClickedRadiobuytocover();
	afx_msg void OnBnClickedRadioselltocover();

};
