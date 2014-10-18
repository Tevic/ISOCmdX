#include "stdafx.h"
#include "VirtualDrive.h"

void ExecuteCmd(CString csCmd,CString csCurrentDirectory);
void ExecuteCmdAsyn(CString csCmd,CString csCurrentDirectory);
CString GetResultFromCmd(CString csCmd,CString csCurrentDirectory);
CString GetResultFromCmdFile(CString csCmd,CString csCurrentDirectory);

VirtualDrive::VirtualDrive(void)
{
	GetCurrentDirectory(MAX_PATH,m_CurrentDirectory);
	m_CurrentISOCmdDirectory=CString(m_CurrentDirectory)+L"\\ISOCmd\\";
	m_DriveState=FALSE;
}


VirtualDrive::~VirtualDrive(void)
{
}


//��װ����
BOOL VirtualDrive::InstallDriver()
{
	if (!IsISOCmdExist())
	{
		return FALSE;
	}
	//�����������Ϣ
	CString subPath= L"SYSTEM\\CurrentControlSet\\Services\\ISODrive";
	RegDeleteKey(HKEY_LOCAL_MACHINE,subPath);
	//��װ������
	ExecuteCmd(L"CMD /C ISOCmd -Install",m_CurrentISOCmdDirectory);
	//�趨�̷�����
	ExecuteCmd(L"CMD /C ISOCmd -Number 1",m_CurrentISOCmdDirectory);
	//��ȡ��ǰ�̷�
	m_DrivrName=GetDriveName();
	if (!IsDriveLoadSuccess())
	{
		return FALSE;
	}
	return TRUE;
}

//ж������
BOOL VirtualDrive::UnInstallDriver()
{
	//ExecuteCmdAsyn(L"ISOCmd -Remove",m_CurrentISOCmdDirectory);
	ExecuteCmd(L"CMD /C ISOCmd -Remove",m_CurrentISOCmdDirectory);
	return TRUE;
}

//���ؾ���
BOOL VirtualDrive::Mount(CString csImagePath)
{
	//m_DriveNameArray.Add(csImagePath);
	//CHAR chDriveLabel[10];
	//_itoa_s(m_DriveNameArray.GetCount()-1,chDriveLabel,10);
	//ExecuteCmd(L"CMD /C ISOCmd -Mount "+CString(chDriveLabel)+L": \""+csImagePath+L"\"",m_CurrentISOCmdDirectory);
	ExecuteCmd(L"CMD /C ISOCmd -Mount 0: \""+csImagePath+L"\"",m_CurrentISOCmdDirectory);
	//ExecuteCmdAsyn(L"ISOCmd -Mount 0: \""+csImagePath+L"\"",m_CurrentISOCmdDirectory);
	SHChangeNotify (SHCNE_DRIVEADD, SHCNF_PATH, m_DrivrName, NULL);
	m_DriveState=TRUE;
	return TRUE;
}

//ж�ؾ���
BOOL VirtualDrive::UnMount()
{
	CHAR chDriveLabel[10];
	_itoa_s(m_DriveNameArray.GetCount()-1,chDriveLabel,10);
	//ExecuteCmdAsyn(L"ISOCmd -Eject "+CString(chDriveLabel)+L":",m_CurrentISOCmdDirectory);
	ExecuteCmd(L"CMD /C ISOCmd -Eject 0:",m_CurrentISOCmdDirectory);
	SHChangeNotify (SHCNE_DRIVEREMOVED, SHCNF_PATH, m_DrivrName, NULL);
	m_DriveState=FALSE;
	//�˴������з�������ж�ز���ȫ
	Sleep(500);
	return TRUE;
}

//��ȡ��ǰ������״̬
BOOL VirtualDrive::GetDriveState()
{
	return m_DriveState;
}

//��ȡ��ǰ�����̷�
CString VirtualDrive::GetDriveName(void)
{
	CString strInfo=GetDriveInfo();	
	if (strInfo!=L"")
	{
		return strInfo.Mid(strInfo.Find(L"[",0)+1,1)+L":";
	}
	else 
		return L"";
}


//�������й���
void VirtualDrive::UnMountAll()
{
	for (int i = 0; i < GetDriveCount(); i++)
	{
		UnMount();
	}
}

//��õ�ǰ���������������
int VirtualDrive::GetDriveCount()
{
	return m_DriveNameArray.GetCount();
}

//�ж������Ƿ��Ѿ��ɹ�����
BOOL VirtualDrive::IsDriveLoadSuccess(void)
{
	HKEY hKEY;
	CString subPath= L"SYSTEM\\CurrentControlSet\\Services\\ISODrive";
	DWORD status=RegOpenKeyExW(HKEY_LOCAL_MACHINE,subPath,0,KEY_QUERY_VALUE, &hKEY);
	if(status!=ERROR_SUCCESS)
	{
		RegCloseKey(hKEY);
		return FALSE;
	}
	return TRUE;
}

//�ж�ISOCmd�ļ��Ƿ����
BOOL VirtualDrive::IsISOCmdExist(void)
{
	BOOL isExist=PathFileExists(m_CurrentISOCmdDirectory+L"ISOCmd.exe")
		&&PathFileExists(m_CurrentISOCmdDirectory+L"ISODrive.sys")
		&&PathFileExists(m_CurrentISOCmdDirectory+L"ISODrv64.sys");
	return isExist;
}

//��ȡ��ǰ���������Ϣ
CString VirtualDrive::GetDriveInfo()
{
	//return GetResultFromCmd(L"CMD /C ISOCmd -Print",m_CurrentISOCmdDirectory);
	return GetResultFromCmdFile(L"CMD /C ISOCmd -Print",m_CurrentISOCmdDirectory);
}

//ִ�������в��ȴ�ִ�����
void ExecuteCmd(CString csCmd,CString csCurrentDirectory)
{
	STARTUPINFO   si={sizeof(si)};
	ZeroMemory(&si,sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
	si.cb=sizeof(STARTUPINFO);   
	GetStartupInfo(&si);
	si.wShowWindow=SW_HIDE;   
	si.dwFlags   = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES; 
	if   (!CreateProcessW(NULL,csCmd.GetBuffer(),NULL,
		NULL,TRUE,NULL,NULL,csCurrentDirectory,&si,&pi))   
	{     
		AfxMessageBox(L"Error On CreateProcess",MB_OK|MB_ICONERROR);
		return;  
	}    
	DWORD dwResult=WaitForSingleObject(pi.hProcess,1500);
	if (WAIT_TIMEOUT==dwResult)
	{	
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		AfxMessageBox(L"Wait TimeOut",MB_OK|MB_ICONERROR);
		return;
	}
}

//�첽ִ��������
void ExecuteCmdAsyn(CString csCmd,CString csCurrentDirectory)
{
	ShellExecute(NULL,L"Open",L"CMD",L"/C "+csCmd,csCurrentDirectory,SW_HIDE);
}

//ִ�в���ȡ���������
CString GetResultFromCmd(CString csCmd,CString csCurrentDirectory)
{
	#define RESULT_LEN 4096
	HANDLE hRead,hWrite;
	SECURITY_ATTRIBUTES   sa;     
	sa.nLength   =   sizeof(SECURITY_ATTRIBUTES);   
	sa.lpSecurityDescriptor   =   NULL;   
	sa.bInheritHandle   =   TRUE; 
	if   (!CreatePipe(&hRead,&hWrite,&sa,0))   
	{   
		AfxMessageBox(L"Error On CreatePipe",MB_OK|MB_ICONERROR);
		return NULL;   
	}     
	STARTUPINFO   si={sizeof(si)};
	ZeroMemory(&si,sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
	si.cb=sizeof(STARTUPINFO);   
	GetStartupInfo(&si);     
	si.hStdError=GetStdHandle(STD_ERROR_HANDLE);;   
	si.hStdOutput=hWrite;
	si.hStdInput=hRead;
	si.wShowWindow=SW_HIDE;   
	si.dwFlags   = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES; 
	if   (!CreateProcessW(NULL,csCmd.GetBuffer(),NULL,
		NULL,TRUE,NULL,NULL,csCurrentDirectory,&si,&pi))   
	{     
		CloseHandle(hWrite);
		CloseHandle(hRead);
		hRead=NULL;
		hWrite=NULL;
		AfxMessageBox(L"Error On CreateProcess",MB_OK|MB_ICONERROR);
		return NULL;  
	}    
	CHAR Buffer[RESULT_LEN]={0};   
	DWORD dwRead;     
	if (WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess,1500))
	{	
		CloseHandle(hWrite);
		CloseHandle(hRead);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		hRead=NULL;
		hWrite=NULL;
		AfxMessageBox(L"Wait TimeOut",MB_OK|MB_ICONERROR);
		return NULL;
	}

	CString strRead=L"";
	if (!ReadFile(hRead,Buffer,RESULT_LEN-1,&dwRead,NULL))
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		AfxMessageBox(L"Read Error",MB_OK|MB_ICONERROR);
		return NULL;
	}
	strRead=CString(Buffer);
	CloseHandle(hWrite);
	CloseHandle(hRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	hRead=NULL;
	hWrite=NULL;
	return strRead;
}

//ִ�в����ļ���ȡ���������
CString GetResultFromCmdFile(CString csCmd,CString csCurrentDirectory)
{
	#define RESULT_LEN 1024
	STARTUPINFO   si={sizeof(si)};
	ZeroMemory(&si,sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
	si.cb=sizeof(STARTUPINFO);   
	GetStartupInfo(&si);     
	si.wShowWindow=SW_HIDE;   
	si.dwFlags   = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES; 
	csCmd+=L">Info";
	if   (!CreateProcessW(NULL,csCmd.GetBuffer(),NULL,
		NULL,TRUE,NULL,NULL,csCurrentDirectory,&si,&pi))   
	{     
		AfxMessageBox(L"Error On CreateProcess",MB_OK|MB_ICONERROR);
		return NULL;  
	}    
	if (WAIT_TIMEOUT==WaitForSingleObject(pi.hProcess,5000))
	{	
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		AfxMessageBox(L"Wait TimeOut",MB_OK|MB_ICONERROR);
		return NULL;
	}
	CFile myFile;
	if (!myFile.Open(csCurrentDirectory+L"Info",CFile::modeRead))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		AfxMessageBox(L"Error On ReadFile",MB_OK|MB_ICONERROR);
		return NULL;  
	}
	CString strRead;
	CHAR chResult[RESULT_LEN]={0};
	while (myFile.Read(chResult,RESULT_LEN-1))
	{
		strRead+=CString(chResult)+L"\r\n";
	}
	myFile.Close();
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	DeleteFile(csCurrentDirectory+L"Info");
	return strRead;
}