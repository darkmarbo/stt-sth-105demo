// TTSInitDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TTSDemo.h"
#include "TTSDemoUtils.h"
#include "TTSDemoDlg.h"
#include "TTSInitDlg.h"
#include "afxdialogex.h"


// CTTSInitDlg �Ի���

IMPLEMENT_DYNAMIC(CTTSInitDlg, CDialogEx)

CTTSInitDlg::CTTSInitDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTTSInitDlg::IDD, pParent)
	, m_init_result(-1)
	, mainDlg(NULL)
	, m_selectedModule(_T(""))
{

}

CTTSInitDlg::~CTTSInitDlg()
{
}

void CTTSInitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CB_MODULE, m_selectedModule);
}


BEGIN_MESSAGE_MAP(CTTSInitDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTTSInitDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTTSInitDlg ��Ϣ�������


void CTTSInitDlg::OnBnClickedOk()
{
	// �ڴ���ӿؼ�֪ͨ����������

	// TODO: Init here
	UpdateData(TRUE);
	//MessageBox(m_selectedModule);

	GetDlgItem(IDC_LB_INIT_MESSAGE)->SetWindowText(_T("[Status: Initializing...]"));

	CStringA moduleName = CTTSDemoUtils::UTF16_UTF8(m_selectedModule);
	m_init_result = mainDlg->tts->init(moduleName.GetBuffer());
	moduleName.ReleaseBuffer();

	CString message;
	if (m_init_result < 0) message.Format(_T("[Status: Initialization Error(%d)]"), m_init_result);
	else message.Format(_T("[Status:Initialization Success(%d)]"), m_init_result);

	GetDlgItem(IDC_LB_INIT_MESSAGE)->SetWindowText(message);
	//Sleep(1000);
	Sleep(10);

	CDialogEx::OnOK();
}


BOOL CTTSInitDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//�ڴ���Ӷ���ĳ�ʼ��
	CComboBox * cb = (CComboBox *)GetDlgItem(IDC_CB_MODULE);
	cb->AddString(_T("model/model-so"));
	cb->AddString(_T("model/model-lucheng"));
	cb->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
