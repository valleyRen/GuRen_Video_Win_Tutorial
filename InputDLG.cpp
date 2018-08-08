// InputDLG.cpp : implementation file
//

#include "stdafx.h"
#include "TestxRTCDemo.h"
#include "InputDLG.h"
#include "afxdialogex.h"


// CInputDLG dialog

IMPLEMENT_DYNAMIC(CInputDLG, CDialogEx)

CInputDLG::CInputDLG(CWnd* pParent /*=NULL*/)
: CDialogEx(CInputDLG::IDD, pParent)
, m_inputText(_T(""))
{

}

CInputDLG::~CInputDLG()
{
}

void CInputDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT, m_inputText);
	DDV_MaxChars(pDX, m_inputText, 255);
}


BEGIN_MESSAGE_MAP(CInputDLG, CDialogEx)
	ON_BN_CLICKED(IDOK, &CInputDLG::OnBnClickedOk)
END_MESSAGE_MAP()


// CInputDLG message handlers
BOOL CInputDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_caption);

	if (!m_title.IsEmpty())
	{
		SetDlgItemText(IDC_TITLE, m_title);
	}

	return true;
}

void CInputDLG::OnBnClickedOk()
{
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
