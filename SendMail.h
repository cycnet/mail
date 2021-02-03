#include "StdAfx.h"
#include "winsock2.h"  
#include "Afxtempl.h"
#include <vector>

#pragma comment(lib,"WSOCK32")  
using namespace std;
struct sMailInfo 
{  
    char   m_pcUserName[MAX_PATH];//用户登录邮箱的名称
    char   m_pcUserPassWord[MAX_PATH];//用户登录邮箱的密码  
    char   m_pcSenderName[MAX_PATH];//用户发送时显示的名称  
    char   m_pcSender[MAX_PATH];//发送者的邮箱地址  
    char   m_pcReceiver[MAX_PATH];//接收者的邮箱地址  
    char   m_pcTitle[MAX_PATH];//邮箱标题  
    char   m_pcBody[MAX_PATH];//邮件文本正文  
    char   m_pcIPAddr[MAX_PATH]; //服务器的IP 
    char   m_pcIPName[MAX_PATH];//服务器的名称（IP与名称二选一，优先取名称）  
    sMailInfo(){memset(this,0,sizeof(sMailInfo));}  
};  
class CSendMail  
{  
public:  
    CSendMail(void);  
    ~CSendMail(void);  
  
public:  
    bool SendMail(sMailInfo &smailInfo,vector<string> &m_rec);//发送邮件，需要在发送的时候初始化邮件信息  
    void AddFilePath(char * pcFilePath);//添加附件的决定路径到附件列表中  
    void DeleteFilePath(char* pcFilePath);//删除附件路径，如果有的话  
    void DeleteAllPath(void);//删除全部附件的路径  

	
protected:  
    void GetFileName(char* fileName,char* filePath);//从附件的路径中获取文件名称  
    void Char2Base64(char* pBuff64,char* pSrcBuff,int iLen);//把char类型转换成Base64类型  
    bool CReateSocket(SOCKET &sock);//建立socket连接  
    bool Logon(SOCKET &sock);//登录邮箱，主要进行发邮件前的准备工作  
    int GetFileData(char* FilePath);//由文件路径获取附件内容  
  
    bool SendHead(SOCKET &sock);//发送邮件头  
    bool SendTextBody(SOCKET &sock);//发送邮件文本正文  
    bool SendFileBody(SOCKET &sock);//发送邮件附件  
    bool SendEnd(SOCKET &sock);//发送邮件结尾  
protected:  
    CList<char*,char*> m_pcFilePathList;//记录附件路径  
    char  m_cSendBuff[4096];//发送缓冲区  
    char  m_cReceiveBuff[1024];  
    char* m_pcFileBuff;//指向附件内容 
    sMailInfo m_sMailInfo; 
	vector<string> m_receiver;
}; 
