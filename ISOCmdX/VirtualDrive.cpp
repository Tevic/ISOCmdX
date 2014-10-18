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


//安装驱动
BOOL VirtualDrive::InstallDriver()
{
	if (!IsISOCmdExist())
	{
		return FALSE;
	}
	//清理旧驱动信息
	CString subPath= L"SYSTEM\\CurrentControlSet\\Services\\ISODrive";
	RegDeleteKey(HKEY_LOCAL_MACHINE,subPath);
	//安装新驱动
	ExecuteCmd(L"CMD /C ISOCmd -Install",m_CurrentISOCmdDirectory);
	//设定盘符数量
	ExecuteCmd(L"CMD /C ISOCmd -Number 1",m_CurrentISOCmdDirectory);
	//获取当前盘符
	m_DrivrName=GetDriveName();
	if (!IsDriveLoadSuccess())
	{
		return FALSE;
	}
	return TRUE;
}

//卸载驱动
BOOL VirtualDrive::UnInstallDriver()
{
	//ExecuteCmdAsyn(L"ISOCmd -Remove",m_CurrentISOCmdDirectory);
	ExecuteCmd(L"CMD /C ISOCmd -Remove",m_CurrentISOCmdDirectory);
	return TRUE;
}

//加载镜像
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

//卸载镜像
BOOL VirtualDrive::UnMount()
{
	CHAR chDriveLabel[10];
	_itoa_s(m_DriveNameArray.GetCount()-1,chDriveLabel,10);
	//ExecuteCmdAsyn(L"ISOCmd -Eject "+CString(chDriveLabel)+L":",m_CurrentISOCmdDirectory);
	ExecuteCmd(L"CMD /C ISOCmd -Eject 0:",m_CurrentISOCmdDirectory);
	SHChangeNotify (SHCNE_DRIVEREMOVED, SHCNF_PATH, m_DrivrName, NULL);
	m_DriveState=FALSE;
	//此处必须有否则驱动卸载不完全
	Sleep(500);
	return TRUE;
}

//获取当前驱动器状态
BOOL VirtualDrive::GetDriveState()
{
	return m_DriveState;
}

//获取当前光驱盘符
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


//弹出所有光驱
void VirtualDrive::UnMountAll()
{
	for (int i = 0; i < GetDriveCount(); i++)
	{
		UnMount();
	}
}

//获得当前加载虚拟光驱数量
int VirtualDrive::GetDriveCount()
{
	return m_DriveNameArray.GetCount();
}

//判断驱动是否已经成功加载
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

//判断ISOCmd文件是否存在
BOOL VirtualDrive::IsISOCmdExist(void)
{
	BOOL isExist=PathFileExists(m_CurrentISOCmdDirectory+L"ISOCmd.exe")
		&&PathFileExists(m_CurrentISOCmdDirectory+L"ISODrive.sys")
		&&PathFileExists(m_CurrentISOCmdDirectory+L"ISODrv64.sys");
	return isExist;
}

//获取当前虚拟光驱信息
CString VirtualDrive::GetDriveInfo()
{
	//return GetResultFromCmd(L"CMD /C ISOCmd -Print",m_CurrentISOCmdDirectory);
	return GetResultFromCmdFile(L"CMD /C ISOCmd -Print",m_CurrentISOCmdDirectory);
}

//执行命令行并等待执行完毕
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

//异步执行命令行
void ExecuteCmdAsyn(CString csCmd,CString csCurrentDirectory)
{
	ShellExecute(NULL,L"Open",L"CMD",L"/C "+csCmd,csCurrentDirectory,SW_HIDE);
}

//执行并获取命令行输出
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

//执行并从文件获取命令行输出
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