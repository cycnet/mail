// MailJobDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MailJob.h"
#include "MailJobDlg.h"
#include "SendMail.h"

#include <iostream> 
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //如果没有这行，会出现link错误
    
#pragma comment(lib, "comsupp.lib") 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace std;
HANDLE hMutex;


void split(const char * str,const char * deli, vector<string> *list);
// CAboutDlg dialog used for App About
vector<string> mpathlist;
vector<string> mailsendlist;
vector<string> sendlist;
vector<int> mailcount;


DWORD WINAPI SendMail(LPVOID LParam);


struct MailStruct
{
	CMailJobDlg *dlg;
};
bool resendmail(string parm,string s4,string s5,string s6,string s8,string filename,MailStruct *m_mail,string files,ofstream logfile,int count);
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMailJobDlg dialog

CMailJobDlg::CMailJobDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMailJobDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMailJobDlg)
	m_time = _T("");
	m_bianhao = _T("");
	m_path = _T("");
	m_count = 0;
	m_allcount = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMailJobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMailJobDlg)
	DDX_Control(pDX, IDC_LIST2, m_list3);
	DDX_Control(pDX, IDC_PROGRESS1, m_pro);
	DDX_Control(pDX, IDC_GIFFIRST, m_Picture);
	DDX_Control(pDX, IDC_LIST3, m_list2);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_EDIT3, m_time);
	DDX_Text(pDX, IDC_EDIT2, m_bianhao);
	DDX_Text(pDX, IDC_EDIT4, m_path);
	DDX_Text(pDX, IDC_EDIT5, m_count);
	DDV_MinMaxUInt(pDX, m_count, 0, 10000000);
	DDX_Text(pDX, IDC_ALLCOUNT, m_allcount);
	DDV_MinMaxInt(pDX, m_allcount, 0, 1000000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMailJobDlg, CDialog)
	//{{AFX_MSG_MAP(CMailJobDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_READY_SEND, OnReadySend)
	ON_BN_CLICKED(IDC_MAIL_BTN, OnMailBtn)
	ON_BN_CLICKED(IDC_CHECK_BTN, OnCheckBtn)
	ON_BN_CLICKED(IDC_DISCHECK_BTN, OnDischeckBtn)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelcancelList1)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_OPEN_LOG, OnOpenLog)
	ON_BN_CLICKED(IDC_OPEN_PIC, OnOpenPic)
	ON_MESSAGE(WM_UPTRUEMESSAGE,MyUpTrue)
	ON_MESSAGE(WM_UPFALSEMESSAGE,MyUpFalse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMailJobDlg message handlers

BOOL CMailJobDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CTime tmDay=CTime::GetCurrentTime();
	char timestr[256];
	sprintf(timestr,"%02d%02d%02d%02d",tmDay.GetYear()%100,tmDay.GetMonth(),tmDay.GetDay(),tmDay.GetHour());
	m_time=timestr;

	SetWindowText("邮件群发");

	GetModuleFileName( NULL, szPath, MAX_PATH );
	_tsplitpath( szPath,drive,dir,fname,ext );
	strcpy( szPath, drive );
	strcat( szPath, dir );

	sprintf(listPath,"%smail_list\\list.txt",szPath);

	
	::sprintf(oracleini,"%s\\oracle.ini",szPath);
	GetPrivateProfileString("oracle","ipaddr",0,m_ipaddr,sizeof(m_ipaddr),oracleini);
	GetPrivateProfileString("oracle","username",0,m_username,sizeof(m_username),oracleini);
	GetPrivateProfileString("oracle","password",0,m_password,sizeof(m_password),oracleini);
	GetPrivateProfileString("oracle","dir",0,m_dir,sizeof(m_dir),oracleini);
	GetPrivateProfileString("oracle","ext",0,m_ext,sizeof(m_ext),oracleini);
	
	

	GetSendMail();

	m_path=m_dir;
	UpdateData(false);

	hMutex=CreateMutex(NULL,FALSE,NULL);

	//m_list.SetCheckStyle(BS_
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMailJobDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMailJobDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMailJobDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CMailJobDlg::OnReadySend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if(m_bianhao.GetLength()==0 || m_time.GetLength() == 0)
	{
		MessageBox("请填写编号和时间");
		return;
	}

	_variant_t var_t;
	char sqlsr[256];
//	char connstr[256];
	CString title,mailfile;
	char showStr[256];
	char filename[MAX_PATH];
	char mailfilefull[MAX_PATH];

	
//	sprintf(connstr,"Provider=oraOLEDB.Oracle;Data Source=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=1521))(CONNECT_DATA=(SERVICE_NAME=SZQX)));User ID=%s;Password=%s;Extended Properties=PLSQLSet=1;",m_ipaddr,m_username,m_password);
/*	if(!m_ado.ConnecDataLibrary(connstr,m_username,m_password,adModeUnknown))
	{
		MessageBox("请检查数据库设置");	
		return;
	}
	sprintf(sqlsr,"SELECT * FROM T_TYPC_CONFIG WHERE FILENAME is not null order by id");*/


	m_list.ResetContent();
	mpathlist.clear();
	mailcount.clear();
	m_allcount=0;

	
	vector<string> mlist;
	ifstream in(listPath);  
	string line;  
	
	if(in) // 有该文件  
	{  
	//	_RecordsetPtr rs=m_ado.Execute(sqlsr,adCmdText);
	//	while(!rs->adoEOF)
		while (getline (in, line)) // line中不包括每行的换行符  
		{
			/*var_t =rs->GetCollect("showtitle");
			if(var_t.vt != VT_NULL)
			{
				title=(char *)_bstr_t(var_t);
			}*/
			mlist.clear();
			split(line.c_str(),",",&mlist);
			char tmp[256],mailfile_c[256],pdffile[256],mailfilefull[MAX_PATH];
		/*	
			sprintf(pdffile,"%spdf\\%s%s.pdf",szPath,mlist[2].c_str(),m_time);
			sprintf(mailfilefull,"%smail\\%s.txt",szPath,mlist[0].c_str());
			mpathlist.push_back(pdffile);*/

			sprintf(mailfile_c,"%s.txt",mlist[0].c_str());
			mailfile=mailfile_c;
			title=mlist[2].c_str();
		//	var_t =rs->GetCollect("mailsmtp");
			//if(var_t.vt != VT_NULL)
		//	{
				//mailfile=(char *)_bstr_t(var_t);

				sprintf(filename,"%s\\%s%s%s.%s",m_path,title,m_bianhao,m_time,m_ext);
				sprintf(mailfilefull,"%smail_list\\%s",szPath,mailfile);
				if (PathFileExists(filename))
				{
					if(PathFileExists(mailfilefull))
					{
						sprintf(showStr,"项目:%s,  邮件参数:%s存在,  待发送文件,%s%s%s.%s 存在",title,mailfile,title,m_bianhao,m_time,m_ext);
						ifstream in(mailfilefull); 
						string line;  
						int tmpcount=0;
						while (getline(in, line)) // line中不包括每行的换行符  
						{   
							//m_allcount++;
							tmpcount++;
						}

						mailcount.push_back(tmpcount);

					}else
					{
						sprintf(showStr,"项目:%s,  邮件参数:%s不存在,  待发送文件,%s%s%s.%s 存在",title,mailfile,title,m_bianhao,m_time,m_ext);
						mailcount.push_back(0);
					}
				
				}else
				{
					if(PathFileExists(mailfilefull))
					{
						sprintf(showStr,"项目:%s,  邮件参数:%s存在,  待发送文件,%s%s%s.%s 不存在",title,mailfile,title,m_bianhao,m_time,m_ext);
					}else
					{
						sprintf(showStr,"项目:%s,  邮件参数:%s不存在,  待发送文件,%s%s%s.%s 不存在",title,mailfile,title,m_bianhao,m_time,m_ext);
					}
					mailcount.push_back(0);
				}
				mpathlist.push_back(filename);
				m_list.AddString(showStr);
			//}

		//	rs->MoveNext();
		}
	}else
	{
		m_list2.AddString("异常,配置文件未读入");
	}

	UpdateData(false);
}

void CMailJobDlg::OnMailBtn() 
{
	// TODO: Add your control notification handler code here
	WaitForSingleObject(hMutex,INFINITE);
	MailStruct *m_mail=new MailStruct;
	m_mail->dlg=this;
	ReleaseMutex(hMutex);
	HANDLE hThread=::CreateThread(NULL,0,SendMail,(void *)m_mail,0,NULL);
	CloseHandle(hThread);

}

DWORD WINAPI SendMail(LPVOID LParam)
{
	
	WaitForSingleObject(hMutex,INFINITE);
	MailStruct *m_mail_dlg = (MailStruct *)LParam;

	::SendMessage(m_mail_dlg->dlg->m_hWnd,WM_UPTRUEMESSAGE,0,0);
	char mailfullfile[MAX_PATH];
	vector<string> mlist;


	if(m_mail_dlg->dlg->m_bianhao.GetLength()==0 || m_mail_dlg->dlg->m_time.GetLength() == 0)
	{
		AfxMessageBox("请填写编号和时间");
		return 0;
	}


	char logpath[MAX_PATH];

	CTime tTime = CTime::GetCurrentTime();
	CString strTime = tTime.Format("%Y%m%d%H%M%S");
	sprintf(logpath,"%slog\\%s.log",m_mail_dlg->dlg->szPath,strTime);

	ofstream logfile(logpath);

    m_mail_dlg->dlg->m_list2.ResetContent();
	m_mail_dlg->dlg->m_count=0;

	_variant_t var_t;
	char sqlsr[256];
	char connstr[256];
	CString title,mailfile;
	char showStr[256];
	char filename[MAX_PATH];
	char files[MAX_PATH];
	char result[MAX_PATH];

	/*
	sprintf(connstr,"Provider=oraOLEDB.Oracle;Data Source=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=1521))(CONNECT_DATA=(SERVICE_NAME=SZQX)));User ID=%s;Password=%s;Extended Properties=PLSQLSet=1;",m_mail_dlg->dlg->m_ipaddr,m_mail_dlg->dlg->m_username,m_mail_dlg->dlg->m_password);
	if(!m_mail_dlg->dlg->m_ado.ConnecDataLibrary(connstr,m_mail_dlg->dlg->m_username,m_mail_dlg->dlg->m_password,adModeUnknown))
	{
		AfxMessageBox("请检查数据库设置");	
		return 0;
	}
	sprintf(sqlsr,"SELECT * FROM T_TYPC_CONFIG WHERE FILENAME is not null order by id");*/
	int rol=0;
	//int insert_count=0;

	vector<string> mlist1;
	ifstream in1(m_mail_dlg->dlg->listPath);  
	string line1;  
	char tmp[256],mailfile_c[MAX_PATH],mailfilefull[MAX_PATH];
	if(in1) // 有该文件  
	{
		//_RecordsetPtr rs=m_mail_dlg->dlg->m_ado.Execute(sqlsr,adCmdText);
		//while(!rs->adoEOF)
		while (getline (in1, line1)) // line中不包括每行的换行符  
		{
		
			mlist1.clear();
			split(line1.c_str(),",",&mlist1);
			
			memset(mailfilefull,0,MAX_PATH);
			memset(mailfile_c,0,MAX_PATH);
			sprintf(mailfile_c,"%s.txt",mlist1[0].c_str());
			mailfile=mailfile_c;
			title=mlist1[2].c_str();
			sprintf(mailfilefull,"%smail_list\\%s",m_mail_dlg->dlg->szPath,mailfile_c);


				sprintf(files,"%s%s%s.%s",title,m_mail_dlg->dlg->m_bianhao,m_mail_dlg->dlg->m_time,m_mail_dlg->dlg->m_ext);
				sprintf(filename,"%s\\%s%s%s.%s",m_mail_dlg->dlg->m_path,title,m_mail_dlg->dlg->m_bianhao,m_mail_dlg->dlg->m_time,m_mail_dlg->dlg->m_ext);

				bool chk=m_mail_dlg->dlg->m_list.GetCheck(rol);

				//Sleep(10);
			
		
				//m_mail_dlg->dlg->m_list2.AddString(filename);
				//m_mail_dlg->dlg->m_list2.AddString(mailfilefull);
				
			//	char test[256];
			//	sprintf(test,"%i",chk);
			//	m_mail_dlg->dlg->m_list2.AddString(test);
				if (PathFileExists(filename) && PathFileExists(mailfilefull)  && chk)
				{
					
					////////////////////////////////////////////////////
					ifstream in(mailfilefull);  
					string line;  
					char tmpchar[256];
					
				
					if(in) // 有该文件  
					{  

							CSendMail m_mail;
							sMailInfo smi;
							vector<string> receiver;
							while (getline (in, line)) // line中不包括每行的换行符  
							{  
								split(line.c_str(),",",&mlist);
								receiver.push_back(mlist[8].c_str());
							}

	
							strcpy(smi.m_pcUserName,mlist[1].c_str());
							strcpy(smi.m_pcUserPassWord,mlist[2].c_str());
							strcpy(smi.m_pcTitle,mlist[5].c_str());
							strcpy(smi.m_pcSenderName,mlist[4].c_str());
							strcpy(smi.m_pcSender,mlist[0].c_str());
							strcpy(smi.m_pcReceiver,receiver[0].c_str());
							strcpy(smi.m_pcIPName,mlist[3].c_str());
							strcpy(smi.m_pcIPAddr,"");
							char tmpbody[MAX_PATH];
							CTime tTime = CTime::GetCurrentTime();
							CString strTime = tTime.Format("%Y-%m-%d %H:%M:%S");

							
							char serial[MAX_PATH];
							char tmp[256];
							memset(serial,0,MAX_PATH);
							memset(tmp,0,256);
							srand((unsigned)time(0));
							int r = rand()%10;
							sprintf(serial,"%i",r);
							for(int i=0;i<r;i++)
							{
								int j = rand()%100+20;
								sprintf(tmp,"%i",j);
								strcat(serial,tmp);
							}


							sprintf(tmpbody,"%s,serial:%s<br>%s<BR><a href='cid:1.jpg'>下载附件 download Tc warning Pic</a><br><img src='cid:1.jpg'><br><a href='cid:1.jpg'>下载附件 download Tc warning Pic</a>",strTime,serial,mlist[6].c_str());
							strcpy(smi.m_pcBody,tmpbody);
							m_mail.AddFilePath(filename);
							if(m_mail.SendMail(smi,receiver))
							{
								sprintf(result,"项目:%s,%s存在,%s群发%i个邮箱成功",title,files,mlist[0].c_str(),mailcount[rol]);
								m_mail_dlg->dlg->m_list2.AddString(result);
								m_mail_dlg->dlg->m_count+=mailcount[rol];
								logfile << result << endl;
				
							}else
							{
								sprintf(result,"项目:%s,%s存在,%s群发%i个邮箱失败",title,files,mlist[0].c_str(),mailcount[rol]);
								m_mail_dlg->dlg->m_list2.AddString(result);
								logfile << result << endl;

								for(int i=0;i<sendlist.size();i++)
								{
									vector<string> msendlist;
									split(sendlist[i].c_str(),",",&msendlist);

									strcpy(smi.m_pcUserName,msendlist[1].c_str());
									strcpy(smi.m_pcUserPassWord,msendlist[2].c_str());
									strcpy(smi.m_pcTitle,mlist[5].c_str());
									strcpy(smi.m_pcSenderName,mlist[4].c_str());
									strcpy(smi.m_pcSender,msendlist[0].c_str());
									strcpy(smi.m_pcReceiver,receiver[0].c_str());
									strcpy(smi.m_pcIPName,msendlist[3].c_str());
									strcpy(smi.m_pcIPAddr,"");
									char tmpbody[MAX_PATH];
									CTime tTime = CTime::GetCurrentTime();
									CString strTime = tTime.Format("%Y-%m-%d %H:%M:%S");
									sprintf(tmpbody,"%s<BR>%s<br><img src='cid:1.jpg'>",mlist[6].c_str(),strTime);
									strcpy(smi.m_pcBody,tmpbody);
									m_mail.AddFilePath(filename);
									if(m_mail.SendMail(smi,receiver))
									{
										sprintf(result,"项目:%s,%s存在,第%i次%s群发%i个邮箱成功",title,files,i+1,msendlist[0].c_str(),mailcount[rol]);
										m_mail_dlg->dlg->m_list2.AddString(result);
										m_mail_dlg->dlg->m_count+=mailcount[rol];
										logfile << result << endl;
										break;
									}else
									{
										sprintf(result,"项目:%s,%s存在,第%i次%s群发%i个邮箱失败",title,files,i+1,msendlist[0].c_str(),mailcount[rol]);
										m_mail_dlg->dlg->m_list2.AddString(result);
										logfile << result << endl;
									}
								}
							}


						
						
					}  

					////////////////////////////////////////
				//}  
					
			
			}
		
				rol++;
		//	rs->MoveNext();
		}
	}
	logfile.close();
	::SendMessage(m_mail_dlg->dlg->m_hWnd,WM_UPFALSEMESSAGE,0,0);
	ReleaseMutex(hMutex);
	MessageBox(m_mail_dlg->dlg->m_hWnd,"发送完成","提示信息",MB_OK);

	
	delete m_mail_dlg;
}


void split(const char * str,const char * deli, vector<string> *list) 
{ 
    char buff[1024]; 
    strcpy(buff,str); 
    char * gg; 
    char *p = strtok(buff, deli);

    list->clear(); 
    while(p !=NULL) 
    { 
        list->push_back(p); 
        p = strtok(NULL, deli); 
    }; 
}

void CMailJobDlg::OnCheckBtn() 
{
	// TODO: Add your control notification handler code here
	int selcount=0;
	for (int i = 0; i < m_list.GetCount(); i++)
	{
			m_list.SetCheck(i,true);
			selcount+=mailcount[i];
	}
	m_allcount=selcount;
	m_pro.SetRange(0,m_allcount);
	m_pro.SetStep(1);
	UpdateData(false);
}	

void CMailJobDlg::OnDischeckBtn() 
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		m_list.SetCheck(i,false);
	}
	m_allcount=0;
	m_pro.SetRange(0,m_allcount);
	m_pro.SetStep(1);
	UpdateData(false);
}


void CMailJobDlg::OnSelcancelList1() 
{
	// TODO: Add your control notification handler code here
	if (m_Picture.Load(mpathlist[m_list.GetCurSel()].c_str()))
		m_Picture.Draw();
	int selcount=0;
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		if( m_list.GetCheck(i) )
		{
			selcount+=mailcount[i];
		}
	}
	m_allcount=selcount;

	m_pro.SetRange(0,m_allcount);
	m_pro.SetStep(1);
	UpdateData(false);
}

void CMailJobDlg::OnCopy() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	char batfile[MAX_PATH];
	char timefile[MAX_PATH];

	
	sprintf(timefile,"%s1.vbs",szPath);
	if(m_bianhao.GetLength() > 0 && m_bianhao.GetLength() > 0)
	{
		ofstream tfile(timefile);
		tfile <<"WSH.Echo "<< m_bianhao.GetBuffer(m_bianhao.GetLength()) << m_time.GetBuffer(m_time.GetLength()) << endl;
		tfile.close();

		sprintf(batfile,"%s\\copy.bat",szPath);
		WinExec(batfile,SW_SHOWNORMAL/*SW_HIDE*/);
	}else
	{
		MessageBox("请准确填写编号,时间,并配置好图片路径");
	}

}

void CMailJobDlg::OnOpenLog() 
{
	// TODO: Add your control notification handler code here
	char logpath[MAX_PATH];
	sprintf(logpath,"%slog\\",szPath);
	ShellExecute(NULL,"open","explorer.exe",logpath,NULL,SW_SHOWNORMAL);
}

void CMailJobDlg::OnOpenPic() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL,"open","explorer.exe",m_dir,NULL,SW_SHOWNORMAL);
}

LRESULT CMailJobDlg::MyUpTrue(WPARAM wParam, LPARAM lParam)
{
	UpdateData(true);
	return 0;
}

LRESULT CMailJobDlg::MyUpFalse(WPARAM wParam, LPARAM lParam)
{
	UpdateData(false);
	return 0;
}

void CMailJobDlg::GetSendMail()
{
	////////////////////////////////////////////////////
	char sendmail[256];
	sprintf(sendmail,"%smail_list\\sendmail.txt",szPath);
	ifstream sendin(sendmail);  

	string sendline;  
	char result[256];
	
	if(sendin) // 有该文件  
	{  

		while (getline (sendin, sendline)) // line中不包括每行的换行符  
		{   
		//	sendstruct *m_send=new sendstruct;
			split(sendline.c_str(),",",&mailsendlist);
			sprintf(result,"发件箱：%s，发件箱smtp服务器%s",mailsendlist[0].c_str(),mailsendlist[3].c_str());
			sendlist.push_back(sendline);	
			m_list3.AddString(result);
		}
	}  
	////////////////////////////////////////
}
