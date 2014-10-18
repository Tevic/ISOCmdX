#pragma once
class VirtualDrive
{
public:
	VirtualDrive(void);
	~VirtualDrive(void);
	BOOL InstallDriver();
	BOOL UnInstallDriver();
	BOOL Mount(CString csImagePath);
	BOOL UnMount();
	BOOL IsISOCmdExist(void);
	BOOL IsDriveLoadSuccess(void);
	CString GetDriveInfo(void);
	CString GetDriveName(void);
	int GetDriveCount();
	void UnMountAll();
	BOOL GetDriveState();
private:
	CString m_CurrentISOCmdDirectory;
	TCHAR m_CurrentDirectory[MAX_PATH];
	CStringArray m_DriveNameArray;
	CString m_DrivrName;
	BOOL m_DriveState;

};

