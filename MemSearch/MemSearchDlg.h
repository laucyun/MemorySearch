// MemSearchDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include "VMInfo.h"

// CMemSearchDlg �Ի���
class CMemSearchDlg : public CDialog
{
// ����
public:
	CMemSearchDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MEMSEARCH_DIALOG };

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
	CListBox AddrList;
public:
	DWORD dwTagValue;
public:
	DWORD dwTagAddr;
public:
	DWORD dwNewValue;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) ;
	afx_msg void OnBnClickedSelect();
public:
	afx_msg void OnBnClickedFirstSearch();
public:
	afx_msg void OnBnClickedReSearch();
public:
	afx_msg void OnBnClickedModify();
public:
	afx_msg void OnBnClickedExit();
public:
	CString szPathInfo;

private:
	CVMInfo	VMInfo ;
	DWORD	dwSelectedProcessId ;
	CString	szSelectedProcessName ;

public:
	VOID	UpdateAddrListInfo () ;
public:
	afx_msg void OnLbnSelchangeAddrList();
public:
	afx_msg void OnEnChangeTagValue();
public:
	afx_msg void OnEnChangeNewValue();
};
