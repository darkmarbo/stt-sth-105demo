
// TTSDemoDlg.cpp : ʵ���ļ�
//
#include <stdio.h>
#include "stdafx.h"
#include "TTSDemo.h"
#include "TTSDemoDlg.h"
#include "TTSDemoUtils.h"
#include "TTSInitDlg.h"
#include "afxdialogex.h"

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WAV_SIZE 60000000	//10+min  44100*600*2=6000 0000
#define MAX_LINE 1000		//
//**Max Memory 18G

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
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


// CTTSDemoDlg �Ի���

CTTSDemoDlg::CTTSDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTTSDemoDlg::IDD, pParent)
	, m_selModel(_T(""))
	, m_newText(_T("�������������Ƽ����޹�˾��һ��ȫ�����ȵġ������ԡ���������˹�����������Դ��������ݷ�����ṩ�̡�\r\n\r\n�й���������ʮ�˽�����ίԱ�������ȫ����飬��2016��10��24����27���ڱ������С���ϯ���ȫ����У�����ίԱ197�ˣ�������ίԱ151�ˡ�������ɼ��ίԱ��ίԱ���йط��渺��ͬ־��ϯ���顣����ʮ�˴�����в��ֻ���ͬ־��ר��ѧ��Ҳ��ϯ���顣\r\n\r\n����ʱ��10��28�գ���������ʱ��10��27�գ���ȫ�����������������ٶȣ������˽���2016��9��30�յ�������δ����ƵĲ��񱨸档�ü��ȣ��ٶ���Ӫ��Ϊ182.53��Ԫ��Լ��27.37����Ԫ�����Ƴ�ȥ����Ӱ�죬ʵ��ͬ������6.7%��������Ϊ31.02��Ԫ��Լ��4.652����Ԫ����ͬ������9.2%�������ƶ�Ӫ��ռ�ȳ���������64%��\r\n\r\n�������ξ����ŷ����˺���յ����ڱ������е����ŷ������ϱ�ʾ��2017�ꡰ��һ�����ڣ��й���������ҵ�Ӿ�������ģʽ����ȫ������ģʽ��ת��̬�����ԡ����������Ͳ�Ʒ�𽥱���������Ŀ�ĵز�Ʒ��ȡ����԰���Ͳ�Ʒ���ͻ�𣬸�����ҵ̬��Ʒȫ�濪����������Ρ������ܱ��Ρ��ųǹ����εȲ�Ʒ�����𱬡�\r\n\r\n���⣬�Լ��β�Ʒ�����𱬡�����һ���ڼ䣬�����ܱ���壬�Լ��ο��쳣�𱬣��������λ��Ŀ�ͻ�����ժ����԰�Ȼ��������������������ε���ȿ�����������η�ƶ��������彨��ĳ�����չ����廷�����ϸ��ƣ��߽���塢Ʒζũ�ϡ��ع���Ȼ��Ϊ���������ѡ��"))
	{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	lpWaveFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));
	lpWaveFormat->wFormatTag = 0x01;
	lpWaveFormat->nChannels = 1;
	// mod-szm
	lpWaveFormat->nSamplesPerSec = 44100;
	lpWaveFormat->wBitsPerSample = 16;
	lpWaveFormat->nBlockAlign = lpWaveFormat->wBitsPerSample * lpWaveFormat->nChannels / 8;
	lpWaveFormat->nAvgBytesPerSec = lpWaveFormat->nSamplesPerSec * lpWaveFormat->nBlockAlign;
	lpWaveFormat->cbSize = 0;

	pWaveOutHdr.dwFlags = 0;

	if (tts != NULL)
	{
		delete tts;
		tts = NULL;
	}
	tts = new TTS_ALL();
}

CTTSDemoDlg::~CTTSDemoDlg()
{
	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
		bufferLength = 0;
		bufferOffset = 0;
	}
	
	if (hWaveOut)
	{
		if (!(pWaveOutHdr.dwFlags & WHDR_DONE) && pWaveOutHdr.dwFlags != 0) waveOutReset(hWaveOut);
		if (pWaveOutHdr.dwFlags & WHDR_PREPARED) waveOutUnprepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
		waveOutClose(hWaveOut);
	}
	if (lpWaveFormat) free(lpWaveFormat);

	if (tts != NULL)
	{
		delete tts;
		tts = NULL;
	}
}

void CTTSDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CB_MODULE, m_selModel);
	DDX_Text(pDX, IDC_TB_TEXT, m_newText);
	DDX_Control(pDX, IDC_SLIDER_TIME, m_sliderTime);
}

BEGIN_MESSAGE_MAP(CTTSDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_TRY, &CTTSDemoDlg::OnBnClickedBtTry)
	ON_COMMAND(ID_ABOUT_TSSDEMO, &CTTSDemoDlg::OnAboutTssdemo)
	ON_BN_CLICKED(IDC_BT_SYNTHESIZE, &CTTSDemoDlg::OnBnClickedBtSynthesize)
	ON_BN_CLICKED(IDC_BT_PLAYPAUSE, &CTTSDemoDlg::OnBnClickedBtPlaypause)
	ON_BN_CLICKED(IDC_BT_STOP, &CTTSDemoDlg::OnBnClickedBtStop)
	ON_CBN_SELCHANGE(IDC_CB_MODULE, &CTTSDemoDlg::OnCbnSelchangeCbModule)
	ON_WM_TIMER()
//	ON_WM_HSCROLL()
ON_WM_HSCROLL()
ON_WM_SIZE()
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CTTSDemoDlg ��Ϣ�������

BOOL CTTSDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// �ڴ���Ӷ���ĳ�ʼ������

	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, lpWaveFormat, NULL, NULL, CALLBACK_NULL) != MMSYSERR_NOERROR)
	{
		MessageBox(_T("Could not open device for playback, main program will halt."));
		this->OnCancel();
	}

	CComboBox * cb = (CComboBox *)GetDlgItem(IDC_CB_MODULE);
	// ���� ���Ŀ¼�µ� model 
	std::vector<std::string> files;
	std::string model_str = "model";
	getDirs(model_str, files);
	for (int ii = 0; ii < files.size(); ii++)
	{
		std::string ml = model_str + "/" + files[ii];
		CString b;
		b = ml.c_str();
		cb->AddString( b );		
	}
	
	//cb->AddString(_T("model/model-so"));
	//cb->AddString(_T("model/model-lucheng"));
	//cb->SetCurSel(-1);  // ԭ�� 
	
	cb->SetCurSel(0);  // �޸� ��ʼģ��
	OnCbnSelchangeCbModule();   // ���� �����ͳ�ʼ�� 
	GetDlgItem(IDC_BT_SYNTHESIZE)->EnableWindow(TRUE); // �޸�  ԭ���� FALSE
	GetDlgItem(IDC_BT_PLAYPAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BT_STOP)->EnableWindow(TRUE);

	m_sliderTime.SetRange(0,100);

	isControlReady = true;
	RefreshControlPosition();
#ifdef _DEBUG
	GetDlgItem(IDC_BT_TRY)->ShowWindow(SW_SHOW);
#else
	GetDlgItem(IDC_BT_TRY)->ShowWindow(SW_HIDE);
#endif

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTTSDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTTSDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTTSDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ڴ���Ӷ���ĳ�ʼ��
	TCHAR szFileName[MAX_PATH + 1] = { 0 };
	if (GetModuleFileName(NULL, szFileName, MAX_PATH))
	{
		CString fileVersion;
		fileVersion.Format(_T("TTSDemo v%s"), CTTSDemoUtils::GetFileVersion(szFileName));
		GetDlgItem(IDC_LB_VERSION)->SetWindowText(fileVersion);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CTTSDemoDlg::OnBnClickedBtTry()
{
	CRect windowRect;
	CRect rect;
	CString str;
	this->GetClientRect(&windowRect);
	GetDlgItem(IDC_LB_INIT_MESSAGE)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	str.Format(_T("l:%d,t:%d,r:%d,b:%d,w:%d,h:%d\n"), rect.left, rect.top, rect.top, rect.bottom, rect.Width(), rect.Height());
	MessageBox(str);
	// �ڴ���ӿؼ�֪ͨ����������
	//BYTE* lostcause = new BYTE[100]; //Memory Leak Detect Test

	//int ret = 0;
	//int wav_size = WAV_SIZE;
	//char *tline = new char[MAX_LINE];
	//short *buff_tmp = new short[WAV_SIZE];
	//short *buff = buff_tmp;

	//FILE *fp = fopen("sample/test.txt", "r");
	//FILE * fp_wav = fopen("sample/test.pcm", "wb");
	//char *model_dir = "model/model-so";
	////char *model_dir = "model/model-lucheng";

	//ret = tts.init(model_dir);
	//if (ret < 0)
	//{
	//	MessageBox(_T("tts init err!"));
	//	return;
	//}

	////char *teof = fgets(tline, MAX_LINE, fp);
	////while (teof)
	////{
	////	ret = tts.tts(tline, buff, wav_size);
	////	buff += ret;
	////	wav_size -= ret;

	////	teof = fgets(tline, MAX_LINE, fp);
	////}

	//for (int i = 0; i<buff - buff_tmp; i++)
	//{

	//	fwrite(&buff_tmp[i], sizeof(short), 1, fp_wav);
	//}

	//fclose(fp_wav);
	//fclose(fp);

	//delete tline;
	//delete buff_tmp;

	//MessageBox(_T("OK"));

	/*UpdateData(TRUE);
	m_newText.Replace(_T("\r\n"), _T("\n"));
	MessageBox(m_newText);*/
}


void CTTSDemoDlg::OnAboutTssdemo()
{
	//�ڴ���������������
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CTTSDemoDlg::OnBnClickedBtSynthesize()
{
	GetDlgItem(IDC_BT_PLAYPAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_STOP)->EnableWindow(FALSE);

	if (hWaveOut)
	{
		if (!(pWaveOutHdr.dwFlags & WHDR_DONE) && pWaveOutHdr.dwFlags != 0) waveOutReset(hWaveOut);
		if (pWaveOutHdr.dwFlags & WHDR_PREPARED) waveOutUnprepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
		pWaveOutHdr.lpData = NULL;
		pWaveOutHdr.dwBufferLength = 0;
		pWaveOutHdr.dwFlags = 0;
	}

	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
		bufferLength = 0;
		bufferOffset = 0;
	}
	buffer = new short[WAV_SIZE];

	if (curModelState < 0) return;

	UpdateData(TRUE);

	if (m_newText.GetLength() == 0)
	{
		MessageBox(_T("Empty Input"));
		return;
	}

	GetDlgItem(IDC_LB_MESSAGE)->SetWindowText(_T("Synthesizing..."));
	bufferLength = 0;

	m_newText.Replace(_T("\r\n"), _T("\n"));
	CStringA textA = CTTSDemoUtils::UTF16_UTF8(m_newText);
	//CStringA textA = CTTSDemoUtils::UTF16_UTF8(_T("123"));
	bufferLength = tts->tts(textA.GetBuffer(), buffer, WAV_SIZE);
	
	// ���� wav ������ļ�
	FILE * fp_wav = NULL;
	fp_wav = fopen("test.pcm", "wb");
	if (fp_wav != NULL)
	{

		for (int i = 0; i < bufferLength; i++)
		{
			fwrite(&buffer[i], sizeof(short), 1, fp_wav);
		}
		fclose(fp_wav);
		fp_wav = NULL;
	}
	textA.ReleaseBuffer();

	GetDlgItem(IDC_LB_MESSAGE)->SetWindowText(_T("Synthesis Finished"));

	m_sliderTime.SetPos(0);
	m_sliderTime.SetRange(0, bufferLength);
	UpdateTime();

	GetDlgItem(IDC_BT_PLAYPAUSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BT_STOP)->EnableWindow(TRUE);
}


void CTTSDemoDlg::OnBnClickedBtPlaypause()
{
	// �ڴ���ӿؼ�֪ͨ����������
	if (bufferLength < 0) return;
	int curPos = m_sliderTime.GetPos();
	int curSample = 0;
	MMTIME time;
	time.wType = TIME_SAMPLES;

	if (hWaveOut)
	{
		waveOutGetPosition(hWaveOut, &time, sizeof(MMTIME));
		curSample = time.u.sample;
	}

	if (curPos < bufferLength - 1 && (curPos - curSample > 1600 || curPos - curSample < -1600))	//DragPlay
	{
		/*CString message;
		message.Format(_T("%d-%d=%d\n"), curPos, curSample, curPos - curSample);
		OutputDebugString(message);*/
		if (!(pWaveOutHdr.dwFlags & WHDR_DONE) && pWaveOutHdr.dwLoops != 0) waveOutReset(hWaveOut);
		if (pWaveOutHdr.dwFlags & WHDR_PREPARED) waveOutUnprepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
		bufferOffset = curPos;
		pWaveOutHdr.dwFlags = 0;
		pWaveOutHdr.lpData = (LPSTR)(&buffer[bufferOffset]);
		pWaveOutHdr.dwBufferLength = (bufferLength - bufferOffset) * 2;
		waveOutPrepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
		isPlaying = true;
		SetTimer(1, 200, NULL);
	}
	else   //Normal Play & pause
	{
		if (pWaveOutHdr.dwFlags & WHDR_PREPARED)
		{
			//Pause
			if (pWaveOutHdr.dwFlags & WHDR_DONE)
			{
				waveOutUnprepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
				pWaveOutHdr.dwFlags = 0;
				pWaveOutHdr.lpData = (LPSTR)buffer;
				pWaveOutHdr.dwBufferLength = bufferLength * 2;
				waveOutPrepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
				waveOutWrite(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
				isPlaying = true;
				SetTimer(1, 200, NULL);
			}
			else
			{
				if (isPlaying)
				{
					waveOutPause(hWaveOut);
					isPlaying = false;
					KillTimer(1);
				}
				else
				{
					waveOutRestart(hWaveOut);
					isPlaying = true;
					SetTimer(1, 200, NULL);
				}
			}
		}
		else
		{
			//Play
			pWaveOutHdr.dwFlags = 0;
			pWaveOutHdr.lpData = (LPSTR)buffer;
			pWaveOutHdr.dwBufferLength = bufferLength * 2;
			waveOutPrepareHeader(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &pWaveOutHdr, sizeof(WAVEHDR));
			isPlaying = true;
			SetTimer(1, 200, NULL);
		}
	}
}


void CTTSDemoDlg::OnBnClickedBtStop()
{
	// �ڴ���ӿؼ�֪ͨ����������
	if (bufferLength < 0) return;

	if (!(pWaveOutHdr.dwFlags & WHDR_DONE)) waveOutReset(hWaveOut);
	isPlaying = false;
	m_sliderTime.SetPos(0);
	bufferOffset = 0;
	UpdateTime();
}


void CTTSDemoDlg::OnCbnSelchangeCbModule()
{
	// �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CComboBox * cb = (CComboBox *)GetDlgItem(IDC_CB_MODULE);
	int index = cb->GetCurSel();
	if (index < 0)
	{
		GetDlgItem(IDC_LB_INIT_MESSAGE)->SetWindowText(_T("[Status: Uninitialed]"));
		GetDlgItem(IDC_BT_SYNTHESIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_PLAYPAUSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_STOP)->EnableWindow(FALSE);
		return;
	}
	else
	{
		GetDlgItem(IDC_LB_INIT_MESSAGE)->SetWindowText(_T("[Status: Initializing...]"));
		CStringA modelStringA = CTTSDemoUtils::UTF16_UTF8(m_selModel);
		curModelState = tts->init(modelStringA.GetBuffer());
		modelStringA.ReleaseBuffer();

		if (curModelState < 0)
		{
			cb->SetCurSel(-1);
			MessageBox(_T("TTS Engine Inital Failed."));
			GetDlgItem(IDC_LB_INIT_MESSAGE)->SetWindowText(_T("[Status: Uninitialed]"));
			GetDlgItem(IDC_BT_SYNTHESIZE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BT_PLAYPAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BT_STOP)->EnableWindow(FALSE);
		}
		else
		{
			CString message;
			message.Format(_T("[Status:Initialization Success(%s)]"), m_selModel);
			GetDlgItem(IDC_LB_INIT_MESSAGE)->SetWindowText(message);
			GetDlgItem(IDC_BT_SYNTHESIZE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BT_PLAYPAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BT_STOP)->EnableWindow(FALSE);
		}
	}
}


void CTTSDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	//�ڴ������Ϣ�����������/�����Ĭ��ֵ
	MMTIME time;
	time.wType = TIME_SAMPLES;

	switch (nIDEvent)
	{
		case 1:
			KillTimer(1);
			if (!hWaveOut) break;
			if (waveOutGetPosition(hWaveOut, &time, sizeof(time)) == 0)
			{
				int curSample = time.u.sample;
				m_sliderTime.SetPos(curSample + bufferOffset);
				UpdateTime();
			}
			if (pWaveOutHdr.dwFlags&WHDR_DONE)	//Check if the play is finished.
			{
				waveOutReset(hWaveOut);
				isPlaying = false;
				bufferOffset = 0;
			}
			else SetTimer(1, 200, NULL);
			break;
		default:
			;//Do nothing
	}

	CDialogEx::OnTimer(nIDEvent);
}



void CTTSDemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//�ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	UpdateTime();

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


CString CTTSDemoDlg::FormatTime(int seconds)
{
	CString ret;
	int h = seconds / 3600;
	int m = (seconds % 3600) / 60;
	int s = seconds % 60;

	ret.Format(_T("%02d:%02d:%02d"), h, m, s);

	return ret;
}


void CTTSDemoDlg::UpdateTime()
{
	if (bufferLength <= 0)
	{
		GetDlgItem(IDC_LB_TIME)->SetWindowText(_T("00:00:00|00:00:00"));
		return;
	}

	int pos = m_sliderTime.GetPos() + bufferOffset;
	// mod-szm
	int allSec = bufferLength / 44100;
	int curSec = pos / 44100;

	CString timeStr;
	timeStr.Format(_T("%s|%s"), FormatTime(curSec), FormatTime(allSec));
	GetDlgItem(IDC_LB_TIME)->SetWindowText(timeStr);
}


void CTTSDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	RefreshControlPosition();
}


void CTTSDemoDlg::RefreshControlPosition()
{
	if (!isControlReady) return;

	int x1, x2, y1, y2, ID;
	CRect windowRect;
	CRect rect;
	this->GetClientRect(&windowRect);
	
	//IDC_LB_MESSAGE
	ID = IDC_LB_MESSAGE;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = 12;
	y1 = windowRect.Height() - 12 - 20;
	x2 = windowRect.Width() - 12;
	y2 = y1 + 20;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_BT_SYHTHESIZE
	ID = IDC_BT_SYNTHESIZE;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = 20;
	y1 = windowRect.Height() - 12 - 20 - 5 - 35;
	x2 = x1 + 100;
	y2 = y1 + 35;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_BT_PLAYPAUSE
	ID = IDC_BT_PLAYPAUSE;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = windowRect.Width() - 360;
	y1 = windowRect.Height() - 12 - 20 - 5 - 35;
	x2 = x1 + 100;
	y2 = y1 + 35;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_BT_STOP
	ID = IDC_BT_STOP;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = windowRect.Width() - 360 + 100 + 10;
	y1 = windowRect.Height() - 12 - 20 - 5 - 35;
	x2 = x1 + 100;
	y2 = y1 + 35;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_LB_TIME
	ID = IDC_LB_TIME;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = windowRect.Width() - 360 + 100 + 10 + 100 + 10;
	y1 = windowRect.Height() - 12 - 20 - 5 - 35;
	x2 = windowRect.Width() - 12;
	y2 = y1 + 20;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_BT_TRY
	ID = IDC_BT_TRY;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = windowRect.Width() - 12 - 80;
	y1 = windowRect.Height() - 12 - 20 - 20;
	x2 = windowRect.Width() - 12;
	y2 = y1 + 20;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_SLIDER_TIME
	ID = IDC_SLIDER_TIME;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = 12;
	y1 = windowRect.Height() - 12 - 20 - 5 - 35 - 5 - 25;
	x2 = windowRect.Width() - 12;
	y2 = y1 + 25;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_TB_TEXT
	ID = IDC_TB_TEXT;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = 12;
	y1 = 45;
	x2 = windowRect.Width() - 12;
	y2 = windowRect.Height() - 12 - 20 - 5 - 35 - 5 - 25 - 5;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_LB_INIT_MESSAGE
	ID = IDC_LB_INIT_MESSAGE;
	GetDlgItem(ID)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	x1 = 245;
	y1 = 12;
	x2 = windowRect.Width() - 12;
	y2 = 12 + 20;
	rect.SetRect(x1, y1, x2, y2);
	GetDlgItem(ID)->MoveWindow(rect);

	//IDC_CB_MODULE (Use Defalut)
	//IDC_STATIC (Use Default)

	this->Invalidate();
}


void CTTSDemoDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	//�������Ϣ�����������/�����Ĭ��ֵ

	lpMMI->ptMinTrackSize.x = 570;
	lpMMI->ptMinTrackSize.y = 400;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}
