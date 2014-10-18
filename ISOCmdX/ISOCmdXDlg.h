
// ISOCmdXDlg.h : ͷ�ļ�
//
#include "VirtualDrive.h"

#pragma once


// CISOCmdXDlg �Ի���
class CISOCmdXDlg : public CDialogEx
{
// ����
public:
	CISOCmdXDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ISOCMDX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
