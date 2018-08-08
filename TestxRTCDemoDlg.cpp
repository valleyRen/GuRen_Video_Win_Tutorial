
// TestxRTCDemoDlg.cpp : ʵ���ļ�
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


	//��ʼ������SDK, �������̸ú���ֻ��Ҫ����һ�Σ� ������SDK�����ˣ��������CleanSDK����ڴ���Դ���չ���
	//appfolder ����·��������Ҫ����ж�дȨ�ޣ����ڴ洢��־��������Ϣ�ļ�����ʱ�ļ��ȵĹ���·��  
	const char* localconfig = nullptr;// "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><localversion>1</localversion></root>";
	Valley_InitSDK(appfolder, localconfig);


	//�����������ʵ����������InitSDK����ã������ʧ�ܣ�����������ʵ��ֻ�����ڵ�¼һ�����䣬���Ҫ��¼������䣬���Դ������ʵ������� 
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


// CTestxRTCDemoDlg ��Ϣ�������

BOOL CTestxRTCDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	 
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_bDoTest = FALSE ;
	//m_MyPlayer.SetWindowPos(NULL, 24, 68, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	//m_Player1.SetWindowPos(NULL, 24 + 280, 68, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	//m_Player2.SetWindowPos(NULL, 24, 68 + 200, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
	//m_Player3.SetWindowPos(NULL, 24 + 280, 68 + 200, 240, 180, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);

	m_szRoomID.SetWindowText( "345") ;

	char czUserName[64] ;

	itoa( GetTickCount() & 0x7ff, czUserName, 10 ) ;
	m_szUserID.SetWindowText( czUserName ) ;

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetTimer(100, 200, NULL); 

	CMenu mu;
	mu.LoadMenuA(IDR_MENU1);

	int ec = GetLastError();

	::SetMenu(m_hWnd, mu.GetSafeHmenu());
	mu.Detach();

	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestxRTCDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestxRTCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestxRTCDemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	str.Format("���뷿�� %s ...", szRoomID);
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
	ShowMessage("�Ѿ��˳�����", 0);
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
		ShowMessage("���뷿��ʧ��", resp->ec);
	}
	else
	{
		ShowMessage("���뷿��ɹ�", 0);
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
			ShowMessage("��ʼ���ɹ�", resp->ec);
			GetDlgItem(IDC_IM_INIT)->EnableWindow(FALSE);
		}
		else
		{ 
			ShowMessage("��ʼ��ʧ��", resp->ec);
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
				ShowMessage("��������ɹ�");
			}
			else
			{
				ShowMessage("��������ʧ��", resp->ec);
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
					str.Format("�յ�����%s���뷿��%s�����룬 �������Ѿ��ڷ�������", msg->fromuserid, msg->token);
					::MessageBoxA(m_hWnd, str, "�յ�����", MB_OK);
				}
				else
				{
					CString str;
					str.Format("�յ�����%s���뷿��%s�����룬 �Ƿ���뷿��?", msg->fromuserid, msg->token); 
					if (IDYES == ::MessageBoxA(m_hWnd, str, "�յ�����", MB_YESNO))
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
					str.Format("�յ�����%s���뷿��%s��Ӧ�� �������Ѿ��ڷ�������", msg->fromuserid, msg->token);
					::MessageBoxA(m_hWnd, str, "�յ�Ӧ��", MB_OK);
				}
				else
				{
					CString str;
					str.Format("�յ�����%s���뷿��%s��Ӧ�� ���뷿��", msg->fromuserid, msg->token);
					ShowMessage(str, 0); 
					m_szRoomID.SetWindowTextA(msg->token);
					OnBnClickedButton1();
				}
			}
			else if (0 == strcmp(msg->data, "refuse"))
			{ 
				CString str;
				str.Format("%s�ܾ����������", msg->fromuserid);
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
		ShowMessage("�û�ID����Ϊ��");
		return;
	}
	 

	//GetDlgItemText(IDC_AUTHO_KEY, szAuthoKey);  
	//szAuthoKey = "5a00e500d503f7f6ollnTdu";

	//SetAuthoKey(szAuthoKey);
	GetDlgItem(IDC_IM_INIT)->EnableWindow(FALSE);
	 
	int ec = Valley_Login(m_xRtcGlobalRoom, "98", (const char*)szUserID); 
	if (ERR_SUCCEED != ec)
	{
		ShowMessage("��ʼ��ʧ��", ec);
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
			AfxMessageBox("ʹ�øù���ǰ��IM�����ʼ��");
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
			AfxMessageBox("ʹ�øù���ǰ��IM�����ʼ��");
			return;
		}
		 
		CInputDLG inp;
		inp.m_title = "Input �Է�userid";
		if (IDOK != inp.DoModal())
			return;

		if (inp.m_inputText.IsEmpty())
		{
			::MessageBoxA(m_hWnd, "�Է���ID����Ϊ��", "����", 0);
			return;
		}

		CString szRoomKey;
		m_szRoomID.GetWindowText(szRoomKey);
		if (szRoomKey.IsEmpty())
		{
			::MessageBoxA(m_hWnd, "����Ų���Ϊ��", "����", 0);
			return;
		} 

		const char* to_userid = inp.m_inputText;

		int ec = Valley_SendMsgr(m_xRtcGlobalRoom, msgtypeCmd, "Invite", 7, (const char*)szRoomKey, to_userid);

		if (ERR_SUCCEED != ec) 
			ShowMessage("������ʧ��", ec); 
	}
	break;
	default:
		break;
	}
}


