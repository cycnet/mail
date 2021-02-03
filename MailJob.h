// MailJob.h : main header file for the MAILJOB application
//

#if !defined(AFX_MAILJOB_H__1BBD80D6_8E76_4E70_95B2_3D07014EE3C1__INCLUDED_)
#define AFX_MAILJOB_H__1BBD80D6_8E76_4E70_95B2_3D07014EE3C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMailJobApp:
// See MailJob.cpp for the implementation of this class
//

class CMailJobApp : public CWinApp
{
public:
	CMailJobApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMailJobApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMailJobApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAILJOB_H__1BBD80D6_8E76_4E70_95B2_3D07014EE3C1__INCLUDED_)
