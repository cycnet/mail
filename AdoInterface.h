#if !defined(AFX_ADO_)
#define AFX_ADO_
#pragma once
//#pragma warning (push)
//#pragma warning(disable:C4018)
#include "DataBelong.h"
#include <vector>
using namespace std;

class CAdoInterface
{	
public:
	//获取_ConnectionPtr、_RecordsetPtr
	_ConnectionPtr& GetConnPtr()    {return m_pCon;}
	_RecordsetPtr& GetRecoPtr()     {return m_pRec;}

	//连接某个数据库
	bool ConnecDataLibrary(LPCTSTR ConnStr,LPCTSTR UserID,
		LPCTSTR PassWord,long Options=adModeUnknown);
	//关库
	void CloseDataLibrary();
	//以某种方式打开表单
	void OpenSheet(LPCTSTR Sql,
		enum CursorTypeEnum CursorType=adOpenDynamic,
		enum LockTypeEnum LockType=adLockOptimistic,
		long Options=adCmdText);
	//关表
	void CloseSheet();	

	//头
	BOOL BOF()          {return m_pRec->BOF;}
	//尾
	BOOL adoEOF()       {return m_pRec->adoEOF;}
	//第一条记录
	void MoveFirst()    {m_pRec->MoveFirst();}
	//最后一条记录
	void MoveLast()     {m_pRec->MoveLast();}
	//向下移动一条记录
	void MoveNext()     {m_pRec->MoveNext();}
	//向上移动一条记录
	void MovePrevious() {m_pRec->MovePrevious();}
	//移动到指定位置
	void Move(long Num) {m_pRec->Move(Num);}

	//执行SQL语句
	_RecordsetPtr Execute(LPCTSTR strSQL, long lOptions = adCmdText);	

	//获取字段内容
	BOOL GetCollect(LPCTSTR Name,_variant_t& OutCol);
	
	//---------------------增值操作--------------------------------
	//插入一条新的记录
	void AddNewRecode(AddNewCode* code,long ColCount);
	//删除一条记录
	void Delet(enum AffectEnum AffectRecords=adAffectCurrent);
	//获取一条记录的容
	void GetOneRecord(_variant_t* ColName,long ColCount,
		                                 _variant_t* OutValue);
	//查找
	BOOL Find(LPCTSTR lpszFind, 
		SearchDirectionEnum SearchDirection = adSearchForward);
	//查找下一个
	BOOL FindNext();
	//排序
	BOOL Sort(LPCTSTR lpszSort);
	//过滤
	BOOL Filter(LPCTSTR lpszFilter);	

	//----------------------扩展包装方法------------------------
	//插入一条新的记录
	void AddNewRecodeEx(CString ColName, ... );
	//读取一条新的记录
	void GetOneRecordEx(CString ColName, ... );

private:
	_ConnectionPtr       m_pCon;
	_RecordsetPtr        m_pRec;
	CString              m_strFind;
	SearchDirectionEnum  m_SearchDirection;
    //-----------------------算法-----------------------
	void Ufo(CString InStr,vector<CString>& OutStr);	
	void GetNameandType(CString ColName,vector<FieldInfor>& OutVnt);	
};

#endif