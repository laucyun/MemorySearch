#pragma once
#include "afxwin.h"


// CSelProcess �Ի���

typedef struct _MY_PROCESS_INFO {
	DWORD	dwProcessId ;
	CString	szFilePath ;
} MY_PROCESS_INFO, *PMY_PROCESS_INFO ;

class CSelProcess : public CDialog
{
	DECLARE_DYNAMIC(CSelProcess)

public:
	CSelProcess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelProcess();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

private:
	int		nCurSel ;
	vector<MY_PROCESS_INFO> ProcList ;

public: 
	DWORD	GetSelProcessId () ;
	CString	GetSelProcessName();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect();
public:
	afx_msg void OnBnClickedFlush();
public:
	CListBox ProcInfoList;
public:
	virtual BOOL OnInitDialog();
};
