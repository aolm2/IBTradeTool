// OrderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IBTradeTool.h"
#include "IBTradeToolDlg.h"

#include "OrderDlg.h"
#include "afxdialogex.h"


// COrderDlg dialog

IMPLEMENT_DYNAMIC(COrderDlg, CDialog)

COrderDlg::COrderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COrderDlg::IDD, pParent)
{
	m_forextpair = NULL;

	m_forexindex = 0;
	m_orderaction = 1;
	m_ordertype = 1;
	m_conditionprice = 0;
	m_targetprice = 0;
	m_stopprice = 0;
	m_balert = true;
	m_quantity = 100000;


}

COrderDlg::~COrderDlg()
{
}

void COrderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOCURRENCY, m_combocurrency);
	DDX_Control(pDX, IDC_COMBOQUANTITY, m_comboquantity);
	DDX_Control(pDX, IDC_CHECKALERT, m_checkAlert);
	DDX_Control(pDX, IDC_EDITQUANTITY, m_editquantity);
	//  DDX_Control(pDX, IDC_RADIOBUYLMT, m_raidobuylmt);
	DDX_Control(pDX, IDC_RADIOBUYSTP, m_radiobuystp);
	DDX_Control(pDX, IDC_RADIOSELLLMT, m_radioselllmt);
	DDX_Control(pDX, IDC_RADIOSELLSTP, m_radiosellstp);
	DDX_Control(pDX, IDC_RADIOBUYLMT, m_radiobuylmt);
	DDX_Control(pDX, IDC_RADIOBUYTOCOVER, m_radiobuytocover);
	DDX_Control(pDX, IDC_RADIOSELLTOCOVER, m_radioselltocover);
}


BEGIN_MESSAGE_MAP(COrderDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIOBUYLMT, &COrderDlg::OnBnClickedRadiobuylmt)
	ON_BN_CLICKED(IDC_RADIOBUYSTP, &COrderDlg::OnBnClickedRadiobuystp)
	ON_BN_CLICKED(IDC_RADIOSELLLMT, &COrderDlg::OnBnClickedRadioselllmt)
	ON_BN_CLICKED(IDC_RADIOSELLSTP, &COrderDlg::OnBnClickedRadiosellstp)
	ON_CBN_SELCHANGE(IDC_COMBOQUANTITY, &COrderDlg::OnCbnSelchangeComboquantity)
	ON_CBN_SELCHANGE(IDC_COMBOCURRENCY, &COrderDlg::OnCbnSelchangeCombocurrency)
	ON_BN_CLICKED(IDOK, &COrderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECKALERT, &COrderDlg::OnBnClickedCheckalert)
	ON_BN_CLICKED(IDC_RADIOBUYTOCOVER, &COrderDlg::OnBnClickedRadiobuytocover)
	ON_BN_CLICKED(IDC_RADIOSELLTOCOVER, &COrderDlg::OnBnClickedRadioselltocover)
END_MESSAGE_MAP()


// COrderDlg message handlers


BOOL COrderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editconditionprice.SubclassDlgItem(IDC_EDITCONDITIONPRICE,this);
	m_edittargetprice.SubclassDlgItem(IDC_EDITTARGETPRICE,this);
	m_editstopprice.SubclassDlgItem(IDC_EDITSTOPPRICE,this);

	CString strformat;
	
	int i;
	for (i=0;i<5;i++)
	{
		m_combocurrency.AddString(m_forextpair[i].strDisplay);
	}
	m_combocurrency.SetCurSel(m_forexindex);

	CString str,formatstr;
	formatstr.Format("%%.%df", m_forextpair[m_forexindex].numberafterdot);
	str.Format(formatstr,m_conditionprice);
	m_editconditionprice.SetWindowTextA(str);

	str = "";
	if (m_targetprice > 0)
	{
		str.Format(formatstr,m_targetprice);
	}
	m_edittargetprice.SetWindowTextA(str);

	str = "";
	if (m_stopprice > 0)
	{
		str.Format(formatstr,m_stopprice);
	}
	m_editstopprice.SetWindowTextA(str);

	if ((m_orderaction == 1)&&(m_ordertype == 1))
	{
		m_radiobuylmt.SetCheck(1);
	}
	else if ((m_orderaction == 1)&&(m_ordertype == 2))
	{
		m_radiobuystp.SetCheck(1);
	}
	else if ((m_orderaction == 1)&&(m_ordertype == 3))
	{
		m_radiobuytocover.SetCheck(1);
		m_editconditionprice.EnableWindow(FALSE);
	}
	else if ((m_orderaction == 2)&&(m_ordertype == 1))
	{
		m_radioselllmt.SetCheck(1);
	}
	else if ((m_orderaction == 2)&&(m_ordertype == 2))
	{
		m_radiosellstp.SetCheck(1);
	}
	else if ((m_orderaction == 2)&&(m_ordertype == 3))
	{
		m_radioselltocover.SetCheck(1);
		m_editconditionprice.EnableWindow(FALSE);
	}
	
	m_checkAlert.SetCheck(m_balert);


	if (m_quantity == 100000)
	{
		m_comboquantity.SetCurSel(0);
		m_editquantity.SetReadOnly(TRUE);
	}
	else if (m_quantity == 200000)
	{
		m_comboquantity.SetCurSel(1);
		m_editquantity.SetReadOnly(TRUE);
	}
	else if (m_quantity == 300000)
	{
		m_comboquantity.SetCurSel(2);
		m_editquantity.SetReadOnly(TRUE);
	}	
	else if (m_quantity == 400000)
	{
		m_comboquantity.SetCurSel(3);
		m_editquantity.SetReadOnly(TRUE);
	}
	else if (m_quantity == 500000)
	{
		m_comboquantity.SetCurSel(4);
		m_editquantity.SetReadOnly(TRUE);
	}
	else
	{
		m_comboquantity.SetCurSel(5);
		m_editquantity.SetReadOnly(FALSE);
	}
	str.Format("%d",m_quantity);
	m_editquantity.SetWindowText(str);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COrderDlg::OnBnClickedRadiobuylmt()
{
	m_orderaction = 1;
	m_ordertype = 1;

	m_editconditionprice.EnableWindow(TRUE);
}


void COrderDlg::OnBnClickedRadiobuystp()
{
	m_orderaction = 1;
	m_ordertype = 2;

	m_editconditionprice.EnableWindow(TRUE);
}

void COrderDlg::OnBnClickedRadiobuytocover()
{
	m_orderaction = 1;
	m_ordertype = 3;

	m_editconditionprice.EnableWindow(FALSE);
	m_editconditionprice.SetWindowTextA("");
}


void COrderDlg::OnBnClickedRadioselllmt()
{
	m_orderaction = 2;
	m_ordertype = 1;

	m_editconditionprice.EnableWindow(TRUE);
}


void COrderDlg::OnBnClickedRadiosellstp()
{
	m_orderaction = 2;
	m_ordertype = 2;

	m_editconditionprice.EnableWindow(TRUE);
}

void COrderDlg::OnBnClickedRadioselltocover()
{
	m_orderaction = 2;
	m_ordertype = 3;

	m_editconditionprice.EnableWindow(FALSE);
	m_editconditionprice.SetWindowTextA("");
}





void COrderDlg::OnCbnSelchangeComboquantity()
{
	int sel = m_comboquantity.GetCurSel();
	CString str;
	if (sel < 5)
	{
		m_quantity = (sel + 1) * 100000;
		str.Format("%d",m_quantity);
		m_editquantity.SetReadOnly(TRUE);
		m_editquantity.SetWindowTextA(str);
	}
	else
	{
		m_editquantity.SetReadOnly(FALSE);
		m_editquantity.SetFocus();
	}
}


void COrderDlg::OnCbnSelchangeCombocurrency()
{
	int sel = m_combocurrency.GetCurSel();
	m_edittargetprice.SetWindowTextA("");
	m_editstopprice.SetWindowTextA("");

	double price;
	if (m_orderaction == 1)
	{
		price = m_forextpair[sel].lastask;
	}
	else if (m_orderaction == 2)
	{
		price = m_forextpair[sel].lastbid;
	}

	m_forexindex = sel;

	CString str,formatstr;
	formatstr.Format("%%.%df", m_forextpair[m_forexindex].numberafterdot);
	str.Format(formatstr,price);
	m_editconditionprice.SetWindowTextA(str);
}


void COrderDlg::OnBnClickedOk()
{
	CString strprice,strtargetprice,strstopprice;
	m_editconditionprice.GetWindowTextA(strprice);
	m_edittargetprice.GetWindowTextA(strtargetprice);
	m_editstopprice.GetWindowTextA(strstopprice);

	int numberofdot = m_forextpair[m_forexindex].numberafterdot;
	double lastbid = m_forextpair[m_forexindex].lastbid;
	double lastask = m_forextpair[m_forexindex].lastask;
	double dotprice = m_forextpair[m_forexindex].dotprice;

	if (!(IsPriceValid(strprice,numberofdot)&&IsPriceValid(strtargetprice,numberofdot)
		&&IsPriceValid(strstopprice,numberofdot)))
	{
		MessageBox("Price format not correct!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return ;
	}

	m_conditionprice = atof(strprice);
	m_targetprice = atof(strtargetprice);
	m_stopprice = atof(strstopprice);

	if ((m_orderaction == 1)&&(m_ordertype == 1))//buy lmt
	{
		if (m_conditionprice < 0.0001)
		{
			MessageBox("请输入触发价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_conditionprice >= lastask)
		{
			MessageBox("价格将立刻执行,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_targetprice < m_conditionprice + MINTARGETDOT * dotprice)&&(m_targetprice>0))
		{
			MessageBox("目标价小于最小目标范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_stopprice > m_conditionprice - MINSTOPDOT * dotprice)&&(m_stopprice>0))
		{
			MessageBox("止损价最小目标范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}
	}
	else if ((m_orderaction == 1)&&(m_ordertype == 2))//buy stp
	{
		if (m_conditionprice < 0.0001)
		{
			MessageBox("请输入触发价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_conditionprice <= lastask)
		{
			MessageBox("价格将立刻执行,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_targetprice < m_conditionprice + MINTARGETDOT * dotprice)&&(m_targetprice>0))
		{
			MessageBox("目标价小于最小目标范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_stopprice > m_conditionprice - MINSTOPDOT * dotprice)&&(m_stopprice>0))
		{
			MessageBox("止损价小于最小目标范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}
	}
	else if ((m_orderaction == 1)&&(m_ordertype == 3))//buy to cover
	{

		if (m_targetprice < 0.0001)
		{
			MessageBox("请输入目标价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_stopprice < 0.0001)
		{
			MessageBox("请输入止损价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_targetprice >= lastask)
		{
			MessageBox("目标价将立刻执行,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_stopprice <= lastask)
		{
			MessageBox("止损价将立刻执行,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}


	}
	else if ((m_orderaction == 2)&&(m_ordertype == 1))//sell lmt
	{
		if (m_conditionprice < 0.0001)
		{
			MessageBox("请输入触发价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_conditionprice <= lastbid)
		{
			MessageBox("价格将立刻执行,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_targetprice > m_conditionprice - MINTARGETDOT * dotprice)&&(m_targetprice>0))
		{
			MessageBox("目标价小于最小目标范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_stopprice < m_conditionprice + MINSTOPDOT * dotprice)&&(m_stopprice>0))
		{
			MessageBox("止损价小于最小止损范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}
	}
	else if ((m_orderaction == 2)&&(m_ordertype == 2))//sell stp
	{
		if (m_conditionprice < 0.0001)
		{
			MessageBox("请输入触发价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_conditionprice >= lastbid)
		{
			MessageBox("价格将立刻执行,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_targetprice > m_conditionprice - MINTARGETDOT * dotprice)&&(m_targetprice>0))
		{
			MessageBox("目标价小于最小目标范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if ((m_stopprice < m_conditionprice + MINSTOPDOT * dotprice)&&(m_stopprice>0))
		{
			MessageBox("止损价小于最小止损范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}
	}
	else if ((m_orderaction == 2)&&(m_ordertype == 3))//sell to cover
	{
		if (m_targetprice < 0.0001)
		{
			MessageBox("请输入目标价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_stopprice < 0.0001)
		{
			MessageBox("请输入止损价格!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_targetprice <= lastbid)
		{
			MessageBox("目标价将立刻触发,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}

		if (m_stopprice >= lastbid)
		{
			MessageBox("止损价将立刻触发,请修改!","Warning",MB_OK|MB_ICONEXCLAMATION);
			return ;
		}
	}

	CString strquantity;
	m_editquantity.GetWindowTextA(strquantity);
	m_quantity = atoi(strquantity);
	if ((m_quantity <=0)||(m_quantity > 2000000))
	{
		MessageBox("定单大小超出范围!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return ;
	}

	CDialog::OnOK();
}


void COrderDlg::OnBnClickedCheckalert()
{
	if (m_checkAlert.GetCheck())
		m_balert = true;
	else
		m_balert = false;
}

bool COrderDlg::IsPriceValid( CString strprice,int numberofdot )
{
	strprice = strprice.Trim();
	if (strprice == "")
		return true;

	if (strprice.GetLength() < numberofdot + 2)
		return false;

	CString str = strprice.Right(numberofdot+1);
	str = str.Left(1);
	if (str != ".")
		return false;

	str = strprice.Right(1);
	if ((str != "5")&&(str != "0"))
		return false;

	return true;

}




