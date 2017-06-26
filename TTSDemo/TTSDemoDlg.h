
// TTSDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "TTS_ALL.h"
#include "mmsystem.h"
#include "afxcmn.h"
#include "pre_pro.h"


// CTTSDemoDlg �Ի���
class CTTSDemoDlg : public CDialogEx
{
// ����
public:
	CTTSDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CTTSDemoDlg();

// �Ի�������
	enum { IDD = IDD_TTSDEMO_DIALOG };

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

//Custom
private:
	//TTS & Play Buffer
	short * buffer = NULL;
	int bufferLength = 0;
	int bufferOffset = 0;

	//TTS Model
	CString curModel;
	int curModelState;

	//WINMM Sound Playback
	HWAVEOUT hWaveOut = NULL;
	WAVEFORMATEX * lpWaveFormat = NULL;
	WAVEHDR pWaveOutHdr;
	bool isPlaying = false;

	//Custome Functions
	CString FormatTime(int seconds);
	void UpdateTime();
	void RefreshControlPosition();

	bool isControlReady = false;

public:
	TTS_ALL *tts = NULL;
	CString m_selModel;
	CString m_newText;
	CSliderCtrl m_sliderTime;

	afx_msg void OnBnClickedBtTry();
	afx_msg void OnAboutTssdemo();
	afx_msg void OnBnClickedBtSynthesize();
	afx_msg void OnBnClickedBtPlaypause();
	afx_msg void OnBnClickedBtStop();
	afx_msg void OnCbnSelchangeCbModule();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);



};
