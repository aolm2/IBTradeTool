
// IBTradeToolDlg.h : header file
//

#pragma once

#include "EWrapper.h"       // '<INSTALL_DIR>\SocketClient\include' must be added to include path
#include "HScrollListBox.h" // '<INSTALL_DIR>\Shared' must be added to include path
#include "Contract.h"
#include "Execution.h"

#include "FXStatic.h"


#include <memory>

class EClient;
class CHScrollListBox;

struct Contract;
struct ContractDetails;
struct Order;
struct OrderState;
struct Execution;
struct ExecutionFilter;
struct ScannerSubscription;



#define NOT_AN_FA_ACCOUNT_ERROR 321

#include <list>


#include "FXStatic.h"
#define LABELWIDTH 230
#define LABELHEIGHT 20

struct ConditionOrder
{
	int orderindex;
	int reqId;
	int orderaction;//1-buy,2-sell
	int ordertype;//1-lmt, 2- stp, 3- cover
	double conditionprice;
	double targetprice;
	double stopprice;
	int quantity;
	int filledquantity;
	bool needalert;
	bool onlyalert;

	int processtime;//gettickcount

	ConditionOrder()
	{
		orderindex = 0;
		reqId = 0;
		orderaction = 1;
		ordertype = 1;
		conditionprice = 0;
		targetprice = 0;
		stopprice = 0;
		quantity = 100000;
		filledquantity = 0;
		needalert = true;
		onlyalert = false;
		processtime = 0;

	}

};


struct BarPrice
{
	float fOpen;
	float fClose;
	float fHigh;
	float fLow;
};


struct ForexPair
{
	int tickid;//==数组下标
	CString strDisplay;
	int numberafterdot;
	double lmtmoveprice;
	double halfstpmoveprice;
	double profittake;
	double stoploss;
	double dotprice;//一个点对应的price
	Contract contract;
	double lastbid,lastask;

	CFXStatic *pstatic;
	std::list<ConditionOrder> orderlist;
	std::list<ConditionOrder> processingorderlist;

	ForexPair()
	{
		strDisplay = "XXX/XXX";
		numberafterdot = 5;
		lastbid = 0;
		lastask = 0;
		lmtmoveprice = 0.0002;//2个点对应numberafterdot=5就是 0.0002
		halfstpmoveprice = 0.0005;//10个点对应numberafterdot=5止损一半就是 0.0005
		profittake = 0.0050;
		stoploss = 0.0050;//50个点默认
		dotprice = 0.0001;
	}

};


// CIBTradeToolDlg dialog
class CIBTradeToolDlg : public CDialog, public EWrapper
{

public:
	HWND m_hwndTws;
	CString m_strTwsTitle;

	CString m_strinifilepath;
	ForexPair m_forexpair[5];
	CFXStatic m_fxstatic[5];
	CListCtrl m_list;

	int m_orderreqId;
	bool m_bhidelist;
	int m_trendpoints;//趋势单的止损止盈
	int m_trendquantity;//趋势单默认大小
	bool m_bhideflashorderbutton;

	void AddOrder(int forexindex, int action,int type,int quantity,
		double fprice,double ftarget,double fstop,bool balert);

	void ExecuteOrder(ForexPair* pforex,ConditionOrder order,double price);

	void UpdateListData();

// Construction
public:
	CIBTradeToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IBTRADETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	// handle events from client
	void tickPrice( TickerId ddeId, TickType field, double price, int canAutoExecute);
	void tickSize( TickerId ddeId, TickType field, int size);
	void tickOptionComputation( TickerId ddeId, TickType field, double impliedVol,
		double delta, double optPrice, double pvDividend,
		double gamma, double vega, double theta, double undPrice);
	void tickGeneric(TickerId tickerId, TickType tickType, double value);
	void tickString(TickerId tickerId, TickType tickType, const CString& value);
	void tickEFP(TickerId tickerId, TickType tickType, double basisPoints,
		const CString& formattedBasisPoints, double totalDividends, int holdDays,
		const CString& futureExpiry, double dividendImpact, double dividendsToExpiry);
	void orderStatus( OrderId orderId, const CString &status, int filled, int remaining, 
		double avgFillPrice, int permId, int parentId, double lastFillPrice,
		int clientId, const CString& whyHeld);
	void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&);
	void openOrderEnd();
	void winError( const CString &str, int lastError);
	void connectionClosed();
	void updateAccountValue(const CString &key, const CString &val,
		const CString &currency, const CString &accountName);
	virtual void updatePortfolio( const Contract& contract, int position,
		double marketPrice, double marketValue, double averageCost,
		double unrealizedPNL, double realizedPNL, const CString &accountName);
	void updateAccountTime(const CString &timeStamp);
	void accountDownloadEnd(const CString &accountName);
	void nextValidId( OrderId orderId);
	void contractDetails( int reqId, const ContractDetails& contractDetails);
	void bondContractDetails( int reqId, const ContractDetails& contractDetails);
	void contractDetailsEnd( int reqId);
	void execDetails( int reqId, const Contract& contract, const Execution& execution);
	void execDetailsEnd( int reqId);
	void error(const int id, const int errorCode, const CString errorString);
	void error(const CString errorString);
	void updateMktDepth( TickerId id, int position, int operation, int side, 
		double price, int size);
	void updateMktDepthL2( TickerId id, int position, CString marketMaker, int operation, 
		int side, double price, int size);
	void updateNewsBulletin(int msgId, int msgType, const CString& newsMessage, const CString& originExch);
	void managedAccounts(const CString& accountsList);
	void receiveFA(faDataType pFaDataType, const CString& cxml);
	void historicalData(TickerId reqId, const CString& date, double open, double high, double low,
		double close, int volume, int barCount, double WAP, int hasGaps) ;
	void scannerParameters(const CString &xml);
	void scannerData(int reqId, int rank, const ContractDetails &contractDetails, const CString &distance,
		const CString &benchmark, const CString &projection, const CString &legsStr);
	void scannerDataEnd(int reqId);
	void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
		long volume, double wap, int count);
	void currentTime(long time);
	void fundamentalData(TickerId reqId, const CString& data);
	void deltaNeutralValidation(int reqId, const UnderComp& underComp);
	void tickSnapshotEnd( int reqId);
	void marketDataType( TickerId reqId, int marketDataType);

private:
	EClient     *m_pClient;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtoncancelorder();

	afx_msg void OnBnClickedButtonbuytocover();
	afx_msg void OnBnClickedButtonselltocover();
	afx_msg void OnBnClickedButtonbuyontrends();
	afx_msg void OnBnClickedButtonsellontrends();
};
