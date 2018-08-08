#include "StdAfx.h"
#include "TestxVideoChannel.h"
#include "TestxRTCDemoDlg.h"
 
   
CTestxVideoChannel::CTestxVideoChannel(void)
{
	m_pxRTCEngine = NULL ;
	m_UserWndCount = 0; 
	m_hMyWnd = NULL ; 
}

CTestxVideoChannel::~CTestxVideoChannel(void)
{
}


void CTestxVideoChannel::Test( const char *pzRoomName, const char* uUserID ) 
{
	if ( m_pxRTCEngine != NULL )
	{
		return ;
	}
	 
	m_pxRTCEngine = Valley_CreateChannel(true); 
	Valley_SetLocalVideo(m_pxRTCEngine, m_hMyWnd);
	Valley_SetVideoProfile(m_pxRTCEngine, VIDEO_CAPTURE_TYPE_4X3_640);
	 
	Valley_Login(m_pxRTCEngine, pzRoomName, uUserID);

}

void CTestxVideoChannel::Close() 
{
	if ( m_pxRTCEngine == NULL )
	{
		return ;
	}

	for (int index = 0; index < m_UserWndCount; index++)
	{
		if (!m_hUserWnd[index].uUserID.empty())
		{
			Valley_RemoveUserVideo(m_pxRTCEngine, m_hUserWnd[index].uUserID.c_str());
			m_hUserWnd[index].uUserID.clear();
		}

		m_hUserWnd[index].hWnd = nullptr;
	}

	Valley_Logout(m_pxRTCEngine); 
	Valley_Release(m_pxRTCEngine);
	m_pxRTCEngine = NULL ; 
	 

	m_hMyWnd = nullptr;
	m_UserWndCount = 0;
	m_bFoundUserLeave = false;
}

bool CTestxVideoChannel::IsLogined()
{
	if (nullptr == m_pxRTCEngine)
		return false;

	return (LOGIN_STATUS_LOGINED == Valley_GetLoginStatus(m_pxRTCEngine));
}

void CTestxVideoChannel::UserEnter(const char* uUserID ) 
{ 
	
	int index = 0;
	for (; index < m_UserWndCount; index++)
	{
		if (m_hUserWnd[index].uUserID.empty())
			break;
	}

	if (index == m_UserWndCount)
		return;
	 
	m_hUserWnd[index].uUserID = uUserID; 

	int ec = Valley_SetUserVideo(m_pxRTCEngine, uUserID, m_hUserWnd[index].hWnd);

	CString strMsg;
	strMsg.Format("%s UserEnter ec=%d, wndIndex=%d\n", (int)uUserID, ec, index);
	OutputDebugString(strMsg);

}

void CTestxVideoChannel::UserLeave( const char* uUserID ) 
{ 
	int index = 0;
	for (; index < m_UserWndCount; index++)
	{
		if (0 == m_hUserWnd[index].uUserID.compare(uUserID))
			break;
	}

	if (index == m_UserWndCount)
		return;
	 
	Valley_RemoveUserVideo(m_pxRTCEngine, uUserID);

	m_hUserWnd[index].uUserID.clear(); 
	m_bFoundUserLeave = true;

	CString strMsg;
	strMsg.Format("%s UserLeave WndIndex=%d\n", uUserID, index);
	OutputDebugString(strMsg);
} 

void CTestxVideoChannel::OnTimer()
{ 
	objBaseEvent* e = Valley_GetEvent(m_pxRTCEngine);
	if (NULL != e)
	{
		if (RTC_RESP_LOGINED == e->evttype)
		{
			m_dlg->OnEnterRoomResult(e);
		}
		else if (RTC_NTF_USER_ENTER == e->evttype)
		{
			UserEnter(((objNtfUserEnter*)e)->userid);
		}
		else if (RTC_NTF_USER_LEAVE == e->evttype)
		{
			UserLeave(((objNtfUserLeave*)e)->userid);
		}


		Valley_ReleaseEvent(e);
	} 

	if (m_bFoundUserLeave)
	{
		InvalidateRect(GetParent(m_hMyWnd), NULL, true);
		m_bFoundUserLeave = false;
	}
}