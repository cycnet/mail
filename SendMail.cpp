#include "StdAfx.h"  
#include "SendMail.h"  
#include "winsock2.h"  
#pragma comment(lib,"WSOCK32")  
 
using namespace std;
CSendMail::CSendMail(void)  
{  
    m_pcFileBuff=NULL;  
    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    memset(m_cReceiveBuff,0,sizeof(m_cReceiveBuff));  
}  
  
  
CSendMail::~CSendMail(void)  
{  
    DeleteAllPath();  
}  
  
  
void CSendMail::Char2Base64(char* pBuff64,char* pSrcBuff,int iLen)  
{  
  
    static char Base64Encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//base64???????  
    int point;
    point=2;  
    int i;  
    int iIndex;
    char n=0;
    for(i=0;i<iLen;i++)  
    {  
        if(point==2)  
        {  
            iIndex=((*pSrcBuff)>>point)&0x3f;
        }  
        else if (point==4)  
        {  
            iIndex=((*pSrcBuff)>>point)&0xf;
        }  
        else if(point==6)  
        {  
            iIndex=((*pSrcBuff)>>point)&0x3;
        }  
        iIndex+=n;
        *pBuff64++=Base64Encode[iIndex];
        n=((*pSrcBuff)<<(6-point));
        n=n&0x3f;
        point+=2;
        if(point==8)
        {  
            iIndex=(*pSrcBuff)&0x3f;
            *pBuff64++=Base64Encode[iIndex];
            n=0;
            point=2;
        }  
        pSrcBuff++;  
  
    }  
    if(n!=0)  
    {  
        *pBuff64++=Base64Encode[n];  
    }  
    if(iLen%3==2)
    {  
        *pBuff64='=';  
    }  
    else if(iLen%3==1)  
    {  
        *pBuff64++='=';  
        *pBuff64='=';  
    }  
}  
  
void CSendMail::AddFilePath(char * pcFilePath)
{  
    if(pcFilePath==NULL)  
    {  
        return;  
    }  
    int i;  
    char* temp;  
    for(i=0;i<m_pcFilePathList.GetCount();i++)  
    {  
        temp=m_pcFilePathList.GetAt(m_pcFilePathList.FindIndex(i));  
        if(strcmp(pcFilePath,temp)==0)
        {  
            return;  
        }  
    }  
    m_pcFilePathList.AddTail(pcFilePath);  
}  
  
void CSendMail::DeleteFilePath(char* pcFilePath)
{  
    int i;  
    char* temp;  
    for(i=0;i<m_pcFilePathList.GetCount();i++)  
    {  
        temp=m_pcFilePathList.GetAt(m_pcFilePathList.FindIndex(i));  
        if(strcmp(temp,pcFilePath)==0)
        {  
            m_pcFilePathList.RemoveAt(m_pcFilePathList.FindIndex(i));  
            delete[] temp;  
            return;  
        }  
    }  
}  
  
  
void CSendMail::DeleteAllPath(void)  
{  
    m_pcFilePathList.RemoveAll();  
}  
  
int CSendMail::GetFileData(char* FilePath)  
{  
    m_pcFileBuff=NULL;  
    if(FilePath==NULL)  
    {  
        return 0;  
    }  
    CFile f;  
    int len;  
      
      
    //USES_CONVERSION;  
    if(!f.Open(FilePath,CFile::modeRead|CFile::modeNoTruncate|CFile::typeBinary))  
    {  
        return 0;  
    }  
    len=(int)f.GetLength();  
    m_pcFileBuff=new char[len+1];  
    memset(m_pcFileBuff,0,len+1);  
    f.Read(m_pcFileBuff,len);  
  
    f.Close();  
    return len;  
}  
  
void CSendMail::GetFileName(char* fileName,char* filePath)  
{  
    if(filePath==NULL || fileName==NULL)  
    {  
        return;  
    }  
    int i;  
    for(i=0;i<(int)strlen(filePath);i++)  
    {  
        if(filePath[strlen(filePath)-1-i]=='\\')  
        {  
            memcpy(fileName,&filePath[strlen(filePath)-i],i);  
            return;  
        }  
    }  
}  
  
bool CSendMail::CReateSocket(SOCKET &sock)  
{  
    WORD wVersionRequested;  
    WSADATA wsaData;  
    int err;  
    wVersionRequested = MAKEWORD( 2, 2 );  
    err = WSAStartup( wVersionRequested, &wsaData );  
    if ( err != 0 )   
    {  
        return false;  
    }  
    if ( LOBYTE( wsaData.wVersion ) != 2 ||  
        HIBYTE( wsaData.wVersion ) != 2 )  
    {  
        WSACleanup( );  
        return false;   
    }  
    sock = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);  
    if (sock == INVALID_SOCKET)  
    {  
        return false;  
    }  
  
    sockaddr_in servaddr;  
    memset(&servaddr,0,sizeof(sockaddr_in));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(25);
    if(m_sMailInfo.m_pcIPName=="")  
    {  
        servaddr.sin_addr.s_addr = inet_addr(m_sMailInfo.m_pcIPAddr);
    }  
    else  
    {  
        struct hostent *hp=gethostbyname(m_sMailInfo.m_pcIPName);
        servaddr.sin_addr.s_addr=*(int*)(*hp->h_addr_list);  
    }  

    int ret = connect(sock,(sockaddr*)&servaddr,sizeof(servaddr));
    if (ret == SOCKET_ERROR)  
    {  
        return false;  
    }  
  
    return true;  
}  
  
  
bool CSendMail::Logon(SOCKET &sock)  
{  
	memset(m_cReceiveBuff,0,1024);
    recv(sock,m_cReceiveBuff,1024,0);  
  
    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    sprintf(m_cSendBuff,"helo smtp.163.com\r\n");  
    send(sock,m_cSendBuff,strlen(m_cSendBuff),0);

	memset(m_cReceiveBuff,0,1024);
    recv(sock,m_cReceiveBuff,1024,0); 
	
    if(m_cReceiveBuff[0]!='2' && m_cReceiveBuff[1]!='5' || m_cReceiveBuff[2]!='0')  
    {  
        return false;  
    }  


    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    sprintf(m_cSendBuff,"auth login\r\n");  
    send(sock,m_cSendBuff,strlen(m_cSendBuff),0);

	memset(m_cReceiveBuff,0,1024);
    recv(sock,m_cReceiveBuff,1024,0);
	
    if(m_cReceiveBuff[0]!='3' || m_cReceiveBuff[1]!='3' || m_cReceiveBuff[2]!='4')  
    {  
        return false;  
    }  

	
    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    Char2Base64(m_cSendBuff,m_sMailInfo.m_pcUserName,strlen(m_sMailInfo.m_pcUserName));  

	char username[MAX_PATH];
	sprintf(username,"%s\r\n",m_cSendBuff);
		
    send(sock,username,strlen(username),0);

	memset(m_cReceiveBuff,0,1024);
    recv(sock,m_cReceiveBuff,1024,0);  

	
    if(m_cReceiveBuff[0]!='3' || m_cReceiveBuff[1]!='3' || m_cReceiveBuff[2]!='4')  
    {  
        return false;  
    }  


    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    Char2Base64(m_cSendBuff,m_sMailInfo.m_pcUserPassWord,strlen(m_sMailInfo.m_pcUserPassWord));  
    char password[MAX_PATH];
	sprintf(password,"%s\r\n",m_cSendBuff);
		
    send(sock,password,strlen(password),0);
	
	memset(m_cReceiveBuff,0,1024);
    recv(sock,m_cReceiveBuff,1024,0); 
	
    if(m_cReceiveBuff[0]!='2' || m_cReceiveBuff[1]!='3' || m_cReceiveBuff[2]!='5')  
    {  
        return false;  
    }  
    return true;
}  
  
bool CSendMail::SendHead(SOCKET &sock)  
{  
    int rt;  
    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    sprintf(m_cSendBuff,"MAIL FROM:<%s>\r\n",m_sMailInfo.m_pcSender);  
    rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
      
    if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  
    recv(sock,m_cReceiveBuff,1024,0);  
  	
    memset(m_cSendBuff,0,sizeof(m_cSendBuff)); 
    sprintf(m_cSendBuff,"RCPT TO:<%s>\r\n",m_sMailInfo.m_pcReceiver);  
    rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
    if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  
    recv(sock,m_cReceiveBuff,1024,0);  
	
	for(int i=1;i<m_receiver.size();i++)
	{
		memset(m_cSendBuff,0,sizeof(m_cSendBuff)); 
		sprintf(m_cSendBuff,"RCPT TO:<%s>\r\n",m_receiver[i].c_str());  
		rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
		if(rt!=strlen(m_cSendBuff))  
		{  
			return false;  
		}  
		recv(sock,m_cReceiveBuff,1024,0);  
	}

    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    memcpy(m_cSendBuff,"DATA\r\n",strlen("DATA\r\n"));  
    rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
    if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  
    recv(sock,m_cReceiveBuff,1024,0);  
  
    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    sprintf(m_cSendBuff,"From:%s<%s>\r\n",m_sMailInfo.m_pcSenderName,m_sMailInfo.m_pcSender);
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
	if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  

    sprintf(m_cSendBuff,"To:<%s>\r\n",m_sMailInfo.m_pcReceiver);  
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
	if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  
    sprintf(m_cSendBuff,"Subject:%s\r\nMime-Version: 1.0\r\nContent-Type: multipart/mixed;   boundary=\"INVT\"\r\n\r\n",m_sMailInfo.m_pcTitle);  
    rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
    if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  
  
    return true;  
}  
  
bool CSendMail::SendTextBody(SOCKET &sock)  
{  
    int rt;  
    memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
    sprintf(m_cSendBuff,"--INVT\r\nContent-Type: text/html;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n",m_sMailInfo.m_pcBody);  
    rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
    if(rt!=strlen(m_cSendBuff))  
    {  
        return false;  
    }  
    else  
    {  
        return true;  
    }  
}  
  
bool CSendMail::SendFileBody(SOCKET &sock)  
{  
    int i;  
    char* filePath;  
    int rt;  
    int len;  
    int pt=0;  
    char fileName[128];  
    for(i=0;i<m_pcFilePathList.GetCount();i++)  
    {  
        pt=0;  
        memset(fileName,0,128);  
        filePath=m_pcFilePathList.GetAt(m_pcFilePathList.FindIndex(i));  
        len=GetFileData(filePath);  
        GetFileName(fileName,filePath);  
  
        sprintf(m_cSendBuff,"--INVT\r\nContent-Type: application/octet-stream;\r\n  name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-ID:<1.jpg>\r\nContent-Disposition: attachment;\r\n filename=\"%s\"\r\n\r\n",fileName,fileName);  
        send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
        while (pt<len)  
        {  
            memset(m_cSendBuff,0,sizeof(m_cSendBuff));  
            Char2Base64(m_cSendBuff,&m_pcFileBuff[pt],min(len-pt,3000));  
            m_cSendBuff[strlen(m_cSendBuff)]='\r';  
            m_cSendBuff[strlen(m_cSendBuff)]='\n';  
            rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
            pt+=min(len-pt,3000);  
            if(rt!=strlen(m_cSendBuff))  
            {  
                return false;  
            }  
        }  
        if(len!=0)  
        {  
            delete [] m_pcFileBuff;  
        } 
	
    }  
  
    return true;  
}  
  
bool CSendMail::SendEnd(SOCKET &sock)  
{  
    sprintf(m_cSendBuff,".\r\n");  
    send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
  
   // sprintf(m_cSendBuff,"QUIT\r\n");  
  //  send(sock,m_cSendBuff,strlen(m_cSendBuff),0);  
	recv(sock,m_cReceiveBuff,1024,0);  

    closesocket(sock);  
    WSACleanup();  
	if(m_cReceiveBuff[0]!='2' || m_cReceiveBuff[1]!='5' || m_cReceiveBuff[2]!='0')  
    {  
        return false;  
    }  
    return true;  
}  
  
  
bool CSendMail::SendMail(sMailInfo &smailInfo,vector<string> &m_rec)  
{  
    memcpy(&m_sMailInfo,&smailInfo,sizeof(smailInfo));  
	m_receiver=m_rec;

    if(m_sMailInfo.m_pcBody==NULL  
        || m_sMailInfo.m_pcIPAddr==NULL  
        || m_sMailInfo.m_pcIPName==NULL  
        || m_sMailInfo.m_pcReceiver==NULL  
        || m_sMailInfo.m_pcSender==NULL  
        || m_sMailInfo.m_pcSenderName==NULL  
        || m_sMailInfo.m_pcTitle==NULL  
        || m_sMailInfo.m_pcUserName==NULL  
        || m_sMailInfo.m_pcUserPassWord==NULL)  
    {  
        return false;  
    }  


    SOCKET sock;  
    if(!CReateSocket(sock))
    {  
        return false;  
    }  
  
    if(!Logon(sock))
    {  
        return false;  
    }  
  
    if(!SendHead(sock))
    {  
        return false;  
    }  
  
    if(!SendTextBody(sock))
    {  
        return false;  
    }  
  
    if(!SendFileBody(sock))
    {  
        return false;  
    }  
  
    if(!SendEnd(sock))
    {  
        return false;  
    }  
  
    return true;  
}  