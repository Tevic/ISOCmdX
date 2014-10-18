
// ISOCmdXDlg.h : 头文件
//
#include "VirtualDrive.h"

#pragma once


// CISOCmdXDlg 对话框
class CISOCmdXDlg : public CDialogEx
{
// 构造
public:
	CISOCmdXDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ISOCMDX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonMount();
	afx_msg void OnBnClickedButtonUnmount();
public:
	afx_msg void OnBnClickedButtonChoose();
	BOOL IsValidFileType(CString szFileExt);
private:
	VirtualDrive m_VirtualDrive;
	CString m_ImagePath;
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
