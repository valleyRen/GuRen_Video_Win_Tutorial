
// TestxRTCDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestxRTCDemo.h"
#include "TestxRTCDemoDlg.h"
#include "InputDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "sdk/libs/ValleyRtcAPI_C.lib")  

CTestxRTCDemoDlg::CTestxRTCDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TESTXRTCDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	 
	char appfolder[512];
	DWORD dwEnd = GetModuleFileNameA(NULL, appfolder, 512);
	appfolder[dwEnd] = 0;
	*(strrchr(appfolder, '\\')) = 0;
	strcat(appfolder, "\\wokfolder");


	//初始化语音SDK, 整个进程该函数只需要调用一次， 当不用SDK功能了，必须调用CleanSDK完成内存资源回收功能
	//appfolder 工作路径，必须要填，具有读写权限，用于存储日志、语音消息文件、临时文件等的工作路径  
	const char* localconfig = nullptr;// "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><localversion>1</localversion></root>";
	Valley_InitSDK(appfolder, localconfig);


	//创建房间操作实例，必须在InitSDK后调用，否则会失败，创建出来的实例只能用于登录一个房间，如果要登录多个房间，可以创建多个实例来完成 
	m_xRtcGlobalRoom = Valley_CreateChannel(false);
//	Valley_SetChannelAttr(m_xRtcGlobalRoom, RESERVE_CHANNEL_ATTR_CONFIG, "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><mcu><channel>s-10.80.64.56:8002</channel></mcu></root>");
	Valley_EnableInterface(m_xRtcGlobalRoom, IID_RTCMSGR); 
}

CTestxRTCDemoDlg::~CTestxRTCDemoDlg()
{
	m_xRtcVideoRoom.Close();

	Valley_Logout(m_xRtcGlobalRoom);
	Valley_Release(m_xRtcGlobalRoom);

	Valley_CleanSDK();
}

void CTestxRTCDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MY, m_MyPlayer);
	DDX_Control(pDX, IDC_STATIC_1, m_Player1);
	DDX_Control(pDX, IDC_STATIC_2, m_Player2);
	DDX_Control(pDX, IDC_STATIC_3, m_Player3);
	DDX_Control(pDX, IDC_EDIT_ROOM, m_szRoomID);
	DDX_Control(pDX, IDC_EDIT_NAME, m_szUserID);
}

BEGIN_MESSAGE_MAP(CTestxRTCDemoDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON() 
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestxRTCDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestxRTCDemoDlg::OnBnClickedButton2) 
	ON_WM_CLOSE() 
	ON_COMMAND_RANGE(ID_SHOWMSG, ID_SHOWMSG + 100, &CTestxRTCDemoDlg::OnMenuCommand)
	ON_BN_CLICKED(IDC_IM_INIT, &CTestxRTCDemoDlg::OnBnClickedImInit)
END_MESSAGE_MAP()


// CTestxRTCDemoDlg 消息处理程序

BOOL CTestxRTCDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	 
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_bDoTest = FALSE ;
	//m_MyPlayer.SetWindowPos(NULL, 24, 68, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	//m_Player1.SetWindowPos(NULL, 24 + 280, 68, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	//m_Player2.SetWindowPos(NULL, 24, 68 + 200, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	//m_Player3.SetWindowPos(NULL, 24 + 280, 68 + 200, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);

	m_szRoomID.SetWindowText( "345") ;

	char czUserName[64] ;

	itoa( GetTickCount() & 0x7ff, czUserName, 10 ) ;
	m_szUserID.SetWindowText( czUserName ) ;

	// TODO: 在此添加额外的初始化代码
	SetTimer(100, 200, NULL); 

	CMenu mu;
	mu.LoadMenuA(IDR_MENU1);

	int ec = GetLastError();

	::SetMenu(m_hWnd, mu.GetSafeHmenu());
	mu.Detach();

	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CTestxRTCDemoDlg::OnTimer(UINT_PTR timerID)
{
	if (100 == timerID)
	{
		objBaseEvent* e = Valley_GetEvent(m_xRtcGlobalRoom);
		if (NULL != e)
		{
			if (e->evttype > RTC_EVT_NTF_FROM)
			{
				OnGlobalNotify(e);
			}
			else
			{
				OnGlobalRespond(e);
			} 
			Valley_ReleaseEvent(e); 
		}


		m_xRtcVideoRoom.OnTimer();
	}
}

void CTestxRTCDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
 
		CDialog::OnSysCommand(nID, lParam); 
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestxRTCDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestxRTCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestxRTCDemoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( m_bDoTest )
	{
		return ; 
	}

	m_bDoTest = TRUE  ;

	char szRoomID[32] ;
	char szUserID[32] ;

	uint32_t uRoomLength = ::GetWindowText( m_szRoomID.GetSafeHwnd(), szRoomID, 30 ) ;
	uint32_t uUserIDLength = ::GetWindowText( m_szUserID.GetSafeHwnd(), szUserID, 30  ) ;
	if ( uRoomLength ==0 || uUserIDLength == 0 )
	{
		return ;
	}
	 
	m_xRtcVideoRoom.AddMyWnd(m_MyPlayer.GetSafeHwnd());
	m_xRtcVideoRoom.AddUserWnd(m_Player1.GetSafeHwnd());
	m_xRtcVideoRoom.AddUserWnd(m_Player2.GetSafeHwnd());
	m_xRtcVideoRoom.AddUserWnd(m_Player3.GetSafeHwnd());

	CString str;
	str.Format("进入房间 %s ...", szRoomID);
	ShowMessage(str, 0);

	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	m_szRoomID.EnableWindow(FALSE);
	m_szUserID.EnableWindow(FALSE);

	m_xRtcVideoRoom.m_dlg = this;
	m_xRtcVideoRoom.Test(szRoomID, szUserID); 
}


void CTestxRTCDemoDlg::OnBnClickedButton2()
{ 
	m_xRtcVideoRoom.Close();
	m_bDoTest = false; 

	m_szRoomID.EnableWindow(TRUE);
	m_szUserID.EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	ShowMessage("已经退出房间", 0);
	Invalidate();
}


void CTestxRTCDemoDlg::OnEnterRoomResult(objBaseEvent* e)
{
	objRespLogin* resp = (objRespLogin*)e;
	if (0 != resp->ec)
	{
		m_szRoomID.EnableWindow(TRUE);
		m_szUserID.EnableWindow(TRUE);
		m_xRtcVideoRoom.Close();
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		ShowMessage("进入房间失败", resp->ec);
	}
	else
	{
		ShowMessage("进入房间成功", 0);
	}
}


void CTestxRTCDemoDlg::OnCancel()
{
	m_xRtcVideoRoom.Close();

	if (!m_xRtcGlobalRoom)
	{
		Valley_Logout(m_xRtcGlobalRoom);
		m_xRtcGlobalRoom = nullptr;
	}


	CDialog::OnCancel();
}
 

void CTestxRTCDemoDlg::OnClose()
{ 
	//m_xRtcVideoRoom.Close();
	//Valley_Logout(m_xRtcGlobalRoom);
	CDialog::OnClose();
}

void CTestxRTCDemoDlg::ShowMessage(const CString& msg, int ec)
{
	if (0 != ec)
	{
		CString msgErr;
		msgErr.Format("%s, ec=%d (%s)", msg, ec, Valley_GetErrDesc(ec));
		SetDlgItemText(IDC_STATUS, msgErr);
	} 
	else
	{
		//AfxMessageBox(msg);
		SetDlgItemText(IDC_STATUS, msg); 
	}
}


void CTestxRTCDemoDlg::OnGlobalRespond(objBaseEvent* e)
{ 
	if (RTC_RESP_LOGINED == e->evttype)
	{
		objRespLogin* resp = (objRespLogin*)e;

		if (ERR_SUCCEED == resp->ec)
		{ 
			ShowMessage("初始化成功", resp->ec);
			GetDlgItem(IDC_IM_INIT)->EnableWindow(FALSE);
		}
		else
		{ 
			ShowMessage("初始化失败", resp->ec);
		}
		 
	}
	else if (RTC_RESP_SEND_MSG == e->evttype)
	{
		objRespMsg* resp = (objRespMsg*)e;
		if (msgtypeText == resp->msgtype)
		{ 
			m_msgdlg->ShowSendResult(resp->touserid, resp->data, resp->ec); 
		}
		else if (msgtypeCmd == resp->msgtype)
		{ 
			if (ERR_SUCCEED == resp->ec)
			{
				ShowMessage("发送邀请成功");
			}
			else
			{
				ShowMessage("发送邀请失败", resp->ec);
			}
		}
	}
}


void CTestxRTCDemoDlg::OnGlobalNotify(objBaseEvent* e)
{ 
	if (RTC_NTF_RECV_MSG == e->evttype)
	{
		objNtfMsg* msg = (objNtfMsg*)e;
		if (msgtypeCmd == msg->msgtype)
		{
			if (0 == strcmp(msg->data, "Invite"))
			{
				if (m_xRtcVideoRoom.IsLogined())
				{
					CString str;
					str.Format("收到来自%s进入房间%s的邀请， 但是你已经在房间中了", msg->fromuserid, msg->token);
					::MessageBoxA(m_hWnd, str, "收到邀请", MB_OK);
				}
				else
				{
					CString str;
					str.Format("收到来自%s进入房间%s的邀请， 是否进入房间?", msg->fromuserid, msg->token); 
					if (IDYES == ::MessageBoxA(m_hWnd, str, "收到邀请", MB_YESNO))
					{
						m_szRoomID.SetWindowTextA(msg->token);
						Valley_SendMsgr(m_xRtcGlobalRoom, msgtypeCmd, "enter", 6, msg->token, msg->fromuserid);
						OnBnClickedButton1();
					}
					else
					{
						Valley_SendMsgr(m_xRtcGlobalRoom, msgtypeCmd, "refuse", 7, msg->token, msg->fromuserid);
					}
				} 
			}
			else if (0 == strcmp(msg->data, "enter"))
			{
				if (m_xRtcVideoRoom.IsLogined())
				{
					CString str;
					str.Format("收到来自%s进入房间%s的应答， 但是你已经在房间中了", msg->fromuserid, msg->token);
					::MessageBoxA(m_hWnd, str, "收到应答", MB_OK);
				}
				else
				{
					CString str;
					str.Format("收到来自%s进入房间%s的应答， 进入房间", msg->fromuserid, msg->token);
					ShowMessage(str, 0); 
					m_szRoomID.SetWindowTextA(msg->token);
					OnBnClickedButton1();
				}
			}
			else if (0 == strcmp(msg->data, "refuse"))
			{ 
				CString str;
				str.Format("%s拒绝了你的邀请", msg->fromuserid);
				ShowMessage(str, 0);
			}
		} 
		else if (msgtypeText == msg->msgtype)
		{
			if (!m_msgdlg)
			{
				CMSGDLG dlg(m_xRtcGlobalRoom, this);
				m_msgdlg = &dlg;
				m_msgdlg->ShowMessage(msg->fromuserid, msg->data);
				dlg.DoModal();
				m_msgdlg = NULL; 
			}
			else
			{
				m_msgdlg->ShowMessage(msg->fromuserid, msg->data);
			} 
		}
	}
}
 

void CTestxRTCDemoDlg::OnBnClickedImInit()
{
	if (LOGIN_STATUS_NONE != Valley_GetLoginStatus(m_xRtcGlobalRoom))
		return;

	CString szUserID;
	CString szAuthoKey;

	m_szUserID.GetWindowText(szUserID);
	 
	if (szUserID.IsEmpty())
	{
		ShowMessage("用户ID不能为空");
		return;
	}
	 

	//GetDlgItemText(IDC_AUTHO_KEY, szAuthoKey);  
	//szAuthoKey = "5a00e500d503f7f6ollnTdu";

	//SetAuthoKey(szAuthoKey);
	GetDlgItem(IDC_IM_INIT)->EnableWindow(FALSE);
	 
	int ec = Valley_Login(m_xRtcGlobalRoom, "98", (const char*)szUserID); 
	if (ERR_SUCCEED != ec)
	{
		ShowMessage("初始化失败", ec);
	}
	else
	{
		GetDlgItem(IDC_IM_INIT)->EnableWindow(FALSE);
	}
}


void CTestxRTCDemoDlg::OnMenuCommand(UINT nID)
{
	switch (nID)
	{ 
	case ID_SHOWMSG:
	{
		if (LOGIN_STATUS_LOGINED != Valley_GetLoginStatus(m_xRtcGlobalRoom))
		{
			AfxMessageBox("使用该功能前，IM必须初始化");
			return;
		}

		CMSGDLG dlg(m_xRtcGlobalRoom, this);
		m_msgdlg = &dlg;
		dlg.DoModal();
		m_msgdlg = NULL;
	}
	break;
	case ID_INVITE:
	{
		if (LOGIN_STATUS_LOGINED != Valley_GetLoginStatus(m_xRtcGlobalRoom))
		{
			AfxMessageBox("使用该功能前，IM必须初始化");
			return;
		}
		 
		CInputDLG inp;
		inp.m_title = "Input 对方userid";
		if (IDOK != inp.DoModal())
			return;

		if (inp.m_inputText.IsEmpty())
		{
			::MessageBoxA(m_hWnd, "对方的ID不能为空", "邀请", 0);
			return;
		}

		CString szRoomKey;
		m_szRoomID.GetWindowText(szRoomKey);
		if (szRoomKey.IsEmpty())
		{
			::MessageBoxA(m_hWnd, "房间号不能为空", "邀请", 0);
			return;
		} 

		const char* to_userid = inp.m_inputText;

		int ec = Valley_SendMsgr(m_xRtcGlobalRoom, msgtypeCmd, "Invite", 7, (const char*)szRoomKey, to_userid);

		if (ERR_SUCCEED != ec) 
			ShowMessage("发邀请失败", ec); 
	}
	break;
	default:
		break;
	}
}


