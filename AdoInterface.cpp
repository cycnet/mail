#include "stdafx.h"
#include "AdoInterface.h"
#include <exception>

bool CAdoInterface::ConnecDataLibrary(LPCTSTR ConnStr,LPCTSTR UserID,
		           LPCTSTR PassWord,long Options)
{
//	m_pCon.CreateInstance(__uuidof(Connection));
	CoInitialize(NULL);//初始化COM库
	m_pCon.CreateInstance("ADODB.Connection");//创建Connection对象 
	try
	{
		m_pCon->Open(_bstr_t(ConnStr),_bstr_t(UserID),
			                       _bstr_t(PassWord),Options);
	}
	catch(_com_error e)
	{
		//AfxMessageBox("连接数据库失败！");
		printf("exception: %s\n",e.ErrorMessage());
		return false;
	}
	return true;
}

void CAdoInterface::CloseDataLibrary()
{
	m_pCon->Close();
	CoUninitialize();
}

void CAdoInterface::OpenSheet(LPCTSTR Sql,
		enum CursorTypeEnum CursorType,
		enum LockTypeEnum LockType,	long Options)
{
	m_pRec.CreateInstance(__uuidof(Recordset));
	try
	{		
		m_pRec->Open(Sql,m_pCon.GetInterfacePtr(),CursorType,LockType,Options);			
	}
	catch(...)
	{
		//AfxMessageBox("打开表单失败！");
	}
}

void CAdoInterface::CloseSheet()
{
	m_pRec->Close();
}

_RecordsetPtr CAdoInterface::Execute(LPCTSTR lpszSQL, long lOptions)
{
	ASSERT(m_pCon != NULL);
	ASSERT(AfxIsValidString(lpszSQL));
	try
	{
		return m_pCon->Execute(_bstr_t(lpszSQL), NULL, lOptions);
	}
	catch (...)
	{
		//AfxMessageBox("执行SQL语句出错！");
	} 
	return FALSE;
}

BOOL CAdoInterface::GetCollect(LPCTSTR Name,_variant_t& OutCol)
{
	_variant_t vt;
	vt = m_pRec->GetCollect(Name);
	if(vt.vt != VT_NULL)
	{
		OutCol = vt;
		return TRUE;
	}
	else
		return FALSE;			
}

//----------------------增值操作-------------------------------
void CAdoInterface::AddNewRecode(AddNewCode* code,long ColCount)
{
	try
	{
		m_pRec->AddNew();
		for(int i=0;i<ColCount;++i)
			m_pRec->PutCollect(code[i].ColName,code[i].Value);

		m_pRec->Update();
	}
	catch(...)
	{
		//AfxMessageBox("插入记录不成功！");
	}
}

void CAdoInterface::Delet(enum AffectEnum AffectRecords)
{
	m_pRec->Delete(AffectRecords);
	m_pRec->Update();
}

void CAdoInterface::GetOneRecord(_variant_t* ColName,long ColCount,
		                                 _variant_t* OutValue)
{
	try
	{
		for(int i=0;i<ColCount;++i)
		{
			OutValue[i] = m_pRec->GetCollect(ColName[i]);
		}
	}
	catch(...)
	{
		//AfxMessageBox("获取一条记录不成功！");
	}
}

BOOL CAdoInterface::Find(LPCTSTR lpszFind, 
		SearchDirectionEnum SearchDirection)
{
	ASSERT(m_pRec != NULL);
	ASSERT(AfxIsValidString(lpszFind));
	try
	{
		if (strcmp(lpszFind, _T("")) != 0)
		{
			m_strFind = lpszFind;
		}

		if (m_strFind.IsEmpty()) return FALSE;

		m_pRec->Find(_bstr_t(m_strFind), 0, SearchDirection, "");
		
		if ((adoEOF() || BOF()) )
		{
			return FALSE;
		}
		else
		{
			m_SearchDirection = SearchDirection;
			return TRUE;
		}
	}
	catch (...)
	{
		//AfxMessageBox("Find出错！");
		return FALSE;
	}
}

BOOL CAdoInterface::FindNext()
{
	ASSERT(m_pRec != NULL);
	try
	{
		if (m_strFind.IsEmpty()) return FALSE;
		m_pRec->MoveNext();		
		if ((m_pRec->adoEOF || m_pRec->BOF) )
		{
			return FALSE;
		}
		else
		{
			m_pRec->Find(_bstr_t(m_strFind),0, m_SearchDirection);
			if ((m_pRec->adoEOF || m_pRec->BOF) )
			{
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
		
	}
	catch (...)
	{
		//AfxMessageBox("FindNext出错！");
		return FALSE;
	}
}

BOOL CAdoInterface::Sort(LPCTSTR lpszSort)
{
	try
	{
		m_pRec->PutSort(lpszSort);
		return TRUE;
	}
	catch (...)
	{
		//AfxMessageBox("Sort出错！");
		return FALSE;
	}
}

BOOL CAdoInterface::Filter(LPCTSTR lpszFilter)
{
	ASSERT(m_pRec != NULL);
	try
	{
		//m_pRec->PutFilter(lpszFilter);
		return TRUE;
	}
	catch (...)
	{
		//AfxMessageBox("Filter出错！");
		return FALSE;
	}
}

//----------------------扩展包装方法--------------------------------
void CAdoInterface::AddNewRecodeEx(CString ColName, ... )
{
	//顺序取得字符串ColName中的字段名及字段类型，并将其存入vector中
	vector<FieldInfor> vfi;
	GetNameandType(ColName,vfi);	
	
    //将指定字符串ColName后面的省略号处的所有参数按顺序存入void*数组中
	vector<void*> vpn;
	void* pp = (&ColName) + (sizeof(ColName)/sizeof(CString));
	vpn.push_back(pp);
	for(int i=1;i<vfi.size();++i)
	{	
		if(vfi[i-1].Type == 4 || vfi[i-1].Type == 5)
		{
			void* pp = (double*)vpn[i-1] +      
				(sizeof(*(double*)vpn[i-1])/sizeof(double));
			vpn.push_back(pp);	
		}
		else
		{
			void* pp = (CString*)vpn[i-1] + 
				(sizeof(*(CString*)vpn[i-1])/sizeof(CString));
			vpn.push_back(pp);
		}		
	}
		
    //写数据
	m_pRec->AddNew();
	for(i=0;i<vfi.size();i++)
	{
		if(vfi[i].Type == 2 
			|| vfi[i].Type == 3 
			|| vfi[i].Type == 16 
			|| vfi[i].Type == 17
			|| vfi[i].Type == 18
			|| vfi[i].Type == 19
			|| vfi[i].Type == 20
			|| vfi[i].Type == 21)
		{
			m_pRec->PutCollect(vfi[i].Name , _variant_t(*(long*)vpn[i]));
		}
		else if(vfi[i].Type == 4 || vfi[i].Type == 5)
		{
			m_pRec->PutCollect(vfi[i].Name , _variant_t(*(double*)vpn[i]));
		}
		else
		{
			m_pRec->PutCollect(vfi[i].Name , _variant_t(*(CString*)vpn[i]));
		}
	}
	m_pRec->Update();    
}

void CAdoInterface::GetOneRecordEx(CString ColName, ... )
{
	//顺序取得字符串ColName中的字段名及字段类型，并将其存入vector中
	vector<FieldInfor> vfi;
	GetNameandType(ColName,vfi);
	
	//将指定字符串ColName后面的省略号处的所有参数按顺序存入void**数组中
	vector<void**> vpn;
	void** pp = (void**)((&ColName) + (sizeof(ColName)/sizeof(CString)));
	vpn.push_back(pp);
	for(int i=1;i<vfi.size();++i)
	{
		void** pp = vpn[i-1] + 1;
		vpn.push_back(pp);
	}

	//获取字段数据，并按字段名类型向外传出值
	for(i=0;i<vfi.size();++i)
	{
		_variant_t var;
		var = m_pRec->GetCollect(vfi[i].Name);
		if(var.vt != VT_NULL)
		{
			if(vfi[i].Type == 2 
			|| vfi[i].Type == 3 
			|| vfi[i].Type == 16 
			|| vfi[i].Type == 17
			|| vfi[i].Type == 18
			|| vfi[i].Type == 19
			|| vfi[i].Type == 20
			|| vfi[i].Type == 21)
			{
				*((long*)(*vpn[i])) = var;				
			}
			else if(vfi[i].Type == 4 || vfi[i].Type == 5)
			{
				*((double*)(*vpn[i])) = var;
			}
			else
			{
				*((CString*)(*vpn[i])) = (LPCSTR)_bstr_t(var);
			}
		}		
	}
}

//---------------------------算法--------------------------------
void CAdoInterface::Ufo(CString InStr,vector<CString>& OutStr)
{
	vector<char> bug;
	vector<CString> newbug;
	vector<CString> result;
	for(int i=0;i<InStr.GetLength();++i)
	{
		if(InStr[i] != ',')
			bug.push_back(InStr[i]);
		else
		{
			CString sa;
			for(int i=0;i<bug.size();++i)
			{
				sa += bug[i];
			}
			result.push_back(sa);
			bug.clear();
		}		
	}
	CString sa;
	for(i=0;i<bug.size();++i)
	{
		sa += bug[i];
	}
	result.push_back(sa);	
	OutStr = result;
}

void CAdoInterface::GetNameandType(CString ColName,vector<FieldInfor>& OutVnt)
{
	//将指定字符串ColName中的字段名按顺序萃取到数组中，并且获取相应
	//字段的类型信息,该算法需要依赖一个Name与Type的结构才能完成
	//1、取出某表所有字段名及字段类型
	FieldPtr fp;
	vector<FieldInfor> vfi;
	int Count = m_pRec->Fields->GetCount();
	for(int i=0;i<Count;++i)
	{
		FieldInfor fi;
		fp = m_pRec->Fields->GetItem(long(i));
		fi.Name = fp->Name;
		fi.Type = fp->Type;
		vfi.push_back(fi);
	}	
	//2、将指定字符串ColName中的字段名按顺序萃取到数组中
	vector<CString> vColName;
	Ufo(ColName,vColName);
	//3、从全部字段名中找出与字符串ColName中字段名相符的字段，并连同
	//该字段的类型一同存入出参的OutVnt中
	for(i=0;i<vColName.size();++i)
	{
		for(int j=0;j<vfi.size();++j)
		{
			if(vColName[i] == (LPCTSTR)vfi[j].Name)
			{
				OutVnt.push_back(vfi[j]);				
			}									
		}
	}
}


