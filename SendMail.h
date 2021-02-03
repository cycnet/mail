#include "StdAfx.h"
#include "winsock2.h"  
#include "Afxtempl.h"
#include <vector>

#pragma comment(lib,"WSOCK32")  
using namespace std;
struct sMailInfo 
{  
    char   m_pcUserName[MAX_PATH];//�û���¼���������
    char   m_pcUserPassWord[MAX_PATH];//�û���¼���������  
    char   m_pcSenderName[MAX_PATH];//�û�����ʱ��ʾ������  
    char   m_pcSender[MAX_PATH];//�����ߵ������ַ  
    char   m_pcReceiver[MAX_PATH];//�����ߵ������ַ  
    char   m_pcTitle[MAX_PATH];//�������  
    char   m_pcBody[MAX_PATH];//�ʼ��ı�����  
    char   m_pcIPAddr[MAX_PATH]; //��������IP 
    char   m_pcIPName[MAX_PATH];//�����������ƣ�IP�����ƶ�ѡһ������ȡ���ƣ�  
    sMailInfo(){memset(this,0,sizeof(sMailInfo));}  
};  
class CSendMail  
{  
public:  
    CSendMail(void);  
    ~CSendMail(void);  
  
public:  
    bool SendMail(sMailInfo &smailInfo,vector<string> &m_rec);//�����ʼ�����Ҫ�ڷ��͵�ʱ���ʼ���ʼ���Ϣ  
    void AddFilePath(char * pcFilePath);//��Ӹ����ľ���·���������б���  
    void DeleteFilePath(char* pcFilePath);//ɾ������·��������еĻ�  
    void DeleteAllPath(void);//ɾ��ȫ��������·��  

	
protected:  
    void GetFileName(char* fileName,char* filePath);//�Ӹ�����·���л�ȡ�ļ�����  
    void Char2Base64(char* pBuff64,char* pSrcBuff,int iLen);//��char����ת����Base64����  
    bool CReateSocket(SOCKET &sock);//����socket����  
    bool Logon(SOCKET &sock);//��¼���䣬��Ҫ���з��ʼ�ǰ��׼������  
    int GetFileData(char* FilePath);//���ļ�·����ȡ��������  
  
    bool SendHead(SOCKET &sock);//�����ʼ�ͷ  
    bool SendTextBody(SOCKET &sock);//�����ʼ��ı�����  
    bool SendFileBody(SOCKET &sock);//�����ʼ�����  
    bool SendEnd(SOCKET &sock);//�����ʼ���β  
protected:  
    CList<char*,char*> m_pcFilePathList;//��¼����·��  
    char  m_cSendBuff[4096];//���ͻ�����  
    char  m_cReceiveBuff[1024];  
    char* m_pcFileBuff;//ָ�򸽼����� 
    sMailInfo m_sMailInfo; 
	vector<string> m_receiver;
}; 
