
// IBTradeToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IBTradeTool.h"
#include "IBTradeToolDlg.h"
#include "afxdialogex.h"

#include "OrderDlg.h"

#include "EClientSocket.h"   // C:\JTS\SocketClient\include must be added to include path
#include "TwsSocketClientErrors.h"   // C:\JTS\SocketClient\include must be added to include path

#include "Contract.h"
#include "Execution.h"
#include "OrderState.h"

#include "mmsystem.h"
#pragma  comment(lib,"winmm.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIBTradeToolDlg dialog




CIBTradeToolDlg::CIBTradeToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIBTradeToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	CString strAppPath = exeFullPath;
	int nPos = strAppPath.ReverseFind('\\');
	m_strinifilepath = strAppPath.Left(nPos + 1) + "iboption.ini";
	
	m_pClient = new EClientSocket( this);

	m_hwndTws = NULL;
	m_strTwsTitle = "";

	m_bhidelist = false;

	m_trendpoints = 30;
	m_trendquantity = 100000;
	m_bhideflashorderbutton = true;

}

void CIBTradeToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIBTradeToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CIBTradeToolDlg::OnBnClickedButton1)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON3, &CIBTradeToolDlg::OnBnClickedButton3)
	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_BUTTONCANCELORDER, &CIBTradeToolDlg::OnBnClickedButtoncancelorder)
	ON_BN_CLICKED(IDC_BUTTONBUYTOCOVER, &CIBTradeToolDlg::OnBnClickedButtonbuytocover)
	ON_BN_CLICKED(IDC_BUTTONSELLTOCOVER, &CIBTradeToolDlg::OnBnClickedButtonselltocover)
	ON_BN_CLICKED(IDC_BUTTONBUYONTRENDS, &CIBTradeToolDlg::OnBnClickedButtonbuyontrends)
	ON_BN_CLICKED(IDC_BUTTONSELLONTRENDS, &CIBTradeToolDlg::OnBnClickedButtonsellontrends)
END_MESSAGE_MAP()


// CIBTradeToolDlg message handlers

BOOL CIBTradeToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon




	// TODO: Add extra initialization here
	m_forexpair[0].strDisplay = "EUR/USD";
	m_forexpair[1].strDisplay = "USD/JPY";
	m_forexpair[2].strDisplay = "GBP/USD";
	m_forexpair[3].strDisplay = "AUD/USD";
	m_forexpair[4].strDisplay = "EUR/CHF";
	m_forexpair[1].numberafterdot = 3;
	m_forexpair[1].lmtmoveprice = 0.02;//USDJPY的2个点就是0.02  = 2 * (0.1^(numberafterdot-1))
	m_forexpair[1].halfstpmoveprice = 0.05;
	m_forexpair[1].profittake = 0.50;
	m_forexpair[1].stoploss = 0.50;
	m_forexpair[1].dotprice = 0.01;

	m_fxstatic[0].SubclassDlgItem(IDC_STATIC1,this);
	m_fxstatic[1].SubclassDlgItem(IDC_STATIC2,this);
	m_fxstatic[2].SubclassDlgItem(IDC_STATIC3,this);
	m_fxstatic[3].SubclassDlgItem(IDC_STATIC4,this);
	m_fxstatic[4].SubclassDlgItem(IDC_STATIC5,this);

	m_forexpair[0].pstatic = &m_fxstatic[0];
	m_forexpair[1].pstatic = &m_fxstatic[1];
	m_forexpair[2].pstatic = &m_fxstatic[2];
	m_forexpair[3].pstatic = &m_fxstatic[3];
	m_forexpair[4].pstatic = &m_fxstatic[4];

	CString strfirst,strsecond,strf,strt;
	int i;
	for (i=0;i<5;i++)
	{
		strfirst = m_forexpair[i].strDisplay.Left(3);
		strsecond = m_forexpair[i].strDisplay.Right(3);

		m_forexpair[i].tickid = i;
		m_forexpair[i].contract.symbol = strfirst;
		m_forexpair[i].contract.secType = "cash";
		m_forexpair[i].contract.expiry = "";
		m_forexpair[i].contract.strike = 0.0;
		m_forexpair[i].contract.right = "";
		m_forexpair[i].contract.multiplier = "";
		m_forexpair[i].contract.exchange = "idealpro";
		m_forexpair[i].contract.currency = strsecond;
		m_forexpair[i].contract.localSymbol = "";
		m_forexpair[i].contract.includeExpired = false;
		m_forexpair[i].contract.secIdType = "";
		m_forexpair[i].contract.secId = "";
		m_forexpair[i].contract.comboLegsDescrip = "";
		m_forexpair[i].contract.primaryExchange = "";
		m_forexpair[i].contract.conId= 0;

		m_forexpair[i].pstatic->m_strcurrency = m_forexpair[i].strDisplay;
		strt.Format("%%7.%df",m_forexpair[i].numberafterdot);
		strf = strt;
		strf += "   ";
		strf += strt;
		strf += " ";

		m_forexpair[i].pstatic->m_strpriceformat = strf;
		m_forexpair[i].pstatic->m_index = i;
		m_forexpair[i].pstatic->m_fxstatics = m_fxstatic;
		m_forexpair[i].pstatic->MoveWindow(10,10 + i*LABELHEIGHT,LABELWIDTH,LABELHEIGHT);
	}

	CRect dlgrect;
	GetClientRect(dlgrect);
	m_list.SubclassDlgItem(IDC_LIST1,this);
	m_list.MoveWindow(0, i*LABELHEIGHT + 60,dlgrect.Width()-1,dlgrect.Height() - i*LABELHEIGHT - 61);
	m_list.InsertColumn(0,"Id",0,100);
	m_list.InsertColumn(1,"Currency",0,70);
	m_list.InsertColumn(2,"Action",0,50);
	m_list.InsertColumn(3,"Quantity",0,70);
	m_list.InsertColumn(4,"Type",0,50);
	m_list.InsertColumn(5,"Price",0,70);
	m_list.InsertColumn(6,"Target",0,120);
	m_list.InsertColumn(7,"Stop",0,120);
	
	m_list.SetExtendedStyle(m_list.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT);


	CString strServerIp;
	UINT port,clientid;
	char cip[MAX_PATH];
	GetPrivateProfileStringA("General","ServerIp","127.0.0.1",cip,MAX_PATH,m_strinifilepath);
	strServerIp = cip;
	port = GetPrivateProfileIntA("General","Port",7496,m_strinifilepath);
	clientid = GetPrivateProfileIntA("General","ClientId",0,m_strinifilepath);

	m_trendpoints = GetPrivateProfileIntA("FlashOrder","TrendPoints",30,m_strinifilepath);
	m_trendquantity = GetPrivateProfileIntA("FlashOrder","TrendQuantity",100000,m_strinifilepath);

	char title[MAX_PATH];
	GetPrivateProfileStringA("General","TWSTitle","U582496 IB TWS",title,MAX_PATH,m_strinifilepath);
	m_strTwsTitle = title;

	m_hwndTws = ::FindWindowA(NULL, m_strTwsTitle);
	if (m_hwndTws == NULL)
	{
		GetPrivateProfileStringA("General","SimluatedTWSTitle","DU111599 IB TWS (Simulated Trading)",title,MAX_PATH,m_strinifilepath);
		m_strTwsTitle = title;
	}

	m_hwndTws = ::FindWindowA(NULL, m_strTwsTitle);
	if ((m_hwndTws == NULL)&&(strServerIp=="127.0.0.1"))
	{
		::MessageBoxA(NULL,"Failed to find TWS window!","IBTradeTool - Warning",MB_OK|MB_ICONEXCLAMATION);
		PostQuitMessage(1);
		return TRUE;
	}


	m_pClient->eConnect(strServerIp,port,clientid);
	if( m_pClient->serverVersion() <= 0)
	{
		::MessageBoxA(NULL,"Failed to connect to the server!","IBTradeTool - Warning",MB_OK|MB_ICONEXCLAMATION);
		PostQuitMessage(1);
		return TRUE;

	}

	//发送数据请求
	SetTimer(1,1000,NULL);

	//定时隐藏Order List
	SetTimer(4,100000,NULL);


	m_bhideflashorderbutton = true;
	GetDlgItem(IDC_BUTTONBUYONTRENDS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTONSELLONTRENDS)->ShowWindow(SW_HIDE);

	/*
	CRect rb;
	GetDlgItem(IDC_BUTTON1)->GetWindowRect(rb);
	ScreenToClient(rb);
	GetDlgItem(IDC_BUTTONBUYONTRENDS)->MoveWindow(rb);
	GetDlgItem(IDC_BUTTON3)->GetWindowRect(rb);
	ScreenToClient(rb);
	GetDlgItem(IDC_BUTTONSELLONTRENDS)->MoveWindow(rb);
	*/


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIBTradeToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIBTradeToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIBTradeToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIBTradeToolDlg::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute )
{

	if (price < 0.001)
		return;

	int direction = 0;
	ForexPair *pfp = &m_forexpair[tickerId];
	if (tickType == BID)
	{
		direction = price > pfp->lastbid ? 1 : 2 ;
		pfp->lastbid = price;

		std::list<ConditionOrder>::iterator p;
		for (p=pfp->orderlist.begin();p!=pfp->orderlist.end();)
		{
			if (p->orderaction == 2)
			{
				if ((p->ordertype == 1)&&(price >= p->conditionprice))//sell lmt
				{
					ExecuteOrder(pfp, *p, price);
					p = pfp->orderlist.erase(p);
					UpdateListData();
					continue;
				}
				else if ((p->ordertype == 2)&&(price <= p->conditionprice))//sell stp
				{
					ExecuteOrder(pfp, *p, price);
					p = pfp->orderlist.erase(p);
					UpdateListData();
					continue;
				}
				else if (p->ordertype == 3)//sell to cover
				{
					if (price >= p->targetprice)
					{
						ConditionOrder co;
						co.orderindex = p->orderindex;
						co.orderaction = 2;
						co.ordertype = 1;//sell lmt
						co.conditionprice = p->targetprice;
						co.quantity = p->quantity;
						co.needalert = p->needalert;

						ExecuteOrder(pfp, co, price);
						p = pfp->orderlist.erase(p);
						UpdateListData();
						continue;

					}
					else if (price <= p->stopprice)
					{
						ConditionOrder co;
						co.orderindex = p->orderindex;
						co.orderaction = 2;
						co.ordertype = 2;//sell stp
						co.conditionprice = p->stopprice;
						co.quantity = p->quantity;
						co.needalert = p->needalert;

						ExecuteOrder(pfp, co, price);
						p = pfp->orderlist.erase(p);
						UpdateListData();
						continue;

					}

				}

			}

			p++;

		}


	}
	else if (tickType == ASK)
	{
		direction = price > pfp->lastask? 1 : 2 ;
		pfp->lastask = price;

		std::list<ConditionOrder>::iterator p;
		for (p=pfp->orderlist.begin();p!=pfp->orderlist.end();)
		{
			if (p->orderaction == 1)
			{
				if ((p->ordertype == 1)&&(price <= p->conditionprice))//buy lmt
				{
					ExecuteOrder(pfp, *p, price);
					p = pfp->orderlist.erase(p);
					UpdateListData();
					continue;
				}
				else if ((p->ordertype == 2)&&(price >= p->conditionprice))//buy stp
				{
					ExecuteOrder(pfp, *p, price);
					p = pfp->orderlist.erase(p);
					UpdateListData();
					continue;
				}
				else if (p->ordertype == 3)//buy to cover
				{
					if (price <= p->targetprice)
					{
						ConditionOrder co;
						co.orderindex = p->orderindex;
						co.orderaction = 1;
						co.ordertype = 1;//buy lmt
						co.conditionprice = p->targetprice;
						co.quantity = p->quantity;
						co.needalert = p->needalert;

						ExecuteOrder(pfp, co, price);
						p = pfp->orderlist.erase(p);
						UpdateListData();
						continue;

					}
					else if (price >= p->stopprice)
					{
						ConditionOrder co;
						co.orderindex = p->orderindex;
						co.orderaction = 1;
						co.ordertype = 2;//buy stp
						co.conditionprice = p->stopprice;
						co.quantity = p->quantity;
						co.needalert = p->needalert;

						ExecuteOrder(pfp, co, price);
						p = pfp->orderlist.erase(p);
						UpdateListData();
						continue;
					}



				}
			}

			p++;

		}
	}

	pfp->pstatic->SetPriceAndDirection(pfp->lastbid,pfp->lastask,direction);



}

void CIBTradeToolDlg::tickSize( TickerId ddeId, TickType field, int size )
{

}

void CIBTradeToolDlg::tickOptionComputation( TickerId ddeId, TickType field, double impliedVol, double delta, double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice )
{

}

void CIBTradeToolDlg::tickGeneric( TickerId tickerId, TickType tickType, double value )
{

}

void CIBTradeToolDlg::tickString( TickerId tickerId, TickType tickType, const CString& value )
{

}

void CIBTradeToolDlg::tickEFP( TickerId tickerId, TickType tickType, double basisPoints, const CString& formattedBasisPoints, double totalDividends, int holdDays, const CString& futureExpiry, double dividendImpact, double dividendsToExpiry )
{

}

void CIBTradeToolDlg::orderStatus( OrderId orderId, const CString &status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const CString& whyHeld )
{
	int i;
	for (i=0;i<5;i++)
	{
		std::list<ConditionOrder>::iterator p;
		for (p=m_forexpair[i].processingorderlist.begin();p!=m_forexpair[i].processingorderlist.end();)
		{
			if (p->reqId == orderId)
			{
				if ((remaining==0)&&(status=="Filled"))//定单执行完毕
				{
					if (p->orderaction == 1)//buy
					{
						if ((p->targetprice >0) &&(p->stopprice > 0))
						{
							AddOrder(i,2,3,p->quantity,0,p->targetprice,p->stopprice,p->needalert);
						}
						else if (p->targetprice > 0)
						{
							AddOrder(i,2,1,p->quantity,p->targetprice,0,0,p->needalert);
						}
						else if (p->stopprice > 0)
						{
							AddOrder(i,2,2,p->quantity,p->stopprice,0,0,p->needalert);
						}


					}
					else if (p->orderaction == 2)//sell
					{
						if ((p->targetprice >0) &&(p->stopprice > 0))
						{
							AddOrder(i,1,3,p->quantity,0,p->targetprice,p->stopprice,p->needalert);

						}
						else if (p->targetprice > 0)
						{
							AddOrder(i,1,1,p->quantity,p->targetprice,0,0,p->needalert);
						}
						else if (p->stopprice > 0)
						{
							AddOrder(i,1,2,p->quantity,p->stopprice,0,0,p->needalert);
						}


					}

					m_forexpair[i].processingorderlist.erase(p);

				}
				else if (status == "Cancelled")
				{
					m_forexpair[i].processingorderlist.erase(p);
				}

				break;
			}
			else
			{
				p++;
			}

		}
	}



}

void CIBTradeToolDlg::openOrder( OrderId orderId, const Contract&, const Order&, const OrderState& )
{

}

void CIBTradeToolDlg::openOrderEnd()
{

}

void CIBTradeToolDlg::winError( const CString &str, int lastError )
{

}

void CIBTradeToolDlg::connectionClosed()
{
	CString str;
	GetWindowTextA(str);
	SetWindowText(str + " ### Connection closed ###");
	MessageBox( "Connection closed!","Warning",MB_OK|MB_ICONEXCLAMATION);
	
}

void CIBTradeToolDlg::updateAccountValue( const CString &key, const CString &val, const CString &currency, const CString &accountName )
{

}

void CIBTradeToolDlg::updatePortfolio( const Contract& contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const CString &accountName )
{

}

void CIBTradeToolDlg::updateAccountTime( const CString &timeStamp )
{

}

void CIBTradeToolDlg::accountDownloadEnd( const CString &accountName )
{

}

void CIBTradeToolDlg::nextValidId( OrderId orderId )
{
	m_orderreqId = orderId;
}

void CIBTradeToolDlg::contractDetails( int reqId, const ContractDetails& contractDetails )
{

}

void CIBTradeToolDlg::bondContractDetails( int reqId, const ContractDetails& contractDetails )
{

}

void CIBTradeToolDlg::contractDetailsEnd( int reqId )
{

}

void CIBTradeToolDlg::execDetails( int reqId, const Contract& contract, const Execution& execution )
{

}

void CIBTradeToolDlg::execDetailsEnd( int reqId )
{

}

void CIBTradeToolDlg::error( const int id, const int errorCode, const CString errorString )
{

}

void CIBTradeToolDlg::updateMktDepth( TickerId id, int position, int operation, int side, double price, int size )
{

}

void CIBTradeToolDlg::updateMktDepthL2( TickerId id, int position, CString marketMaker, int operation, int side, double price, int size )
{

}

void CIBTradeToolDlg::updateNewsBulletin( int msgId, int msgType, const CString& newsMessage, const CString& originExch )
{

}

void CIBTradeToolDlg::managedAccounts( const CString& accountsList )
{

}

void CIBTradeToolDlg::receiveFA( faDataType pFaDataType, const CString& cxml )
{

}

void CIBTradeToolDlg::historicalData( TickerId reqId, const CString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps )
{

}

void CIBTradeToolDlg::scannerParameters( const CString &xml )
{

}

void CIBTradeToolDlg::scannerData( int reqId, int rank, const ContractDetails &contractDetails, const CString &distance, const CString &benchmark, const CString &projection, const CString &legsStr )
{

}

void CIBTradeToolDlg::scannerDataEnd( int reqId )
{

}

void CIBTradeToolDlg::realtimeBar( TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count )
{

}

void CIBTradeToolDlg::currentTime( long time )
{

}

void CIBTradeToolDlg::fundamentalData( TickerId reqId, const CString& data )
{

}

void CIBTradeToolDlg::deltaNeutralValidation( int reqId, const UnderComp& underComp )
{

}

void CIBTradeToolDlg::tickSnapshotEnd( int reqId )
{

}

void CIBTradeToolDlg::marketDataType( TickerId reqId, int marketDataType )
{

}



void CIBTradeToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1)
	{
		KillTimer(1);
		int i;
		for (i=0;i<5;i++)
		{

			m_pClient->reqMktData(m_forexpair[i].tickid, m_forexpair[i].contract,
				_T("100,101,104,105,106,107,165,221,225,233,236,258,293,294,295,318"), 
				false);
		}

		SetTimer(3,1000,NULL);
	}
	else if (nIDEvent == 2)
	{
		KillTimer(2);
		if (m_hwndTws != NULL)
			SwitchToThisWindow(m_hwndTws,TRUE);
	}
	else if (nIDEvent == 3)
	{

		int i, tickcount = GetTickCount();
		for (i=0;i<5;i++)
		{
			std::list<ConditionOrder>::iterator p;
			for (p=m_forexpair[i].processingorderlist.begin();p != m_forexpair[i].processingorderlist.end();p++)
			{

				if ((tickcount - p->processtime > 5000)&&(p->processtime >0))//not executed in 5 seconds
				{
					p->processtime = 0;
					PlaySoundA("c:\\noorder.wav",NULL,SND_ASYNC|SND_LOOP);
					
				}
			}
		}

	}
	else if (nIDEvent == 4)
	{
		if (!m_bhidelist)
		{
			CRect rc,rclist;
			GetWindowRect(rc);
			m_list.GetWindowRect(rclist);
			SetWindowPos(0,0,0,rc.Width(),rc.Height() - rclist.Height(), SWP_NOZORDER|SWP_NOMOVE);
			m_list.ShowWindow(SW_HIDE);

			m_bhidelist = true;
		}

	}



	__super::OnTimer(nIDEvent);
}


HBRUSH CIBTradeToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd == this)
	{
		return (HBRUSH)GetStockObject(BLACK_BRUSH);
	}
	

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CIBTradeToolDlg::OnSize(UINT nType, int cx, int cy)
{
	
	__super::OnSize(nType, cx, cy);


	
	// TODO: Add your message handler code here
}


void CIBTradeToolDlg::OnBnClickedButton1()
{
	int i,sel = -1;
	for (i=0;i<5;i++)
	{
		if (m_fxstatic[i].m_bselected)
		{
			sel = i;
			break;
		}
	}

	if (sel == -1)
	{
		MessageBox("Please select a currency quote!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	COrderDlg odlg;
	odlg.m_forextpair = m_forexpair;
	odlg.m_forexindex = sel;
	odlg.m_orderaction = 1;
	odlg.m_ordertype = 1;
	odlg.m_conditionprice = m_forexpair[sel].lastask;
	odlg.m_targetprice = 0;
	odlg.m_stopprice = 0;
	odlg.m_quantity = 100000;
	odlg.m_balert = true;

	if (odlg.DoModal() == IDOK)
	{

		AddOrder(odlg.m_forexindex,odlg.m_orderaction,odlg.m_ordertype,odlg.m_quantity,
			odlg.m_conditionprice,odlg.m_targetprice,odlg.m_stopprice,odlg.m_balert);


	}
}


void CIBTradeToolDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	__super::OnLButtonDown(nFlags, point);
}


void CIBTradeToolDlg::OnBnClickedButton3()
{
	int i,sel = -1;
	for (i=0;i<5;i++)
	{
		if (m_fxstatic[i].m_bselected)
		{
			sel = i;
			break;
		}
	}

	if (sel == -1)
	{
		MessageBox("Please select a currency quote!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	COrderDlg odlg;
	odlg.m_forextpair = m_forexpair;
	odlg.m_forexindex = sel;
	odlg.m_orderaction = 2;
	odlg.m_ordertype = 1;
	odlg.m_conditionprice = m_forexpair[sel].lastbid;
	odlg.m_targetprice = 0;
	odlg.m_stopprice = 0;
	odlg.m_quantity = 100000;
	odlg.m_balert = true;

	if (odlg.DoModal() == IDOK)
	{

		AddOrder(odlg.m_forexindex,odlg.m_orderaction,odlg.m_ordertype,odlg.m_quantity,
			odlg.m_conditionprice,odlg.m_targetprice,odlg.m_stopprice,odlg.m_balert);


	}
}

void CIBTradeToolDlg::AddOrder( int forexindex, int action,int type,int quantity, double fprice,double ftarget,double fstop,bool balert )
{
	ConditionOrder corder;
	CTime now = CTime::GetCurrentTime();

	corder.orderindex = now.GetTime();
	corder.orderaction = action;
	corder.ordertype = type;
	corder.quantity = quantity;
	corder.conditionprice = fprice;
	corder.targetprice = ftarget;
	corder.stopprice = fstop;
	corder.needalert = balert;

	m_forexpair[forexindex].orderlist.push_front(corder);
	UpdateListData();


}

void CIBTradeToolDlg::UpdateListData()
{
	m_list.DeleteAllItems();
	int i,count=0;
	for (i=0;i<5;i++)
	{
		std::list<ConditionOrder>::iterator p;
		for (p=m_forexpair[i].orderlist.begin();p != m_forexpair[i].orderlist.end();p++)
		{
			CString str;
			str.Format("%d",p->orderindex);
			m_list.InsertItem(count,str);
			m_list.SetItemText(count,1,m_forexpair[i].strDisplay);

			str = p->orderaction == 1 ? "BUY":"SELL";
			m_list.SetItemText(count,2,str);

			if (p->quantity % 1000 == 0)
			{
				str.Format("%dK",p->quantity/1000);
			}
			else
			{
				str.Format("%d",p->quantity);
			}
			m_list.SetItemText(count,3,str);


			if (p->ordertype == 1)
			{
				str = "LMT";
			}
			else if (p->ordertype == 2)
			{
				str = "STP";
			}
			else if (p->ordertype == 3)
			{
				str = "COV";
			}

			m_list.SetItemText(count,4,str);

			CString formatstring;
			formatstring.Format("%%.%df",m_forexpair[i].numberafterdot);

			str = "-";
			if (p->conditionprice > 0)
				str.Format(formatstring,p->conditionprice);
			m_list.SetItemText(count,5,str);

			str = "-";
			if (p->targetprice > 0)
			{
				if (p->conditionprice > 0)
				{
					double delta = fabs(p->targetprice - p->conditionprice);
					str.Format(formatstring + " (%d pipes)",p->targetprice, int(delta/m_forexpair[i].dotprice + 0.1));
				}
				else
				{
					str.Format(formatstring,p->targetprice);
				}
			
			}
			m_list.SetItemText(count,6,str);

			str = "-";
			if (p->stopprice > 0)
			{
				if (p->conditionprice > 0)
				{
					double delta = fabs(p->stopprice - p->conditionprice);
					str.Format(formatstring + " (-%d pipes)",p->stopprice, int(delta/m_forexpair[i].dotprice + 0.1));
				}
				else
				{
					str.Format(formatstring,p->stopprice);
				}

			}
			m_list.SetItemText(count,7,str);

			count ++;

		}
	}


	//定时隐藏Order List
	KillTimer(4);
	SetTimer(4,100000,NULL);


}

void CIBTradeToolDlg::ExecuteOrder( ForexPair* pforex,ConditionOrder order,double price )
{
	

	if (order.orderaction == 1)
	{
		
		Order sysorder;
		sysorder.orderId = m_orderreqId;
		sysorder.action = "BUY";
		sysorder.totalQuantity = order.quantity;

		if (order.ordertype == 1)
		{
			sysorder.orderType = "LMT";

			sysorder.lmtPrice = price + pforex->lmtmoveprice;
			sysorder.whatIf = false;
			sysorder.hidden = true;
			sysorder.displaySize = 0;

		}
		else if (order.ordertype == 2)
		{
			sysorder.orderType = "LMT";
			double lmtprice = price + pforex->halfstpmoveprice + pforex->halfstpmoveprice;
			while (lmtprice - order.conditionprice > pforex->halfstpmoveprice * 3)
			{
				lmtprice -= pforex->halfstpmoveprice;

			}

			sysorder.lmtPrice = lmtprice;
			sysorder.whatIf = false;
			sysorder.hidden = true;
			sysorder.displaySize = 0;
		}

		//move to processing order list
		order.reqId = m_orderreqId;
		order.processtime = GetTickCount();
		pforex->processingorderlist.push_front(order);

		// place order
		m_pClient->placeOrder(m_orderreqId, pforex->contract, sysorder);
		//get next order id
		m_pClient->reqIds(20);

		SetTimer(2,300,NULL);//switch window to tws

		if (order.needalert)
		{
			PlaySoundA("c:\\check.wav",NULL,SND_ASYNC|SND_LOOP);
		}

	}
	else if (order.orderaction == 2)
	{

		Order sysorder;
		sysorder.orderId = m_orderreqId;
		sysorder.action = "SELL";
		sysorder.totalQuantity = order.quantity;

		if (order.ordertype == 1)
		{
			sysorder.orderType = "LMT";
			sysorder.lmtPrice = price - pforex->lmtmoveprice;
			sysorder.whatIf = false;
			sysorder.hidden = true;
			sysorder.displaySize = 0;
		}
		else if (order.ordertype == 2)
		{
			sysorder.orderType = "LMT";
			double lmtprice = price - pforex->halfstpmoveprice - pforex->halfstpmoveprice;
			while (order.conditionprice - lmtprice > pforex->halfstpmoveprice * 3)
			{
				lmtprice += pforex->halfstpmoveprice;

			}

			sysorder.lmtPrice = lmtprice;
			sysorder.whatIf = false;
			sysorder.hidden = true;
			sysorder.displaySize = 0;
		}


		//move to processing order list
		order.reqId = m_orderreqId;
		order.processtime = GetTickCount();
		pforex->processingorderlist.push_front(order);

		// place order
		m_pClient->placeOrder(m_orderreqId, pforex->contract, sysorder);
		//get next order id
		m_pClient->reqIds(20);

		SetTimer(2,300,NULL);//switch window to tws

		if (order.needalert)
		{
			PlaySoundA("c:\\check.wav",NULL,SND_ASYNC|SND_LOOP);
		}

	
	}


}


void CIBTradeToolDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	__super::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CIBTradeToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_CHAR)
	{
		if ((pMsg->wParam == 'h')||(pMsg->wParam == 'H'))
		{
			if (m_bhidelist)
			{
				CRect rc,rclist;
				GetWindowRect(rc);
				m_list.GetWindowRect(rclist);
				SetWindowPos(0,0,0,rc.Width(),rc.Height() + rclist.Height(), SWP_NOZORDER|SWP_NOMOVE);
				m_list.ShowWindow(SW_SHOW);

				m_bhidelist = false;
			}
			else
			{
				CRect rc,rclist;
				GetWindowRect(rc);
				m_list.GetWindowRect(rclist);
				SetWindowPos(0,0,0,rc.Width(),rc.Height() - rclist.Height(), SWP_NOZORDER|SWP_NOMOVE);
				m_list.ShowWindow(SW_HIDE);

				m_bhidelist = true;
			}

		}
		else if ((pMsg->wParam == 't')||(pMsg->wParam == 'T'))
		{
			if (m_bhideflashorderbutton)
			{
				m_bhideflashorderbutton = false;
				GetDlgItem(IDC_BUTTONBUYONTRENDS)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_BUTTONSELLONTRENDS)->ShowWindow(SW_SHOW);
				//GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);
			}
			else
			{
				m_bhideflashorderbutton = true;
				GetDlgItem(IDC_BUTTONBUYONTRENDS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BUTTONSELLONTRENDS)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_SHOW);
				//GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_SHOW);
			}

		}
		else if ((pMsg->wParam == 'o')||(pMsg->wParam == 'O'))
		{

			PlaySoundA(NULL,NULL,NULL);

		}

	}


	return __super::PreTranslateMessage(pMsg);
}


void CIBTradeToolDlg::OnBnClickedButtoncancelorder()
{
	if (m_bhidelist)
	{
		MessageBeep(1);
		return;
	}

	int index = 0,oid = 0;
	int count = m_list.GetItemCount();
	for (index = 0;index < count;index++)
	{
		if( m_list.GetItemState(index, LVIS_SELECTED) == LVIS_SELECTED )
		{
			oid = atoi(m_list.GetItemText(index, 0));
			break;
		}
	}


	if (oid == 0)
	{
		MessageBox("请选中一行定单!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	for (index = 0;index < 5;index ++)
	{
		std::list<ConditionOrder>::iterator p;
		for (p=m_forexpair[index].orderlist.begin();p!=m_forexpair[index].orderlist.end();)
		{
			if (p->orderindex == oid)
			{
				m_forexpair[index].orderlist.erase(p);
				break;
			}
			else
			{
				p++;
			}
		}
	}


	UpdateListData();


}



void CIBTradeToolDlg::OnBnClickedButtonbuytocover()
{
	int i,sel = -1;
	for (i=0;i<5;i++)
	{
		if (m_fxstatic[i].m_bselected)
		{
			sel = i;
			break;
		}
	}

	if (sel == -1)
	{
		MessageBox("Please select a currency quote!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	double curaask = m_forexpair[sel].lastask;

	COrderDlg odlg;
	odlg.m_forextpair = m_forexpair;
	odlg.m_forexindex = sel;
	odlg.m_orderaction = 1;//buy to cover
	odlg.m_ordertype = 3;
	odlg.m_conditionprice = 0;
	odlg.m_targetprice = curaask - m_forexpair[sel].profittake;
	odlg.m_stopprice = curaask + m_forexpair[sel].stoploss;
	odlg.m_quantity = 100000;
	odlg.m_balert = true;

	if (odlg.DoModal() == IDOK)
	{

		AddOrder(odlg.m_forexindex,odlg.m_orderaction,odlg.m_ordertype,odlg.m_quantity,
			odlg.m_conditionprice,odlg.m_targetprice,odlg.m_stopprice,odlg.m_balert);

	}
}


void CIBTradeToolDlg::OnBnClickedButtonselltocover()
{
	int i,sel = -1;
	for (i=0;i<5;i++)
	{
		if (m_fxstatic[i].m_bselected)
		{
			sel = i;
			break;
		}
	}

	if (sel == -1)
	{
		MessageBox("Please select a currency quote!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	double curbid = m_forexpair[sel].lastbid;

	COrderDlg odlg;
	odlg.m_forextpair = m_forexpair;
	odlg.m_forexindex = sel;
	odlg.m_orderaction = 2;//sell to cover
	odlg.m_ordertype = 3;
	odlg.m_conditionprice = 0;
	odlg.m_targetprice = curbid + m_forexpair[sel].profittake;
	odlg.m_stopprice = curbid - m_forexpair[sel].stoploss;
	odlg.m_quantity = 100000;
	odlg.m_balert = true;

	if (odlg.DoModal() == IDOK)
	{

		AddOrder(odlg.m_forexindex,odlg.m_orderaction,odlg.m_ordertype,odlg.m_quantity,
			odlg.m_conditionprice,odlg.m_targetprice,odlg.m_stopprice,odlg.m_balert);

	}

}


void CIBTradeToolDlg::OnBnClickedButtonbuyontrends()
{

	int i,sel = -1;
	for (i=0;i<5;i++)
	{
		if (m_fxstatic[i].m_bselected)
		{
			sel = i;
			break;
		}
	}

	if (sel == -1)
	{
		MessageBox("Please select a currency quote!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	
	CString formatstring;
	formatstring.Format("%%.%df",m_forexpair[sel].numberafterdot);

	double ftarget,fstop;
	double fprice = m_forexpair[sel].lastask;
	ftarget = fprice + m_forexpair[sel].dotprice * m_trendpoints;
	fstop = fprice - m_forexpair[sel].dotprice * m_trendpoints;

	CString strprop;
	strprop.Format("Buy @" + formatstring + "\r\nTarget @" + formatstring + " (%d pipes)\r\nStop @" + formatstring + " (%d pipes)\r\n\r\nTransmit?",
		fprice,ftarget,m_trendpoints,fstop,m_trendpoints);

	if (MessageBox(strprop,"Confirm",MB_YESNO|MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	ConditionOrder corder;
	CTime now = CTime::GetCurrentTime();

	corder.orderindex = now.GetTime();
	corder.orderaction = 1;
	corder.ordertype = 1;//buy lmt
	corder.quantity = m_trendquantity;
	corder.conditionprice = fprice;
	corder.targetprice = ftarget;
	corder.stopprice = fstop;
	corder.needalert = false;

	ExecuteOrder(&m_forexpair[sel], corder, fprice);

	m_bhideflashorderbutton = true;
	GetDlgItem(IDC_BUTTONBUYONTRENDS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTONSELLONTRENDS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_SHOW);
}


void CIBTradeToolDlg::OnBnClickedButtonsellontrends()
{
	
	int i,sel = -1;
	for (i=0;i<5;i++)
	{
		if (m_fxstatic[i].m_bselected)
		{
			sel = i;
			break;
		}
	}

	if (sel == -1)
	{
		MessageBox("Please select a currency quote!","Warning",MB_OK|MB_ICONEXCLAMATION);
		return;
	}


	CString formatstring;
	formatstring.Format("%%.%df",m_forexpair[sel].numberafterdot);

	double ftarget,fstop;
	double fprice = m_forexpair[sel].lastbid;
	ftarget = fprice - m_forexpair[sel].dotprice * m_trendpoints;
	fstop = fprice + m_forexpair[sel].dotprice * m_trendpoints;

	CString strprop;
	strprop.Format("Sell @" + formatstring + "\r\nTarget @" + formatstring + " (%d pipes)\r\nStop @" + formatstring + " (%d pipes)\r\n\r\nTransmit?",
		fprice,ftarget,m_trendpoints,fstop,m_trendpoints);

	if (MessageBox(strprop,"Confirm",MB_YESNO|MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	ConditionOrder corder;
	CTime now = CTime::GetCurrentTime();

	corder.orderindex = now.GetTime();
	corder.orderaction = 2;
	corder.ordertype = 1;//sell lmt
	corder.quantity = m_trendquantity;
	corder.conditionprice = fprice;
	corder.targetprice = ftarget;
	corder.stopprice = fstop;
	corder.needalert = false;

	ExecuteOrder(&m_forexpair[sel], corder, fprice);

	m_bhideflashorderbutton = true;
	GetDlgItem(IDC_BUTTONBUYONTRENDS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTONSELLONTRENDS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_SHOW);
}
