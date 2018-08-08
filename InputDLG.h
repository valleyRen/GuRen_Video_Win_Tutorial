#pragma once


// CInputDLG dialog

class CInputDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CInputDLG)

public:
	CInputDLG(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputDLG();

	// Dialog Data
	enum { IDD = IDD_INPUT_TEST };
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_inputText;
	CString m_title;
	CString m_caption;
};
