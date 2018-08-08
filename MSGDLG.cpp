// MSGDLG.cpp : implementation file
//

#include "stdafx.h"
#include "TestxRTCDemo.h"
#include "MSGDLG.h"
#include "afxdialogex.h"


// CMSGDLG dialog

IMPLEMENT_DYNAMIC(CMSGDLG, CDialogEx)

CMSGDLG::CMSGDLG(handle_t xRoom, CWnd* pParent /*=NULL*/)
: CDialogEx(IDD_MGSDLG, pParent)
{
	m_xRtcGlobalRoom = xRoom;
}

CMSGDLG::~CMSGDLG()
{
}

void CMSGDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMSGDLG, CDialogEx)
	ON_BN_CLICKED(IDC_SEND, &CMSGDLG::OnBnClickedSend)
END_MESSAGE_MAP()

BOOL CMSGDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!m_msgShow.IsEmpty())
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SHOW);
		pEdit->SetSel(-1, -1);
		pEdit->ReplaceSel(m_msgShow);
	}

	return true;
}

// CMSGDLG message handlers
void CMSGDLG::ShowMessage(const char* userid, const char* lpmsg)
{  
	m_msgShow.Format("\r\n[收到来自 %s 的消息]", userid);
	m_msgShow.Append(lpmsg);

	if (!m_hWnd)
		return;
	
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SHOW);
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(m_msgShow); 
}


void CMSGDLG::ShowSendResult(const char* userid, const char* lpmsg, int ec)
{ 
	if (0 == ec)
	{
		m_msgShow.Format("\r\n[发给 %s 的消息] ", userid);
		m_msgShow.Append(lpmsg);

		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SHOW);
		pEdit->SetSel(-1, -1);
		pEdit->ReplaceSel(m_msgShow);
	}
	else
	{
		SetDlgItemText(IDC_INPUT, lpmsg);
		CString msg;
		msg.Format("发送失败，错误： %s", Valley_GetErrDesc(ec));
		AfxMessageBox(msg);
	} 
}


void CMSGDLG::OnBnClickedSend()
{
	CString strmsg; 
	CString toUserID;

	GetDlgItemTextA(IDC_INPUT, strmsg);
	GetDlgItemTextA(IDC_RECVER, toUserID);
	if (toUserID.IsEmpty())
	{
		AfxMessageBox("接收者不能为空");
		return;
	}

	if (strmsg.IsEmpty())
	{
		AfxMessageBox("消息内容不能为空");
		return;
	}

	Valley_SendMsgr(m_xRtcGlobalRoom, msgtypeText, (const char*)strmsg, strmsg.GetLength() + 1, nullptr, (const char*)toUserID);

	SetDlgItemText(IDC_INPUT, "");
}
