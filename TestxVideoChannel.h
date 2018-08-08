#pragma once

#include <stdint.h>
#include <string>
#include "sdk/include/ValleyRtcAPI_C.h"

 
class CTestxRTCDemoDlg;
class CTestxVideoChannel
{
	struct tUserWnd
	{
		std::string uUserID;
		HWND	    hWnd = nullptr; 
	};
public:
	CTestxVideoChannel(void);
	virtual ~CTestxVideoChannel(void);

public:

	void AddMyWnd(HWND hWnd )
	{
		m_hMyWnd = hWnd ;
	}

	void AddUserWnd(HWND hWnd )
	{
		m_hUserWnd[m_UserWndCount].hWnd = hWnd; 
		m_UserWndCount++;
	}


	void OnTimer(); 
public:
	void Test(const char *pzRoomName, const char* uUserID) ;
	void Close() ;

	void UserEnter( const char* uUserID ) ;
	void UserLeave( const char* uUserID ) ;
	bool IsLogined();
protected:
	handle_t     m_pxRTCEngine;   	
	HWND		 m_hMyWnd = nullptr;
	tUserWnd	 m_hUserWnd[3];
	int          m_UserWndCount = 0;
	bool         m_bFoundUserLeave = false;

public:
	CTestxRTCDemoDlg* m_dlg = nullptr;
};
