#pragma once


// CVideoWnd

class CVideoWnd : public CStatic
{
	DECLARE_DYNAMIC(CVideoWnd)

public:
	CVideoWnd();
	virtual ~CVideoWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();  
};


