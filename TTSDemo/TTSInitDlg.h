#pragma once


// CTTSInitDlg �Ի���

class CTTSInitDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTTSInitDlg)

public:
	CTTSInitDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTTSInitDlg();

// �Ի�������
	enum { IDD = IDD_TTSINIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_init_result;
	afx_msg void OnBnClickedOk();
	CTTSDemoDlg * mainDlg;
	virtual BOOL OnInitDialog();
	CString m_selectedModule;
};
