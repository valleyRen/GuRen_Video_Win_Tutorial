// VideoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TestxRTCDemo.h"
#include "VideoWnd.h"


// CVideoWnd

IMPLEMENT_DYNAMIC(CVideoWnd, CStatic)

CVideoWnd::CVideoWnd()
{

}

CVideoWnd::~CVideoWnd()
{
}


BEGIN_MESSAGE_MAP(CVideoWnd, CStatic)
	ON_WM_PAINT() 
END_MESSAGE_MAP()



// CVideoWnd message handlers

 
void CVideoWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect r;
	GetClientRect(&r);
	dc.FillSolidRect(r, 0x0);
}
 
 