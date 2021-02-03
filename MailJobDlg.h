// MailJobDlg.h : header file
//

#if !defined(AFX_MAILJOBDLG_H__B657710A_666F_49EC_B6DF_88B74AD0AC3A__INCLUDED_)
#define AFX_MAILJOBDLG_H__B657710A_666F_49EC_B6DF_88B74AD0AC3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMailJobDlg dialog
#define WM_UPTRUEMESSAGE WM_USER+30
#define WM_UPFALSEMESSAGE WM_USER+31

#include "AdoInterface.h"
#include "PictureEx.h"	// Added by ClassView

class CMailJobDlg : public CDialog
{
// Construction
public:
	void GetSendMail();
	CMailJobDlg(CWnd* pParent = NULL);	// standard constructor
	CAdoInterface m_ado;
	TCHAR szPath[MAX_PATH],drive[MAX_PATH],dir[MAX_PATH],fname[MAX_PATH],ext[MAX_PATH],yubaodll[MAX_PATH],zhizuodll[MAX_PATH],listPath[MAX_PATH];
	char oracleini[MAX_PATH];
	char m_ipaddr[256];
	char m_username[256];
	char m_password[256];
	char m_dir[256];
	char m_ext[256];
	// Dialog Data
	//{{AFX_DATA(CMailJobDlg)
	enum { IDD = IDD_MAILJOB_DIALOG };
	CListBox	m_list3;
	CProgressCtrl	m_pro;
	CPictureEx	m_Picture;
	CListBox	m_list2;
	CCheckListBox 	m_list;
	CString	m_time;
	CString	m_bianhao;
	CString	m_path;
	UINT	m_count;
	int		m_allcount;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMailJobDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMailJobDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReadySend();
	afx_msg void OnMailBtn();
	afx_msg void OnCheckBtn();
	afx_msg void OnDischeckBtn();
	afx_msg void OnSelcancelList1();
	afx_msg void OnCopy();
	afx_msg void OnOpenLog();
	afx_msg void OnOpenPic();
	afx_msg LRESULT MyUpTrue(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT MyUpFalse(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAILJOBDLG_H__B657710A_666F_49EC_B6DF_88B74AD0AC3A__INCLUDED_)
