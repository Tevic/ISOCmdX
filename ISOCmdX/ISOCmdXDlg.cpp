
// ISOCmdXDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ISOCmdX.h"
#include "ISOCmdXDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CISOCmdXDlg 对话框



CISOCmdXDlg::CISOCmdXDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CISOCmdXDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ImagePath=L"";
}

void CISOCmdXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CISOCmdXDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CISOCmdXDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_MOUNT, &CISOCmdXDlg::OnBnClickedButtonMount)
	ON_BN_CLICKED(IDC_BUTTON_UNMOUNT, &CISOCmdXDlg::OnBnClickedButtonUnmount)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, &CISOCmdXDlg::OnBnClickedButtonChoose)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CISOCmdXDlg 消息处理程序

BOOL CISOCmdXDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	if (!m_VirtualDrive.IsISOCmdExist())
	{
		MessageBox(L"文件损坏，未找到ISOCmd主程序，请下载ISOCmd主程序并将ISOCmd.exe,ISODrive.sys,ISODrv64.sys放在主程序的ISOCmd目录下！",L"错误",MB_ICONERROR);
		ShellExecute(m_hWnd,L"Open",L"http://pan.baidu.com/share/link?shareid=4121172740&uk=1746924341",NULL,NULL,SW_SHOWNORMAL);
		ExitProcess(1);
	}

	//安装驱动
	if (!m_VirtualDrive.InstallDriver())
	{
		MessageBox(L"驱动安装失败，请以管理员身份重新运行或重启后再运行！",L"错误",MB_ICONERROR);
		ExitProcess(1);
	}

	((CButton*)GetDlgItem(IDC_BUTTON_UNMOUNT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_MOUNT))->EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CISOCmdXDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CISOCmdXDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CISOCmdXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CISOCmdXDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

void CISOCmdXDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_VirtualDrive.GetDriveState())
	{
		if (MessageBox(L"当前已有镜像加载到虚拟光驱，关闭程序将卸载镜像！是否继续？",L"提醒",MB_YESNO|MB_ICONQUESTION)==IDYES)
		{
			m_VirtualDrive.UnMount();
			m_VirtualDrive.UnInstallDriver();
			CDialogEx::OnClose();
		}
	}
	else
	{
		m_VirtualDrive.UnInstallDriver();
		CDialogEx::OnClose();
	}
}

void CISOCmdXDlg::OnBnClickedButtonMount()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_BUTTON_MOUNT))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_CHOOSE))->EnableWindow(FALSE);
	m_VirtualDrive.Mount(m_ImagePath);
	((CButton*)GetDlgItem(IDC_BUTTON_UNMOUNT))->EnableWindow(TRUE);

}


void CISOCmdXDlg::OnBnClickedButtonUnmount()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_BUTTON_UNMOUNT))->EnableWindow(FALSE);
	m_VirtualDrive.UnMount();
	((CButton*)GetDlgItem(IDC_BUTTON_MOUNT))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_CHOOSE))->EnableWindow(TRUE);
}


void CISOCmdXDlg::OnBnClickedButtonChoose()
{
	// TODO: 在此添加控件通知处理程序代码
	//创建文件对话框
	CString strFilter;
	strFilter.LoadStringW(IDS_STRING_FILTER);
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | 
		OFN_OVERWRITEPROMPT, strFilter);
	dlg.m_ofn.lpstrTitle = _T("请选择光盘镜像");
	if(dlg.DoModal() == IDOK)
	{
		//获得源文件路径
		m_ImagePath = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_IMAGE_PATH, m_ImagePath);
		((CButton*)GetDlgItem(IDC_BUTTON_UNMOUNT))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON_MOUNT))->EnableWindow(TRUE);
	}
}


BOOL CISOCmdXDlg::IsValidFileType(CString szFileExt)
{
	BOOL lResult=FALSE;
	CString arrFileExt[50]={L".iso",L".isz",
		L".bin",L".cue",L".mds",L".mdf",
		L".nrg",L".ashdisc",L".b6t",L".b6i",
		L".b5t",L".b5i",L".bwt",L".bwi",
		L".lcd",L".ccd",L".img",L".dvd",
		L".000",L".daa",L".cdi",L".cif",
		L".xmf",L".xmd",L".pdi",L".dmg",
		L".timg",L".hfs",L".ncd",L".pxi",
		L".p2i",L".rif",L".rdf",L".gi",
		L".uif",L".vc4",L".fcd",L".vcd",
		L".ima",L".bif",L".flp",L".c2d"};
	for (int i = 0; i < 50; i++)
	{
		if (szFileExt.CompareNoCase(arrFileExt[i])==0)
		{
			lResult=TRUE;
			return lResult;
		}
	}
	return lResult;
}

void CISOCmdXDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szFilePathName[MAX_PATH+1]={0};          
	DragQueryFileW(hDropInfo, 0, szFilePathName, MAX_PATH);
	if (!PathIsDirectory(szFilePathName))
	{
		if (GetDlgItem(IDC_BUTTON_CHOOSE)->IsWindowEnabled())
		{
			CString szFileName(szFilePathName);
			CString szFileExt(szFileName.Mid(szFileName.ReverseFind(L'.')));
			if (IsValidFileType(szFileExt))
			{
				m_ImagePath=szFileName;
				SetDlgItemText(IDC_EDIT_IMAGE_PATH,szFileName);
				((CButton*)GetDlgItem(IDC_BUTTON_UNMOUNT))->EnableWindow(FALSE);
				((CButton*)GetDlgItem(IDC_BUTTON_MOUNT))->EnableWindow(TRUE);
			}
			else
			{
				MessageBox(L"不兼容的文件格式！",L"提醒",MB_ICONWARNING);
			}
		}
		else
		{
			MessageBox(L"请先卸载当前镜像！",L"提醒",MB_ICONWARNING);
		}
	}
	DragFinish(hDropInfo);
	CDialogEx::OnDropFiles(hDropInfo);
}
