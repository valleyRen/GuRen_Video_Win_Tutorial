#pragma once


#include "TestxVideoChannel.h" 


// CMSGDLG dialog

class CMSGDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CMSGDLG)

public:
	CMSGDLG(handle_t xRoom, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMSGDLG();

// Dialog Data 

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSend();
	virtual BOOL OnInitDialog();
public:
	void ShowMessage(const char* userid, const char* lpmsg);
	void ShowSendResult(const char* userid, const char* lpmsg, int ec);


protected:
	handle_t m_xRtcGlobalRoom;
	CString  m_msgShow;
};
