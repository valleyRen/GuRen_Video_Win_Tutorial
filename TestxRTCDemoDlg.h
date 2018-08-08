
// TestxRTCDemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "VideoWnd.h"
#include "MSGDLG.h"

#include "TestxVideoChannel.h" 


// CTestxRTCDemoDlg 对话框
class CTestxRTCDemoDlg : public CDialog 
{
// 构造
public:
	CTestxRTCDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CTestxRTCDemoDlg();	 

// 对话框数据 

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected: 

protected: 
	void OnGlobalRespond(objBaseEvent* e); 
	void OnGlobalNotify(objBaseEvent* e);
	void ShowMessage(const CString& msg, int ec=0);

	void OnTimer(UINT_PTR timerID);
public:
	afx_msg void OnBnClickedOk();
	CVideoWnd m_MyPlayer;
	CVideoWnd m_Player1;
	CVideoWnd m_Player2;
	CVideoWnd m_Player3;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2(); 
	CEdit m_szRoomID;
	CEdit m_szUserID;
	afx_msg void OnClose(); 
	virtual void OnMenuCommand(UINT nID);
	void OnEnterRoomResult(objBaseEvent* e);

	void OnCancel();
private:
		bool				m_bDoTest;
		CTestxVideoChannel  m_xRtcVideoRoom;
		handle_t            m_xRtcGlobalRoom;
		CMSGDLG*            m_msgdlg = nullptr; 
public:
	afx_msg void OnBnClickedImInit();
};
